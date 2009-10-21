// Copyright (c) 2007 Michael Kazakov
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
/////////////////////////////////////////////////////////////////////
#pragma once

#include <Loki/Singleton.h>
#include <map>
#include <string>
#include <string.h>
#include <mluabind/mluabind.h>

extern "C"
{
    #include <lauxlib.h>
    #include <lualib.h>
}

using namespace mluabind;

struct CMLuaBindTestBase;
typedef std::map<std::string, CMLuaBindTestBase*> TestsT;
typedef Loki::SingletonHolder<TestsT> Tests;

struct MyError : public std::exception
{
	MyError(const char *s) : std::exception(s) {};
};

class CTestHost : public mluabind::CHost
{
public:
	CTestHost() : CHost(lua_open())
	{
		luaL_openlibs(GetLua());
		BindStdLibrary(this);
	}

	void Error(const char *_format, ...)
	{
		va_list va;
		va_start(va, _format);
		static char buf[65536];
		_vsnprintf(buf, 65536, _format, va);
		va_end(va);
		throw MyError(buf);
	};

	void DoString(const char *_str)
	{
	    lua_pushcclosure(GetLua(), &DummyHandler, 0);

		if (luaL_loadbuffer(GetLua(), _str, strlen(_str), _str))
		{
			std::string err(lua_tostring(GetLua(), -1));
			lua_pop(GetLua(), 2);
			throw MyError(err.c_str());
		}

		if (lua_pcall(GetLua(), 0, 0, -2))
		{
	        std::string err(lua_tostring(GetLua(), -1));
		    lua_pop(GetLua(), 2);
			throw MyError(err.c_str());
		}

		lua_pop(GetLua(), 1);
	};

private:
	static int DummyHandler(lua_State* L)
	{
		return 1;
	}
};

struct CMLuaBindTestBase
{
	CMLuaBindTestBase(const char *_name):
		m_Name(_name)
	{
		Tests::Instance().insert(std::make_pair(_name, this));
	};
	virtual bool Run() = 0;
	CTestHost host;
	const char *m_Name;
};

#define DOSTRING(a)\
try\
{\
	host.DoString(a);\
}\
catch (MyError e)\
{\
	printf("Host respond error: %s\n", e.what());\
	printf("Test failure at %s(%d)\n", __FILE__, __LINE__);\
	return false;\
}\
catch (...)\
{\
	printf("General exception caught at %s(%d)\n", __FILE__, __LINE__);\
	return false;\
}

#define DOSTRINGEXPECT(a)\
{\
bool b = false;\
try\
{\
	host.DoString(a);\
}\
catch (MyError e)\
{\
	b = true;\
}\
catch (...)\
{\
	printf("General exception caught at %s(%d)\n", __FILE__, __LINE__);\
	return false;\
}\
if(!b)\
{\
	printf("Test failure at %s(%d)\n", __FILE__, __LINE__);\
	return false;\
}\
}

#define CPPASSERT(a)\
if(!(a))\
{\
	printf("Test failure at %s(%d)\n", __FILE__, __LINE__);\
	return false;\
}
