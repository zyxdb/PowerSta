
// PowerStaDemo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CPowerStaDemoApp:
// 有关此类的实现，请参阅 PowerStaDemo.cpp
//

class CPowerStaDemoApp : public CWinApp
{
public:
	CPowerStaDemoApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPowerStaDemoApp theApp;


#define THREAD_SELFCHECK_STATUS 0  
#define THREAD_MEASURE_STATUS   1  
#define THREAD_MEASURE_DATA     2  