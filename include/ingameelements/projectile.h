#pragma once

#include "datastructures.h"
#include "gamestate.h"

class Projectile : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_);
        virtual ~Projectile() override = default;
        virtual bool isrootobject() override {return true;}
        virtual bool isowner(EntityPtr potential_owner) {return potential_owner == owner;}
        virtual double dealdamage(Gamestate &state, Entity &target) = 0;
        virtual PenetrationLevel penetration() = 0;
        virtual std::string spriteid() = 0;
        virtual bool checkcollision(Gamestate &state, Entity &target);
        virtual void midstep(Gamestate &state, double frametime) override;
        EntityPtr owner;
        Team team;
    protected:
    private:
};

