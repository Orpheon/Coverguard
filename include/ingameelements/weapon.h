#pragma once

#include "ingameelements/movingentity.h"
#include "datastructures.h"
#include "animation.h"


class Weapon : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_);
        virtual ~Weapon() override = default;
        virtual void setaim(double x_, double y_);

        virtual bool isrootobject() override {return false;}
        virtual bool isowner(EntityPtr potential_owner) override {return potential_owner == owner;}

        EntityPtr owner;
        double aimdirection;
        Team team;

        virtual void beginstep(Gamestate &state, double frametime) override;
        virtual void midstep(Gamestate &state, double frametime) override;
        virtual void endstep(Gamestate &state, double frametime) override;
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        virtual void serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate) override;
        virtual void deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate) override;

        virtual void fireprimary(Gamestate &state) = 0;
        virtual void wantfireprimary(Gamestate &state) = 0;
        virtual void firesecondary(Gamestate &state) = 0;
        virtual void wantfiresecondary(Gamestate &state) = 0;
        virtual bool hasclip() {return false;}
        virtual double getattachpoint_x() = 0;
        virtual double getattachpoint_y() = 0;
        virtual std::string herofolder() = 0;

        std::string idlesprite;
        Animation firinganim;
    protected:
    private:
};

