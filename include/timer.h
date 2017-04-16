#pragma once

#include "entity.h"
#include <functional>

// Circular reference
class Gamestate;

class Timer
{
    public:
        virtual ~Timer() = default;
        virtual void init(double duration_, std::function<void(Gamestate &state)> eventfunc_);
        virtual void init(double duration_);
        void update(Gamestate &state, double dt);
        double getpercent();
        void interpolate(Timer &prev_timer, Timer &next_timer, double alpha);
        void reset();
        double timer = 0;
        double duration = 0;
        bool active = false;
        bool inited = false;
        std::function<void(Gamestate &state)> eventfunc;
};

