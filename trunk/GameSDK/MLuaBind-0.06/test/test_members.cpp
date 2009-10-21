// Copyright (c) 2007-2008 Michael Kazakov
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

class MyClass
{
public:
	MyClass(const std::string& _s):m_String(_s){};

	const std::string &GetString() const { return m_String; };
	void SetString(const std::string &_s) { m_String = _s; };

	std::string m_String;
};

const std::string &GetString(const MyClass* obj)
{
	return obj->m_String;
};

void SetString(MyClass* obj, const std::string &_s)
{
	obj->m_String = _s;
};

}

struct TestMembers : CMLuaBindTestBase
{
	TestMembers() : CMLuaBindTestBase("Object members") {};

	bool Run()
	{
		host.Insert(
			Class<MyClass>("MyClass")
				.Constructor<const std::string&>()
				.Member("m_String", &MyClass::m_String)
				.Member("String1", &MyClass::GetString, &MyClass::SetString)
				.Member("String2", &MyClass::GetString)
				.Member("String3", &GetString, &SetString)
				.Member("String4", &GetString)
			);
		DOSTRING("local t = MyClass(\"test\")\n"
			"assert(t.m_String.c_str() == \"test\")\n"
			"t.m_String = \"test1\"\n"
			"assert(t.m_String.c_str() == \"test1\")\n"
			"assert(t.String1 == t.m_String)"
			"t.String1 = \"test2\"\n"
			"assert(t.String1.c_str() == \"test2\")\n"
			"assert(t.String2.c_str() == \"test2\")\n"
			"assert(t.String3.c_str() == \"test2\")\n"
			"assert(t.String4.c_str() == \"test2\")\n"
			"t.String3 = \"test3\"\n"
			"assert(t.m_String.c_str() == \"test3\")\n"
			"t.String3 = __const(std.string(\"test4\"))\n"
			"assert(t.m_String.c_str() == \"test4\")\n"
			);

		DOSTRINGEXPECT(
			"local t = MyClass(\"test\")\n"
			"__host_const(t)\n"
			"t.m_String = \"\"");

		DOSTRINGEXPECT(
			"local t = MyClass(\"test\")\n"
			"t.String2 = \"111\"");

		DOSTRINGEXPECT(
			"local t = MyClass(\"test\")\n"
			"t.String4 = \"111\"");

		return true;
	};

};

static TestMembers _;
