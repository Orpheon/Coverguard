#pragma once

#include "ingameelements/character.h"
#include "renderer.h"
#include "datastructures.h"
#include "ingameelements/weapons/peacemaker.h"

#include <string>

class Mccree : public Character
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Mccree() override = default;
        void midstep(Gamestate &state, double frametime) override;
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate &state) override;
        Rect getstandingcollisionrect(Gamestate &state) override;
        std::string currentsprite(Gamestate &state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Mccree(*this));}
        bool cangetinput(Gamestate &state) override {return not rollanim.active() and not ultcooldown.active and Character::cangetinput(state);}
        bool canuseweapons(Gamestate &state) override {return not ulting.active and Character::canuseweapons(state);}
        bool canuseabilities(Gamestate &state) override {return not ulting.active and Character::canuseabilities(state);}
        void useability1(Gamestate &state) override;
        void useability2(Gamestate &state) override;
        void useultimate(Gamestate &state) override;
        void interrupt(Gamestate &state) override;
        void resetafterult(Gamestate &state);
        void drawhud(Renderer &renderer, Gamestate &state) override;
        bool weaponvisible(Gamestate &state) override;

        Health maxhp() override {return Health(200, 0, 0);}
        double passiveultcharge() override {return 20;}//0.4166666666666667;}
        Heroclass heroclass() override {return MCCREE;}
        std::string herofolder() override {return "heroes/mccree/";}
        double runpower() override {return 1.8;}
        EntityPtr constructweapon(Gamestate &state) {return state.make_entity<Peacemaker>(state, owner);}
        double maxhspeed(Gamestate &state) override {return ulting.active ? 20 : Character::maxhspeed(state);}

        Animation rollanim;
        Animation flashbanganim;
        Timer rollcooldown;
        Timer flashbangcooldown;
        LoopAnimation ultwalkanim;
        LoopAnimation fallanim;
        Timer ulting;
        Timer ultcooldown;
    protected:
    private:
};

