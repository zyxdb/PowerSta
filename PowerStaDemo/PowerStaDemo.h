
// PowerStaDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPowerStaDemoApp:
// �йش����ʵ�֣������ PowerStaDemo.cpp
//

class CPowerStaDemoApp : public CWinApp
{
public:
	CPowerStaDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPowerStaDemoApp theApp;


#define THREAD_SELFCHECK_STATUS 0  
#define THREAD_MEASURE_STATUS   1  
#define THREAD_MEASURE_DATA     2  