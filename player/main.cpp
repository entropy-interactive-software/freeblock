#include <game.hpp>
#include <settings.hpp>

#include "fb_game.hpp"

int main(int argc, char** argv) {
  rdm::Settings::singleton()->parseCommandLine(argv, argc);
  freeblock::Game game;
  game.setEditor(false);
  game.mainLoop();
  rdm::Settings::singleton()->save();
}
