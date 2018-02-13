#pragma once
#include "datastructures.h"
#include "entity.h"

class GamemodeManager : public Entity
{
    public:
        virtual void init(uint64_t id_, std::vector<Rect> &spawnareas1, std::vector<Rect> &spawnareas2);
        virtual void activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc);
        virtual void beginstep(Gamestate &state, double frametime) override {};
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override {};
        virtual void serializefull(WriteBuffer &buffer, Gamestate &state) = 0;
        virtual void deserializefull(ReadBuffer &buffer, Gamestate &state) = 0;
        virtual void destroy(Gamestate &state) override;
        std::vector<std::vector<Rect>> spawnareas;
        std::vector<EntityPtr> spawnrooms;
        bool active = false;
};