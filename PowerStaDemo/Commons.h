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
	DWORD m_dwFrameSize;    //������Ϣ
	HANDLE m_hComm;    //���ڱ�־
	DCB Dcb;          //�������ýṹ��
	bool m_bRunning;
	bool m_bSetting;
	bool m_bReading;
	bool overloop;
	CallbackFunction callbackFunc;
public:
	BOOL OpenPort(LPCWSTR pcCommName,bool OVERLOOP);   //�򿪴���
	BOOL ClosePort();                                            //�رմ���
	BOOL SetPortParm(int BaudRate=CBR_9600,int bytesize=8,
			int parity=NOPARITY ,int stopbits=ONESTOPBIT);       //����DCB
	LONG SendData(char* pOutpuData,DWORD dwSize);                //��������
	LONG RecData (char* pOutpuData,int size,DWORD* num);         //��������
	COMSTAT ClearInputBuffer();
	bool drawing = false;
};

#endif