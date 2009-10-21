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

#include <vector>
#include <stack>
#include <queue>
#include <list>

struct TestStlContainersCompile : CMLuaBindTestBase
{
	TestStlContainersCompile() : CMLuaBindTestBase("STL containers compile & general check") {};

	bool Run()
	{
		host.Insert(BindVector<std::vector<int> >("vector_int"));
		host.Insert(BindVector<std::vector<std::string> >("vector_string"));
		host.Insert(BindStack<std::stack<std::string> >("stack_string"));
		host.Insert(BindQueue<std::queue<std::string> >("queue_string"));
		host.Insert(BindList<std::list<std::string> >("list_string"));
		DOSTRING("assert(vector_int(5).size() == 5)")
		DOSTRING("assert(vector_int(5, -1).at(3) == -1)")
		DOSTRING("assert(vector_string(5, \"a\").size() == 5)")
		DOSTRING("assert(vector_string(5, \"a\").at(3) == std.string(\"a\"))")
		DOSTRING("assert(std.string(5, \"a\").size() == 5)")
		DOSTRING("assert(std.string(5, \"a\").at(3) == \"a\")")
		DOSTRING("local s = stack_string()\n"
			"s.push(\"test1\")\n"
			"s.push(\"test2\")\n"
			"assert(s.size() == 2)\n"
			"assert(s.top() == std.string(\"test2\"))\n"
			"s.top().append(\"2\")\n"
			"assert(s.top() == std.string(\"test22\"))")
		DOSTRING("local s = queue_string()\n"
			"s.push(\"test1\")\n"
			"s.push(\"test2\")\n"
			"assert(s.size() == 2)\n"
			"assert(s.front() == std.string(\"test1\"))\n"
			"assert(s.back() == std.string(\"test2\"))")
		DOSTRING("local s = list_string(2, \"test\")\n"
			"assert(s.begin().value == std.string(\"test\"))\n"
			"s.begin().value.assign(\"test2\")\n"
			"assert(s.begin().value == std.string(\"test2\"))\n"
			"local i = s.begin()\n"
			"i.inc()\n"
			"i.inc()\n"
			"assert(i == s._end())")
		return true;
	};
};

static TestStlContainersCompile _;
