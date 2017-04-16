#pragma once

#include <string>
#include <functional>
#include "networking/buffer.h"

class Gamestate;

enum Heroclass {ANA,
                BASTION,
                DVA,
                GENJI,
                HANZO,
                JUNKRAT,
                LUCIO,
                MCCREE,
                MEI,
                MERCY,
                PHARAH,
                REAPER,
                REINHARDT,
                ROADHOG,
                SOMBRA,
                SOLDIER76,
                SYMMETRA,
                TORBJORN,
                TRACER,
                WIDOWMAKER,
                WINSTON,
                ZARYA,
                ZENYATTA};

enum EVENTTYPE {SERVER_SNAPSHOTUPDATE,
                SERVER_FULLUPDATE,
                CLIENT_INPUT,
                PLAYER_JOINED,
                PLAYER_LEFT,
                PLAYER_CHANGECLASS,
                PLAYER_SPAWNED,
                PLAYER_DIED,
                PRIMARY_FIRED,
                SECONDARY_FIRED,
                ABILITY1_USED,
                ABILITY2_USED,
                ULTIMATE_USED };

enum PenetrationLevel { PENETRATE_NOTHING = 0,
                        PENETRATE_CHARACTER = 1,
                        PENETRATE_SHIELD = 2,
                        PENETRATE_WALLMASK = 4};

enum class ENTITYTYPE { PLAYER,
                        CHARACTER,
                        WEAPON,
                        PROJECTILE,
                        EXPLOSION };

enum Team { TEAM1,
            TEAM2,
            SPECTATOR,
            NO_TEAM };

enum class Gamemode {ASSAULT,
                     ESCORT,
                     HYBRID,
                     CONTROL,
                     ARCADE,
                     EVENT };

struct ReducedInputContainer
{
    public:
        bool LEFT;
        bool RIGHT;
        bool CROUCH;
        bool PRIMARY_FIRE;
        bool SECONDARY_FIRE;
        bool ABILITY_1;
        bool ABILITY_2;

        void serialize(WriteBuffer &buffer)
        {
            uint8_t d = 0;
            d |= LEFT<<0;
            d |= RIGHT<<1;
            d |= CROUCH<<2;
            d |= PRIMARY_FIRE<<3;
            d |= SECONDARY_FIRE<<4;
            d |= ABILITY_1<<5;
            d |= ABILITY_2<<6;
            buffer.write<uint8_t>(d);
        }

        void deserialize(ReadBuffer &buffer)
        {
            uint8_t d = buffer.read<uint8_t>();
            LEFT = d & 1<<0;
            RIGHT = d & 1<<1;
            CROUCH = d & 1<<2;
            PRIMARY_FIRE = d & 1<<3;
            SECONDARY_FIRE = d & 1<<4;
            ABILITY_1 = d & 1<<5;
            ABILITY_2 = d & 1<<6;
        }

        void reset()
        {
            LEFT = false;
            RIGHT = false;
            CROUCH = false;
            PRIMARY_FIRE = false;
            SECONDARY_FIRE = false;
            ABILITY_1 = false;
            ABILITY_2 = false;
        }
};

struct InputContainer : public ReducedInputContainer
{
    bool JUMP;
    bool RELOAD;
    bool ULTIMATE;

    void serialize(WriteBuffer &buffer)
    {
        ReducedInputContainer::serialize(buffer);
        uint8_t d = 0;
        d |= JUMP<<0;
        d |= RELOAD<<1;
        d |= ULTIMATE<<2;
        buffer.write<uint8_t>(d);
    }

    void deserialize(ReadBuffer &buffer)
    {
        ReducedInputContainer::deserialize(buffer);
        uint8_t d = buffer.read<uint8_t>();
        JUMP = d & 1<<0;
        RELOAD = d & 1<<1;
        ULTIMATE = d & 1<<2;
    }

    void reset()
    {
        ReducedInputContainer::reset();
        JUMP = false;
        RELOAD = false;
        ULTIMATE = false;
    }

    ReducedInputContainer reduce()
    {
        ReducedInputContainer i;
        i.LEFT = LEFT;
        i.RIGHT = RIGHT;
        i.CROUCH = CROUCH;
        i.PRIMARY_FIRE = PRIMARY_FIRE;
        i.SECONDARY_FIRE = SECONDARY_FIRE;
        i.ABILITY_1 = ABILITY_1;
        i.ABILITY_2 = ABILITY_2;
        return i;
    }

    void update(ReducedInputContainer r)
    {
        LEFT = r.LEFT;
        RIGHT = r.RIGHT;
        CROUCH = r.CROUCH;
        PRIMARY_FIRE = r.PRIMARY_FIRE;
        SECONDARY_FIRE = r.SECONDARY_FIRE;
        ABILITY_1 = r.ABILITY_1;
        ABILITY_2 = r.ABILITY_2;
    }
};

struct Health
{
    Health() : normal(0), armor(0), shields(0) {}
    Health(double n, double a, double s) : normal(n), armor(a), shields(s) {}

    double normal;
    double armor;
    double shields;

    double total()
    {
        return normal + armor + shields;
    }
};

struct EntityPtr
{
    EntityPtr(uint64_t id_) : id(id_) {}
    EntityPtr() : id(0) {}
    operator int() const { return id; }
    uint64_t id;
};

struct Rect
{
    double x;
    double y;
    double w;
    double h;

    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(double x_, double y_, double w_, double h_) : x(x_), y(y_), w(w_), h(h_) {}
    Rect offset(double x_, double y_) {return Rect(x+x_, y+y_, w, h);}
};
