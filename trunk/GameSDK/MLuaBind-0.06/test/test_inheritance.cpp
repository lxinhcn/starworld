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

#include <Loki/SmartPtr.h>

namespace
{

struct A{ A():b(777){}; virtual ~A(){}; int b;};
struct B:A{ B():v(666){}; int v; };

Loki::SmartPtr<A> C() { return new A; };
Loki::SmartPtr<A> D() { return new B; };
void F1(A&_) {assert(_.b == 777);};
void F2(B&_) {assert(_.v == 666);};

};

struct TestInheritance : CMLuaBindTestBase
{
	TestInheritance() : CMLuaBindTestBase("Classes inheritance / smart pointers") {};

	bool Run()
	{
		host.Insert(Declare()
			+Function("C", &C)
			+Function("D", &D)
			+Function("F1", &F1)
			+Function("F2", &F2)
			+Class<A>("A")
				.Constructor()
			+Class<B>("B")
				.Base<A>()
				.Constructor()
			+Class<Loki::SmartPtr<A> >()
				.SmartPtr<A>()
			);
		DOSTRING("F1(B())");
		DOSTRING("F2(B())");
		DOSTRING("F1(A())");
		DOSTRINGEXPECT("F2(A())");
		DOSTRING("local a = C()\n"
			"F1(a)");
		DOSTRING("local a = D()\n"
			"F2(__cast(a, B))");
		DOSTRING("local a = D()\n"
			"a = __cast(a, A)\n"
			"a = __cast_unsafe(a, B)\n"
			"F2(a)");

		return true;
	};
};

static TestInheritance _;
