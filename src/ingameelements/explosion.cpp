#include "ingameelements/explosion.h"
#include "renderer.h"

void Explosion::init(uint64_t id_, Gamestate &state, std::string animationpath, double direction_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::EXPLOSION;
    explosionanim.init(animationpath, std::bind(&Explosion::destroy, this, std::placeholders::_1));
    direction = direction_;
}

void Explosion::midstep(Gamestate &state, double frametime)
{
    explosionanim.update(state, frametime);
}

void Explosion::render(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite = explosionanim.getframepath();
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;

    al_set_target_bitmap(renderer.foreground);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, direction, 0);
}
