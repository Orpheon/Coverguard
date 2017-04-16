project(Vanguard)
cmake_minimum_required(VERSION 2.8)

option(COMPILE_STATIC "Compile statically instead of dynamically" OFF)

set (Vanguardcompileflags "-std=c++11 -Wall -pedantic -fwrapv -Wno-format-security")
if (COMPILE_STATIC)
    set (Vanguardlib allegro_monolith-static allegro_dialog-static jpeg vorbis ogg glu32 opengl32 freetype png16 z kernel32 user32 gdi32 comdlg32 ole32 dinput ddraw dxguid dsound wsock32 psapi shlwapi enet)
    set (Vanguardlinkflags "-static-libstdc++ -static-libgcc -static ")
else (COMPILE_STATIC)
    set (Vanguardlib allegro allegro_image allegro_primitives allegro_font allegro_ttf allegro_dialog enet)
    set (Vanguardlinkflags "")
endif (COMPILE_STATIC)

if (WIN32)
    set (Vanguardlib $(Vanguardlib) ws2_32 winmm)
endif (WIN32)

include_directories("${PROJECT_SOURCE_DIR}/include")

set (SRC_FILES
        "main.cpp"
        "src/engine.cpp"
        "src/inputcatcher.cpp"
        "src/gamestate.cpp"
        "src/mainmenu.cpp"
        "src/spriteloader.cpp"
        "src/animation.cpp"
        "src/timer.cpp"
        "src/renderer.cpp"
        "src/entity.cpp"
        "src/configloader.cpp"
        "src/colorpalette.cpp"
        "src/global.cpp"
        "src/networking/networker.cpp"
        "src/networking/buffer.cpp"
        "src/networking/servernetworker.cpp"
        "src/networking/clientnetworker.cpp"
        "src/ingameelements/corpse.cpp"
        "src/ingameelements/player.cpp"
        "src/ingameelements/explosion.cpp"
        "src/ingameelements/projectile.cpp"
        "src/ingameelements/map.cpp"
        "src/ingameelements/character.cpp"
        "src/ingameelements/movingentity.cpp"
        "src/ingameelements/weapon.cpp"
        "src/ingameelements/clipweapon.cpp"
        "src/ingameelements/trail.cpp"
        "src/ingameelements/weapon.cpp"
        "src/ingameelements/clipweapon.cpp"
        "src/ingameelements/trail.cpp"
        "src/ingameelements/shield.cpp"
        "src/ingameelements/heroes/reinhardt.cpp"
        "src/ingameelements/heroes/mccree.cpp"
        "src/ingameelements/weapons/hammer.cpp"
        "src/ingameelements/weapons/peacemaker.cpp"
        "src/ingameelements/weapons/reinhardtshield.cpp"
        "src/ingameelements/projectiles/flashbang.cpp"
        "src/mapelements/spawnroom.cpp"
        "src/mapelements/controlpoint.cpp"
        "src/ingameelements/gamemodes/kothmanager.cpp"
        "src/ingameelements/gamemodes/gamemodemanager.cpp")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${Vanguardcompileflags}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${Vanguardlinkflags}" )

add_executable(Vanguard ${SRC_FILES})
target_link_libraries(Vanguard ${Vanguardlib})
