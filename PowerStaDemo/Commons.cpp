#pragma once
#include "stdafx.h"
#include "Commons.h"


Cport::Cport()
{
 	overloop=false;
	m_hComm =NULL;
	m_bRunning=FALSE;
	m_bSetting=FALSE;
	m_bReading = FALSE;
}

Cport::~Cport()
{
}
//打开串口
BOOL Cport::OpenPort(LPCWSTR pcCommName,bool OVERLOOP)
{
	//同步或者异步传输(OVERLOOP为NULL时，同步传输；FILE_FLAG_OVERLAPPED为异步传输)
	DWORD style;
	if (OVERLOOP){
		overloop = true;
		style = FILE_FLAG_OVERLAPPED;
	}
	else{
		overloop = false;
		style = NULL;
	}
	//打开串口:将串口作为一个文件来看，用CreateFile()函数打开串口，返回结果存储在m_hCom中
	/*参数详解：
	HANDLE WINAPI CreateFile(
	_In_ LPCTSTR lpFileName,                                   要打开的文件的名或设备名
	_In_ DWORD dwDesiredAccess,                                指定类型的访问对象,GENERIC_READ 表示允许对设备进行读访问为 GENERIC_WRITE 表示允许对设备进行写访问（可组合使用）。
	_In_ DWORD dwShareMode,                                    共享模式， 如果是零表示不共享； 
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,       指向一个SECURITY_ATTRIBUTES结构的指针
	_In_ DWORD dwCreationDisposition,                          OPEN_EXISTING 文件必须已经存在。由设备提出要求
	_In_ DWORD dwFlagsAndAttributes,                           FILE_FLAG_OVERLAPPED 允许对文件进行重叠操作;
	_In_opt_ HANDLE hTemplateFile                              一般为NULL，表示不使用模板。
	);
	*/
	m_hComm = CreateFile(pcCommName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,style,NULL);
	if (m_hComm ==INVALID_HANDLE_VALUE){
		return false;
	}
	GetCommState(m_hComm,&Dcb);
	m_bRunning = TRUE;
	return true;
}
//关闭串口
BOOL Cport::ClosePort()
{
	bool value=false;
	m_bRunning = FALSE;
	if (m_hComm && m_hComm !=INVALID_HANDLE_VALUE)
		value=CloseHandle(m_hComm);
	Sleep(300);
	return value;
}

//设置DCB（Device Control Block）——定义了串口通信设备的控制设置
BOOL Cport::SetPortParm(int BaudRate,int bytesize,int parity,int stopbits)
{
	//设置波特率
	m_bSetting = TRUE;
	int i;
	for (i=0;i<10;i++)
	{
		if (m_bReading)
		{
			ClearInputBuffer();
			Sleep(300);
		}
		else break;
	}
	if (i >= 10) return FALSE;

	Dcb.BaudRate=BaudRate;//读取并设置波特率参数
	Dcb.ByteSize=bytesize;//读取并设置数据位参数
    Dcb.Parity=parity;    //读取并设置校验位参数
    Dcb.StopBits=stopbits;//读取并设置停止位参数
	if(SetCommState(m_hComm,&Dcb))//设置串口参数信息，如果设置成功返回1，失败返回0
	{
		SetupComm(m_hComm,1024,1024);//第2、3个参数分别为读缓存区和写缓存区大小,此处为1024字节
		COMMTIMEOUTS TimeOuts; //设定读超时
		TimeOuts.ReadIntervalTimeout=100;      /* 读间隔超时 */
		TimeOuts.ReadTotalTimeoutMultiplier=10;/* 读时间系数 */
		TimeOuts.ReadTotalTimeoutConstant=300; /* 读时间常量 */
		TimeOuts.WriteTotalTimeoutMultiplier=7;/* 写时间系数 */
		TimeOuts.WriteTotalTimeoutConstant=300;/* 写时间常量 */
		SetCommTimeouts(m_hComm,&TimeOuts); //设置超时
		m_bSetting = FALSE;
		return true;
	}
	else
	{
		m_bSetting = FALSE;
		return false;
	}
}
  

//发送数据
LONG Cport::SendData(char* pOutpuData,DWORD dwSize)
{
	if(!m_hComm) return -1;
	DWORD dwErr;

	/*
	typedef struct _COMSTAT { // cst
	DWORD fCtsHold : 1;   // Tx waiting for CTS signal
	DWORD fDsrHold : 1;   // Tx waiting for DSR signal
	DWORD fRlsdHold : 1;  // Tx waiting for RLSD signal
	DWORD fXoffHold : 1;  // Tx waiting, XOFF char rec''d
	DWORD fXoffSent : 1;  // Tx waiting, XOFF char sent
	DWORD fEof : 1;       // EOF character sent
	DWORD fTxim : 1;      // character waiting for Tx
	DWORD fReserved : 25; // reserved   保留
	DWORD cbInQue;        // bytes in input buffer该成员变量的值代表输入缓冲区的字节数
	DWORD cbOutQue;       // bytes in output buffer记录着输出缓冲区中字节数
	} COMSTAT, *LPCOMSTAT;
	*/
	COMSTAT cs;

	/*
	Windows系统利用此函数清除硬件的通讯错误以及获取通讯设备的当前状态，ClearCommError函数声明如下：
	BOOL ClearCommError(
		HANDLE hFile,     //由CreateFile函数返回指向已打开串行口的句柄
		LPDWORD lpErrors, //指向定义了错误类型的32位变量
		LPCOMSTAT lpStat  //指向一个返回设备状态的控制块COMSTAT
	);
	*/
	ClearCommError(m_hComm,&dwErr,&cs);

	/*
	overlapped I/O是WIN32的一项技术，你可以要求操作系统为你传送数据，并且在传送完毕时通知你。这项技术使你的程序在I/O进行过程中仍然能够继续处理事务。
	typedef struct _OVERLAPPED { 
　　	  DWORD Internal;      //预留给操作系统使用
　　	  DWORD InternalHigh;  //预留给操作系统使用
　　	  DWORD Offset;        //该文件的位置是从文件起始处的字节偏移量。
　　	  DWORD OffsetHigh;    //指定文件传送的字节偏移量的高位字
　　	  HANDLE hEvent;       //在转移完成时处理一个事件设置为有信号状态
	} OVERLAPPED
	*/
	static	OVERLAPPED ov;
	if (!overloop){
		if(WriteFile(m_hComm,pOutpuData,dwSize,&dwSize,NULL))//同步发送
		{
			return dwSize;
		}
		else{
			PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
			return 0;
		}
	}
	else{
		/*
		CreateEvent是一个Windows API函数。它用来创建或打开一个命名的或无名的事件对象。
		HANDLE  CreateEvent(
			LPSECURITY_ATTRIBUTESlpEventAttributes,// 安全属性，确定返回的句柄是否可被子进程继承。如果是NULL，此句柄不能被继承。
			BOOLbManualReset,                      // 复位方式，指定将事件对象创建成手动复原还是自动复原。如果设置为FALSE，当一个等待线程被释放以后，系统将会自动将事件状态复原为无信号状态。
			BOOLbInitialState,                     // 初始状态，指定事件对象的初始状态。如果为TRUE，初始状态为有信号状态；否则为无信号状态
			LPCTSTRlpName                          // 对象名称，指定事件的对象的名称，是一个以0结束的字符串指针。如果lpName为NULL，将创建一个无名的事件对象
		);
		*/
		ov.hEvent=CreateEvent(NULL,TRUE,FALSE,L"eCommSend");//创建一个事件对象，将其赋值给os结构体。程序可以通过检查该事件得知是否读写完毕。

		if(WriteFile(m_hComm,pOutpuData,dwSize,&dwSize,&ov))//异步发送
		{
			return dwSize;
		}
		else
		{
			if(ERROR_IO_PENDING==(dwErr=GetLastError()))
			{
				WaitForSingleObject(ov.hEvent, INFINITE);
				ResetEvent(ov.hEvent);
				return ov.InternalHigh;
			}
		}
		return 0;
	}
}

//接收数据
LONG Cport::RecData(char* pOutpuData,int size,DWORD* num)
{
	DWORD	nByteRead=size;
	DWORD	dwEvent,dwError;
	COMSTAT cs;
	static	OVERLAPPED ov;
	if (m_bSetting)
	{
		m_bReading = FALSE;
		Sleep(300);
		return 0;
	}
	m_bReading = TRUE;

	if (!overloop){
		if(ReadFile(m_hComm,pOutpuData,size,num,NULL)){//串口句柄,数据首地址,要读取的数据最大字节数,用来接收返回成功读取的数据字节数,NULL为同步发送，OVERLAPPED*为异步发送
			m_bReading = FALSE;
			return *num;
		}
		else{
			PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
			ClearCommError(m_hComm,&dwError,&cs);
			m_bReading = FALSE;
			return 0;
		}
	}
	else{

		DWORD dwErr;
		//ReadFile(handle,pOutpuData,size,&nByteRead,&ov);
		ReadFile(m_hComm,pOutpuData,nByteRead,&nByteRead,&ov);
		if (!nByteRead)
		{
			if(ERROR_IO_PENDING==(dwErr=GetLastError()))
			{
				ClearCommError(m_hComm,&dwError,&cs);
				WaitForSingleObject(ov.hEvent, INFINITE);
				ResetEvent(ov.hEvent);
				m_bReading = FALSE;
				return nByteRead;
			}
		}
		else
		{
			return nByteRead;
			m_bReading = FALSE;
		}
		ClearCommError(m_hComm,&dwError,&cs);
		m_bReading = FALSE;
		return 0;
	}
}


//接收数据
COMSTAT Cport::ClearInputBuffer()
{
	DWORD	dwError;
	COMSTAT cs;
	PurgeComm(m_hComm,PURGE_RXABORT |PURGE_RXCLEAR);
	ClearCommError(m_hComm, &dwError, &cs);
	return cs;
}