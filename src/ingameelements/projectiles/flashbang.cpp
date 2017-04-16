//#include "ingameelements/projectiles/flashbang.h"
//#include "renderer.h"
//#include "ingameelements/explosion.h"
//#include "engine.h"
//
//#include <functional>
//
//void Flashbang::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
//{
//    Projectile::init(id_, state, owner_);
//
//    countdown.init(0.3, std::bind(&Flashbang::explode, this, std::placeholders::_1));
//}
//
//void Flashbang::midstep(Gamestate &state, double frametime)
//{
//    Projectile::midstep(state, frametime);
//    countdown.update(state, frametime);
//}
//
//void Flashbang::render(Renderer &renderer, Gamestate &state)
//{
//    std::string mainsprite = spritepath;
//    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
//    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
//    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
//    double rel_x = (x - renderer.cam_x)*renderer.zoom;
//    double rel_y = (y - renderer.cam_y)*renderer.zoom;
//
//    double direction = std::atan2(vspeed, hspeed);
//    al_set_target_bitmap(renderer.background);
//    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, direction, 0);
//}
//
////bool Flashbang::collides(Gamestate &state, EntityPtr otherentity, double radius)
////{
////    MovingEntity *m = state.get<MovingEntity>(otherentity);
////    Rect other = state.engine.maskloader.get_rect(m->getsprite(state, true)).offset(m->x, m->y);
////
////    if (std::hypot(x - other.x, y - other.y) <= radius + std::hypot(other.w, other.h))
////    {
////        // We're close enough that an actual collision might happen
////        // Check the sprites
////        double r2 = radius*radius;
////        ALLEGRO_BITMAP *othersprite = state.engine.maskloader.requestsprite(m->getsprite(state, true));
////
////        for (int i=-radius; i<radius; ++i)
////        {
////            for (int j=-radius; j<radius; ++j)
////            {
////                if (i*i+j*j <= r2 and al_get_pixel(othersprite, x+i - other.x, y+j - other.y).a != 0)
////                {
////                    return true;
////                }
////            }
////        }
////    }
////    return false;
////}
//
//void Flashbang::explode(Gamestate &state)
//{
//    Explosion &e = state.get<Explosion>(state.make_entity<Explosion>(state, "heroes/mccree/flashbang_explosion/", 0));
//    e.x = x;
//    e.y = y;
//
////    for (auto pptr : state.playerlist)
////    {
////        Player &p = state.get<Player>(pptr);
////        if (p.team != team)
////        {
////            if (state.exists(p.character))
////            {
////                Character &c = p.getcharacter(state);
////                if (collides(state, p.character, EXPLOSION_RADIUS))
////                {
////                    // Check that they aren't behind a wall or something
////                    if (not state.currentmap.collideline(x, y, c.x, c.y))
////                    {
////                        c.stun(state);
////                    }
////                }
////            }
////        }
////    }
//
//    destroy(state);
//}
