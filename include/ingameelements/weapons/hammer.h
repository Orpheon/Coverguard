#pragma once

#include "ingameelements/weapon.h"
#include "ingameelements/weapons/reinhardtshield.h"

class Hammer : public Weapon
{
    public:
        constexpr static double DAMAGE = 70;

        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Hammer() override = default;

        void renderbehind(Renderer &renderer, Gamestate &state);
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Hammer(*this));}

        void fireprimary(Gamestate &state) override;
        void wantfireprimary(Gamestate &state) override;
        void firesecondary(Gamestate &state) override;
        void wantfiresecondary(Gamestate &state) override;
        void beginstep(Gamestate &state, double frametime) override;
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override;
        void destroy(Gamestate &state) override;

        double getattachpoint_x() override {return 0;}
        double getattachpoint_y() override {return 8;}
        std::string herofolder() {return "heroes/reinhardt/";}

        ReinhardtShield& barrier(Gamestate &state);

        EntityPtr barrierptr;

    protected:
    private:
};

