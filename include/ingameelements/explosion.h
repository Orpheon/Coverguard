#pragma once

#include <ingameelements/movingentity.h>
#include "animation.h"


class Explosion : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, std::string animationpath, double direction_);
        virtual ~Explosion() override = default;
        void beginstep(Gamestate &state, double frametime) override {}
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override {}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Explosion(*this));}
        void render(Renderer &renderer, Gamestate &state) override;
        bool isrootobject() override {return true;}

        Animation explosionanim;
        double direction;
    protected:
    private:
};

