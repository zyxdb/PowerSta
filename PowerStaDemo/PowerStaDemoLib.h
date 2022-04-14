#pragma once
typedef void(*CallbackFunction)(PCHAR  pcData, DWORD  dwDataSize);
PVOID psConnectDevice(DWORD  dwComm,DWORD  dwFrameSize, CallbackFunction callbackFunc=NULL);
PVOID psSendData(PVOID	m_pHandleComm, DWORD  dwFrameSize = 8);
PVOID psKeepListen(PVOID m_pHandleComm);
BOOL  psDisconnectDevice(PVOID pHandle);
LONG  psWriteData(PVOID pHandle, PCHAR  pcData, DWORD  dwDataSize);
BOOL  psSetPort(PVOID pHandle, int BaudRate = CBR_9600, int bytesize = 8, int parity = NOPARITY, int stopbits = ONESTOPBIT);
void SendDataThread(LPVOID lpParameter);
