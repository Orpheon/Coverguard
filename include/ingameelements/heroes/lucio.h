#pragma once

#include "ingameelements/character.h"
#include "ingameelements/weapons/sonicamp.h"
#include "renderer.h"
#include "datastructures.h"

class Lucio : public Character
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Lucio() override = default;
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate &state) override;
        Rect getstandingcollisionrect(Gamestate &state) override;
        std::string currentsprite(Gamestate &state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Lucio(*this));}
        void useability1(Gamestate &state) override;
        void useability2(Gamestate &state) override;
        void useultimate(Gamestate &state) override;
        void interrupt(Gamestate &state) override;

        Health initializehealth() override {return Health(200, 0, 0);}
        double passiveultcharge() override {return 20;}//0.4166666666666667;}
        Heroclass heroclass() override {return LUCIO;}
        std::string herofolder() override {return "heroes/lucio/";}
        double runpower() override {return 1.8;}
        EntityPtr constructweapon(Gamestate &state) {return state.make_entity<Sonicamp>(state, owner);}
    protected:
    private:
};