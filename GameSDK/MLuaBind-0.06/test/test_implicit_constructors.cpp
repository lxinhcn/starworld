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

struct A
{
	A(int _a): val(_a) {};
	int val;
};

const char *AcceptStdString(const std::string& _str)
{
	return _str.c_str();
};

int AcceptA(const A& _a)
{
	return _a.val;
};

struct B_Base
{
	B_Base(int _val):val(_val){};
	int Get() const { return val; };
private:
	int val;
};

struct B_ConstRef:B_Base { B_ConstRef(const A &_a): B_Base(_a.val)  {}; };
struct B_ConstPtr:B_Base {	B_ConstPtr(const A *_a): B_Base(_a->val) {};};
struct B_Ref:B_Base      {	B_Ref(A &_a): B_Base(_a.val) {}; };
struct B_Ptr:B_Base      {	B_Ptr(A *_a): B_Base(_a->val) {}; };
struct B_Val:B_Base      {	B_Val(A  _a): B_Base(_a.val) {}; };
struct B_ConstVal:B_Base {	B_ConstVal(const A _a): B_Base(_a.val) {}; };

};

#define FUNCTION(a) +Function(#a, &a)

struct TestImplicitConstructors: CMLuaBindTestBase
{
	TestImplicitConstructors() : CMLuaBindTestBase("Object implicit construction") {};

	bool Run()
	{
		host.Insert(Declare()
			FUNCTION(AcceptStdString)
			FUNCTION(AcceptA)
			+Class<A>("A")
				.Constructor<int>()
			+Class<B_Base>()
				.Method("Get", &B_Base::Get)
			+Class<B_ConstRef>("B_ConstRef")
				.Constructor<const A&>()
				.BaseNoDynCast<B_Base>()
			+Class<B_ConstPtr>("B_ConstPtr")
				.Constructor<const A*>()
				.BaseNoDynCast<B_Base>()
			+Class<B_Ref>("B_Ref")
				.Constructor<A&>()
				.BaseNoDynCast<B_Base>()
			+Class<B_Ptr>("B_Ptr")
				.Constructor<A*>()
				.BaseNoDynCast<B_Base>()
			+Class<B_Val>("B_Val")
				.Constructor<A>()
				.BaseNoDynCast<B_Base>()
			+Class<B_ConstVal>("B_ConstVal")
				.Constructor<A>()
				.BaseNoDynCast<B_Base>()
			);

		// most simple implicit construction - pass lua string as a paramer,
		// mluabind will find basic_string(value_type *) construction and implicilty
		// create parameter object
		DOSTRING("assert(AcceptStdString(\"test\") == \"test\")");

		// class A created from int.
		DOSTRING("assert(AcceptA(666) == 666)");


		// test implicit object creation from another object
		DOSTRING("assert(B_ConstRef(A(666)).Get() == 666)"); // pass by const &
		DOSTRING("assert(B_ConstPtr(A(666)).Get() == 666)"); // pass by const *
		DOSTRING("assert(B_Ref(A(666)).Get() == 666)"); // pass by &
		DOSTRING("assert(B_Ptr(A(666)).Get() == 666)"); // pass by *
		DOSTRING("assert(B_Val(A(666)).Get() == 666)"); // pass by val
		DOSTRING("assert(B_ConstVal(A(666)).Get() == 666)"); // pass by const val

		return true;
	};
};

static TestImplicitConstructors _;
