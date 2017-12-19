#pragma once

#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "spriteloader.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"

#include <libsocket/inetclientstream.hpp>


class Lobbymenu : public Menu
{
    public:
        Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_);
        virtual ~Lobbymenu() = default;
        void run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue) override;
        void refreshservers();
        void quit();

        MenuLoopAnimation background;
        Spriteloader spriteloader;

        std::vector<ServerData> servers;
        libsocket::inet_stream lobbysocket;
    protected:
    private:
        int N_SERVERS_TO_DISPLAY = 15;
        double REFRESH_PERIOD = 50;

        ALLEGRO_FONT *serverfont;
        int scrolloffset = 0;
        int selection = -1;
        // Can't use Timer object because of hardcoded Gamestate argument in trigger function
        double refreshtimer = REFRESH_PERIOD;
        bool sentrequest = false;
};