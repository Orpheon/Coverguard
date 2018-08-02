
#include <engine.h>
#include "ingameelements/character.h"
#include "ingameelements/weapons/reinhardtshield.h"
#include "gamestate.h"
#include "renderer.h"

void ReinhardtShield::init(uint64_t id_, Gamestate &state, Team team_, EntityPtr owner_)
{
    Shield::init(id_, state, SHIELD_MAX_HP, team_);
    owner = owner_;
    active = false;
    aimdirection = 0;
    rechargecooldown.init(2, false);
    brokencooldown.init(5, false);
}

void ReinhardtShield::beginstep(Gamestate &state, double frametime)
{
    Character &reinhardt = state.get<Player>(owner).getcharacter(state);
    if (not brokencooldown.active and reinhardt.canuseabilities(state))
    {
        active = reinhardt.heldkeys.SECONDARY_FIRE;
    }
    else
    {
        active = false;
    }
    brokencooldown.update(state, frametime);
    rechargecooldown.update(state, frametime);
    if (active)
    {
        rechargecooldown.reset();
    }
    else if (not rechargecooldown.active)
    {
        hp = std::min(SHIELD_MAX_HP, hp+frametime*SHIELD_RECHARGE);
    }
}

void ReinhardtShield::midstep(Gamestate &state, double frametime)
{
    Shield::midstep(state, frametime);

    Character &reinhardt = state.get<Player>(owner).getcharacter(state);
    Weapon &hammer = reinhardt.getweapon(state);
    aimdirection = hammer.aimdirection;
    x = reinhardt.x + attachpoint_x(state) + DIST_TO_REINHARDT * std::cos(aimdirection);
    y = reinhardt.y + attachpoint_y(state) + DIST_TO_REINHARDT * std::sin(aimdirection);
}

void ReinhardtShield::render(Renderer &renderer, Gamestate &state)
{
    if (active)
    {
        Character &reinhardt = state.get<Player>(owner).getcharacter(state);

        if (reinhardt.weaponvisible(state))
        {
            std::string spritepath = spritestr();
            sf::Sprite sprite;
            renderer.spriteloader.loadsprite(spritepath, sprite);
            sprite.setPosition(x-attachpoint_x(state), y-attachpoint_y(state));

            if (reinhardt.isflipped)
            {
                sprite.setScale(-1, 1);
                sprite.setRotation(aimdirection+3.1415);
            }
            else
            {
                sprite.setRotation(aimdirection);
            }

            renderer.midground.draw(sprite);
        }
    }
}

std::string ReinhardtShield::spritestr()
{
    if (hp/SHIELD_MAX_HP >= 0.6)
    {
        return "heroes/reinhardt/shield/0%";
    }
    else if (hp/SHIELD_MAX_HP >= 0.2)
    {
        return "heroes/reinhardt/shield/40%";
    }
    else
    {
        return "heroes/reinhardt/shield/80%";
    }
}

void ReinhardtShield::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Shield::interpolate(prev_entity, next_entity, alpha);

    ReinhardtShield &prev_e = static_cast<ReinhardtShield&>(prev_entity);
    ReinhardtShield &next_e = static_cast<ReinhardtShield&>(next_entity);

    if (alpha < 0.5)
    {
        active = prev_e.active;
    }
    else
    {
        active = next_e.active;
    }
    brokencooldown.interpolate(prev_e.brokencooldown, next_e.brokencooldown, alpha);
    rechargecooldown.interpolate(prev_e.rechargecooldown, next_e.rechargecooldown, alpha);

    // Angles need some care when interpolating
    if (std::abs(prev_e.aimdirection - next_e.aimdirection) > PI)
    {
        double offset = next_e.aimdirection < prev_e.aimdirection ? 2*PI : -2*PI;
        aimdirection = prev_e.aimdirection + alpha * (next_e.aimdirection - prev_e.aimdirection + offset);
    }
    else
    {
        aimdirection = prev_e.aimdirection + alpha * (next_e.aimdirection - prev_e.aimdirection);
    }
}

double ReinhardtShield::maxdamageabledist(Gamestate &state, double *centerx, double *centery)
{
    *centerx = x;
    *centery = y;
    return state.engine.maskloader.get_rect(spritestr()).h/2.0;
}

bool ReinhardtShield::collides(Gamestate &state, double testx, double testy)
{
    testx -= x;
    testy -= y;
    double cosa = std::cos(-aimdirection);
    double sina = std::sin(-aimdirection);
    double rotx = testx * cosa - testy * sina;
    double roty = testx * sina + testy * cosa;

    std::string spritepath = spritestr();
    double spriteoffset_x = state.engine.maskloader.get_spriteoffset_x(spritepath);
    double spriteoffset_y = state.engine.maskloader.get_spriteoffset_y(spritepath);

    if (al_get_pixel(state.engine.maskloader.requestsprite(spritepath), rotx+spriteoffset_x, roty+spriteoffset_y).a != 0)
    {
        return true;
    }
    return false;
}