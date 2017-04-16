#pragma once

#include "networking/networker.h"
#include <vector>


class ServerNetworker : public Networker
{
    public:
        ServerNetworker(WriteBuffer &sendbuffer_);
        virtual ~ServerNetworker() override;
        void receive(Gamestate &state) override;
        void sendeventdata(Gamestate &state) override;
        void sendframedata(Gamestate &state);
    protected:
    private:
};

