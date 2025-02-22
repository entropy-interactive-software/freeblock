#include <game.hpp>
#include <settings.hpp>

int main(int argc, char** argv) {
  rdm::Settings::singleton()->parseCommandLine(argv, argc);

  rdm::Settings::singleton()->save();
}
