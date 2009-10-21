// Copyright (c) 2008 Michael Kazakov
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

namespace
{

struct TestMiscStruct
{
	std::string data;
};

static TestMiscStruct *ReturnsZeroPtr() { return (TestMiscStruct *)0; };
static void AcceptZeroPtr(TestMiscStruct *_p) { if(_p !=0) throw MyError("expected zero pointer"); };
static void *RawVoid() { return (void*)1234567; }
static const void *RawConstVoid() { return RawVoid(); }
static void AcceptVoid(void *p) { if(p != (void*)1234567) throw MyError("invalid raw pointer"); };
static void AcceptConstVoid(const void *p) { if(p != (void*)1234567) throw MyError("invalid raw pointer"); };
static void AcceptSomeStuff(void *p, int, const char*) {};
static void *GetStringVoidPtr() {static const std::string s("abra"); return (void*)&s; };

struct TestMiscellaneous: CMLuaBindTestBase
{
	TestMiscellaneous() : CMLuaBindTestBase("Miscellaneous issues") {};

	bool Run()
	{
		// test for a bug in version prior to 0.04 with a const operands in assign operations
		host.Insert(
				Class<TestMiscStruct>("TestMiscStruct")
					.Constructor()
					.Member("data", &TestMiscStruct::data)
			);
	
		DOSTRING(
			"local s = std.string(\"666\")\n"
			"local cs = std.string(\"777\")\n"
			"local t = TestMiscStruct()\n"
			"__const(cs)\n"
			"t.data = s\n"
			"assert(t.data.c_str() == \"666\")\n"
			"t.data = cs\n"
			"assert(t.data.c_str() == \"777\")\n"
			);

		// test for a zero-pointers behaviour, passing nil as ptr into C++ included in 0.04
		host.Insert(Function("ReturnsZeroPtr", &ReturnsZeroPtr));
		host.Insert(Function("AcceptZeroPtr", &AcceptZeroPtr));
		DOSTRING(
			"local a = ReturnsZeroPtr()\n"
			"assert(not a)\n"
			"AcceptZeroPtr(nil)\n"
			);

		// test raw pointers transmission (included in 0.05)
		host.Insert( Function("RawVoid", &RawVoid) );
		host.Insert( Function("RawConstVoid", &RawConstVoid) );
		host.Insert( Function("AcceptVoid", &AcceptVoid) );
		host.Insert( Function("AcceptConstVoid", &AcceptConstVoid) );
		host.Insert( Function("GetStringVoidPtr", &GetStringVoidPtr) );
		host.Insert( Function("AcceptSomeStuff", &AcceptSomeStuff) );
		
		
		DOSTRING(
			"local a = RawVoid()\n"
			"local b = RawConstVoid()\n"
			"assert(a == b)\n"
			"assert(__get_type(a) == \"raw pointer\")\n"
			"assert(__get_type(a) == __get_type(b))\n"
			"assert(__get_typeex(a) == __get_type(b))\n"
			"AcceptVoid(a)\n"
			"AcceptVoid(b)\n"
			"AcceptConstVoid(a)\n"
			"AcceptConstVoid(b)\n"
			"local c = GetStringVoidPtr()\n"
			"local d = __cast_unsafe(c, std.string)\n"
			"assert(d.c_str() == \"abra\")\n"
			"AcceptSomeStuff(c, 100, \"cadabra\")\n"
			);

		return true;
	};
};

}

static TestMiscellaneous _;