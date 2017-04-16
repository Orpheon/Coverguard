#include "ingameelements/movingentity.h"
#include "gamestate.h"
#include "entity.h"
#include "engine.h"

#include <cmath>

void MovingEntity::init(uint64_t id_, Gamestate &state)
{
    Entity::init(id_);
    x = 0;
    y = 0;
    hspeed = 0;
    vspeed = 0;
}

void MovingEntity::endstep(Gamestate &state, double frametime)
{
    x += hspeed*frametime;
    y += vspeed*frametime;
}

void MovingEntity::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    MovingEntity &prev_e = static_cast<MovingEntity&>(prev_entity);
    MovingEntity &next_e = static_cast<MovingEntity&>(next_entity);

    x = prev_e.x + alpha*(next_e.x - prev_e.x);
    y = prev_e.y + alpha*(next_e.y - prev_e.y);
    hspeed = prev_e.hspeed + alpha*(next_e.hspeed - prev_e.hspeed);
    vspeed = prev_e.vspeed + alpha*(next_e.vspeed - prev_e.vspeed);
}

void MovingEntity::serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate)
{
    buffer.write<double>(x);
    buffer.write<double>(y);
    buffer.write<float>(hspeed);
    buffer.write<float>(vspeed);
}

void MovingEntity::deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate)
{
    x = buffer.read<double>();
    y = buffer.read<double>();
    hspeed = buffer.read<float>();
    vspeed = buffer.read<float>();
}
