#include "mapelements/spawnroom.h"
#include "gamestate.h"
#include "renderer.h"

#include "allegro5/allegro_primitives.h"

void Spawnroom::init(uint64_t id_, Rect area_, Team team_)
{
    Entity::init(id_);
    area = area_;
    team = team_;
}

void Spawnroom::beginstep(Gamestate &state, double frametime)
{
    for (auto &pptr : state.playerlist)
    {
        Player &p = state.get<Player>(pptr);
        if (p.team == team and state.exists(p.character))
        {
            Character &c = p.getcharacter(state);
            if (isinside(c.x, c.y))
            {
                c.hp = c.maxhp();
            }
        }
    }
}

void Spawnroom::render(Renderer &renderer, Gamestate &state)
{
    al_set_target_bitmap(renderer.background);
    int border = 3;
    double rel_x = (area.x - renderer.cam_x)*renderer.zoom;
    double rel_y = (area.y - renderer.cam_y)*renderer.zoom;
    al_draw_rectangle(rel_x+border/2.0, rel_y+border/2.0, rel_x+renderer.zoom*area.w-border/2.0, rel_y+renderer.zoom*area.h-border/2.0, al_premul_rgba(248, 222, 0, 200), border);
    border = 10;
    al_draw_rectangle(rel_x+border/2.0, rel_y+border/2.0, rel_x+renderer.zoom*area.w-border/2.0, rel_y+renderer.zoom*area.h-border/2.0, al_premul_rgba(248, 222, 0, 200), border);
}
