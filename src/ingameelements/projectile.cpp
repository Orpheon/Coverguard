#include "ingameelements/projectile.h"
#include "engine.h"

#include <cmath>

void Projectile::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::PROJECTILE;
    owner = owner_;
    team = state.get<Player>(owner).team;
}

void Projectile::midstep(Gamestate &state, double frametime)
{
    if (not (penetration() & PENETRATE_WALLMASK) and state.currentmap->collides(x, y, spriteid(), std::atan2(vspeed, hspeed)))
    {
        destroy(state);
    }
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        if (entity.damageableby(team) and checkcollision(state, entity))
        {
            dealdamage(state, entity);
            if (entity.blocks(penetration()))
            {
                destroy(state);
                break;
            }
        }
    }
}

bool Projectile::checkcollision(Gamestate &state, Entity &target)
{
    Rect bbox = state.engine.maskloader.get_rect(spriteid());
    double angle = std::atan2(vspeed, hspeed);
    double target_x = 0, target_y = 0;
    double maxdist = target.maxdamageabledist(state, &target_x, &target_y) + std::hypot(bbox.w, bbox.h);
    if (std::hypot(x - target_x, y - target_y) > maxdist)
    {
        return false;
    }
    // We're close enough that an actual collision might happen
    ALLEGRO_BITMAP *sprite = state.engine.maskloader.requestsprite(spriteid());
    double spriteoffset_x = state.engine.maskloader.get_spriteoffset_x(spriteid());
    double spriteoffset_y = state.engine.maskloader.get_spriteoffset_y(spriteid());
    double cosa = std::cos(angle), sina = std::sin(angle);
    for (int i = 0; i < bbox.w; ++i)
    {
        double relx = i - spriteoffset_x;
        for (int j = 0; j < bbox.h; ++j)
        {
            if (al_get_pixel(sprite, i, j).a != 0)
            {
                // Rotate around (x,y) by angle
                double rely = j - spriteoffset_y;
                double rotx = x + cosa*relx - sina*rely;
                double roty = y + sina*relx + cosa*rely;
                if (target.collides(state, rotx, roty))
                {
                    return true;
                }
            }
        }
    }
    return false;
}