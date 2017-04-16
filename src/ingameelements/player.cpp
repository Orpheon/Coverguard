#include "ingameelements/player.h"
#include "gamestate.h"
#include "datastructures.h"
#include "ingameelements/character.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/heroes/reinhardt.h"
#include "engine.h"
#include "mapelements/spawnroom.h"
#include "global.h"

void Player::init(uint64_t id_, Gamestate &state)
{
    Entity::init(id_);

    entitytype = ENTITYTYPE::PLAYER;
    character = 0;
    heroclass = REINHARDT;
    spawntimer.init(4, std::bind(&Player::spawn, this, std::placeholders::_1));
    spawntimer.active = false;
    // Spawn a character asap
    spawntimer.timer = spawntimer.duration;
    ultcharge.init(100);

    int teambalance = 0;
    for (auto &pptr : state.playerlist)
    {
        if (pptr != EntityPtr(id))
        {
            Player &p = state.get<Player>(pptr);
            if (p.team == TEAM1)
            {
                ++teambalance;
            }
            else if (p.team == TEAM2)
            {
                --teambalance;
            }
        }
    }
    if (teambalance > 0)
    {
        team = TEAM2;
    }
    else
    {
        team = TEAM1;
    }
}

void Player::beginstep(Gamestate &state, double frametime)
{
    if (state.exists(character))
    {
        state.get<Character>(character).beginstep(state, frametime);
    }
}

void Player::midstep(Gamestate &state, double frametime)
{
    spawntimer.update(state, frametime);
    if (state.exists(character))
    {
        state.get<Character>(character).midstep(state, frametime);
    }
}

void Player::endstep(Gamestate &state, double frametime)
{
    if (state.exists(character))
    {
        state.get<Character>(character).endstep(state, frametime);
    }
}

void Player::render(Renderer &renderer, Gamestate &state)
{
    if (state.exists(character))
    {
        state.get<Character>(character).render(renderer, state);
    }
}

void Player::spawn(Gamestate &state)
{
    spawntimer.active = false;
    if (state.exists(character))
    {
        // We already have a character, error and respawn
        Global::logging().panic(__FILE__, __LINE__, "Player tried to spawn character that was already alive");
    }
    if (heroclass == MCCREE)
    {
        character = state.make_entity<Mccree>(state, EntityPtr(id));
    }
    else if (heroclass == REINHARDT)
    {
        character = state.make_entity<Reinhardt>(state, EntityPtr(id));
    }
    else
    {
        Global::logging().panic(__FILE__, __LINE__, "Player tried to spawn character with invalid class %i", heroclass);
    }
    Character &c = state.get<Character>(character);
    Spawnroom &spawn = state.currentmap->spawnroom(state, team);
    do
    {
        c.x = spawn.area.x + spawn.area.w*(rand()/(RAND_MAX+1.0));
        c.y = spawn.area.y + spawn.area.h*(rand()/(RAND_MAX+1.0));
    }
    while (state.currentmap->collides(c.getcollisionrect(state)));

    if (state.engine.isserver)
    {
        state.engine.sendbuffer.write<uint8_t>(PLAYER_SPAWNED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(EntityPtr(id)));
    }
}

void Player::changeclass(Gamestate &state, Heroclass newclass)
{
    heroclass = newclass;
    if (state.exists(character))
    {
        getcharacter(state).die(state);
    }

    ultcharge.reset();
}

Character& Player::getcharacter(Gamestate &state)
{
    return state.get<Character>(character);
}

void Player::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Player &prev_e = static_cast<Player&>(prev_entity);
    Player &next_e = static_cast<Player&>(next_entity);

    ultcharge.timer = prev_e.ultcharge.timer + alpha*(next_e.ultcharge.timer - prev_e.ultcharge.timer);

    if (alpha < 0.5)
    {
        character = prev_e.character;
    }
    else
    {
        character = next_e.character;
    }
}

void Player::serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate)
{
    if (fullupdate)
    {
        buffer.write<bool>(state.exists(character));
    }
    buffer.write<uint16_t>(ultcharge.timer*65536/100.0);
    if (state.exists(character))
    {
        Character &c = state.get<Character>(character);
        c.serialize(state, buffer, fullupdate);
    }
}

void Player::deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate)
{
    if (fullupdate)
    {
        bool hascharacter = buffer.read<bool>();
        if (hascharacter)
        {
            spawn(state);
        }
    }
    ultcharge.timer = 100*buffer.read<uint16_t>()/65536.0;
    if (state.exists(character))
    {
        Character &c = state.get<Character>(character);
        c.deserialize(state, buffer, fullupdate);
    }
}

void Player::destroy(Gamestate &state)
{
    if (state.exists(character))
    {
        state.get<Character>(character).destroy(state);
    }
    destroyentity = true;
}
