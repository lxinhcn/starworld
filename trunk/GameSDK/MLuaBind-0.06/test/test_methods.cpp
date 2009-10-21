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
#include <string>

namespace
{

class MyClass
{
public:
	int Simple(){return 0;};
	int Simple(int){return 1;};
	int Simple(int, int){return 2;};
	int Simple(int, int, int){return 3;};
	int Simple(int, int, int, int){return 4;};
	int Simple(int, int, int, int, int){return 5;};
	int Simple(int, int, int, int, int, int){return 6;};
	int Simple(int, int, int, int, int, int, int){return 7;};
	int Simple(int, int, int, int, int, int, int, int){return 8;};
	int Simple(int, int, int, int, int, int, int, int, int){return 9;};
	int Simple()const{return 100;};
	int Simple(int)const{return -1;};
	int Simple(int, int)const{return -2;};
	int Simple(int, int, int)const{return -3;};
	int Simple(int, int, int, int)const{return -4;};
	int Simple(int, int, int, int, int)const{return -5;};
	int Simple(int, int, int, int, int, int)const{return -6;};
	int Simple(int, int, int, int, int, int, int)const{return -7;};
	int Simple(int, int, int, int, int, int, int, int)const{return -8;};
	int Simple(int, int, int, int, int, int, int, int, int)const{return -9;};
	const MyClass *Const() const { return this; }; 
};

typedef const std::string &T;

const char* Simple(MyClass*,T){return "1";};
const char* Simple(MyClass*,T,T){return "2";};
const char* Simple(MyClass*,T,T,T){return "3";};
const char* Simple(MyClass*,T,T,T,T){return "4";};
const char* Simple(MyClass*,T,T,T,T,T){return "5";};
const char* Simple(MyClass*,T,T,T,T,T,T){return "6";};
const char* Simple(MyClass*,T,T,T,T,T,T,T){return "7";};
const char* Simple(MyClass*,T,T,T,T,T,T,T,T){return "8";};
const char* Simple(MyClass*,T,T,T,T,T,T,T,T,T){return "9";};
const char* Simple(const MyClass*,T){return "-1";};
const char* Simple(const MyClass*,T,T){return "-2";};
const char* Simple(const MyClass*,T,T,T){return "-3";};
const char* Simple(const MyClass*,T,T,T,T){return "-4";};
const char* Simple(const MyClass*,T,T,T,T,T){return "-5";};
const char* Simple(const MyClass*,T,T,T,T,T,T){return "-6";};
const char* Simple(const MyClass*,T,T,T,T,T,T,T){return "-7";};
const char* Simple(const MyClass*,T,T,T,T,T,T,T,T){return "-8";};
const char* Simple(const MyClass*,T,T,T,T,T,T,T,T,T){return "-9";};
}

struct TestMethods : CMLuaBindTestBase
{
	TestMethods() : CMLuaBindTestBase("Object methods") {};

	bool Run()
	{
		host.Insert(
			Class<MyClass>("MyClass")
			.Constructor()
			.Method("Const", &MyClass::Const)
			.Method("Simple", (int (MyClass::*) () ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int, int, int) ) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) () const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (int (MyClass::*) (int, int, int, int, int, int, int, int, int) const) &MyClass::Simple)
			.Method("Simple", (const char* (*) (MyClass*,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (MyClass*,T,T,T,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T,T,T,T,T)) &Simple)
			.Method("Simple", (const char* (*) (const MyClass*,T,T,T,T,T,T,T,T,T)) &Simple)
			);
		
		DOSTRING("local t = MyClass()\n"
			"assert(t.Simple() == 0)\n"
			"assert(t.Simple(0) == 1)\n"
			"assert(t.Simple(0, 0) == 2)\n"
			"assert(t.Simple(0, 0, 0) == 3)\n"
			"assert(t.Simple(0, 0, 0, 0) == 4)\n"
			"assert(t.Simple(0, 0, 0, 0, 0) == 5)\n"
			"assert(t.Simple(0, 0, 0, 0, 0, 0) == 6)\n"
			"assert(t.Simple(0, 0, 0, 0, 0, 0, 0) == 7)\n"
			"assert(t.Simple(0, 0, 0, 0, 0, 0, 0, 0) == 8)\n"
			"assert(t.Simple(0, 0, 0, 0, 0, 0, 0, 0, 0) == 9)\n"
			"local c = t.Const()\n"
			"assert(c.Simple() == 100)\n"
			"assert(c.Simple(0) == -1)\n"
			"assert(c.Simple(0, 0) == -2)\n"
			"assert(c.Simple(0, 0, 0) == -3)\n"
			"assert(c.Simple(0, 0, 0, 0) == -4)\n"
			"assert(c.Simple(0, 0, 0, 0, 0) == -5)\n"
			"assert(c.Simple(0, 0, 0, 0, 0, 0) == -6)\n"
			"assert(c.Simple(0, 0, 0, 0, 0, 0, 0) == -7)\n"
			"assert(c.Simple(0, 0, 0, 0, 0, 0, 0, 0) == -8)\n"
			"assert(c.Simple(0, 0, 0, 0, 0, 0, 0, 0, 0) == -9)\n"
			"local a = \"\"\n"
			"assert(t.Simple(a) == \"1\")\n"
			"assert(t.Simple(a, a) == \"2\")\n"
			"assert(t.Simple(a, a, a) == \"3\")\n"
			"assert(t.Simple(a, a, a, a) == \"4\")\n"
			"assert(t.Simple(a, a, a, a, a) == \"5\")\n"
			"assert(t.Simple(a, a, a, a, a, a) == \"6\")\n"
			"assert(t.Simple(a, a, a, a, a, a, a) == \"7\")\n"
			"assert(t.Simple(a, a, a, a, a, a, a, a) == \"8\")\n"
			"assert(t.Simple(a, a, a, a, a, a, a, a, a) == \"9\")\n"
			"assert(c.Simple(a) == \"-1\")\n"
			"assert(c.Simple(a, a) == \"-2\")\n"
			"assert(c.Simple(a, a, a) == \"-3\")\n"
			"assert(c.Simple(a, a, a, a) == \"-4\")\n"
			"assert(c.Simple(a, a, a, a, a) == \"-5\")\n"
			"assert(c.Simple(a, a, a, a, a, a) == \"-6\")\n"
			"assert(c.Simple(a, a, a, a, a, a, a) == \"-7\")\n"
			"assert(c.Simple(a, a, a, a, a, a, a, a) == \"-8\")\n"
			"assert(c.Simple(a, a, a, a, a, a, a, a, a) == \"-9\")\n");

		return true;
	};

};

static TestMethods _;
