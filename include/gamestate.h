#pragma once

#include <list>
#include <unordered_map>
#include <memory>

#include "ingameelements/map.h"
#include "datastructures.h"
#include "ingameelements/player.h"
#include "ingameelements/character.h"
#include "ingameelements/gamemodes/gamemodemanager.h"

// Circular reference
class Engine;

class Gamestate
{
    public:
        Gamestate(Engine &engine_);
        ~Gamestate();

        template<class EntityT, class ...Args> EntityPtr make_entity(Args&& ...args)
        {
            uint64_t id = entityidcounter++;
            entitylist[id] = std::unique_ptr<Entity>(new EntityT());
            static_cast<EntityT&>(*entitylist.at(id)).init(id, std::forward<Args>(args)...);
            return EntityPtr(id);
        }

        template<class EntityT> EntityT& get(EntityPtr e)
        {
            if (not exists(e))
            {
                Global::logging().panic(__FILE__, __LINE__, "EntityPtr %i requested, but no such entity exists.", e.id);
            }
            return static_cast<EntityT&>(*(entitylist.at(e.id)));
        }

        bool exists(EntityPtr e)
        {
            if (entitylist.count(e.id) == 0 or entitylist.at(e.id)->destroyentity)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        void update(double frametime);
        std::unique_ptr<Gamestate> clone();
        void interpolate(Gamestate &prevstate, Gamestate &nextstate, double alpha);

        EntityPtr addplayer();
        void removeplayer(int playerid);
        Player& findplayer(int playerid);
        int findplayerid(EntityPtr player);

        void serializesnapshot(WriteBuffer &buffer);
        void deserializesnapshot(ReadBuffer &buffer);
        void serializefull(WriteBuffer &buffer);
        void deserializefull(ReadBuffer &buffer);

        EntityPtr collidelinetarget(Gamestate &state, double x1, double y1, MovingEntity &target, Team team,
                                    PenetrationLevel penlevel, double *collisionptx, double *collisionpty);

        EntityPtr collidelinedamageable(Gamestate &state, double x1, double y1, double x2, double y2, Team team,
                                        double *collisionptx, double *collisionpty);

        std::unordered_map<uint64_t, std::unique_ptr<Entity>> entitylist;
        std::vector<EntityPtr> playerlist;

        // Make gamestate move-assigneable, so that " = " doesn't copy but move.
        Gamestate & operator=(Gamestate &&)=default;

        double time;
        std::shared_ptr<Map> currentmap;
        Engine &engine;
    protected:
    private:
        uint64_t entityidcounter;
};

