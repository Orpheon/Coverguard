#include "animation.h"
#include "json.hpp"
#include "configloader.h"
#include "global.h"

#include <fstream>
#include <cmath>

// ######### VALUE ANIMATION #########

void ValueAnimation::init(std::vector<int> sequence_, double duration, std::function<void(Gamestate &state)> eventfunc_)
{
    sequence = sequence_;
    timer.init(duration, eventfunc_);
    inited = true;
}

void ValueAnimation::init(std::vector<int> sequence_, double duration)
{
    init(sequence_, duration, nullptr);
}

unsigned int ValueAnimation::getframe()
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Value Animation getframe() called before initialized");
    }

    size_t f = static_cast<size_t>(std::floor(sequence.size()*timer.getpercent()))+1;
    // This can't be necessary....
    f = std::min(std::max(f, static_cast<size_t>(1)), sequence.size());
    return f;
}

int ValueAnimation::getvalue()
{
    return sequence.at(getframe());
}

void ValueAnimation::update(Gamestate &state, double dt)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Value Animation update() called before initialized!");
    }

    timer.update(state, dt);
}

void ValueAnimation::interpolate(ValueAnimation &prev_anim, ValueAnimation &next_anim, double alpha)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Value Animation interpolate() called before initialized!");
    }

    timer.interpolate(prev_anim.timer, next_anim.timer, alpha);
}

void ValueAnimation::reset()
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Value Animation reset() called before initialized!");
    }

    timer.reset();
}


// ######### ANIMATION #########

void Animation::init(std::string path_, std::function<void(Gamestate &state)> eventfunc_)
{
    path = path_;

    ConfigLoader cfgloader;

    nlohmann::json data = cfgloader.requestconfig("gamedata.json");
    double duration = 0;
    try
    {
        duration = data[path+" duration"];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load %s animation duration", path.c_str());
    }

    nlohmann::json data2 = cfgloader.requestconfig("sprites/spritedata.json");
    try
    {
        nframes = data2[path+" number of frames"];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load %s animation number of frames", path.c_str());
    }

    timer.init(duration, eventfunc_);
    inited = true;
}

void Animation::init(std::string path_)
{
    Animation::init(path_, nullptr);
}

int Animation::getframe()
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Animation getframe() called before initialized!");
        
    }

    int f = static_cast<int>(std::floor(nframes*timer.getpercent()))+1;
    f = std::min(std::max(f, 1), nframes);
    return f;
}

std::string Animation::getframepath()
{
    return path+std::to_string(getframe());
}

void Animation::update(Gamestate &state, double dt)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Animation update() called before initialized!");
    }

    timer.update(state, dt);
}

void Animation::interpolate(Animation &prev_anim, Animation &next_anim, double alpha)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Animation interpolate() called before initialized!");
    }

    timer.interpolate(prev_anim.timer, next_anim.timer, alpha);
}

void Animation::reset()
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "Animation reset() called before initialized!");
    }

    timer.reset();
}


// ######### LOOP ANIMATION #########

void LoopAnimation::update(Gamestate &state, double dt)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "LoopAnimation update() called before initialized!");
    }

    timer.update(state, dt);
    if (not timer.active and timer.timer >= timer.duration)
    {
        timer.timer -= static_cast<int>(timer.timer / timer.duration)*timer.duration;
        timer.active = true;
    }
    if (timer.timer < 0)
    {
        timer.timer += timer.duration;
    }
}

void LoopAnimation::interpolate(Animation &prev_anim, Animation &next_anim, double alpha)
{
    if (not inited)
    {
        Global::logging().panic(__FILE__, __LINE__, "LoopAnimation interpolate() called before initialized!");
    }

    if (std::abs(prev_anim.timer.timer - next_anim.timer.timer) > 0.5*timer.duration)
    {
        // We've probably looped over
        if (prev_anim.timer.timer < next_anim.timer.timer)
        {
            prev_anim.timer.timer += timer.duration;
            timer.interpolate(prev_anim.timer, next_anim.timer, alpha);
            prev_anim.timer.timer -= timer.duration;
        }
        else
        {
            next_anim.timer.timer += timer.duration;
            timer.interpolate(prev_anim.timer, next_anim.timer, alpha);
            next_anim.timer.timer -= timer.duration;
        }
    }
    else
    {
        timer.interpolate(prev_anim.timer, next_anim.timer, alpha);
    }
}
