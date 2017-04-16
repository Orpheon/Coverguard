#pragma once

#include <allegro5/allegro.h>

#include "datastructures.h"
#include "renderer.h"
#include "engine.h"
#include "json.hpp"

class InputCatcher
{
    public:
        InputCatcher(ALLEGRO_DISPLAY *display);
        ~InputCatcher();
        void run(ALLEGRO_DISPLAY *display, Gamestate &state, Networker &networker, Renderer &renderer, EntityPtr myself);
    private:
        ALLEGRO_EVENT_QUEUE *event_queue;
        nlohmann::json config;
};

