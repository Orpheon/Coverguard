#pragma once

#include <allegro5/allegro.h>
#include <string>
#include "timer.h"

class ValueAnimation
{
    public:
        virtual void init(std::vector<int> sequence_, double duration);
        virtual void init(std::vector<int> sequence_, double duration, std::function<void(Gamestate &state)> eventfunc_);
        virtual ~ValueAnimation() = default;
        virtual unsigned int getframe();
        virtual int getvalue();
        virtual void update(Gamestate &state, double dt);
        virtual double getpercent() {return timer.getpercent();}
        virtual void interpolate(ValueAnimation &prev_anim, ValueAnimation &next_anim, double alpha);
        virtual bool active() {return timer.active;}
        virtual void active(bool active_) {timer.active = active_;}
        virtual void reset();
        Timer timer;
        std::vector<int> sequence;
        bool inited = false;
};

class Animation
{
    public:
        virtual void init(std::string path_);
        virtual void init(std::string path_, std::function<void(Gamestate &state)> eventfunc_);
        virtual ~Animation() = default;
        virtual int getframe();
        virtual std::string getframepath();
        virtual void update(Gamestate &state, double dt);
        virtual double getpercent() {return timer.getpercent();}
        virtual void interpolate(Animation &prev_anim, Animation &next_anim, double alpha);
        virtual bool active() {return timer.active;}
        virtual void active(bool active_) {timer.active = active_;}
        virtual void reset();
        Timer timer;
        int nframes;
        std::string path;
        bool inited = false;
};


class LoopAnimation : public Animation
{
    public:
        virtual ~LoopAnimation() override = default;
        void update(Gamestate &state, double dt) override;
        void interpolate(Animation &prev_anim, Animation &next_anim, double alpha) override;
};

