#include "movingentity.h"
#include "gamestate.h"
#include "entity.h"

MovingEntity::MovingEntity(uint64_t id_, Gamestate *state) : Entity(id_)
{
    x = 0;
    y = 0;
    hspeed = 0;
    vspeed = 0;
}

void MovingEntity::endstep(Gamestate *state, double frametime)
{
    x += hspeed*frametime;
    y += vspeed*frametime;
}

void MovingEntity::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    MovingEntity *prev_e = static_cast<MovingEntity*>(prev_entity);
    MovingEntity *next_e = static_cast<MovingEntity*>(next_entity);

    x = prev_e->x + alpha*(next_e->x - prev_e->x);
    y = prev_e->y + alpha*(next_e->y - prev_e->y);
    hspeed = prev_e->hspeed + alpha*(next_e->hspeed - prev_e->hspeed);
    vspeed = prev_e->vspeed + alpha*(next_e->vspeed - prev_e->vspeed);
}
