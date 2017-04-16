#pragma once

#include "renderer.h"

class Trail : public Entity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, ALLEGRO_COLOR color_, double x1_, double y1_, double x2_, double y2_, double duration);
        virtual ~Trail() override = default;
        void beginstep(Gamestate &state, double frametime) override {}
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override {}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Trail(*this));}
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        bool isrootobject() override {return true;}

        ALLEGRO_COLOR color;
        double x1;
        double y1;
        double x2;
        double y2;
        Timer countdown;
    protected:
    private:
};

