#include "script_api.hpp"

#include "logging.hpp"
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
}

static int l_print(lua_State* L) {
  const char* str = lua_tostring(L, -1);
  rdm::Log::printf(rdm::LOG_INFO, "%s", str);
  return 0;
}

namespace freeblock {
void ScriptAPI::add(lua_State* L) {
  lua_pushcfunction(L, l_print);
  lua_setglobal(L, "print");
}
};  // namespace freeblock
