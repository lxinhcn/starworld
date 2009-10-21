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

namespace
{

class A
{
public:
	virtual ~A(){};
	enum {enum_A = 1};
};

class B
{
public:
	virtual ~B(){};
	enum {enum_B = 2};
};

class C : public A, public B
{
public:
	C() : m_Bool(true) {};
	enum {enum_C = 3};
	bool m_Bool;
};

}

static bool GetProperty(const C* _c)
{
	return _c->m_Bool;
};

struct TestNamespace : CMLuaBindTestBase
{
	TestNamespace() : CMLuaBindTestBase("Namespace registration") {};

	bool Run()
	{	
		static C obj_inst;
		host.Insert(Constant("_Pi_", 3.14));
		host.Insert(
			Declare("namespace")
				+Constant("_Pi_", 3.14)
				+(Declare("anothernamespace")
					+Constant("_Pi_", 3.14)
					+Constant("String", "MLuaBind")
					+Constant("Boolean", false) )
				+Class<A>("A")
					.Enum("enum_A", A::enum_A)
					.Constant("string_A", "string_A")
				+Class<B>("B")
					.Enum("enum_B", B::enum_B)
					.Constant("string_B", "string_B")
				+Class<C>("C")
					.Base<A>()
					.Base<B>()
					.Enum("enum_C", C::enum_C)
					.Constant("string_C", "string_C")
					.Member("value_property", &GetProperty)
					.Member("value_direct", &C::m_Bool)
					.Constant("instance", &obj_inst)
					);

		DOSTRING("assert(_Pi_ == 3.14)")
		DOSTRING("assert(namespace._Pi_ == 3.14)")
		DOSTRING("assert(namespace.anothernamespace._Pi_ == 3.14)")
		DOSTRING("assert(namespace.anothernamespace.String == \"MLuaBind\")")
		DOSTRING("assert(namespace.anothernamespace.Boolean == false)")
		DOSTRING("assert(namespace.A.enum_A == 1)")
		DOSTRING("assert(namespace.B.enum_B == 2)")
		DOSTRING("assert(namespace.C.enum_A == 1)")
		DOSTRING("assert(namespace.C.enum_B == 2)")
		DOSTRING("assert(namespace.C.enum_C == 3)")
		DOSTRING("assert(namespace.A.string_A == \"string_A\")")
		DOSTRING("assert(namespace.B.string_B == \"string_B\")")
		DOSTRING("assert(namespace.C.string_A == \"string_A\")")
		DOSTRING("assert(namespace.C.string_B == \"string_B\")")
		DOSTRING("assert(namespace.C.string_C == \"string_C\")")
		DOSTRING("assert(namespace.C.instance.value_direct == true)")
		DOSTRING("assert(namespace.C.instance.value_property == true)")
		DOSTRING(
			"namespace.C.instance.value_direct = false\n"
			"assert(namespace.C.instance.value_property == false)\n"
			"assert(namespace.C.instance.value_direct == false)" )
		CPPASSERT(obj_inst.m_Bool == false)

		return true;
	};
};

static TestNamespace _;
