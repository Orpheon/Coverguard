#include "ingameelements/weapon.h"
#include "ingameelements/character.h"
#include "gamestate.h"
#include "engine.h"

#include <cmath>

void Weapon::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::WEAPON;
    owner = owner_;
    aimdirection = 0;
    team = state.get<Player>(owner).team;
    firinganim.init(herofolder()+"firing/");
    firinganim.active(false);
}

void Weapon::beginstep(Gamestate &state, double frametime)
{
    // FIXME: Why is this method here
    ;
}

void Weapon::midstep(Gamestate &state, double frametime)
{
    firinganim.update(state, frametime);
}

void Weapon::endstep(Gamestate &state, double frametime)
{
    Character &c = state.get<Player>(owner).getcharacter(state);
    double xoffset=0, yoffset=0;
    if (c.weaponvisible(state))
    {
        xoffset = state.engine.maskloader.getweaponoffset_x(c.currentsprite(state, false));
        yoffset = state.engine.maskloader.getweaponoffset_y(c.currentsprite(state, false));
    }
    x = c.x + xoffset*(c.isflipped ? -1:1);
    y = c.y + yoffset;
}

void Weapon::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Weapon &prev_e = static_cast<Weapon&>(prev_entity);
    Weapon &next_e = static_cast<Weapon&>(next_entity);

    firinganim.interpolate(prev_e.firinganim, next_e.firinganim, alpha);

    // Angles need some care when interpolating
    if (std::abs(prev_e.aimdirection - next_e.aimdirection) > PI)
    {
        double offset = next_e.aimdirection < prev_e.aimdirection ? 2*PI : -2*PI;
        aimdirection = prev_e.aimdirection + alpha * (next_e.aimdirection - prev_e.aimdirection + offset);
    }
    else
    {
        aimdirection = prev_e.aimdirection + alpha * (next_e.aimdirection - prev_e.aimdirection);
    }
}

void Weapon::serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate)
{

}

void Weapon::deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate)
{

}

void Weapon::setaim(double x_, double y_)
{
    aimdirection = std::atan2(y_-y, x_-x);
}
