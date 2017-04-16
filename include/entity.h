#pragma once

#include <string>
#include <memory>
#include <vector>
#include <allegro5/allegro.h>
#include "networking/buffer.h"
#include "datastructures.h"

// Circular dependency
class Gamestate;
class Renderer;

class Entity
{
    public:
        virtual ~Entity() = default;
        virtual void init(uint64_t id_);
        virtual void beginstep(Gamestate &state, double frametime) = 0;
        virtual void midstep(Gamestate &state, double frametime) = 0;
        virtual void endstep(Gamestate &state, double frametime) = 0;
        virtual void render(Renderer &renderer, Gamestate &state) = 0;
        virtual bool isrootobject() = 0;
        virtual std::unique_ptr<Entity> clone() = 0;
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) = 0;
        virtual void destroy(Gamestate &state) {destroyentity = true;}
        virtual bool collides(Gamestate &state, double testx, double testy) {return false;}
        virtual double damage(Gamestate &state, double amount) {return amount;};
        virtual bool damageableby(Team projectile_team) {return false;}
        virtual double maxdamageabledist(Gamestate &state, double *centerx, double *centery) {return 0;}
        virtual bool blocks(PenetrationLevel penlevel) {return false;}
        virtual bool isowner(EntityPtr potential_owner) {return false;}
        ENTITYTYPE entitytype;
        uint64_t id = 0;
        bool destroyentity = false;
        bool inited = false;
};
