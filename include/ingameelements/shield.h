#pragma once

#include "ingameelements/movingentity.h"

class Shield : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, double maxhp, Team team_);
        virtual ~Shield() override = default;
        virtual bool isrootobject() override {return true;}
        virtual bool collides(Gamestate &state, double testx, double testy) override = 0;
        virtual double damage(Gamestate &state, double amount) override;
        virtual void shieldbreak(Gamestate &state) {destroy(state);}
        virtual bool damageableby(Team projectile_team) override {return projectile_team != team;}
        virtual double maxdamageabledist(Gamestate &state, double *centerx, double *centery) override = 0;
        virtual bool blocks(PenetrationLevel penlevel) {return penlevel & PENETRATE_SHIELD;}
        virtual bool isowner(EntityPtr potential_owner) {return false;}
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;

        double hp;
        Team team;
};