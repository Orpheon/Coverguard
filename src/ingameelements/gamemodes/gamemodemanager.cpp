
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "mapelements/spawnroom.h"
#include "gamestate.h"

void GamemodeManager::init(uint64_t id_, Rect spawnarea1, Rect spawnarea2)
{
    Entity::init(id_);
    spawnareas.push_back(spawnarea1);
    spawnareas.push_back(spawnarea2);
}

void GamemodeManager::activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc)
{
    spawnrooms.push_back(state.make_entity<Spawnroom>(spawnareas.at(TEAM1), TEAM1));
    spawnrooms.push_back(state.make_entity<Spawnroom>(spawnareas.at(TEAM2), TEAM2));
    active = true;
}

void GamemodeManager::destroy(Gamestate &state)
{
    Entity::destroy(state);
    if (state.exists(spawnrooms.at(TEAM1)))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms.at(TEAM1));
        spawn.destroy(state);
    }
    if (state.exists(spawnrooms.at(TEAM2)))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms.at(TEAM2));
        spawn.destroy(state);
    }
}
