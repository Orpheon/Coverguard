#include <cmath>

#include "ingameelements/weapons/pulserifle.h"
#include "renderer.h"
#include "ingameelements/heroes/soldier76.h"
#include "ingameelements/explosion.h"
#include "ingameelements/trail.h"
#include "engine.h"
#include "colorpalette.h"

void Pulserifle::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Clipweapon::init(id_, state, owner_);
}

void Pulserifle::render(Renderer &renderer, Gamestate &state)
{
    Soldier76 &soldier = state.get<Soldier76&>(state.get<Player&>(owner).character);
    if (soldier.weaponvisible(state))
    {
        std::string spritepath;
        double dir = aimdirection;
        if (firinganim.active())
        {
            spritepath = firinganim.getframepath();
        }
        else if (reloadanim.active())
        {
            spritepath = reloadanim.getframepath();
            dir = 3.1415*soldier.isflipped;
        }
        else
        {
            spritepath = soldier.herofolder()+"arm/1";
        }

        sf::Sprite sprite;
        renderer.spriteloader.loadsprite(spritepath, sprite);
        sprite.setOrigin(sprite.getOrigin()+sf::Vector2f(getattachpoint_x(state), getattachpoint_y(state)));
        sprite.setPosition(x, y);
        sprite.setRotation(dir*180.0/3.1415);
        if (soldier.isflipped)
        {
            sprite.setScale(1, -1);
        }
        renderer.midground.draw(sprite);
    }
}

void Pulserifle::beginstep(Gamestate &state, double frametime)
{
    Clipweapon::beginstep(state, frametime);
}

void Pulserifle::reload(Gamestate &state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active())
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Pulserifle::wantfireprimary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and state.engine.isserver)
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Pulserifle::fireprimary(Gamestate &state)
{
    double cosa = std::cos(aimdirection), sina = std::sin(aimdirection);
    double collisionptx, collisionpty;
    sf::Vector2u mapsize = state.currentmap->size();
    double d = std::hypot(mapsize.x, mapsize.y);
    EntityPtr target = state.collidelinedamageable(x, y, x+cosa*d, y+sina*d, team, &collisionptx, &collisionpty);
    if (state.exists(target))
    {
        double distance = std::hypot(collisionptx-x, collisionpty-y);
        double falloff = 1.0;
        if (distance > FALLOFF_BEGIN)
        {
            falloff = std::max(0.0, 1 - (distance-FALLOFF_BEGIN)/(FALLOFF_END-FALLOFF_BEGIN));
        }
        Entity &e = state.get<Entity>(target);
        double effectivedamage = e.damage(state, MAX_DAMAGE*falloff, owner, REGULAR_ATTACK);
        state.get<Player&>(owner).registerdamage(state, effectivedamage);
    }

    state.make_entity<Trail>(state, COLOR_SOLDIER_TRAIL, x+cosa*24, y+sina*24, collisionptx, collisionpty, 0.1);

    --clip;
    firinganim.reset();
    firinganim.active(true);
}

void Pulserifle::wantfiresecondary(Gamestate &state)
{

}

void Pulserifle::firesecondary(Gamestate &state)
{

}

void Pulserifle::fireultimate(Gamestate &state)
{

}
