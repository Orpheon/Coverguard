#include "networking/servernetworker.h"
#include "global_constants.h"
#include "global.h"

ServerNetworker::ServerNetworker(WriteBuffer &sendbuffer_) : Networker(true, sendbuffer_)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 3224; // 3223-3230
    host = enet_host_create(&address, PLAYER_LIMIT, 1, 0, 0);
    if (host == NULL)
    {
        Global::logging().panic(__FILE__, __LINE__, "Failed to create server host");
        throw -1;
    }
}

ServerNetworker::~ServerNetworker()
{
    //dtor
}

void ServerNetworker::receive(Gamestate &state)
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0))
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            EntityPtr player = state.addplayer();
            event.peer->data = std::malloc(sizeof(int));
            reinterpret_cast<int*>(event.peer->data)[0] = state.playerlist[state.playerlist.size()-1];

            enet_peer_timeout(event.peer, 0, 2000, 3000);

            // Send full update
            WriteBuffer frame = WriteBuffer();
            frame.write<uint8_t>(SERVER_FULLUPDATE);
            state.serializefull(frame);
            ENetPacket *eventpacket = enet_packet_create(frame.getdata(), frame.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, eventpacket);

            // Set the spawn timer for the new player so that they will spawn at next opportunity
            state.get<Player>(player).spawntimer.timer = state.get<Player>(player).spawntimer.duration;
            state.get<Player>(player).spawntimer.active = true;

            // Tell everyone except the new player that a new player joined
            WriteBuffer tmpbuffer;
            tmpbuffer.write<uint8_t>(PLAYER_JOINED);
            eventpacket = enet_packet_create(tmpbuffer.getdata(), tmpbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
            for (unsigned int i=0; i<host->connectedPeers; ++i)
            {
                if (&(host->peers[i]) != event.peer)
                {
                    enet_peer_send(&(host->peers[i]), 0, eventpacket);
                }
            }

            enet_host_flush(host);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            int i = state.findplayerid(reinterpret_cast<int*>(event.peer->data)[0]);
            state.removeplayer(i);
            std::free(event.peer->data);

            // Send disconnect event of that player
            sendbuffer.write<uint8_t>(PLAYER_LEFT);
            sendbuffer.write<uint8_t>(i);
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            while (data.length() > 0)
            {
                int eventtype = data.read<uint8_t>();
                Player &player = state.get<Player>(reinterpret_cast<int*>(event.peer->data)[0]);
                if (eventtype == CLIENT_INPUT)
                {
                    InputContainer heldkeys;
                    heldkeys.deserialize(data);
                    double mouse_x = data.read<int16_t>();
                    double mouse_y = data.read<int16_t>();
                    if (state.exists(player.character))
                    {
                        player.getcharacter(state).setinput(state, heldkeys, mouse_x, mouse_y);
                    }
                }
                else if (eventtype == PLAYER_CHANGECLASS)
                {
                    Heroclass newclass = static_cast<Heroclass>(data.read<uint8_t>());
                    player.changeclass(state, newclass);
                    sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                    sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                    sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                }
                else
                {
                    Global::logging().print(__FILE__, __LINE__, "Invalid packet received on server from player %i: %i",
                                            state.findplayerid(player.id), eventtype);
                }
            }
            enet_packet_destroy(event.packet);
        }
    }
}

void ServerNetworker::sendeventdata(Gamestate &state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(host, 0, eventpacket);
        enet_host_flush(host);
        sendbuffer.reset();
    }
}

void ServerNetworker::sendframedata(Gamestate &state)
{
    WriteBuffer frame = WriteBuffer();
    frame.write<uint8_t>(SERVER_SNAPSHOTUPDATE);
    state.serializesnapshot(frame);
    ENetPacket *framepacket = enet_packet_create(frame.getdata(), frame.length(), 0);
    enet_host_broadcast(host, 0, framepacket);
    enet_host_flush(host);
}
