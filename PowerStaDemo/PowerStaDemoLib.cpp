#include "stdafx.h"
#include "windows.h"
#include "commons.h"
#include "PowerStaDemo.h"
#include "PowerStaDemoDlg.h"
#include "PowerStaDemoLib.h"

double rcv_data;
void callbackFunc(CPowerStaDemoDlg *pPSDlg, PCHAR  pcData, DWORD  dwDataSize)
{
	if (pPSDlg == nullptr || pcData == nullptr) return;
	if (pcData[0] == 0x0D || pcData[0] == 0x0C || pcData[0] == 0x0B) {
		if (pcData[4] == 0x00) {
			if (pcData[0] == 0x0D) pPSDlg->ButtonOn3 = false;
			else if (pcData[0] == 0x0C) pPSDlg->ButtonOn2 = false;
			else if (pcData[0] == 0x0B) pPSDlg->ButtonOn1 = false;
		}
		else {
			if (pcData[0] == 0x0D) pPSDlg->ButtonOn3 = true;
			else if (pcData[0] == 0x0C) pPSDlg->ButtonOn2 = true;
			else if (pcData[0] == 0x0B) pPSDlg->ButtonOn1 = true;
		}
		return;
	}
	if (pcData[0] != 0x01) return;
	//if (pcData[1] != 0xa1) return;
	CString cs = L"接收：";
	for (int i = 0; i < dwDataSize; i++)
	{
		cs.AppendFormat(L"%02X ", (unsigned char)pcData[i]);
	}
	cs.Append(L"\n");
	TRACE(cs);
	if (pPSDlg)
	{
		CString str;
		str.Format(L"%02X%02X%02X", (unsigned char)pcData[3], (unsigned char)pcData[4], (unsigned char)pcData[5]);//合并3个char
		USES_CONVERSION;
		string s(W2A(str));
		const char* cstr = s.c_str();
		int nTemp = (int)strtol(cstr, NULL, 16);//cstring转const char*
		rcv_data = nTemp * 5 / exp2(23);
	}
}


//数据接收函数
LONG lErrorCode = 0;
void ReceiveDataThread(LPVOID lpParameter)
{
	CPowerStaDemoDlg* pdlgMain = (CPowerStaDemoDlg *)lpParameter;
	Cport *commPort = (Cport*)pdlgMain->m_pHandleComm;
	CString csMsg;
	if (!commPort)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return;
	}
	char pcData[8] = { 0x17,0x06,0x00, 0x00 ,0x00 ,0x00 ,0x1c ,0x04 };
	char* pcDataRev = new char[commPort->m_dwFrameSize];//8个bit
	while (commPort->m_bRunning)
	{
		if (commPort->drawing) {
			commPort->SendData(pcData, 8);
			Sleep(400);
		}
		DWORD dwRet;
		memset(pcDataRev, 0, 8);
		LONG XY = commPort->RecData(pcDataRev, commPort->m_dwFrameSize, &dwRet);//数据首地址,要读取的数据最大字节数,用来接收返回成功读取的数据字节数
		if (XY)
		{
			callbackFunc(pdlgMain,pcDataRev, dwRet);
		}
		pdlgMain->currRevData = rcv_data;
		//Sleep(400);

		if (commPort->drawing && pdlgMain->m_bPowerstaConnected && rcv_data)
		{
			//pdlgMain->m_count++;
			//pdlgMain->m_chartctrl.EnableRefresh(false);
			csMsg.Format(L"绘图中...\n");
			pdlgMain->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);

			pdlgMain->PostMessage(WM_THREAD, THREAD_MEASURE_DATA);

			// 显示实时功率
			pdlgMain->avg = 0;
			pdlgMain->DC = 0;
			CString cs = L"";
			cs.Format(L"%.6f", rcv_data);//实时功率 
			pdlgMain->m_cStaticPower.SetWindowText(cs);

			// 显示平均值
			pdlgMain->m_HightSpeedChartArray_sum += rcv_data;
			pdlgMain->avg = pdlgMain->m_HightSpeedChartArray_sum / (pdlgMain->m_count + 1);
			cs.Format(L"%.6f", pdlgMain->avg);
			pdlgMain->m_cStaticPowerAvg.SetWindowText(cs);

			//计算和显示方差
			pdlgMain->DCsum = 0;
			for (int i = 0; i <= pdlgMain->m_count; i++)
			{
				pdlgMain->DCsum += pow(pdlgMain->m_HightSpeedChartArray[i] - pdlgMain->avg, 2);
			}
			pdlgMain->DC = pow((pdlgMain->DCsum / (pdlgMain->m_count + 1)), 0.5);
			cs.Format(L"%.6f", pdlgMain->DC);
			pdlgMain->m_cStaticPowerDC.SetWindowText(cs);

			//计算和显示比值
			double scale = pdlgMain->DC / pdlgMain->avg;
			cs.Format(L"%.6f", scale);
			pdlgMain->m_cStaticPowerScale.SetWindowText(cs);
		}
		else {
			csMsg.Format(L"未接收到功率数值！\n");
			pdlgMain->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		}
		if (pdlgMain->m_count == 14399) {
			pdlgMain->OnBnClickedButtonDataSave();
			pdlgMain->OnBnClickedButton1();
		}
	}
	delete[] pcDataRev;
}

/*
/**
* @author hlf
* @description 2022/6/22 第二个线程已弃用。

void SendDataThread(LPVOID lpParameter)
{
	Cport *commPort = (Cport*)lpParameter;
	if (!commPort)
	{
		lErrorCode = ERROR_INVALID_PARAMETER;
		return;
	}
	//char pcData[8] = { 0x17,0x06,0x00, 0x00 ,0x00 ,0x00 ,0x1c ,0x04 };
	char* pcDataRev = new char[commPort->m_dwFrameSize];//8个bit
	//int count = 0;
	//char rec_bit1,rec_bit2 = 0xA1;
	while (commPort->m_bRunning)
	{
		DWORD dwRet;
		memset(pcDataRev, 0, commPort->m_dwFrameSize);
		//count = 1;
		//commPort->SendData(pcData, 8);
		//CString cs = L"发送：";
		//for (int i = 0; i < 8; i++)
		//{
		//	cs.AppendFormat(L"%02X ", (unsigned char)pcData[i]);
		//}
		//cs.Append(L"\n");
		//TRACE(cs);
		Sleep(400);
		LONG XY = commPort->RecData(pcDataRev, commPort->m_dwFrameSize, &dwRet);
		if (XY && commPort->callbackFunc)
		{
			//rec_bit2 = pcDataRev[1];
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
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //线程属性
	_In_ SIZE_T dwStackSize,                           //栈空间大小
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //线程执行函数地址
	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //传递参数
	_In_ DWORD dwCreationFlags,                        //标志，可以选择挂起
	_Out_opt_ LPDWORD lpThreadId                       //线程id
	);

	DWORD dwThreadId = 0; //线程id
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendDataThread, commPort, 0, &dwThreadId);
	if (hThread != NULL)//如果为NULL，表示创建失败
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

*/

PVOID psKeepListen(PVOID m_pHandleComm) {
	CPowerStaDemoDlg* pdlgMain = (CPowerStaDemoDlg *)m_pHandleComm;
	Cport *commPort = (Cport*)pdlgMain->m_pHandleComm;
	if (!commPort)
	{
		lErrorCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	HANDLE  hThread = NULL;
	commPort->m_dwFrameSize = 8;
	/*
	CreateThread(
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //线程属性
	_In_ SIZE_T dwStackSize,                           //栈空间大小
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //线程执行函数地址
	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //传递参数
	_In_ DWORD dwCreationFlags,                        //标志，可以选择挂起
	_Out_opt_ LPDWORD lpThreadId                       //线程id
	);
	*/
	DWORD dwThreadId = 1; //线程id
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveDataThread, pdlgMain, 0, &dwThreadId);
	if (hThread != NULL)//如果为NULL，表示创建失败
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
	commPort->drawing = false;
	bResult = commPort->OpenPort((LPCWSTR)pcCommName, false);  //打开串口
	return  commPort;
	//if (bResult)
	//{
	//	/* 
	//	CreateThread(
	//	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes, //线程属性
	//	_In_ SIZE_T dwStackSize,                           //栈空间大小
	//	_In_ LPTHREAD_START_ROUTINE lpStartAddress,        //线程执行函数地址
	//	_In_opt_ __drv_aliasesMem LPVOID lpParameter,      //传递参数
	//	_In_ DWORD dwCreationFlags,                        //标志，可以选择挂起
	//	_Out_opt_ LPDWORD lpThreadId                       //线程id
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
