#include "ingameelements/clipweapon.h"

void Clipweapon::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Weapon::init(id_, state, owner_);
    reloadanim.init(herofolder()+"reload/", getreloadfunction(state));
    reloadanim.active(false);
    clip = getclipsize();
}

void Clipweapon::midstep(Gamestate &state, double frametime)
{
    Weapon::midstep(state, frametime);

    if (clip == 0)
    {
        reload(state);
    }
    reloadanim.update(state, frametime);
}

void Clipweapon::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Weapon::interpolate(prev_entity, next_entity, alpha);

    Clipweapon &prev_e = static_cast<Clipweapon&>(prev_entity);
    Clipweapon &next_e = static_cast<Clipweapon&>(next_entity);

    if (alpha < 0.5)
    {
        clip = prev_e.clip;
    }
    else
    {
        clip = next_e.clip;
    }

    reloadanim.interpolate(prev_e.reloadanim, next_e.reloadanim, alpha);
}

void Clipweapon::reload(Gamestate &state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active())
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Clipweapon::serialize(Gamestate &state, WriteBuffer &buffer, bool fullupdate)
{
    Weapon::serialize(state, buffer, fullupdate);

    // Hopefully no clip size goes above 255
    buffer.write<uint8_t>(clip);
}

void Clipweapon::deserialize(Gamestate &state, ReadBuffer &buffer, bool fullupdate)
{
    Weapon::deserialize(state, buffer, fullupdate);

    // Hopefully no clip size goes above 255
    clip = buffer.read<uint8_t>();
}
