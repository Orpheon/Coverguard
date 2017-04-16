#include <allegro5/allegro.h>
#include <string>
#include <fstream>

#include "ingameelements/map.h"
#include "gamestate.h"
#include "engine.h"
#include "mapelements/spawnroom.h"
#include "mapelements/controlpoint.h"
#include "renderer.h"
#include "configloader.h"
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "ingameelements/gamemodes/kothmanager.h"

Map::Map(Gamestate &state, std::string name)
{
    // Load the map data first
    std::string mapfolder = "maps/" + name + "/";
    ConfigLoader configloader;
    mapdata = configloader.requestconfig(mapfolder + "mapdata.json");

    // Load all the images
    std::string bg = mapdata["background"], wg = mapdata["wallmask foreground"], wm = mapdata["wallmask"];
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_load_bitmap((mapfolder + bg).c_str());
    wallground = al_load_bitmap((mapfolder + wg).c_str());

    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    wallmask = al_load_bitmap((mapfolder + wm).c_str());
    al_lock_bitmap(wallmask, al_get_bitmap_format(wallmask), ALLEGRO_LOCK_READONLY);

    if (mapdata.find("gamemodes") == mapdata.end())
    {
        Global::logging().panic(__FILE__, __LINE__, "%s does not have proper gamemode data", (mapfolder+"mapdata.json").c_str());
    }

    for (nlohmann::json gamemode : mapdata["gamemodes"])
    {
        if (gamemode["type"] == "KOTH")
        {
            if (gamemode.find("cp") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a cp field.", (mapfolder+"mapdata.json").c_str());
            }
            if (gamemode.find("spawn 1") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a spawn 1 field.", (mapfolder+"mapdata.json").c_str());
            }
            if (gamemode.find("spawn 2") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a spawn 2 field.", (mapfolder+"mapdata.json").c_str());
            }
            Rect cparea(gamemode["cp"][0], gamemode["cp"][1], gamemode["cp"][2], gamemode["cp"][3]);
            Rect spawnarea1(gamemode["spawn 1"][0], gamemode["spawn 1"][1],
                            gamemode["spawn 1"][2], gamemode["spawn 1"][3]);
            Rect spawnarea2(gamemode["spawn 2"][0], gamemode["spawn 2"][1],
                            gamemode["spawn 2"][2], gamemode["spawn 2"][3]);
            gamemodes.push_back(state.make_entity<KothManager>(spawnarea1, spawnarea2, cparea));
        }
        else
        {
            Global::logging().panic(__FILE__, __LINE__, "Unknown gamemode %s", gamemode["type"]);
        }
    }
    currentgamemode(state).activate(state, std::bind(&Map::gotonextgamemode, this, std::placeholders::_1, std::placeholders::_2));
}

Map::~Map()
{
    al_destroy_bitmap(background);
    al_destroy_bitmap(wallground);
    al_unlock_bitmap(wallmask);
    al_destroy_bitmap(wallmask);
}

void Map::renderbackground(Renderer &renderer)
{
    al_draw_scaled_bitmap(background, renderer.cam_x, renderer.cam_y, VIEWPORT_WIDTH, renderer.WINDOW_HEIGHT/renderer.zoom, 0, 0, renderer.WINDOW_WIDTH, renderer.WINDOW_HEIGHT, 0);
}

void Map::renderwallground(Renderer &renderer)
{
    al_draw_scaled_bitmap(wallground, renderer.cam_x, renderer.cam_y, VIEWPORT_WIDTH, renderer.WINDOW_HEIGHT/renderer.zoom, 0, 0, renderer.WINDOW_WIDTH, renderer.WINDOW_HEIGHT, 0);
}

bool Map::collides(Rect r)
{
    if (r.x < 0 or r.y < 0 or r.x+r.w > width() or r.y+r.h > height())
    {
        return true;
    }
    for (int i=0; i<r.w; ++i)
    {
        for (int j=0; j<r.h; ++j)
        {
            if (al_get_pixel(wallmask, i+r.x, j+r.y).a != 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool Map::collides(double rotx, double roty, std::string spriteid, double angle)
{
    // TODO this thing
    return false;
}

bool Map::collideline(double x1, double y1, double x2, double y2)
{
    double mapw = width(), maph = height();
    if (x1 < 0 or y1 < 0 or x2 < 0 or y2 < 0 or x1 > mapw or x2 > mapw or y1 > maph or y2 > maph)
    {
        return true;
    }

    int nsteps = std::ceil(std::max(std::abs(x1-x2), std::abs(y1-y2)));
    double dx = static_cast<double>(x2-x1)/nsteps, dy = static_cast<double>(y2-y1)*1.0/nsteps;
    for (int i=0; i<nsteps; ++i)
    {
        if (al_get_pixel(wallmask, x1, y1).a != 0)
        {
            return true;
        }
        x1 += dx; y1 += dy;
    }
    return false;
}

Spawnroom& Map::spawnroom(Gamestate &state, Team team)
{
    return state.get<Spawnroom>(currentgamemode(state).spawnrooms.at(team));
}

GamemodeManager& Map::currentgamemode(Gamestate &state)
{
    return state.get<GamemodeManager>(gamemodes.front());
}

void Map::gotonextgamemode(Gamestate &state, Team winners)
{
    currentgamemode(state).destroy(state);
    gamemodes.pop_front();
    if (gamemodes.size() == 0)
    {
        Global::logging().print(__FILE__, __LINE__, "Map is over, won by %i", winners);
    }
    else
    {
        currentgamemode(state).activate(state, std::bind(&Map::gotonextgamemode, this, std::placeholders::_1, std::placeholders::_2));
    }
}