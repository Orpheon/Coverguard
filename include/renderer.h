#pragma once

#include "gamestate.h"
#include "spriteloader.h"
#include "networking/networker.h"
#include "visuals/hud.h"

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
        void changeviewport(int newsize);
        double cam_x;
        double cam_y;
        double zoom;
        EntityPtr myself;
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;
        int VIEWPORT_WIDTH = 960;
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *midground;
        ALLEGRO_BITMAP *foreground;
        ALLEGRO_BITMAP *surfaceground;
        Spriteloader spriteloader;
        ALLEGRO_FONT *font20;
        ALLEGRO_FONT *font12;
        ALLEGRO_FONT *font8;
    protected:
    private:
        double lasttime;
        const int DISPLAY_DEFAULT_TYPE = ALLEGRO_RESIZABLE;
        std::unique_ptr<Hud> currenthud;
        bool changedzoom = false;
};
