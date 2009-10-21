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

enum TestEnum1 {value1 = 0, value2, value3};

static TestEnum1 current;

static void AcceptEnum1(TestEnum1 _val)
{
	if(_val != current)
		throw MyError("invalid enum value");
};

static TestEnum1 AcceptReturnEnum1(TestEnum1 _val)
{
	return _val;
};

struct TestEnums: CMLuaBindTestBase
{
	TestEnums() : CMLuaBindTestBase("Enums") {};

	bool Run()
	{
		// test passing enums as parameters into a global function
		host.Insert(Function("AcceptEnum1", &AcceptEnum1));
		host.Insert(Function("AcceptReturnEnum1", &AcceptReturnEnum1));
		host.Insert(Enum("value1", value1));
		host.Insert(Enum("value2", value2));
		host.Insert(Enum("value3", value3));

		DOSTRING("assert(value1 == 0)");
		current = value1;
		DOSTRING("AcceptEnum1(value1)");
		current = value2;
		DOSTRING("AcceptEnum1(value2)");
		current = value3;
		DOSTRING("AcceptEnum1(value3)");
		current = (TestEnum1)100; // check for a brutal value forcing
		DOSTRING("AcceptEnum1(100)");
		DOSTRINGEXPECT("AcceptEnum1(666)");

		DOSTRING("local a = AcceptReturnEnum1(value3)\n"
			"assert(a == value3)\n"
			"a = AcceptReturnEnum1(1000)\n"
			"assert(a == 1000)");

		// check access to enums(or any constants) of class from object ( included in 0.04 )
		DOSTRING("local a = std.string(\"aaaa\")\n"
			"local b = a.find(\"b\",0)\n"
			"assert(b == a.npos)");

		return true;
	};
};

}

static TestEnums _;