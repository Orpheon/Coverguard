#pragma once

#include "ingameelements/gamemodes/gamemodemanager.h"
#include "timer.h"

class KothManager : public GamemodeManager
{
    public:
        virtual void init(uint64_t id_, Rect spawnarea1, Rect spawnarea2, Rect cparea);
        virtual void activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc_) override;
        virtual void serialize(WriteBuffer &buffer, Gamestate &state) override {}
        virtual void deserialize(ReadBuffer &buffer, Gamestate &state) override {}
        virtual void beginstep(Gamestate &state, double frametime) override;
        virtual void midstep(Gamestate &state, double frametime) override {};
        virtual void endstep(Gamestate &state, double frametime) override {};
        virtual void render(Renderer &renderer, Gamestate &state) override {};
        virtual bool isrootobject() override {return true;}
        virtual std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new KothManager(*this));}
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        virtual void destroy(Gamestate &state) override;
        virtual void createpoint(Gamestate &state, Team owningteam);
        virtual void win(Gamestate &state);

        Timer unlocktimer;
        std::vector<Timer> teamcounters;
        Rect area;
        EntityPtr controlpoint;
        std::function<void(Gamestate &state, Team winners)> winfunc;
};