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

namespace {

typedef const std::wstring &T;

class MyClass
{
public:
	MyClass(): m_String(L"") {};
	MyClass(T _1): m_String(_1) {};
	MyClass(T _1, T _2): m_String(_1 + _2) {};
	MyClass(T _1, T _2, T _3): m_String(_1 + _2 + _3) {};
	MyClass(T _1, T _2, T _3, T _4): m_String(_1 + _2 + _3 + _4) {};
	MyClass(T _1, T _2, T _3, T _4, T _5): m_String(_1 + _2 + _3 + _4 + _5) {};
	MyClass(T _1, T _2, T _3, T _4, T _5, T _6): m_String(_1 + _2 + _3 + _4 + _5 + _6) {};
	MyClass(T _1, T _2, T _3, T _4, T _5, T _6, T _7): m_String(_1 + _2 + _3 + _4 + _5 + _6 + _7) {};
	MyClass(T _1, T _2, T _3, T _4, T _5, T _6, T _7, T _8): m_String(_1 + _2 + _3 + _4 + _5 + _6 + _7 + _8) {};
	MyClass(T _1, T _2, T _3, T _4, T _5, T _6, T _7, T _8, T _9): m_String(_1 + _2 + _3 + _4 + _5 + _6 + _7 + _8 + _9) {};
	const std::wstring &GetWString() const { return m_String; };
private:
	std::wstring m_String;
};

}

struct TestConstructors : CMLuaBindTestBase
{
	TestConstructors() : CMLuaBindTestBase("Object constructors") {};

	bool Run()
	{	
		host.Insert(
			Class<MyClass> ("MyClass")
				.Constructor()
				.Constructor<T>()
				.Constructor<T,T>()
				.Constructor<T,T,T>()
				.Constructor<T,T,T,T>()
				.Constructor<T,T,T,T,T>()
				.Constructor<T,T,T,T,T,T>()
				.Constructor<T,T,T,T,T,T,T>()
				.Constructor<T,T,T,T,T,T,T,T>()
				.Constructor<T,T,T,T,T,T,T,T,T>()
				.Method("GetWString", &MyClass::GetWString)
			);

		DOSTRING("local t = MyClass()\n"
			"assert(t.GetWString() == std.wstring(\"\"))")
		DOSTRING("local t = MyClass(\"1\")\n"
			"assert(t.GetWString() == std.wstring(\"1\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\")\n"
			"assert(t.GetWString() == std.wstring(\"12\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\")\n"
			"assert(t.GetWString() == std.wstring(\"123\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\")\n"
			"assert(t.GetWString() == std.wstring(\"1234\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\",\"5\")\n"
			"assert(t.GetWString() == std.wstring(\"12345\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\",\"5\",\"6\")\n"
			"assert(t.GetWString() == std.wstring(\"123456\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\",\"5\",\"6\",\"7\")\n"
			"assert(t.GetWString() == std.wstring(\"1234567\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\")\n"
			"assert(t.GetWString() == std.wstring(\"12345678\"))")
		DOSTRING("local t = MyClass(\"1\", \"2\",\"3\",\"4\",\"5\",\"6\",\"7\",\"8\",\"9\")\n"
			"assert(t.GetWString() == std.wstring(\"123456789\"))")

		return true;
	};
};

static TestConstructors _;
