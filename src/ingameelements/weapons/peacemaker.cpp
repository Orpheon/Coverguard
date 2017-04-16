#include <cmath>

#include "ingameelements/weapons/peacemaker.h"
#include "renderer.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/explosion.h"
#include "ingameelements/trail.h"
#include "engine.h"

void Peacemaker::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Clipweapon::init(id_, state, owner_);

    fthanim.init(herofolder()+"fanthehammerstart/", std::bind(&Peacemaker::firesecondary, this, std::placeholders::_1));
    fthanim.active(false);
    deadeyeanim.init(herofolder()+"fanthehammerloop/");
    deadeyeanim.active(false);
    isfthing = false;
    isfiringult = false;
}

void Peacemaker::render(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    double dir = aimdirection;
    Mccree &c = state.get<Mccree>(state.get<Player>(owner).character);
    if (firinganim.active())
    {
        mainsprite = firinganim.getframepath();
    }
    else if (reloadanim.active())
    {
        mainsprite = reloadanim.getframepath();
        dir = 3.1415*c.isflipped;
    }
    else if (fthanim.active())
    {
        mainsprite = fthanim.getframepath();
    }
    else
    {
        mainsprite = c.herofolder()+"arm/1";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getattachpoint_x()*renderer.zoom;
    double attachpt_y = getattachpoint_y()*renderer.zoom;

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, 1, -1, dir, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, dir, 0);
        }
    }
}

void Peacemaker::midstep(Gamestate &state, double frametime)
{
    Clipweapon::midstep(state, frametime);

    if (isfthing)
    {
        fthanim.active(true);
    }
    fthanim.update(state, frametime);

    Player &ownerplayer = state.get<Player>(owner);
    Mccree &ownerchar = state.get<Mccree>(ownerplayer.character);
    if (ownerchar.ulting.active and not isfiringult)
    {
        for (auto &p : state.playerlist)
        {
            Player &player = state.get<Player>(p);
            if (player.team != SPECTATOR and player.team != team)
            {
                if (state.exists(player.character))
                {
                    Character &c = player.getcharacter(state);
                    if (not state.currentmap->collideline(x, y, c.x, c.y))
                    {
                        if (deadeyetargets.count(p.id) == 0)
                        {
                            deadeyetargets[p.id] = 0;
                        }
                        deadeyetargets[p.id] += frametime*170;
                    }
                }
            }
        }
    }
}

void Peacemaker::reload(Gamestate &state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active() and not isfthing)
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Peacemaker::wantfireprimary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and not isfthing and not isfiringult and state.engine.isserver)
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Peacemaker::fireprimary(Gamestate &state)
{
    double cosa = std::cos(aimdirection), sina = std::sin(aimdirection);
    double collisionptx, collisionpty;
    double d = std::hypot(state.currentmap->width(), state.currentmap->height());
    EntityPtr target = state.collidelinedamageable(state, x, y, x+cosa*d, y+sina*d, team, &collisionptx, &collisionpty);
    if (state.exists(target))
    {
        double distance = std::hypot(collisionptx-x, collisionpty-y);
        double falloff = 1.0;
        if (distance > FALLOFF_BEGIN)
        {
            falloff = std::max(0.0, 1 - (distance-FALLOFF_BEGIN)/(FALLOFF_END-FALLOFF_BEGIN));
        }
        Entity &e = state.get<Entity>(target);
        e.damage(state, MAX_DAMAGE*falloff);
    }

    state.make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
    Explosion &e = state.get<Explosion>(state.make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection));
    e.x = x+cosa*24;
    e.y = y+sina*24;

    --clip;
    firinganim.reset();
    firinganim.active(true);
}

void Peacemaker::wantfiresecondary(Gamestate &state)
{
    if (clip > 0)
    {
        if (not isfthing and state.engine.isserver and not reloadanim.active() and not firinganim.active() and not isfiringult)
        {
            firesecondary(state);
            state.engine.sendbuffer.write<uint8_t>(SECONDARY_FIRED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
        else if (isfthing and fthanim.getpercent() >= 1)
        {
            firesecondary(state);
            state.engine.sendbuffer.write<uint8_t>(SECONDARY_FIRED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
    }
    else
    {
        isfthing = false;
        fthanim.active(false);
    }
}

void Peacemaker::firesecondary(Gamestate &state)
{
    double spread = (2*(rand()/(RAND_MAX+1.0)) - 1)*25*3.1415/180.0;
    double cosa = std::cos(aimdirection+spread), sina = std::sin(aimdirection+spread);
    double collisionptx, collisionpty;
    EntityPtr target = state.collidelinedamageable(state, x, y, x+cosa*FALLOFF_END, y+sina*FALLOFF_END, team,
                                                   &collisionptx, &collisionpty);
    if (state.exists(target))
    {
        double distance = std::hypot(collisionptx-x, collisionpty-y);
        double falloff = 1.0;
        if (distance > FALLOFF_BEGIN)
        {
            falloff = std::max(0.0, (distance-FALLOFF_BEGIN) / (FALLOFF_END-FALLOFF_BEGIN));
        }
        Entity &e = state.get<Entity>(target);
        e.damage(state, MAX_DAMAGE*falloff);
    }

    state.make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
    Explosion &e = state.get<Explosion>(state.make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection+spread));
    e.x = x+cosa*24;
    e.y = y+sina*24;

    --clip;

    if (clip > 0 and state.engine.isserver)
    {
        if (isfthing)
        {
            fthanim.init("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::wantfiresecondary, this, std::placeholders::_1));
        }
        else
        {
            fthanim.init("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::wantfiresecondary, this, std::placeholders::_1));
            isfthing = true;
        }
    }
}

void Peacemaker::fireultimate(Gamestate &state)
{
    isfiringult = true;
    if (deadeyetargets.size() > 0)
    {
        EntityPtr playerptr = 0;
        double distance = VIEWPORT_WIDTH*10;
        // Select closest target
        for (auto &p : deadeyetargets)
        {
            Player &player = state.get<Player>(p.first);
            if (state.exists(player.character))
            {
                Character &c = player.getcharacter(state);
                double d = std::hypot(c.x-x, c.y-y);
                if (d < distance)
                {
                    distance = d;
                    playerptr = p.first;
                }
            }
        }

        Character &c = state.get<Player>(playerptr).getcharacter(state);
        double collisionptx, collisionpty;
        EntityPtr target = state.collidelinedamageable(state, x, y, c.x, c.y, team, &collisionptx, &collisionpty);
        double angle = std::atan2(c.y-y, c.x-x), cosa = std::cos(angle), sina = std::sin(angle);
        if (state.exists(target))
        {
            Entity &e = state.get<Entity>(target);
            e.damage(state, deadeyetargets.at(playerptr.id));
        }

        state.make_entity<Trail>(state, al_premul_rgba(133, 238, 238, 150), x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);
        Explosion &e = state.get<Explosion>(state.make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", angle));
        e.x = x+cosa*24;
        e.y = y+sina*24;

        deadeyetargets.erase(playerptr.id);

        deadeyeanim.init("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::fireultimate, this, std::placeholders::_1));
    }
    else
    {
        Mccree &ownerchar = state.get<Mccree>(state.get<Player>(owner).character);
        ownerchar.resetafterult(state);
    }
}
