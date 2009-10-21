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
#include "testbase.h"

struct TestLuaAPIStruct{};

struct TestLuaAPI: CMLuaBindTestBase
{
	TestLuaAPI() : CMLuaBindTestBase("Lua API") {};

	bool Run()
	{
		host.Insert(Class<TestLuaAPIStruct>("TestLuaAPIStruct").Constructor());

		DOSTRING("local s = std.string()\n"
			"assert(__get_type(s) == \"std.string\")\n"
			"assert(__get_typeex(s) == \"std.string owned\")\n"
			"assert(__get_typeex(__const(s)) == \"std.string const owned\")\n"
			"assert(__get_typeex(__const(s, false)) == \"std.string owned\")\n"
			"assert(__get_type(6) == \"number\")\n"
			"assert(__get_type(__const(6)) == \"number\")");
		DOSTRING("local q = std.wstring(\"test\")\n"
			"local w = __cast_unsafe(q, std.string)\n"
			"assert(__get_type(w) == \"std.string\")\n"
			"local e = __cast_unsafe(w, std.wstring)\n"
			"assert(__get_type(e) == \"std.wstring\")\n"
			"assert(e.c_str() == \"test\")\n"
			"assert(e == q)");
		DOSTRING("assert(__get_rtti_type(TestLuaAPIStruct()) == \"struct mluabind::impl::CustomClass<struct TestLuaAPIStruct>\")");

		return true;
	};
};

static TestLuaAPI _;
