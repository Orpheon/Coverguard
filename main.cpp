#include <cstdio>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#define boolean enet_boolean
#include <enet/enet.h>

#undef boolean

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "datastructures.h"
#include "global_constants.h"
#include "mainmenu.h"
#include "networking/servernetworker.h"
#include "networking/clientnetworker.h"
#include "global.h"

long int getmillisec();

int main_impl(int argc, char **argv)
{
    std::unique_ptr<PrintLogger> default_logger(new PrintLogger());
    Global::provide_logging(default_logger.get());

    // Initialize Allegro
    if (!al_init())
    {
        Global::logging().panic(__FILE__, __LINE__, "Allegro initialization failed");
    }

    // Initialize the Allegro Image addon, used to load sprites and maps
    if (!al_init_image_addon())
    {
        Global::logging().panic(__FILE__, __LINE__, "Allegro image addon initialization failed");
    }

    // Initialize primitives for drawing
    if (!al_init_primitives_addon())
    {
        Global::logging().panic(__FILE__, __LINE__, "Allegro primitives addon initialization failed");
    }

    // Initialize keyboard modules
    if (!al_install_keyboard())
    {
        Global::logging().panic(__FILE__, __LINE__, "Allegro keyboard initialization failed");
    }

    // Initialize mouse
    if (!al_install_mouse())
    {
        Global::logging().panic(__FILE__, __LINE__, "Allegro mouse initialization failed");
    }

    // Initialize networking system
    if (enet_initialize())
    {
        Global::logging().panic(__FILE__, __LINE__, "Enet initialization failed");
    }

    //load font
    //gg2 font as placeholder for now i guess
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT *font = al_load_font("Vanguard Main Font.ttf", 12, ALLEGRO_TTF_MONOCHROME);
    if (!font)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load Vanguard Main Font.ttf");
    }

//    MainMenu *mainmenu = new MainMenu(display);
    bool isserver;
    if (argc >= 2)
    {
        // If there are any arguments
        isserver = false;
    }
    else
    {
        isserver = true;
    }

//    double lasttimeupdated = al_get_time();
//    bool run = true;
//    while (run)
//    {
//        if (al_get_time() - lasttimeupdated >= MENU_TIMESTEP)
//        {
//            run = mainmenu->run(display, &gametype);
//            lasttimeupdated = al_get_time();
//        }
//    }
//    delete mainmenu;

    Renderer renderer;
    ALLEGRO_DISPLAY* display = renderer.createnewdisplay();

    Engine engine(isserver);
    InputCatcher inputcatcher(display);
    Gamestate renderingstate(engine);

    std::unique_ptr<Networker> networker;
    if (isserver)
    {
        networker = std::unique_ptr<Networker>(new ServerNetworker(engine.sendbuffer));
    }
    else
    {
        networker = std::unique_ptr<Networker>(new ClientNetworker(engine.sendbuffer));
    }

    engine.loadmap("lijiang");
    // FIXME: Hack to make sure the oldstate is properly initialized
    engine.update(0);

    EntityPtr myself(0);
    if (isserver)
    {
        myself = engine.currentstate->addplayer();
        engine.currentstate->get<Player>(myself).spawntimer.active = true;
    }
    else
    {
        ClientNetworker &n = reinterpret_cast<ClientNetworker&>(*networker);
        while (not n.isconnected())
        {
            n.receive(*(engine.currentstate));
        }
        myself = engine.currentstate->playerlist.at(engine.currentstate->playerlist.size()-1);
    }

    double enginetime = al_get_time();
    double networkertime = al_get_time();
    while (true)
    {
        while (al_get_time() - enginetime >= ENGINE_TIMESTEP)
        {
            networker->receive(*(engine.currentstate));
            inputcatcher.run(display, *(engine.currentstate), *networker, renderer, myself);
            engine.update(ENGINE_TIMESTEP);
            networker->sendeventdata(*(engine.currentstate));

            enginetime += ENGINE_TIMESTEP;
        }
        if (isserver)
        {
            if (al_get_time() - networkertime >= NETWORKING_TIMESTEP)
            {
                ServerNetworker &n = reinterpret_cast<ServerNetworker&>(*networker);
                n.sendframedata(*(engine.currentstate));

                networkertime = al_get_time();
            }
        }
        renderingstate.interpolate(*(engine.oldstate), *(engine.currentstate), (al_get_time()-enginetime)/ENGINE_TIMESTEP);
        renderer.render(display, renderingstate, myself, *networker);
    }
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return main_impl(argc, argv);
    }
    catch (...)
    {
        return -1;
    }
}