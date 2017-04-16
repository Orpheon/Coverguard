#include <cstdio>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <engine.h>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"
#include "configloader.h"
#include "global.h"

Renderer::Renderer() : cam_x(0), cam_y(0), zoom(1), myself(0), WINDOW_WIDTH(0), WINDOW_HEIGHT(0), spriteloader(false)
{
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    surfaceground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

    // fps stuff
    lasttime = al_get_time();

    font20 = al_load_font("Vanguard Main Font.ttf", 20, ALLEGRO_TTF_MONOCHROME);
    font10 = al_load_font("Vanguard Main Font.ttf", 10, ALLEGRO_TTF_MONOCHROME);
    font6 = al_load_font("Vanguard Main Font.ttf", 6, ALLEGRO_TTF_MONOCHROME);
    gg2font = al_load_font("gg2bold.ttf", 12, ALLEGRO_TTF_MONOCHROME);
}

Renderer::~Renderer()
{
    // Cleanup
    al_destroy_font(font20);
    al_destroy_font(font10);
    al_destroy_font(font6);
    al_destroy_font(gg2font);
    al_destroy_bitmap(background);
    al_destroy_bitmap(midground);
    al_destroy_bitmap(foreground);
    al_destroy_bitmap(surfaceground);
}

void Renderer::render(ALLEGRO_DISPLAY *display, Gamestate &state, EntityPtr myself_, Networker &networker)
{
    myself = myself_;

    if (WINDOW_WIDTH != al_get_display_width(display) or WINDOW_HEIGHT != al_get_display_height(display))
    {
        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        WINDOW_WIDTH = al_get_display_width(display);
        WINDOW_HEIGHT = al_get_display_height(display);

        al_destroy_bitmap(background);
        al_destroy_bitmap(midground);
        al_destroy_bitmap(foreground);
        al_destroy_bitmap(surfaceground);

        background = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        midground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        foreground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
        surfaceground = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);

        zoom = 1.0*WINDOW_WIDTH / VIEWPORT_WIDTH;
        spriteloader.setzoom(zoom);
    }

    // Set camera
    if (state.exists(myself))
    {
        Player &p = state.get<Player>(myself);
        if (state.exists(p.character))
        {
            Character &c = p.getcharacter(state);
            cam_x = c.x - VIEWPORT_WIDTH/2.0;
            cam_y = c.y - WINDOW_HEIGHT/zoom/2.0;
        }
    }

    al_set_target_bitmap(background);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(midground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(foreground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(surfaceground);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    // Go through all objects and let them render themselves on the layers
    for (auto &e : state.entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->render(*this, state);
        }
    }

    // Set render target to be the display
    al_set_target_backbuffer(display);

    // Clear black
    al_clear_to_color(al_map_rgba(0, 0, 0, 1));

    // Draw the map background first
    state.currentmap->renderbackground(*this);

    // Then draw each layer
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(midground, 0, 0, 0);
    al_draw_bitmap(foreground, 0, 0, 0);

    // Draw the map wallmask on top of everything, to prevent sprites that go through walls
    state.currentmap->renderwallground(*this);

    // Draw the final layer on top of even that, for certain things like character healthbars
    al_draw_bitmap(surfaceground, 0, 0, 0);


    //fps counter mostly borrowed from pygg2
    double frametime = al_get_time() - lasttime;
    lasttime = al_get_time();

    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 0, ALLEGRO_ALIGN_LEFT, ("Frametime: " + std::to_string(frametime * 1000) + "ms").c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 12, ALLEGRO_ALIGN_LEFT, ("FPS: " + std::to_string((int)(1/frametime))).c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 24, ALLEGRO_ALIGN_LEFT, ("Ping: " + std::to_string(networker.host->peers[0].roundTripTime)).c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 60, ALLEGRO_ALIGN_LEFT, ("pos: " + std::to_string(cam_x+WINDOW_WIDTH/2.0) + " " + std::to_string(cam_y+WINDOW_HEIGHT/2.0)).c_str());
    if (state.exists(myself) and state.exists(state.get<Player>(myself).character))
    {
        Player &p = state.get<Player>(myself);
        Character &c = p.getcharacter(state);
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, ("hspeed: " + std::to_string(c.hspeed)).c_str());
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, ("vspeed: " + std::to_string(c.vspeed)).c_str());
    }
    else
    {
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 72, ALLEGRO_ALIGN_LEFT, "hspeed: 0.000000");
        al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 84, ALLEGRO_ALIGN_LEFT, "vspeed: 0.000000");
    }
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 96, ALLEGRO_ALIGN_LEFT, ("#Players: " + std::to_string(state.playerlist.size())).c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 108, ALLEGRO_ALIGN_LEFT, ("Zoom: " + std::to_string(zoom)).c_str());
    al_draw_text(gg2font, al_map_rgb(255, 255, 255), 0, 120, ALLEGRO_ALIGN_LEFT, state.engine.isserver ? "Server" : "Client");


    if (state.exists(myself) and state.exists(state.get<Player>(myself).character))
    {
        Player &p = state.get<Player>(myself);
        p.getcharacter(state).drawhud(*this, state);
    }

    al_flip_display();
}

ALLEGRO_DISPLAY* Renderer::createnewdisplay()
{
    ConfigLoader configloader;
    nlohmann::json config = configloader.requestconfig();
    return createnewdisplay(config);
}

ALLEGRO_DISPLAY* Renderer::createnewdisplay(const nlohmann::json &config)
{
    //default display values are set on header file
    int display_width, display_height, display_type;

    if (config.find("display_resolution") == config.end())
    {
        display_width = DISPLAY_DEFAULT_WIDTH;
        display_height = DISPLAY_DEFAULT_HEIGHT;
    }
    else
    {
        try
        {
            display_width = config["display_resolution"][0];
            display_height = config["display_resolution"][1];
            if (display_width < 640)
            {
                display_width = 640;
            }
            if (display_height < 480)
            {
                display_height = 480;
            }
        }
        catch (std::domain_error)
        {
            Global::logging().print(__FILE__, __LINE__, "Could not load display resolution data, using default values instead");
            display_width = DISPLAY_DEFAULT_WIDTH;
            display_height = DISPLAY_DEFAULT_HEIGHT;
        }
    }

    if (config.find("display_type") == config.end())
    {
        display_type = DISPLAY_DEFAULT_TYPE;
    }
    else
    {
        try
        {
            display_type = config["display_type"];
            // Check whether display type is valid
            if (display_type != ALLEGRO_RESIZABLE && display_type != ALLEGRO_FULLSCREEN && display_type != (ALLEGRO_FRAMELESS | ALLEGRO_MAXIMIZED))
            {
                display_type = DISPLAY_DEFAULT_TYPE;
            }
        }
        catch (std::domain_error)
        {
            Global::logging().print(__FILE__, __LINE__, "Could not load display type data, using default instead");
        }
    }
    // TODO: ADD ANOTHER OPTIONS LIKE VSYNC

    ALLEGRO_DISPLAY *display;
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_OPENGL | display_type);
    display = al_create_display(display_width, display_height);

    if(!display)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not create display");
    }

    /*
    ConfigLoader configloader;
    nlohmann::json config_fixed = configloader.requestconfig();
    config_fixed["display_resolution"][0] = display_width;
    config_fixed["display_resolution"][1] = display_height;
    config_fixed["display_type"] = display_type;
    configloader.saveconfig(config_fixed);
    */

    return display;
}
