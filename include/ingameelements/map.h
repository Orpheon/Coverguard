#pragma once

#include "datastructures.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/character.h"
#include "json.hpp"
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "mapelements/spawnroom.h"

#include <allegro5/allegro.h>
#include <string>
#include <list>

class Map
{
    public:
        Map(Gamestate &state, std::string name);
        virtual ~Map();
        void renderbackground(Renderer &renderer);
        void renderwallground(Renderer &renderer);
        bool collides(Rect rect);
        bool collides(double rotx, double roty, std::string spriteid, double angle);
        bool collideline(double x1, double y1, double x2, double y2);
        bool testpixel(double x, double y) {return x < 0 or y < 0 or x > al_get_bitmap_width(wallmask) or y > al_get_bitmap_height(wallmask) or al_get_pixel(wallmask, x, y).a != 0;}
        double width() {return al_get_bitmap_width(background);}
        double height() {return al_get_bitmap_height(background);}
        Spawnroom& spawnroom(Gamestate &state, Team team);
        GamemodeManager& currentgamemode(Gamestate &state);
        void gotonextgamemode(Gamestate &state, Team winners);
    protected:
    private:
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *wallground;
        ALLEGRO_BITMAP *wallmask;
        nlohmann::json mapdata;
        std::list<EntityPtr> gamemodes;
};

