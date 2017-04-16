#include "colorpalette.h"

const std::array<ColorRGB, static_cast<unsigned char>(Color::COLOR_MAX)> ColorPalette::colorlist = {
    ColorRGB(225, 225, 225), // HP
    ColorRGB(237, 223, 132), // ARMOR
    ColorRGB(101, 206, 240), // SHIELD
    ColorRGB(242, 197,  84), // TORBARMOR
    ColorRGB( 69, 122, 255), // LUCIOSHIELD

    ColorRGB(  0,   0, 255), // ALLY
    ColorRGB(255,   0,   0), // ENEMY
    ColorRGB(248, 222,   0), // SPAWNROOM
    ColorRGB(248, 222,   0), // CP

    ColorRGB(255, 255, 255), // WHITE
    ColorRGB(  0,   0,   0)  // BLACK
};