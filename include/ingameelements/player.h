#pragma once

#include <memory>

#define boolean enet_boolean
#include <enet/enet.h>
#undef boolean

#include "entity.h"
#include "datastructures.h"
#include "timer.h"

// Circular dependency
class Gamestate;
class Character;

class Player : public Entity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state);
        virtual ~Player() override = default;
        void beginstep(Gamestate &state, double frametime) override;
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override;
        void clone(Gamestate *oldstate, Gamestate *newstate);
        void spawn(Gamestate &state);
        void changeclass(Gamestate &state, Heroclass newclass);
        bool isrootobject() override {return true;}
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Player(*this));}
        void serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate);
        void deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate);
        void destroy(Gamestate &state) override;

        Character& getcharacter(Gamestate &state);

        EntityPtr character;
        Timer spawntimer;
        Timer ultcharge;
        Team team;
        Heroclass heroclass;
};

