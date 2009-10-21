#include <mluabind/mluabind.h>

struct Hello
{
	std::string DoIt() { return "Hello World!"; };
};

int main()
{
	using namespace mluabind;


	lua_State* luastate = lua_open();
	luaL_openlibs(luastate);

	CHost host(luastate);
	BindStdLibrary(&host);

	host.Insert(
		Class<Hello>("hello")
			.Constructor()
			.Method("DoIt", &Hello::DoIt)
		);

	luaL_dostring(luastate, 
		"local obj = hello()\n"
		"print(obj.DoIt())");

	return 0;
};
