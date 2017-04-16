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
        void midstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate &state) override;
        Rect getstandingcollisionrect(Gamestate &state) override;
        std::string currentsprite(Gamestate &state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Reinhardt(*this));}
        bool cangetinput(Gamestate &state) override;
        void useability1(Gamestate &state) override;
        void useability2(Gamestate &state) override;
        void useultimate(Gamestate &state) override;
        void begincharge() {chargeanim.reset();}
        void endcharge() {chargeanim.active(false); endchargeanim.reset();}
        void interrupt(Gamestate &state) override;
        void drawhud(Renderer &renderer, Gamestate &state) override;
        bool weaponvisible(Gamestate &state) override;

        double passiveultcharge() override {return 100*0.4166666666666667;}
        Heroclass heroclass() override {return REINHARDT;}
        double runpower() override {return 1.8;}
        Health maxhp() override {return Health(300, 200, 0);}
        std::string herofolder() override {return "heroes/reinhardt/";}
        EntityPtr constructweapon(Gamestate &state) override {return state.make_entity<Hammer>(state, owner);}
        double maxhspeed(Gamestate &state) override {return state.get<Hammer>(weapon).barrier(state).active ? 60.0 : Character::maxhspeed(state);}

        Animation chargeanim;
        Animation preparechargeanim;
        Animation endchargeanim;
        Animation earthshatteranim;
        LoopAnimation shieldrunanim;
    protected:
    private:
};

