#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <WinBase.h>
#include <afxwin.h>

#ifndef _COMMONS_H
#define _COMMONS_H
typedef void(*CallbackFunction)(PCHAR  pcData, DWORD  dwDataSize);
class Cport
{
public:
	Cport();
	~Cport();
public:
	DWORD m_dwFrameSize;    //发送消息
	HANDLE m_hComm;    //串口标志
	DCB Dcb;          //串口配置结构体
	bool m_bRunning;
	bool m_bSetting;
	bool m_bReading;
	bool overloop;
	CallbackFunction callbackFunc;
public:
	BOOL OpenPort(LPCWSTR pcCommName,bool OVERLOOP);   //打开串口
	BOOL ClosePort();                                            //关闭串口
	BOOL SetPortParm(int BaudRate=CBR_9600,int bytesize=8,
			int parity=NOPARITY ,int stopbits=ONESTOPBIT);       //设置DCB
	LONG SendData(char* pOutpuData,DWORD dwSize);                //发送数据
	LONG RecData (char* pOutpuData,int size,DWORD* num);         //接受数据
	COMSTAT ClearInputBuffer();
	bool drawing = false;
};

#endif