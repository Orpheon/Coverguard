#include "ingameelements/trail.h"

#include <allegro5/allegro_primitives.h>

void Trail::init(uint64_t id_, Gamestate &state, ALLEGRO_COLOR color_, double x1_, double y1_, double x2_, double y2_, double duration)
{
    Entity::init(id_);

    color = color_;
    x1 = x1_;
    y1 = y1_;
    x2 = x2_;
    y2 = y2_;
    countdown.init(duration, std::bind(&Trail::destroy, this, std::placeholders::_1));
}

void Trail::midstep(Gamestate &state, double frametime)
{
    countdown.update(state, frametime);
}

void Trail::render(Renderer &renderer, Gamestate &state)
{
    al_set_target_bitmap(renderer.background);
    al_draw_line(renderer.zoom*(x1-renderer.cam_x), renderer.zoom*(y1-renderer.cam_y), renderer.zoom*(x2-renderer.cam_x), renderer.zoom*(y2-renderer.cam_y), color, 1);
}

void Trail::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Trail &prev_e = reinterpret_cast<Trail&>(prev_entity);
    Trail &next_e = reinterpret_cast<Trail&>(next_entity);

    countdown.interpolate(next_e.countdown, prev_e.countdown, alpha);
}
