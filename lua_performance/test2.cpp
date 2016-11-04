#include <lua.hpp>

static int gc2(lua_State *L)
{
	printf("gc2 !!!");

	lua_pushnil(L);
	return 1;
}

static int gc(lua_State *L)
{
	printf("gc begin !\n");

	lua_newtable(L);
	{
		lua_newtable(L);
		{
			lua_pushcfunction(L, gc2);
			lua_setfield(L, -2, "__gc");
		}
		lua_setmetatable(L, -2);
	}
	lua_setfield(L, LUA_REGISTRYINDEX, "tttt2");
	

	lua_pushnil(L);
	printf("gc end !\n");
	return 1;
}

int test2(lua_State *L)
{
	lua_newtable(L);
	{
		lua_newtable(L);
		{
			lua_pushcfunction(L, gc);
			lua_setfield(L, -2, "__gc");
		}
		lua_setmetatable(L, -2);
	}
	lua_setfield(L, LUA_REGISTRYINDEX, "tttt");
	
	lua_pushnil(L);
	return 1;
}