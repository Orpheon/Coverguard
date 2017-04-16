#pragma once

#include "networking/networker.h"


class ClientNetworker : public Networker
{
    public:
        ClientNetworker(WriteBuffer &sendbuffer_);
        virtual ~ClientNetworker() override;
        void receive(Gamestate &state) override;
        void sendeventdata(Gamestate &state) override;
        void sendinput(InputContainer heldkeys, float mouse_x, float mouse_y);
        bool isconnected() {return connected;}
    protected:
    private:
        ENetPeer *server;
        bool connected;
};

