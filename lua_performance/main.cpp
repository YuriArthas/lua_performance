#include <lua.hpp>
#include "lua_performance.h"

extern int test2(lua_State *L);

int main()
{

	auto L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushcfunction(L, lua_performance::wrap);
	lua_setglobal(L, "performance_wrap");

	lua_getglobal(L, "require");
	lua_pushstring(L, "lua_main");

	if (lua_pcall(L, 1, 1, 0) != LUA_OK)
	{
		lua_getglobal(L, "print");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
	}

	lua_close(L);

	return 0;
}