#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>

#include "gamestate.h"
#include "entity.h"
#include "ingameelements/player.h"
#include "ingameelements/projectile.h"
#include "mapelements/spawnroom.h"

Gamestate::Gamestate(Engine &engine_) : entitylist(), playerlist(), currentmap(), engine(engine_), entityidcounter(1)
{
    time = 0;
}

Gamestate::~Gamestate()
{
    ;
}

void Gamestate::update(double frametime)
{
    time += frametime;

    for (auto &e : entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->beginstep(*this, frametime);
        }
    }
    for (auto &e : entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->midstep(*this, frametime);
        }
    }
    for (auto &e : entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->endstep(*this, frametime);
        }
    }
    for (auto e = entitylist.begin(); e != entitylist.end();)
    {
        if (e->second->destroyentity)
        {
            e = entitylist.erase(e);
        }
        else
        {
            ++e;
        }
    }
}

EntityPtr Gamestate::addplayer()
{
    EntityPtr r = make_entity<Player>(*this);
    playerlist.push_back(r);
    return r;
}

void Gamestate::removeplayer(int playerid)
{
    for (auto &e : entitylist)
    {
        auto &entity = *(e.second);
        if (entity.isowner(EntityPtr(playerid)))
        {
            entity.destroy(*this);
        }
    }

    Player &player = findplayer(playerid);
    player.destroy(*this);
    playerlist.erase(playerlist.begin()+playerid);
}

Player& Gamestate::findplayer(int playerid)
{
    return get<Player>(playerlist.at(playerid));
}

int Gamestate::findplayerid(EntityPtr player)
{
    return std::find(playerlist.begin(), playerlist.end(), player) - playerlist.begin();
}


std::unique_ptr<Gamestate> Gamestate::clone()
{
    std::unique_ptr<Gamestate> g = std::unique_ptr<Gamestate>(new Gamestate(engine));
    for (auto &e : entitylist)
    {
        g->entitylist[e.second->id] = e.second->clone();
    }
    g->time = time;
    g->entityidcounter = entityidcounter;
    g->currentmap = currentmap;
    g->playerlist = playerlist;
    return g;
}

void Gamestate::interpolate(Gamestate &prevstate, Gamestate &nextstate, double alpha)
{
    // Use threshold of alpha=0.5 to decide binary choices like entity existence
    Gamestate &preferredstate = alpha < 0.5 ? prevstate : nextstate;
    currentmap = preferredstate.currentmap;
    entityidcounter = preferredstate.entityidcounter;
    playerlist = preferredstate.playerlist;
    time = prevstate.time + alpha*(nextstate.time - prevstate.time);

    entitylist.clear();
    for (auto &e : preferredstate.entitylist)
    {
        Entity &entity = *(e.second);
        entitylist[entity.id] = entity.clone();
        if (prevstate.entitylist.count(entity.id) != 0 && nextstate.entitylist.count(entity.id) != 0)
        {
            // If the instance exists in both states, we need to actually interpolate values
            entitylist[entity.id]->interpolate(*prevstate.entitylist.at(entity.id), *nextstate.entitylist.at(entity.id), alpha);
        }
    }
}


void Gamestate::serializesnapshot(WriteBuffer &buffer)
{
    for (auto &p : playerlist)
    {
        get<Player>(p).serialize(*this, buffer, false);
    }
}

void Gamestate::deserializesnapshot(ReadBuffer &buffer)
{
    for (auto &p : playerlist)
    {
        get<Player>(p).deserialize(*this, buffer, false);
    }
}

void Gamestate::serializefull(WriteBuffer &buffer)
{
    buffer.write<uint32_t>(playerlist.size());
    for (auto &p : playerlist)
    {
        get<Player>(p).serialize(*this, buffer, true);
    }
}

void Gamestate::deserializefull(ReadBuffer &buffer)
{
    int nplayers = buffer.read<uint32_t>();
    for (int i=0; i<nplayers; ++i)
    {
        addplayer();
    }
    for (auto p : playerlist)
    {
        get<Player>(p).deserialize(*this, buffer, true);
    }
}

EntityPtr Gamestate::collidelinetarget(Gamestate &state, double x1, double y1, MovingEntity &target, Team team,
                                       PenetrationLevel penlevel, double *collisionptx, double *collisionpty)
{
    int nsteps = std::ceil(std::max(std::abs(x1-target.x), std::abs(y1-target.y)));
    double dx = static_cast<double>(target.x-x1)/nsteps, dy = static_cast<double>(target.y-y1)*1.0/nsteps;
    *collisionptx = x1;
    *collisionpty = y1;
    for (int i=0; i<nsteps; ++i)
    {
        if ((not (penlevel & PENETRATE_WALLMASK)) and (currentmap->testpixel(*collisionptx, *collisionpty) or
            currentmap->spawnroom(state, team).isinside(*collisionptx, *collisionpty)))
        {
            // We hit wallmask or went out of bounds or hit enemy spawnroom
            return EntityPtr(0);
        }
        for (auto &e : entitylist)
        {
            auto &entity = *(e.second);
            if ((entity.id == target.id or entity.blocks(penlevel)) and entity.damageableby(team))
            {
                double enemycenterx=0, enemycentery=0;
                double dist = entity.maxdamageabledist(state, &enemycenterx, &enemycentery);
                if (std::hypot(enemycenterx-*collisionptx, enemycentery-*collisionpty) <= dist)
                {
                    if (entity.collides(state, *collisionptx, *collisionpty))
                    {
                        return EntityPtr(entity.id);
                    }
                }
            }
        }
        *collisionptx += dx; *collisionpty += dy;
    }
    Global::logging().panic(__FILE__, __LINE__, "Entity %i could not be reached at pt %f|%f", target.id, collisionptx, collisionpty);
    return EntityPtr(0);
}


EntityPtr Gamestate::collidelinedamageable(Gamestate &state, double x1, double y1, double x2, double y2,
                                           Team team, double *collisionptx, double *collisionpty)
{
    int nsteps = std::ceil(std::max(std::abs(x1-x2), std::abs(y1-y2)));
    double dx = static_cast<double>(x2-x1)/nsteps, dy = static_cast<double>(y2-y1)*1.0/nsteps;
    *collisionptx = x1;
    *collisionpty = y1;
    Team enemyteam = team == TEAM1 ? TEAM2 : TEAM1;
    for (int i=0; i<nsteps; ++i)
    {
        if (currentmap->testpixel(*collisionptx, *collisionpty) or currentmap->spawnroom(state, enemyteam).isinside(*collisionptx, *collisionpty))
        {
            // We hit wallmask or went out of bounds or hit enemy spawnroom
            return EntityPtr(0);
        }
        for (auto &e : entitylist)
        {
            auto &entity = *(e.second);
            if (entity.damageableby(team))
            {
                double enemycenterx=0, enemycentery=0;
                double dist = entity.maxdamageabledist(state, &enemycenterx, &enemycentery);
                if (std::hypot(enemycenterx-*collisionptx, enemycentery-*collisionpty) <= dist)
                {
                    if (entity.collides(state, *collisionptx, *collisionpty))
                    {
                        return EntityPtr(entity.id);
                    }
                }
            }
        }
        *collisionptx += dx; *collisionpty += dy;
    }
    return EntityPtr(0);
}
