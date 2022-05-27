#include "stdafx.h"
#include "windows.h"
#include "commons.h"
//#include "PowerStaDemoDlg.h"

#include "PowerStaDemoLib.h"

//���ݽ��պ���
LONG lErrorCode = 0;
void ReceiveDataThread(LPVOID lpParameter)
{
	Cport *commPort=(Cport*)lpParameter;
	if (!commPort)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return;
	}
	char* pcDataRev = new char[commPort->m_dwFrameSize];//8��bit
	while (commPort->m_bRunning)
	{
		DWORD dwRet;
		memset(pcDataRev, 0, commPort->m_dwFrameSize);
		LONG XY = commPort->RecData(pcDataRev, commPort->m_dwFrameSize, &dwRet);//�����׵�ַ,Ҫ��ȡ����������ֽ���,�������շ��سɹ���ȡ�������ֽ���
		if (XY && commPort->callbackFunc)
		{
			commPort->callbackFunc(pcDataRev, dwRet);
		}
	}
	delete[] pcDataRev;
}

void SendDataThread(LPVOID lpParameter)
{
	Cport *commPort = (Cport*)lpParameter;
	if (!commPort)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return;
	}
	char pcData[8] = { 0x17,0x06,0x00, 0x00 ,0x00 ,0x00 ,0x1c ,0x04 };
	char* pcDataRev = new char[commPort->m_dwFrameSize];//8��bit
	int count = 0;
	char rec_bit1,rec_bit2;
	while (commPort->m_bRunning)
	{
		DWORD dwRet;
		memset(pcDataRev, 0, commPort->m_dwFrameSize);
		if (count == 0 || rec_bit2 == 0xA1) {
			count = 1;
			commPort->SendData(pcData, 8);
			CString cs = L"���ͣ�";
			for (int i = 0; i < 8; i++)
			{
				cs.AppendFormat(L"%02X ", (unsigned char)pcData[i]);
			}
			cs.Append(L"\n");
			TRACE(cs);
		}
		Sleep(400);
		LONG XY = commPort->RecData(pcDataRev, commPort->m_dwFrameSize, &dwRet);
		if (XY && commPort->callbackFunc)
		{
			rec_bit2 = pcDataRev[1];
			commPort->callbackFunc(pcDataRev, dwRet);
		}
	}
	delete[] pcDataRev;
}

PVOID psSendData(PVOID	m_pHandleComm, DWORD  dwFrameSize){
	Cport *commPort = (Cport*)m_pHandleComm;
	if (!commPort)
	{
		lErrorCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	BOOL 	bResult = FALSE;
	HANDLE  hThread = NULL;
	commPort->m_dwFrameSize = 8;
	/*
	CreateThread(
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //�߳�����
	_In_ SIZE_T dwStackSize,                           //ջ�ռ��С
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //�߳�ִ�к�����ַ
	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //���ݲ���
	_In_ DWORD dwCreationFlags,                        //��־������ѡ�����
	_Out_opt_ LPDWORD lpThreadId                       //�߳�id
	);
	*/
	DWORD dwThreadId = 0; //�߳�id
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendDataThread, commPort, 0, &dwThreadId);
	if (hThread != NULL)//���ΪNULL����ʾ����ʧ��
	{
		//CloseHandle(hThread);
		lErrorCode = ERROR_SUCCESS;
		return  hThread;
	}
	else
	{
		commPort->ClosePort();
		delete commPort;
		lErrorCode = ERROR_UNKNOWN_FEATURE;
		return NULL;
	}
}

PVOID psKeepListen(PVOID m_pHandleComm) {
	Cport *commPort = (Cport*)m_pHandleComm;
	if (!commPort)
	{
		lErrorCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	HANDLE  hThread = NULL;
	commPort->m_dwFrameSize = 8;
	/*
	CreateThread(
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //�߳�����
	_In_ SIZE_T dwStackSize,                           //ջ�ռ��С
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //�߳�ִ�к�����ַ
	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //���ݲ���
	_In_ DWORD dwCreationFlags,                        //��־������ѡ�����
	_Out_opt_ LPDWORD lpThreadId                       //�߳�id
	);
	*/
	DWORD dwThreadId = 1; //�߳�id
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveDataThread, commPort, 0, NULL);
	if (hThread != NULL)//���ΪNULL����ʾ����ʧ��
	{
		//CloseHandle(hThread);
		lErrorCode = ERROR_SUCCESS;
		return  hThread;
	}
	else
	{
		commPort->ClosePort();
		delete commPort;
		lErrorCode = ERROR_UNKNOWN_FEATURE;
		return NULL;
	}
}

PVOID psConnectDevice(DWORD  dwComm, DWORD  dwFrameSize, CallbackFunction callbackFunc)
{
	if (dwComm>64)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return NULL;
	}
	CString pcCommName;
	Cport *commPort = new  Cport;
	if (!commPort)
	{
		lErrorCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	if (dwComm > 9)
	{
		pcCommName.Format(L"\\\\.\\COM%d", dwComm);
	}
	else
	{
		pcCommName.Format(L"COM%d", dwComm);
	}
	BOOL 	bResult=FALSE;
	HANDLE  hThread=NULL;
	commPort->m_dwFrameSize = dwFrameSize;
	commPort->callbackFunc = callbackFunc;
	bResult = commPort->OpenPort((LPCWSTR)pcCommName, false);  //�򿪴���
	return  commPort;
	//if (bResult)
	//{
	//	/* 
	//	CreateThread(
	//	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //�߳�����
	//	_In_ SIZE_T dwStackSize,                           //ջ�ռ��С
	//	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //�߳�ִ�к�����ַ
	//	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //���ݲ���
	//	_In_ DWORD dwCreationFlags,                        //��־������ѡ�����
	//	_Out_opt_ LPDWORD lpThreadId                       //�߳�id
	//	);
	//	*/
	//	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveDataThread, commPort, 0, NULL);
	//}
	//if (hThread != NULL)
	//{
	//	CloseHandle(hThread);
	//	lErrorCode = ERROR_SUCCESS;
	//	return  commPort;
	//}
	//else
	//{
	//	commPort->ClosePort();
	//	delete commPort;
	//	lErrorCode = ERROR_UNKNOWN_FEATURE;
	//	return NULL;
	//}
}


LONG  psWriteData(PVOID pHandle,PCHAR  pcData, DWORD  dwDataSize)
{
	if (!(pHandle && pcData && dwDataSize))
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return 0;
	}
	Cport *commPort = (Cport*)pHandle;

	return commPort->SendData(pcData, dwDataSize);
}

BOOL  psSetPort(PVOID pHandle, int BaudRate, int bytesize, int parity, int stopbits)
{
	if (!pHandle            || 
		BaudRate<CBR_110    || BaudRate>CBR_256000*4 ||
		bytesize<5          || bytesize>8            ||
		parity<NOPARITY     || parity>SPACEPARITY    ||
		stopbits<ONESTOPBIT || stopbits>TWOSTOPBITS)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return FALSE;
	}
	Cport *commPort = (Cport*)pHandle;

	return commPort->SetPortParm(BaudRate,  bytesize, parity, stopbits);
}

BOOL  psDisconnectDevice(PVOID pHandle)
{
	if (!pHandle)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return FALSE;
	}
	Cport *commPort = (Cport*)pHandle;
	commPort->ClosePort();
	delete commPort;
	return TRUE;
}
