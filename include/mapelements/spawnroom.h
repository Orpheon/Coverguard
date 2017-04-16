#pragma once

#include "entity.h"

class Spawnroom : public Entity
{
    public:
        virtual void init(uint64_t id_, Rect area_, Team team_);
        virtual ~Spawnroom() override = default;
        void beginstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        bool isinside(double x, double y) {return (x > area.x and x < area.x+area.w and y > area.y and y < area.y+area.h);}

        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Spawnroom(*this));}
        bool isrootobject() override {return true;}
        void midstep(Gamestate &state, double frametime) override {}
        void endstep(Gamestate &state, double frametime) override {}
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override {}

        Rect area;
        Team team;
    protected:
    private:
};
