#pragma once

#include "timer.h"
#include "ingameelements/shield.h"

class ReinhardtShield : public Shield
{
    public:
        void init(uint64_t id_, Gamestate &state, Team team_, EntityPtr owner_);
        virtual ~ReinhardtShield() override = default;
        void beginstep(Gamestate &state, double frametime) override {};
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new ReinhardtShield(*this));}
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        bool collides(Gamestate &state, double testx, double testy) override;
        double maxdamageabledist(Gamestate &state, double *centerx, double *centery) override;
        bool damageableby(Team projectile_team) override {return team != projectile_team and active;}
        void shieldbreak(Gamestate &state) override {brokencooldown.reset();}
        bool isrootobject() {return false;}
        std::string spritestr();

        double attachpoint_x() {return 0;}
        double attachpoint_y() {return 0;}

        EntityPtr owner;
        bool active;
        Timer rechargecooldown;
        Timer brokencooldown;
        double aimdirection;
        double SHIELD_RECHARGE = 195;
        double SHIELD_MAX_HP = 2000;
        double DIST_TO_REINHARDT = 30;
};