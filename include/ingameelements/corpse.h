#pragma once

#include <ingameelements/movingentity.h>
#include "timer.h"

class Corpse : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, std::string herofolder, bool isflipped_);
        virtual ~Corpse() override = default;
        void beginstep(Gamestate &state, double frametime) override {}
        void midstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        bool isrootobject() override {return true;};
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Corpse(*this));}
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
    protected:
    private:
        std::string spritepath;
        Timer countdown;
        bool isflipped;
};

