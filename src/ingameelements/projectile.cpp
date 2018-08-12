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

void Projectile::beginstep(Gamestate &state, double frametime)
{
    if (not (penetration() & PENETRATE_WALLMASK)
        and state.currentmap->collides(state, x, y, spriteid(), std::atan2(vspeed, hspeed)))
    {
        destroy(state);
    }
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        if (not entity.destroyentity)
        {
            if (entity.damageableby(team) and checkcollision(state, entity))
            {
                double effectivedamage = dealdamage(state, entity);
                state.get<Player&>(owner).registerdamage(state, effectivedamage);
                if (entity.blocks(penetration()))
                {
                    destroy(state);
                    break;
                }
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
    sf::Image &image = state.engine.maskloader.loadmask(spriteid());
    sf::Vector2i offset = state.engine.maskloader.offsets(spriteid());
    double cosa = std::cos(angle), sina = std::sin(angle);
    for (int i = 0; i < bbox.w; ++i)
    {
        double relx = i - offset.x;
        for (int j = 0; j < bbox.h; ++j)
        {
            // FIXME: If this needs to be optimized, replace this with image.getPixelsPtr() stuff
            if (image.getPixel(i, j).a != 0)
            {
                // Rotate around (x,y) by angle
                double rely = j - offset.y;
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