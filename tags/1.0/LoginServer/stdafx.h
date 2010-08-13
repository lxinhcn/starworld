// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <vector>

#include "Commonlib.h"
#include "Netbase.h"
#include "Database.h"
#include "TransDefine.h"

//#include "C2S_LogonServer.h"
//#include "S2C_LogonServer.h"
// TODO: 在此处引用程序需要的其他头文件
#define LOG _T("LoginServerLog")
using namespace XGC;