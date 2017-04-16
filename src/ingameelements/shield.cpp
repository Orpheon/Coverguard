
#include "ingameelements/shield.h"

void Shield::init(uint64_t id_, Gamestate &state, double maxhp, Team team_)
{
    MovingEntity::init(id_, state);

    hp = maxhp;
    team = team_;
}

double Shield::damage(Gamestate &state, double amount)
{
    double oldhp = hp;
    hp -= amount;
    if (hp <= 0)
    {
        shieldbreak(state);
    }
    return std::min(amount, oldhp);
}

void Shield::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Shield &prev_e = static_cast<Shield&>(prev_entity);
    Shield &next_e = static_cast<Shield&>(next_entity);

    hp = prev_e.hp + alpha*(next_e.hp - prev_e.hp);
}