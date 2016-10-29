#include <vector>
#include <cmath>
#include <allegro5/allegro_primitives.h>

#include "ingameelements/character.h"
#include "gamestate.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/player.h"
#include "global_constants.h"
#include "engine.h"
#include "ingameelements/weapon.h"
#include "ingameelements/corpse.h"
#include "renderer.h"


Character::Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters) : MovingEntity(id_, state),
            owner(owner_), weapon(parameters.weapon), hp(parameters.maxhp), isflipped(false), runanim(parameters.characterfolder+"run/"),
            crouchanim(parameters.characterfolder+"crouchwalk/"), stunanim(parameters.characterfolder+"stun/"), pressed_keys(), held_keys()
{
    acceleration = 300;
    runpower = parameters.runpower;
    // friction factor per second of null movement; calculated directly from Gang Garrison 2
    // from pygg2
    friction = 0.01510305449388463132584804061124;

    entitytype = CHARACTER;

    crouchanim.active(false);
    stunanim.active(false);
}

void Character::setinput(Gamestate *state, InputContainer pressed_keys_, InputContainer held_keys_, double mouse_x_, double mouse_y_)
{
    pressed_keys = pressed_keys_;
    held_keys = held_keys_;
    mouse_x = mouse_x_;
    mouse_y = mouse_y_;
    getweapon(state)->setaim(mouse_x, mouse_y);
}

void Character::beginstep(Gamestate *state, double frametime)
{
    getweapon(state)->beginstep(state, frametime);
}

void Character::midstep(Gamestate *state, double frametime)
{
    if (cangetinput(state))
    {
        double maxhspeed;
        if (crouchanim.active())
        {
            maxhspeed = 60.0;
        }
        else
        {
            maxhspeed = 153.0;
        }

        if (held_keys.LEFT)
        {
            hspeed = std::max(hspeed - acceleration * runpower * frametime, -maxhspeed);
        }
        if (held_keys.RIGHT)
        {
            hspeed = std::min(hspeed + acceleration * runpower * frametime, maxhspeed);
        }

        if (pressed_keys.JUMP)
        {
            if (onground(state))
            {
                vspeed = -240.0;
            }
        }
        if (held_keys.CROUCH)
        {
            if (not crouchanim.active())
            {
                crouchanim.active(true);
                crouchanim.reset();
            }
        }
        else if (crouchanim.active())
        {
            // We're crouched and we'd like to uncrouch
            // Do so only if we have the room
            if (not state->currentmap->collides(getstandingcollisionrect(state)))
            {
                crouchanim.active(false);
            }
        }

        if (held_keys.RELOAD)
        {
            getweapon(state)->reload(state);
        }
        // Shooting
        if (held_keys.PRIMARY_FIRE)
        {
            getweapon(state)->wantfireprimary(state);
        }
        if (held_keys.SECONDARY_FIRE)
        {
            getweapon(state)->wantfiresecondary(state);
        }

        if (isflipped != (mouse_x < x))
        {
            // Spinjumping (compensate for later gravity)
            vspeed -= 540.0*frametime*3.0/4.0;
            isflipped = (mouse_x < x);
        }
    }

    // Gravity
    vspeed += 540.0*frametime;

    // apply friction
    hspeed *= std::pow(friction, frametime);

    // Passive ult charge
    Player *p = state->get<Player>(owner);
    p->ultcharge.update(state, frametime*passiveultcharge());

    getweapon(state)->midstep(state, frametime);
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);

    // Collision with wallmask
    if (state->currentmap->collides(getcollisionrect(state)))
    {
        // We collide, do collision handling
        double hs = hspeed*frametime, vs = vspeed*frametime;
        // Buffers for "undone" horizontal/vertical movement
        double xbuffer = 0.0, ybuffer = 0.0;
        int steps = std::ceil(std::fmax(std::abs(hs), std::abs(vs)));
        double xstep = hs/steps, ystep = vs/steps;
        // Pull us out of the wall
        for (int i=0; i<steps; ++i)
        {
            x -= xstep; y -= ystep;
            xbuffer += xstep; ybuffer += ystep;
            if (not state->currentmap->collides(getcollisionrect(state)))
            {
                break;
            }
        }
        // We're at the point where the character touched the wallmask for the first time
        // Now keep moving one unit in either direction until all possible movement is exhausted
        bool xblocked = false, yblocked = false;
        bool xfinished = false, yfinished = false;
        double oldxbuffer = xbuffer, oldybuffer = ybuffer;
        while (not xfinished or not yfinished)
        {
            oldxbuffer = xbuffer; oldybuffer = ybuffer;
            // Try first moving horizontally
            if (not xfinished)
            {
                if (not state->currentmap->collides(getcollisionrect(state).offset(xstep, 0)))
                {
                    x += xstep;
                    xbuffer -= xstep;
                    if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                    {
                        // We're done for this coordinate
                        xfinished = true;
                    }
                }
                else
                {
                    // There's the possibility we might have walked into a staircase
                    if (not state->currentmap->collides(getcollisionrect(state).offset(xstep, STAIRCASE_STEPSIZE)))
                    {
                        // Indeed we did. Move up
                        x += xstep;
                        y += STAIRCASE_STEPSIZE;
                        xbuffer -= xstep;
                        if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                        {
                            // We're done for this coordinate
                            xfinished = true;
                        }
                    }
                    else
                    {
                        // No more moving, we're blocked
                        xblocked = true;
                        xfinished = true;
                    }
                }
            }
            // Do the same vertically, but without stair code
            if (not yfinished)
            {
                if (not state->currentmap->collides(getcollisionrect(state).offset(0, ystep)))
                {
                    // We cam move through
                    y += ystep;
                    ybuffer -= ystep;
                    if (ybuffer == 0.0 or std::signbit(ybuffer) != std::signbit(oldybuffer))
                    {
                        // We're done for this coordinate
                        yfinished = true;
                    }
                }
                else
                {
                    // No more moving, we're blocked
                    yblocked = true;
                    yfinished = true;
                }
            }
        }
        // Set hspeed and vspeed to what they should be
        if (xblocked)
        {
            hspeed = 0;
        }
        if (yblocked)
        {
            vspeed = 0;
        }
        if (state->currentmap->collides(getcollisionrect(state)))
        {
            printf("\nError: Still in wallmask after collision handling.");
        }
    } // end collision with wallmask

    // Stick to stairs when going down
    if (std::fabs(vspeed*frametime) <= 3.0 and not std::signbit(vspeed*frametime) and not onground(state))
    {
        Rect r = getcollisionrect(state);
        int stepsize = (-STAIRCASE_STEPSIZE)*std::ceil(std::fabs(hspeed)*frametime/(-STAIRCASE_STEPSIZE));
        if (state->currentmap->collides(Rect(r.x, r.y+r.h+stepsize, r.w, 1)))
        {
            for (int i=0; i<stepsize; ++i)
            {
                ++y;
                if (onground(state))
                {
                    break;
                }
            }
        }
    }

    // Running animation
    if (onground(state))
    {
        if (isflipped)
        {
            runanim.update(state, -hspeed*frametime);
            crouchanim.update(state, -hspeed*frametime);
        }
        else
        {
            runanim.update(state, hspeed*frametime);
            crouchanim.update(state, hspeed*frametime);
        }
    }
    stunanim.update(state, frametime);
    if (hspeed == 0.0)
    {
        bool run=runanim.active(), crouch=crouchanim.active();
        runanim.reset();
        runanim.active(run);
        crouchanim.reset();
        crouchanim.active(crouch);
    }

    getweapon(state)->endstep(state, frametime);
}

void Character::render(Renderer *renderer, Gamestate *state)
{
    // --------------- HEALTHBAR ---------------
    al_set_target_bitmap(renderer->surfaceground);
    std::string mainsprite = getsprite(state, false);
    double healthalpha = 1.0;
    double lack_healthalpha = 0.2;

    // Normal, armor, shields
    ALLEGRO_COLOR EXISTING_HEALTH[] = { al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, healthalpha)};
    ALLEGRO_COLOR LACKING_HEALTH[] = {  al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, lack_healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, lack_healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, lack_healthalpha)};

    Health maxhp = getmaxhp();
    double x_;
    float r[8]; // Array used to pass the polygon data for the actual drawing

    // Parameters
    double totalwidth = 60;
    int height = 6;
    int space = 2;
    double slant = 0.3;
    double y_ = renderer->zoom*(y - renderer->spriteloader.get_spriteoffset_y(mainsprite) - renderer->cam_y - 15);

    int nrects = std::ceil(maxhp.total()/25.0);
    double width = totalwidth/nrects;
    Rect box = renderer->spriteloader.get_rect(mainsprite);
    double start_x = renderer->zoom*(x - renderer->spriteloader.get_spriteoffset_x(mainsprite) + box.w/2.0 - renderer->cam_x) - width*(nrects/2.0) - space*((nrects-1)/2.0);

    // Draw first normal health, then armor, then shields
    for (int healthtype=0; healthtype<3; ++healthtype)
    {
        double hppercent = 1.0;
        if (healthtype == 0)
        {
            nrects = std::ceil(maxhp.normal/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.normal/maxhp.normal;
        }
        else if (healthtype == 1)
        {
            nrects = std::ceil(maxhp.armor/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.armor/maxhp.armor;
        }
        else if (healthtype == 2)
        {
            nrects = std::ceil(maxhp.shields/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.shields/maxhp.shields;
        }

        // Full existing health boxes
        for (int i=0; i<std::floor(nrects*hppercent); ++i)
        {
            x_ = start_x + i*width + (i-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);
        }
        // Interface between existing and lacking health
        double leftover = nrects*hppercent - std::floor(nrects*hppercent);
        if (leftover > 0.0)
        {
            // Draw the half-rectangle
            x_ = start_x + std::floor(nrects*hppercent)*width + (std::floor(nrects*hppercent)-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width*leftover;
            r[5] = y_+height;

            r[6] = x_+width*leftover+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);

            // Draw the other half rectangle
            r[0] = x_+width*leftover+height*slant;
            r[1] = y_;

            r[2] = x_+width*leftover;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
        // Full lacking health boxes
        for (int i=std::ceil(nrects*hppercent); i<nrects; ++i)
        {
            x_ = start_x + i*width + (i-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
//        // Outline
//        for (int i=0; i<nrects; ++i)
//        {
//            x_ = start_x + i*width + (i-1)*space;
//            r[0] = x_+height*slant;
//            r[1] = y_;
//
//            r[2] = x_;
//            r[3] = y_+height;
//
//            r[4] = x_+width;
//            r[5] = y_+height;
//
//            r[6] = x_+width+height*slant;
//            r[7] = y_;
//            al_draw_polygon(r, 4, ALLEGRO_LINE_JOIN_ROUND, EXISTING_HEALTH[healthtype], 0, 0);
//        }
        // Offset starting position for the next health
        start_x += nrects*(width + space);
    }

    // --------------- /HEALTHBAR ---------------
}

void Character::drawhud(Renderer *renderer, Gamestate *state)
{
    // Experimental healthbar
    double healthalpha = 1.0;
    double lack_healthalpha = 0.4;

    // Lucio shield
    // al_premul_rgba_f(242/255.0, 197/255.0, 84/255.0, healthalpha)
    // Torb armor
    // al_premul_rgba_f(69/255.0, 122/255.0, 255/255.0, healthalpha)

    // Normal, armor, shields
    ALLEGRO_COLOR EXISTING_HEALTH[] = { al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, healthalpha)};
    ALLEGRO_COLOR LACKING_HEALTH[] = {  al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, lack_healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, lack_healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, lack_healthalpha)};
    double tmpx;
    float r[8]; // Array used to pass the polygon data for the actual drawing

    // Parameters
    int totalwidth = 250;
    Health maxhp = getmaxhp();
    double width = totalwidth/std::ceil(maxhp.total()/25.0);
    int height = 20;
    int space = 20/9.0;
    double slant = 0.3;
    double tmpy = hudheight()*renderer->WINDOW_HEIGHT - height;
    double start_x = renderer->WINDOW_WIDTH/9.0;

    // Draw first normal health, then armor, then shields
    for (int healthtype=0; healthtype<3; ++healthtype)
    {
        int nrects = 0;
        double hppercent = 1.0;
        if (healthtype == 0)
        {
            nrects = std::ceil(maxhp.normal/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.normal/maxhp.normal;
        }
        else if (healthtype == 1)
        {
            nrects = std::ceil(maxhp.armor/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.armor/maxhp.armor;
        }
        else if (healthtype == 2)
        {
            nrects = std::ceil(maxhp.shields/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.shields/maxhp.shields;
        }

        // Full existing health boxes
        for (int i=0; i<std::floor(nrects*hppercent); ++i)
        {
            tmpx = start_x + i*width + (i-1)*space;
            r[0] = tmpx+height*slant;
            r[1] = tmpy;

            r[2] = tmpx;
            r[3] = tmpy+height;

            r[4] = tmpx+width;
            r[5] = tmpy+height;

            r[6] = tmpx+width+height*slant;
            r[7] = tmpy;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);
        }
        // Interface between existing and lacking health
        double leftover = nrects*hppercent - std::floor(nrects*hppercent);
        if (leftover > 0.0)
        {
            // Draw the half-rectangle
            tmpx = start_x + std::floor(nrects*hppercent)*width + (std::floor(nrects*hppercent)-1)*space;
            r[0] = tmpx+height*slant;
            r[1] = tmpy;

            r[2] = tmpx;
            r[3] = tmpy+height;

            r[4] = tmpx+width*leftover;
            r[5] = tmpy+height;

            r[6] = tmpx+width*leftover+height*slant;
            r[7] = tmpy;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);

            // Draw the other half rectangle
            r[0] = tmpx+width*leftover+height*slant;
            r[1] = tmpy;

            r[2] = tmpx+width*leftover;
            r[3] = tmpy+height;

            r[4] = tmpx+width;
            r[5] = tmpy+height;

            r[6] = tmpx+width+height*slant;
            r[7] = tmpy;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
        // Full lacking health boxes
        for (int i=std::ceil(nrects*hppercent); i<nrects; ++i)
        {
            tmpx = start_x + i*width + (i-1)*space;
            r[0] = tmpx+height*slant;
            r[1] = tmpy;

            r[2] = tmpx;
            r[3] = tmpy+height;

            r[4] = tmpx+width;
            r[5] = tmpy+height;

            r[6] = tmpx+width+height*slant;
            r[7] = tmpy;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
        // Outline
        for (int i=0; i<nrects; ++i)
        {
            tmpx = start_x + i*width + (i-1)*space;
            r[0] = tmpx+height*slant;
            r[1] = tmpy;

            r[2] = tmpx;
            r[3] = tmpy+height;

            r[4] = tmpx+width;
            r[5] = tmpy+height;

            r[6] = tmpx+width+height*slant;
            r[7] = tmpy;
            al_draw_polygon(r, 4, ALLEGRO_LINE_JOIN_ROUND, EXISTING_HEALTH[healthtype], 0, 0);
        }
        // Offset starting position for the next health
        start_x += nrects*(width + space);
    }


    // Ammo count
    std::string ammo = std::to_string(getweapon(state)->clip);
    std::string maxammo = "I "+std::to_string(getweapon(state)->getclipsize());
    tmpx = renderer->WINDOW_WIDTH*9/10.0;
    al_draw_text(renderer->font20, al_map_rgb(255, 255, 255), tmpx, renderer->WINDOW_HEIGHT*hudheight()-al_get_font_line_height(renderer->font20), ALLEGRO_ALIGN_LEFT, ammo.c_str());
    al_draw_text(renderer->font10, al_map_rgb(255, 255, 255), tmpx+al_get_text_width(renderer->font20, ammo.c_str()),
                    renderer->WINDOW_HEIGHT*hudheight()-al_get_font_line_height(renderer->font10), ALLEGRO_ALIGN_LEFT, maxammo.c_str());


    // Ult charge meter
    Player *p = state->get<Player>(owner);
    ALLEGRO_BITMAP *ultbar = renderer->spriteloader.requestsprite("ui/ingame/ultbar");
    Rect ultbarrect = renderer->spriteloader.get_rect("ui/ingame/ultbar").offset(renderer->WINDOW_WIDTH/2.0, hudheight()*renderer->WINDOW_HEIGHT);
    al_draw_bitmap(ultbar, ultbarrect.x - ultbarrect.w/2.0, ultbarrect.y - ultbarrect.h/2.0, 0);
    al_draw_arc(ultbarrect.x, ultbarrect.y-8, 33, -3.1415/2.0, 2*3.1415*p->ultcharge.timer/100.0, al_map_rgb(255, 230, 125), 8);
}

bool Character::onground(Gamestate *state)
{
    Rect r = getcollisionrect(state);
    return state->currentmap->collides(Rect(r.x, r.y+r.h, r.w, 1));
}

void Character::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Character *prev_e = static_cast<Character*>(prev_entity);
    Character *next_e = static_cast<Character*>(next_entity);

    if (alpha < 0.5)
    {
        held_keys = prev_e->held_keys;
        pressed_keys = prev_e->pressed_keys;
        crouchanim.active(prev_e->crouchanim.active());
    }
    else
    {
        held_keys = next_e->held_keys;
        pressed_keys = next_e->pressed_keys;
        crouchanim.active(prev_e->crouchanim.active());
    }
    mouse_x = prev_e->mouse_x + alpha*(next_e->mouse_x - prev_e->mouse_x);
    mouse_y = prev_e->mouse_y + alpha*(next_e->mouse_y - prev_e->mouse_y);
    runanim.interpolate(&(prev_e->runanim), &(next_e->runanim), alpha);
    hp.normal = prev_e->hp.normal + alpha*(next_e->hp.normal - prev_e->hp.normal);
    hp.armor = prev_e->hp.armor + alpha*(next_e->hp.armor - prev_e->hp.armor);
    hp.shields = prev_e->hp.shields + alpha*(next_e->hp.shields - prev_e->hp.shields);
}

void Character::serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate)
{
    MovingEntity::serialize(state, buffer, fullupdate);

    buffer->write<uint16_t>(hp.normal);
    buffer->write<uint16_t>(hp.armor);
    buffer->write<uint16_t>(hp.shields);

    ReducedInputContainer input = held_keys.reduce();
    input.serialize(buffer);

    buffer->write<int16_t>(mouse_x);
    buffer->write<int16_t>(mouse_y);

    getweapon(state)->serialize(state, buffer, fullupdate);
}

void Character::deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate)
{
    MovingEntity::deserialize(state, buffer, fullupdate);

    hp.normal = buffer->read<uint16_t>();
    hp.armor = buffer->read<uint16_t>();
    hp.shields = buffer->read<uint16_t>();

    ReducedInputContainer input;
    input.deserialize(buffer);
    held_keys.update(input);

    mouse_x = buffer->read<int16_t>();
    mouse_y = buffer->read<int16_t>();
    getweapon(state)->setaim(mouse_x, mouse_y);

    getweapon(state)->deserialize(state, buffer, fullupdate);
}

void Character::damage(Gamestate *state, double amount)
{
    if (hp.shields < amount)
    {
        amount -= hp.shields;
        hp.shields = 0;
        if (hp.armor > 0)
        {
            amount -= std::min(amount/2.0, 5.0);
        }
        if (hp.armor < amount)
        {
            amount -= hp.armor;
            hp.armor = 0;
            hp.normal -= amount;
            if (hp.normal <= 0)
            {
                // RIP
                if (state->engine->isserver)
                {
                    destroy(state);

                    state->sendbuffer->write<uint8_t>(PLAYER_DIED);
                    state->sendbuffer->write<uint8_t>(state->findplayerid(owner));

                    state->get<Player>(owner)->spawntimer.reset();
                }
            }
        }
        else
        {
            hp.armor -= amount;
        }
    }
    else
    {
        hp.shields -= amount;
    }
}

void Character::destroy(Gamestate *state)
{
    state->get<Player>(owner)->character = 0;
    getweapon(state)->destroy(state);
    Corpse *c = state->get<Corpse>(state->make_entity<Corpse>(state, getcharacterfolder(), isflipped));
    c->x = x;
    c->y = y;

    MovingEntity::destroy(state);
}

Weapon* Character::getweapon(Gamestate *state)
{
    return state->get<Weapon>(weapon);
}
