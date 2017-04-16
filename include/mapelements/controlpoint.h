#pragma once

#include "entity.h"
#include "datastructures.h"
#include "timer.h"
#include "ingameelements/gamemodes/gamemodemanager.h"

class ControlPoint : public Entity
{
    public:
        virtual void init(uint64_t id_, Rect area_, Team owningteam_, std::function<void(Gamestate &state, Team winningteam)> capturefunc_);
        virtual ~ControlPoint() override = default;
        virtual void render(Renderer &renderer, Gamestate &state) override;

        virtual bool isinside(double x, double y) { return (x > area.x and x < area.x + area.w and y > area.y and y < area.y + area.h); }
        virtual std::unique_ptr<Entity> clone() override { return std::unique_ptr<Entity>(new ControlPoint(*this)); }
        virtual bool isrootobject() override { return true; }
        virtual void beginstep(Gamestate &state, double frametime) override;
        virtual void midstep(Gamestate &state, double frametime) override {};
        virtual void endstep(Gamestate &state, double frametime) override {}
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        virtual void capture(Gamestate &state) {capturefunc(state, cappingteam);}
        virtual void serialize(WriteBuffer &buffer, Gamestate &state) {};
        virtual void deserialize(ReadBuffer &buffer, Gamestate &state) {};

        std::function<void(Gamestate &state, Team winningteam)> capturefunc;
        Rect area;
        Timer capamount;
        Timer capfalloff;
        Team owningteam;
        Team cappingteam;
        const int MAXCAP = 100;
        const double CAPRATE = 5;
        const double CAPDEGEN = 5;
};
