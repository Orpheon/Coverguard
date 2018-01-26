#pragma once

#include "ingameelements/character.h"
#include "renderer.h"
#include "datastructures.h"
#include "ingameelements/weapons/hammer.h"

#include <string>

class Reinhardt : public Character
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Reinhardt() override = default;
        void beginstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate &state) override;
        Rect getstandingcollisionrect(Gamestate &state) override;
        bool collides(Gamestate &state, double testx, double testy) override;
        std::string currentsprite(Gamestate &state, bool mask) override;
        std::string currenttorsosprite(Gamestate &state, bool mask);
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Reinhardt(*this));}
        bool cangetinput(Gamestate &state) override;
        bool canuseweapons(Gamestate &state) override;
        bool canuseabilities(Gamestate &state) override;
        void useability1(Gamestate &state) override;
        void useability2(Gamestate &state) override;
        void useultimate(Gamestate &state) override;
        void begincharge();
        void endcharge(Gamestate &state);
        void interrupt(Gamestate &state) override;
        bool weaponvisible(Gamestate &state) override;
        void createearthshatter(Gamestate &state);

        double passiveultcharge() override {return 100*0.4166666666666667;}
        Heroclass heroclass() override {return REINHARDT;}
        double runpower() override {return 1.8;}
        Health initializehealth() override {return Health(300, 200, 0);}
        std::string herofolder() override {return "heroes/reinhardt/";}
        EntityPtr constructweapon(Gamestate &state) override {return state.make_entity<Hammer>(state, owner);}
        double maxhspeed(Gamestate &state) override {return state.get<Hammer>(weapon).barrier(state).active ? 60.0 : Character::maxhspeed(state);}

        Animation chargeanim;
        Animation preparechargeanim;
        Animation endchargeanim;
        Animation earthshatteranim;
        Timer earthshatterdelay;
        LoopAnimation shieldrunanim;
        Timer chargecooldown;

        EntityPtr pintarget;
        std::list<uint64_t> already_bumped_characters;

        static constexpr double CHARGE_SPEED = 500;
        static constexpr double CHARGE_BUMP_DAMAGE = 50;
        static constexpr double CHARGE_PIN_DAMAGE = 300;
        static constexpr double CHARGE_BUMP_STRENGTH = 500;
        static constexpr double CHARGE_BUMP_XOFFSET = 0;
        static constexpr double CHARGE_BUMP_YOFFSET = 16;
    protected:
    private:
};

