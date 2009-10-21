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

namespace {

class Number
{
public:
	Number():m_Data(0){};
	Number(int _a):m_Data(_a){};

	Number operator + (int _a) const { return Number(m_Data+_a); };
	Number operator - (int _a) const { return Number(m_Data-_a); };
	Number operator / (int _a) { return Number(m_Data/_a); };
	Number operator * (int _a) { return Number(m_Data*_a); };
	int    operator() () const { return m_Data; };

	int GetData() const {return m_Data;};
private:
	int m_Data;
};

Number OperatorUnm(const Number *_n)
{
	return Number(-_n->GetData());
};

Number OperatorMod(Number *_n, int _a)
{
	return Number(_n->GetData()%_a);
};

Number operator+(const Number &_l, const Number &_r)
{
	return Number(_l.GetData() + _r.GetData());
};

Number operator-(const Number &_l, const Number &_r)
{
	return Number(_l.GetData() - _r.GetData());
};

Number operator*(const Number &_l, const Number &_r)
{
	return Number(_l.GetData() * _r.GetData());
};

Number operator/(const Number &_l, const Number &_r)
{
	return Number(_l.GetData() / _r.GetData());
};

Number operator%(const Number &_l, const Number &_r)
{
	return Number(_l.GetData() % _r.GetData());
};

bool operator==(const Number &_l, const Number &_r)
{
	return _l.GetData() == _r.GetData();
};

bool operator<(const Number &_l, const Number &_r)
{
	return _l.GetData() < _r.GetData();
};

bool operator<=(const Number &_l, const Number &_r)
{
	return _l.GetData() <= _r.GetData();
};

}

struct TestOperators : CMLuaBindTestBase
{
	TestOperators() : CMLuaBindTestBase("Object operators") {};

	bool Run()
	{
		host.Insert(
			Class<Number>("Number")
				.Constructor()
				.Constructor<int>()
				.Method("GetData", &Number::GetData)
				// test flexi operator declaration
				.Operator(self       + const_self)
				.Operator(const_self - self)
				.Operator(self       * self)
				.Operator(const_self / const_self)
				.Operator(const_self % const_self)
				.Operator(const_self == const_self)
				.Operator(const_self <= const_self)
				.Operator(const_self < const_self)
				// test method operator declaration
				.Operator(Add, &Number::operator +)
				.Operator(Sub, &Number::operator -)
				.Operator(Mul, &Number::operator *)
				// test free function operator
				.Operator(Div, &Number::operator /)
				.Operator(Mod, &OperatorMod)
				// test paramless free function operator
				.Operator(Unm, &OperatorUnm)
				// test paramless method operator
				.Operator(Call, &Number::operator())
			);

		DOSTRING("assert(Number().GetData() == 0)")
		DOSTRING("assert(Number(10)+Number(20) == Number(30))")
		DOSTRING("assert(Number(10)-Number(20) == Number(-10))")
		DOSTRING("assert(Number(10)*Number(20) == Number(200))")
		DOSTRING("assert(Number(20)/Number(10) == Number(2))")
		DOSTRING("assert(Number(10)%Number(3)  == Number(1))")
		DOSTRING("assert(Number(10) <= Number(10))")
		DOSTRING("assert(Number(10) <  Number(11))")
		DOSTRING("assert(Number(10) >= Number(10))")
		DOSTRING("assert(Number(11) >  Number(10))")
		DOSTRING("assert(Number(11) ~= Number(10))")
		DOSTRING("assert(-Number(5) == Number(-5))")
		DOSTRING("assert(Number(10)+20 == Number(30))")
		DOSTRING("assert(Number(10)-20 == Number(-10))")
		DOSTRING("assert(Number(10)*20 == Number(200))")
		DOSTRING("assert(Number(20)/10 == Number(2))")
		DOSTRING("assert(Number(10)%3  == Number(1))")
		DOSTRING("assert((Number(1)+Number(2))()  == 3)")
		// NOTE: block below ulitizes .Operator(const_self + const_self) section,
		// because of implicit object construction
		DOSTRING("assert(10+Number(20) == Number(30))")
		DOSTRING("assert(10-Number(20) == Number(-10))")
		DOSTRING("assert(10*Number(20) == Number(200))")
		DOSTRING("assert(20/Number(10) == Number(2))")
		DOSTRING("assert(10%Number(3)  == Number(1))")
		// concat operator testing, string classes binded thru stdbinds.cpp
		DOSTRING("assert(std.string(\"abra\") .. std.string(\"cadabra\") == std.string(\"abracadabra\"))")
		DOSTRING("assert(std.string(\"abra\") .. \"cadabra\" == std.string(\"abracadabra\"))")
		DOSTRING("assert(\"abra\" .. std.string(\"cadabra\") == std.string(\"abracadabra\"))")
		DOSTRING("assert(std.wstring(\"abra\") .. std.wstring(\"cadabra\") == std.wstring(\"abracadabra\"))")
		DOSTRING("assert(std.wstring(\"abra\") .. \"cadabra\" == std.wstring(\"abracadabra\"))")
		DOSTRING("assert(\"abra\" .. std.wstring(\"cadabra\") == std.wstring(\"abracadabra\"))")
		DOSTRING("assert((std.string(\"abra\") .. std.string(\"cadabra\")).c_str() == (std.wstring(\"abra\") .. std.wstring(\"cadabra\")).c_str())")
		// __tostring operator testing
		DOSTRING("assert(tostring(std.string(\"abra\")) == \"abra\")")

		return true;
	};
};

static TestOperators _;

