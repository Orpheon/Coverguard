#pragma once

#include "gamestate.h"
#define boolean enet_boolean
#include <enet/enet.h>
#undef boolean

class Networker
{
    public:
        Networker(bool isserver, WriteBuffer &sendbuffer_);
        virtual ~Networker();
        virtual void receive(Gamestate &state) = 0;
        virtual void sendeventdata(Gamestate &state) = 0;
        WriteBuffer &sendbuffer;
        ENetHost *host;
    private:
};

