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

static std::string TestDummy("test");
std::string StringByVal() { return TestDummy; };
const std::string StringByConstVal() { return TestDummy; };
std::string &StringByRef() { return TestDummy; };
const std::string &StringByConstRef() { return TestDummy; };
std::string *StringByPtr() { return &TestDummy; };
const std::string *StringByConstPtr() { return &TestDummy; };
std::string *StringByPtrAdopt() { return new std::string(TestDummy); };
const std::string *StringByConstPtrAdopt() { return new std::string(TestDummy); };
signed int SignedInt() { return -10; };
unsigned int UnsignedInt() { return 10; };
signed short SignedShort() { return -10; };
unsigned short UnsignedShort() { return 10; };
signed long SignedLong() { return -10; };
unsigned long UnsignedLong() { return 10; };
float Float() { return 10.01f; };
double Double() { return 10.01; };
long double LongDouble() { return 10.01; };
char *CharP() { return "Abra"; };
const char *ConstCharP() { return "Abra"; };
signed char *SignedCharP() { return (signed char*)"Abra"; };
unsigned char *UnsignedCharP() { return (unsigned char*)"Abra"; };
const signed char *ConstSignedCharP() { return (signed char*)"Abra"; };
const unsigned char *ConstUnsignedCharP() {return (unsigned char*)"Abra"; };
wchar_t *Wchar_tP() { return L"Abra"; };
const wchar_t *ConstWchar_tP() { return L"Abra"; };
char Char() { return 'A'; };
signed char SignedChar() { return (signed char)'A'; };
unsigned char UnsignedChar() { return (unsigned char)'A'; };
wchar_t Wchar_t() { return L'A'; };
bool Bool() { return true; };

}

#define FUNCTION(a) +Function(#a, &a)

struct TestParams: CMLuaBindTestBase
{
	TestParams() : CMLuaBindTestBase("Parameters passing") {};

	bool Run()
	{
		host.Insert(Declare()
			FUNCTION(StringByVal)
			FUNCTION(StringByConstVal)
			FUNCTION(StringByRef)
			FUNCTION(StringByConstRef)
			FUNCTION(StringByPtr)
			FUNCTION(StringByConstPtr)
			+Function("StringByPtrAdopt", &StringByPtrAdopt, Adopt(-1))
			+Function("StringByConstPtrAdopt", &StringByConstPtrAdopt, Adopt(-1))
			FUNCTION(SignedInt)
			FUNCTION(UnsignedInt)
			FUNCTION(SignedShort)
			FUNCTION(UnsignedShort)
			FUNCTION(SignedLong)
			FUNCTION(UnsignedLong)
			FUNCTION(Float)
			FUNCTION(Double)
			FUNCTION(LongDouble)
			FUNCTION(CharP)
			FUNCTION(ConstCharP)
			FUNCTION(SignedCharP)
			FUNCTION(UnsignedCharP)
			FUNCTION(ConstSignedCharP)
			FUNCTION(ConstUnsignedCharP)
			FUNCTION(Wchar_tP)
			FUNCTION(ConstWchar_tP)
			FUNCTION(Char)
			FUNCTION(SignedChar)
			FUNCTION(UnsignedChar)
			FUNCTION(Wchar_t)
			FUNCTION(Bool)
			);

		DOSTRING("local v = StringByVal()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string owned\")");

		DOSTRING("local v = StringByConstVal()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string const owned\")\n");

		DOSTRINGEXPECT("local v = StringByConstVal()\n"
			"v.assign(\"\")"); // should generate an error - call a non-const method at const object

		DOSTRING("local v = StringByRef()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string\")");

		DOSTRING("local v = StringByConstRef()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string const\")\n");

		DOSTRINGEXPECT("local v = StringByConstRef()\n"
			"v.assign(\"\")"); // should generate an error - call a non-const method at const object

		DOSTRING("local v = StringByPtr()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string\")");

		DOSTRING("local v = StringByConstPtr()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string const\")\n");

		DOSTRINGEXPECT("local v = StringByConstPtr()\n"
			"v.assign(\"\")"); // should generate an error - call a non-const method at const object

		DOSTRING("local v = StringByPtrAdopt()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string owned\")");

		DOSTRING("local v = StringByConstPtrAdopt()\n"
			"assert(v.c_str() == \"test\")\n"
			"assert(__get_typeex(v) == \"std.string const owned\")\n");

		DOSTRINGEXPECT("local v = StringByConstPtrAdopt()\n"
			"v.assign(\"\")"); // should generate an error - call a non-const method at const object

		DOSTRING("assert(SignedInt() == -10)");
		DOSTRING("assert(UnsignedInt() == 10)");
		DOSTRING("assert(SignedShort() == -10)");
		DOSTRING("assert(UnsignedShort() == 10)");
		DOSTRING("assert(SignedLong() == -10)");
		DOSTRING("assert(UnsignedLong() == 10)");
		DOSTRING("assert(std.abs(Float() - 10.01) < 0.000001)");
		DOSTRING("assert(std.abs(Double() - 10.01) < 0.000001)");
		DOSTRING("assert(std.abs(LongDouble() - 10.01) < 0.000001)");

		DOSTRING("assert(CharP() == \"Abra\")")
		DOSTRING("assert(ConstCharP() == \"Abra\")")
		DOSTRING("assert(SignedCharP() == \"Abra\")")
		DOSTRING("assert(UnsignedCharP() == \"Abra\")")
		DOSTRING("assert(ConstSignedCharP() == \"Abra\")")
		DOSTRING("assert(ConstUnsignedCharP() == \"Abra\")")
		DOSTRING("assert(Wchar_tP() == \"Abra\")")
		DOSTRING("assert(ConstWchar_tP() == \"Abra\")")
		DOSTRING("assert(Char() == \"A\")")
		DOSTRING("assert(SignedChar() == \"A\")")
		DOSTRING("assert(UnsignedChar() == \"A\")")
		DOSTRING("assert(Wchar_t() == \"A\")")

		DOSTRING("assert(Bool() == true)")

		return true;
	};
};

static TestParams _;
