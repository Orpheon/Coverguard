#include <allegro5/allegro.h>
#include <cstdio>
#include <fstream>

#include "inputcatcher.h"
#include "datastructures.h"
#include "renderer.h"
#include "networking/clientnetworker.h"
#include "ingameelements/character.h"
#include "global_constants.h"
#include "global.h"

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

InputCatcher::InputCatcher(ALLEGRO_DISPLAY *display)
{
    // Create an event queue, and error if it fails
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not create event queue");
    }

    // Connect the window, keyboard and mouse events to this event queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
//    al_register_event_source(event_queue, al_get_mouse_event_source(display));

    std::ifstream configfile("config.json");
    config << configfile;
    configfile.close();
}

InputCatcher::~InputCatcher()
{
    al_destroy_event_queue(event_queue);
}

void InputCatcher::run(ALLEGRO_DISPLAY *display, Gamestate &state, Networker &networker, Renderer &renderer, EntityPtr myself)
{
    InputContainer heldkeys;
    heldkeys.reset();

    Player &player = state.get<Player>(myself);

    ALLEGRO_EVENT event;
    // Catch all events that have stacked up this frame. al_get_next_event() returns false when event_queue is empty, and contents of event are undefined
    while (al_get_next_event(event_queue, &event))
    {
        switch (event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // Deliberate closing, not an error
                throw 0;

            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(display);
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                Heroclass newclass = player.heroclass;
                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_1:
                        newclass = MCCREE;
                        break;

                    case ALLEGRO_KEY_2:
                        newclass = REINHARDT;
                        break;

                    case ALLEGRO_KEY_ESCAPE:
                        // Exit game
                        throw 0;
                }
                if (newclass != player.heroclass)
                {
                    // Player desires a class change
                    if (state.engine.isserver)
                    {
                        player.changeclass(state, newclass);
                        networker.sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                        networker.sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                        networker.sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                    }
                    else
                    {
                        networker.sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                        networker.sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                    }
                }
                break;
        }
    }

    ALLEGRO_KEYBOARD_STATE keystate;
    al_get_keyboard_state(&keystate);
    if (al_key_down(&keystate, config["jump"]) or al_key_down(&keystate, config["jump_alt1"]) or al_key_down(&keystate, config["jump_alt2"]))
    {
        heldkeys.JUMP = true;
    }
    if (al_key_down(&keystate, config["crouch"]) or al_key_down(&keystate, config["crouch_alt1"]) or al_key_down(&keystate, config["crouch_alt2"]))
    {
        heldkeys.CROUCH = true;
    }
    if (al_key_down(&keystate, config["left"]) or al_key_down(&keystate, config["left_alt1"]) or al_key_down(&keystate, config["left_alt2"]))
    {
        heldkeys.LEFT = true;
    }
    if (al_key_down(&keystate, config["right"]) or al_key_down(&keystate, config["right_alt1"]) or al_key_down(&keystate, config["right_alt2"]))
    {
        heldkeys.RIGHT = true;
    }
    if (al_key_down(&keystate, config["ability1"]) or al_key_down(&keystate, config["ability1_alt1"]) or al_key_down(&keystate, config["ability1_alt2"]))
    {
        heldkeys.ABILITY_1 = true;
    }
    if (al_key_down(&keystate, config["ability2"]) or al_key_down(&keystate, config["ability2_alt1"]) or al_key_down(&keystate, config["ability2_alt2"]))
    {
        heldkeys.ABILITY_2 = true;
    }
    if (al_key_down(&keystate, config["ultimate"]) or al_key_down(&keystate, config["ultimate_alt1"]) or al_key_down(&keystate, config["ultimate_alt2"]))
    {
        heldkeys.ULTIMATE = true;
    }
    if (al_key_down(&keystate, config["reload"]) or al_key_down(&keystate, config["reload_alt1"]) or al_key_down(&keystate, config["reload_alt2"]))
    {
        heldkeys.RELOAD = true;
    }

    ALLEGRO_MOUSE_STATE mousestate;
    al_get_mouse_state(&mousestate);
    // FIXME: I have no idea if these constants are correct, allegro docs don't mention the specifics, just that it starts with 1.
    if (mousestate.buttons & LEFT_MOUSE_BUTTON)
    {
        heldkeys.PRIMARY_FIRE = true;
    }
    if (mousestate.buttons & RIGHT_MOUSE_BUTTON)
    {
        heldkeys.SECONDARY_FIRE = true;
    }

    if (state.exists(player.character))
    {
        Character &c = player.getcharacter(state);
        // Set the input for our current character
        c.setinput(state, heldkeys, mousestate.x/renderer.zoom+renderer.cam_x, mousestate.y/renderer.zoom+renderer.cam_y);

        // If this is a client, send the input off to the server
        if (not state.engine.isserver)
        {
            ClientNetworker &n = reinterpret_cast<ClientNetworker&>(networker);
            n.sendinput(heldkeys, mousestate.x/renderer.zoom+renderer.cam_x, mousestate.y/renderer.zoom+renderer.cam_y);
        }
    }
}
