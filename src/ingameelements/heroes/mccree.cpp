#include "ingameelements/heroes/mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"
#include "ingameelements/projectiles/flashbang.h"

#include <memory>
#include <cmath>
#include <allegro5/allegro_primitives.h>

void Mccree::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    rollanim.init(herofolder()+"roll/");
    rollanim.active(false);
    flashbanganim.init(herofolder()+"flashbang/");
    flashbanganim.active(false);
    ultwalkanim.init(herofolder()+"ultwalk/");
    fallanim.init(herofolder()+"falling/");

    rollcooldown.init(8);
    rollcooldown.active = false;
    flashbangcooldown.init(10);
    flashbangcooldown.active = false;
    ulting.init(6, std::bind(&Mccree::resetafterult, this, std::placeholders::_1));
    ulting.active = false;
    ultcooldown.init(0.5);
    ultcooldown.active = false;
}

void Mccree::render(Renderer &renderer, Gamestate &state)
{
    Character::render(renderer, state);
    al_set_target_bitmap(renderer.midground);

    std::string mainsprite;
    ALLEGRO_BITMAP *sprite;
    double spriteoffset_x, spriteoffset_y;
    double rel_x, rel_y;
    rel_x = (x-renderer.cam_x)*renderer.zoom;
    rel_y = (y-renderer.cam_y)*renderer.zoom;

    if (flashbanganim.active())
    {
        std::string armsprite = flashbanganim.getframepath();
        sprite = renderer.spriteloader.requestsprite(armsprite);
        spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(armsprite)*renderer.zoom;
        spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(armsprite)*renderer.zoom;
        if (isflipped)
        {
            // Flip horizontally
            al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
        }
        else
        {
            al_draw_bitmap(sprite, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
        }
    }

    mainsprite = currentsprite(state, false);
    sprite = renderer.spriteloader.requestsprite(mainsprite);
    spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline
            al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, spriteoffset_x, spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
        }
    }
    else
    {
        al_draw_bitmap(sprite, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline
            al_draw_tinted_bitmap(outline, outlinecolor, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
        }
    }

    state.get<Weapon>(weapon).render(renderer, state);
}

void Mccree::drawhud(Renderer &renderer, Gamestate &state)
{
    Character::drawhud(renderer, state);

    double space = 3;
    float r[8];

    ALLEGRO_BITMAP *sprite;
    Rect spriterect = renderer.spriteloader.get_rect("ui/ingame/"+herofolder()+"rolling");
    if (rollcooldown.active)
    {
        sprite = renderer.spriteloader.requestsprite("ui/ingame/"+herofolder()+"rollingcooldown", 1.0);
    }
    else
    {
        sprite = renderer.spriteloader.requestsprite("ui/ingame/"+herofolder()+"rolling", 1.0);
    }
    spriterect.x = renderer.WINDOW_WIDTH*6/7.0 - spriterect.w*2 - space;
    spriterect.y = renderer.WINDOW_HEIGHT*hudheight()-spriterect.h;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (rollcooldown.active)
    {
        // Draw the fill-in
        r[0] = spriterect.x+17*rollcooldown.getpercent();
        r[1] = spriterect.y+2+34*(1-rollcooldown.getpercent());

        r[2] = spriterect.x;
        r[3] = spriterect.y+spriterect.h-2;

        r[4] = spriterect.x+39;
        r[5] = spriterect.y+spriterect.h-2;

        r[6] = spriterect.x+17*rollcooldown.getpercent()+39;
        r[7] = spriterect.y+2+34*(1-rollcooldown.getpercent());

        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));

        al_draw_text(renderer.font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer.font10)/2.0,
                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(rollcooldown.duration - rollcooldown.timer)).c_str());
    }

    if (flashbangcooldown.active)
    {
        sprite = renderer.spriteloader.requestsprite("ui/ingame/"+herofolder()+"flashbangcooldown", 1.0);
    }
    else
    {
        sprite = renderer.spriteloader.requestsprite("ui/ingame/"+herofolder()+"flashbang", 1.0);
    }
    spriterect.x = spriterect.x + spriterect.w+space;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (flashbangcooldown.active)
    {
        // Draw the fill-in
        r[0] = spriterect.x+17*flashbangcooldown.getpercent();
        r[1] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());

        r[2] = spriterect.x;
        r[3] = spriterect.y+spriterect.h-2;

        r[4] = spriterect.x+39;
        r[5] = spriterect.y+spriterect.h-2;

        r[6] = spriterect.x+17*flashbangcooldown.getpercent()+39;
        r[7] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());

        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));

        al_draw_text(renderer.font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer.font10)/2.0,
                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(flashbangcooldown.duration - flashbangcooldown.timer)).c_str());
    }
}


void Mccree::midstep(Gamestate &state, double frametime)
{
    Character::midstep(state, frametime);

    rollanim.update(state, frametime);
    if (rollanim.active())
    {
        if (isflipped)
        {
            hspeed = -360;
        }
        else
        {
            hspeed = 360;
        }
    }
    flashbanganim.update(state, frametime);
    rollcooldown.update(state, frametime);
    flashbangcooldown.update(state, frametime);
    ulting.update(state, frametime);
    ultcooldown.update(state, frametime);

    fallanim.update(state, vspeed*frametime);

    if (isflipped)
    {
        ultwalkanim.update(state, -hspeed*frametime);
    }
    else
    {
        ultwalkanim.update(state, hspeed*frametime);
    }

    if (canuseabilities(state))
    {
        if (heldkeys.ABILITY_1 and not rollcooldown.active and onground(state) and state.engine.isserver)
        {
            useability1(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY1_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
        if (heldkeys.ABILITY_2 and not flashbangcooldown.active and state.engine.isserver)
        {
            useability2(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY2_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
    }

    if (heldkeys.PRIMARY_FIRE and ulting.active and state.engine.isserver)
    {
        state.engine.sendbuffer.write<uint8_t>(ULTIMATE_USED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        Peacemaker &w = state.get<Peacemaker>(weapon);
        w.fireultimate(state);
    }
}

void Mccree::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Character::interpolate(prev_entity, next_entity, alpha);

    Mccree &p = static_cast<Mccree&>(prev_entity);
    Mccree &n = static_cast<Mccree&>(next_entity);

    rollanim.interpolate(p.rollanim, n.rollanim, alpha);
    rollcooldown.interpolate(p.rollcooldown, n.rollcooldown, alpha);
    flashbanganim.interpolate(p.flashbanganim, n.flashbanganim, alpha);
    flashbangcooldown.interpolate(p.flashbangcooldown, n.flashbangcooldown, alpha);
    ulting.interpolate(p.ulting, n.ulting, alpha);
    ultwalkanim.interpolate(p.ultwalkanim, n.ultwalkanim, alpha);
    ultcooldown.interpolate(p.ultcooldown, n.ultcooldown, alpha);
    fallanim.interpolate(p.fallanim, n.fallanim, alpha);
}

void Mccree::useability1(Gamestate &state)
{
    if (heldkeys.LEFT and not heldkeys.RIGHT)
    {
        isflipped = true;
    }
    else if (heldkeys.RIGHT and not heldkeys.LEFT)
    {
        isflipped = false;
    }
    // Lets roll
    rollanim.reset();
    rollcooldown.reset();
    Peacemaker &w = reinterpret_cast<Peacemaker&>(getweapon(state));
    w.clip = w.getclipsize();
    w.reloadanim.active(0);
    w.isfthing = false;
    w.fthanim.active(false);
    vspeed = 0;
}

void Mccree::useability2(Gamestate &state)
{
    // Flashbang
    flashbanganim.reset();
    flashbangcooldown.reset();
//    Flashbang &f = state.get<Flashbang>(state.make_entity<Flashbang>(state, owner));
//    f.x = x;
//    f.y = y;
//    double dir = std::atan2(mouse_y-y, mouse_x-x);
//    f.hspeed = std::cos(dir) * 300;
//    f.vspeed = std::sin(dir) * 300;
}

void Mccree::useultimate(Gamestate &state)
{
    if (ulting.active)
    {
        // We are already ulting and now want to fire
        Peacemaker &p = state.get<Peacemaker>(weapon);
        p.fireultimate(state);
    }
    else
    {
        // Start charging
        ulting.reset();
        state.get<Peacemaker>(weapon).deadeyetargets.clear();
    }
}

void Mccree::resetafterult(Gamestate &state)
{
    ulting.active = false;
    ultwalkanim.reset();
    ultcooldown.reset();
    Player &ownerplayer = state.get<Player>(owner);
    ownerplayer.ultcharge.active = true;
    Peacemaker &w = state.get<Peacemaker>(weapon);
    w.isfiringult = false;
    w.deadeyeanim.active(false);
    w.clip = w.getclipsize();
}

void Mccree::interrupt(Gamestate &state)
{
    if (ulting.active)
    {
        resetafterult(state);
    }
    rollanim.active(false);
}

Rect Mccree::getcollisionrect(Gamestate &state)
{
    if (crouchanim.active())
    {
        return state.engine.maskloader.get_rect_from_json(herofolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Mccree::getstandingcollisionrect(Gamestate &state)
{
    return state.engine.maskloader.get_rect_from_json(herofolder()).offset(x, y);
}

std::string Mccree::currentsprite(Gamestate &state, bool mask)
{
    if (stunanim.active())
    {
        return stunanim.getframepath();
    }
    if (ulting.active)
    {
        if (std::fabs(hspeed) < 5.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
        {
            return herofolder()+"ult/1";
        }
        return ultwalkanim.getframepath();
    }
    if (rollanim.active())
    {
        return rollanim.getframepath();
    }
    if (crouchanim.active())
    {
        return crouchanim.getframepath();
    }
    if (not ongroundsmooth.active)
    {
        if (vspeed > 100)
        {
            return fallanim.getframepath();
        }
        else
        {
            return herofolder()+"jump/1";
        }
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idle/1";
    }
    return runanim.getframepath();
}

bool Mccree::weaponvisible(Gamestate &state)
{
    return not rollanim.active() and not stunanim.active() and (not ulting.active or state.get<Peacemaker>(weapon).isfiringult);
}
