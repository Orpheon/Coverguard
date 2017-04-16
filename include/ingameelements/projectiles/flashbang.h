//#pragma once
//
//#include <ingameelements/projectile.h>
//
//
//class Flashbang : public Projectile
//{
//    public:
//        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
//        virtual ~Flashbang() override = default;
//        void beginstep(Gamestate &state, double frametime) override {}
//        void midstep(Gamestate &state, double frametime) override;
//        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Flashbang(*this));}
//        Rect getrect() override {return Rect(x-2, y-4, 3, 5);}
//        double damage() override {return 0;}
//        void render(Renderer &renderer, Gamestate &state) override;
//        void explode(Gamestate &state);
////        bool collides(Gamestate &state, EntityPtr otherentity, double angle) override;
////        void oncollision(Gamestate &state) override {explode(state);}
////        void oncollision(Gamestate &state, Character &c) override {explode(state);}
//
//        Timer countdown;
//        std::string spritepath = "heroes/mccree/projectiles/flashbang";
//    protected:
//    private:
//        double EXPLOSION_RADIUS = 40;
//};
//
