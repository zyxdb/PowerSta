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
//�򿪴���
BOOL Cport::OpenPort(LPCWSTR pcCommName,bool OVERLOOP)
{
	//ͬ�������첽����(OVERLOOPΪNULLʱ��ͬ�����䣻FILE_FLAG_OVERLAPPEDΪ�첽����)
	DWORD style;
	if (OVERLOOP){
		overloop = true;
		style = FILE_FLAG_OVERLAPPED;
	}
	else{
		overloop = false;
		style = NULL;
	}
	//�򿪴���:��������Ϊһ���ļ���������CreateFile()�����򿪴��ڣ����ؽ���洢��m_hCom��
	/*������⣺
	HANDLE WINAPI CreateFile(
	_In_ LPCTSTR lpFileName,                                   Ҫ�򿪵��ļ��������豸��
	_In_ DWORD dwDesiredAccess,                                ָ�����͵ķ��ʶ���,GENERIC_READ ��ʾ������豸���ж�����Ϊ GENERIC_WRITE ��ʾ������豸����д���ʣ������ʹ�ã���
	_In_ DWORD dwShareMode,                                    ����ģʽ�� ��������ʾ������ 
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,       ָ��һ��SECURITY_ATTRIBUTES�ṹ��ָ��
	_In_ DWORD dwCreationDisposition,                          OPEN_EXISTING �ļ������Ѿ����ڡ����豸���Ҫ��
	_In_ DWORD dwFlagsAndAttributes,                           FILE_FLAG_OVERLAPPED ������ļ������ص�����;
	_In_opt_ HANDLE hTemplateFile                              һ��ΪNULL����ʾ��ʹ��ģ�塣
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
//�رմ���
BOOL Cport::ClosePort()
{
	bool value=false;
	m_bRunning = FALSE;
	if (m_hComm && m_hComm !=INVALID_HANDLE_VALUE)
		value=CloseHandle(m_hComm);
	Sleep(300);
	return value;
}

//����DCB��Device Control Block�����������˴���ͨ���豸�Ŀ�������
BOOL Cport::SetPortParm(int BaudRate,int bytesize,int parity,int stopbits)
{
	//���ò�����
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

	Dcb.BaudRate=BaudRate;//��ȡ�����ò����ʲ���
	Dcb.ByteSize=bytesize;//��ȡ����������λ����
    Dcb.Parity=parity;    //��ȡ������У��λ����
    Dcb.StopBits=stopbits;//��ȡ������ֹͣλ����
	if(SetCommState(m_hComm,&Dcb))//���ô��ڲ�����Ϣ��������óɹ�����1��ʧ�ܷ���0
	{
		SetupComm(m_hComm,1024,1024);//��2��3�������ֱ�Ϊ����������д��������С,�˴�Ϊ1024�ֽ�
		COMMTIMEOUTS TimeOuts; //�趨����ʱ
		TimeOuts.ReadIntervalTimeout=100;      /* �������ʱ */
		TimeOuts.ReadTotalTimeoutMultiplier=10;/* ��ʱ��ϵ�� */
		TimeOuts.ReadTotalTimeoutConstant=300; /* ��ʱ�䳣�� */
		TimeOuts.WriteTotalTimeoutMultiplier=7;/* дʱ��ϵ�� */
		TimeOuts.WriteTotalTimeoutConstant=300;/* дʱ�䳣�� */
		SetCommTimeouts(m_hComm,&TimeOuts); //���ó�ʱ
		m_bSetting = FALSE;
		return true;
	}
	else
	{
		m_bSetting = FALSE;
		return false;
	}
}
  

//��������
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
	DWORD fReserved : 25; // reserved   ����
	DWORD cbInQue;        // bytes in input buffer�ó�Ա������ֵ�������뻺�������ֽ���
	DWORD cbOutQue;       // bytes in output buffer��¼��������������ֽ���
	} COMSTAT, *LPCOMSTAT;
	*/
	COMSTAT cs;

	/*
	Windowsϵͳ���ô˺������Ӳ����ͨѶ�����Լ���ȡͨѶ�豸�ĵ�ǰ״̬��ClearCommError�����������£�
	BOOL ClearCommError(
		HANDLE hFile,     //��CreateFile��������ָ���Ѵ򿪴��пڵľ��
		LPDWORD lpErrors, //ָ�����˴������͵�32λ����
		LPCOMSTAT lpStat  //ָ��һ�������豸״̬�Ŀ��ƿ�COMSTAT
	);
	*/
	ClearCommError(m_hComm,&dwErr,&cs);

	/*
	overlapped I/O��WIN32��һ����������Ҫ�����ϵͳΪ�㴫�����ݣ������ڴ������ʱ֪ͨ�㡣�����ʹ��ĳ�����I/O���й�������Ȼ�ܹ�������������
	typedef struct _OVERLAPPED { 
����	  DWORD Internal;      //Ԥ��������ϵͳʹ��
����	  DWORD InternalHigh;  //Ԥ��������ϵͳʹ��
����	  DWORD Offset;        //���ļ���λ���Ǵ��ļ���ʼ�����ֽ�ƫ������
����	  DWORD OffsetHigh;    //ָ���ļ����͵��ֽ�ƫ�����ĸ�λ��
����	  HANDLE hEvent;       //��ת�����ʱ����һ���¼�����Ϊ���ź�״̬
	} OVERLAPPED
	*/
	static	OVERLAPPED ov;
	if (!overloop){
		if(WriteFile(m_hComm,pOutpuData,dwSize,&dwSize,NULL))//ͬ������
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
		CreateEvent��һ��Windows API�������������������һ�������Ļ��������¼�����
		HANDLE  CreateEvent(
			LPSECURITY_ATTRIBUTESlpEventAttributes,// ��ȫ���ԣ�ȷ�����صľ���Ƿ�ɱ��ӽ��̼̳С������NULL���˾�����ܱ��̳С�
			BOOLbManualReset,                      // ��λ��ʽ��ָ�����¼����󴴽����ֶ���ԭ�����Զ���ԭ���������ΪFALSE����һ���ȴ��̱߳��ͷ��Ժ�ϵͳ�����Զ����¼�״̬��ԭΪ���ź�״̬��
			BOOLbInitialState,                     // ��ʼ״̬��ָ���¼�����ĳ�ʼ״̬�����ΪTRUE����ʼ״̬Ϊ���ź�״̬������Ϊ���ź�״̬
			LPCTSTRlpName                          // �������ƣ�ָ���¼��Ķ�������ƣ���һ����0�������ַ���ָ�롣���lpNameΪNULL��������һ���������¼�����
		);
		*/
		ov.hEvent=CreateEvent(NULL,TRUE,FALSE,L"eCommSend");//����һ���¼����󣬽��丳ֵ��os�ṹ�塣�������ͨ�������¼���֪�Ƿ��д��ϡ�

		if(WriteFile(m_hComm,pOutpuData,dwSize,&dwSize,&ov))//�첽����
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

//��������
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
		if(ReadFile(m_hComm,pOutpuData,size,num,NULL)){//���ھ��,�����׵�ַ,Ҫ��ȡ����������ֽ���,�������շ��سɹ���ȡ�������ֽ���,NULLΪͬ�����ͣ�OVERLAPPED*Ϊ�첽����
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


//��������
COMSTAT Cport::ClearInputBuffer()
{
	DWORD	dwError;
	COMSTAT cs;
	PurgeComm(m_hComm,PURGE_RXABORT |PURGE_RXCLEAR);
	ClearCommError(m_hComm, &dwError, &cs);
	return cs;
}