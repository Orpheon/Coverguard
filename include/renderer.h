#pragma once

#include "gamestate.h"
#include "spriteloader.h"
#include "networking/networker.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

class Renderer
{
    public:
        Renderer();
        virtual ~Renderer();
        void render(ALLEGRO_DISPLAY *display, Gamestate &state, EntityPtr myself_, Networker &networker);
        ALLEGRO_DISPLAY* createnewdisplay();
        ALLEGRO_DISPLAY* createnewdisplay(const nlohmann::json &config);
        double cam_x;
        double cam_y;
        double zoom;
        EntityPtr myself;
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *midground;
        ALLEGRO_BITMAP *foreground;
        ALLEGRO_BITMAP *surfaceground;
        Spriteloader spriteloader;
        ALLEGRO_FONT *font20;
        ALLEGRO_FONT *font10;
        ALLEGRO_FONT *font6;
        ALLEGRO_FONT *gg2font;
    protected:
    private:
        double lasttime;
        const int DISPLAY_DEFAULT_WIDTH = 1280;
        const int DISPLAY_DEFAULT_HEIGHT = 1024;
        const int DISPLAY_DEFAULT_TYPE = ALLEGRO_RESIZABLE;
};
