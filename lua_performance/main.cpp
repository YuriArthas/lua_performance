#include <lua.hpp>

int main()
{

	auto L = luaL_newstate();
	luaL_openlibs(L);

	

	return 0;
}