#pragma once
// PowerStaDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PowerStaDemo.h"
#include "PowerStaDemoLib.h"
#include "PowerStaDemoDlg.h"
#include "afxdialogex.h"
#include "Commons.h"
#include <string>
#include <cmath>
#include<iostream> 
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartTitle.h"
#include "ChartCtrl\ChartLineSerie.h"
#include "ChartCtrl\ChartAxisLabel.h"
#include "commons.h"
#include <vector>  
#include <fstream>  
#include <sstream>  
#include "handleapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
CPowerStaDemoDlg *pPSDlg = NULL;

int i_count=0;
double rcv_data;
void callbackFunc(PCHAR  pcData, DWORD  dwDataSize)
{
	//int i;
	//double sum, avg, DCsum, DC;
	//sum = avg = DCsum = DC =  0;
	//double y_sum[10000];
	CString cs=L"接收：";
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
		std::string s(W2A(str));
		const char* cstr = s.c_str();
		int nTemp = (int)strtol(cstr, NULL, 16);//cstring转const char*
		rcv_data = nTemp * 5 / exp2(23);
		//y_sum[i_count] = y;
		//float *p = (float *)(pcData + 2);
		//DWORD *pdwParam = (DWORD *)(pcData + 2); *p * 5 / exp2(23)
		//cs.Format(L"%.6f", y);
		//pPSDlg->m_cStaticPower.SetWindowText(cs);
		//for (i = 0; i < i_count; i++)//计算均值
		//{
		//	sum += y_sum[i];
		//}
		//if (i_count > 0) {
		//	avg = sum / i_count;
		//}
		//cs.Format(L"%.4f", avg);
		//pPSDlg->m_cStaticPowerAvg.SetWindowText(cs);
		//for (i = 1; i < i_count; i++)//计算方差
		//{
		//	DCsum += pow(y_sum[i] - avg, 2);
		//}
		//if (i_count > 0) {
		//	DC = pow((DCsum / i_count), 0.5);
		//}
		//cs.Format(L"%.4f", DC);
		//pPSDlg->m_cStaticPowerDC.SetWindowText(cs);
		//i_count++;
	}
}



// CPowerStaDemoDlg 对话框

CPowerStaDemoDlg::CPowerStaDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPowerStaDemoDlg::IDD, pParent)
	//, m_fParam1(5)
	, m_fParam2(10)
	, m_fParam3(0)
	, m_dwParam4(0)
	, m_fPowerSetting(0)
	, m_c_arrayLength(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bPowerstaConnected = false;
}

CPowerStaDemoDlg::~CPowerStaDemoDlg()
{
	//释放资源
}

void CPowerStaDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_POWERSTA_COMM, m_cComboPowerstaCommCtl);
	DDX_Control(pDX, IDC_STATIC_POWER_TITLE, m_cStaticPower);
	//  DDX_Text(pDX, IDC_EDIT_PARAM_1, m_fParam1);
	DDX_Text(pDX, IDC_EDIT_PARAM_2, m_fParam2);
	DDX_Text(pDX, IDC_EDIT_PARAM_3, m_fParam3);
	DDX_Text(pDX, IDC_EDIT_PARAM_4, m_dwParam4);
	DDX_Text(pDX, IDC_EDIT_POWER_SET, m_fPowerSetting);
	//DDX_Control(pDX, IDC_ChartCtrl1, m_chartctrl);
	DDX_Control(pDX, IDC_STATIC_average, m_cStaticPowerAvg);
	DDX_Control(pDX, IDC_STATIC_DC, m_cStaticPowerDC);
}

BEGIN_MESSAGE_MAP(CPowerStaDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
//	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PARAM_1, &CPowerStaDemoDlg::OnDeltaposSpinParam_1)
	ON_STN_CLICKED(IDC_STATIC_POWERSTA_CONNECT, &CPowerStaDemoDlg::OnStaticClickedPowerStaConnect)
	ON_STN_DBLCLK(IDC_STATIC_POWERSTA_CONNECT, &CPowerStaDemoDlg::OnStaticClickedPowerStaConnect)
	ON_BN_CLICKED(IDC_BUTTON_POWER_SET, &CPowerStaDemoDlg::OnBnClickedButtonPowerSet)
	ON_BN_CLICKED(IDC_BUTTON_POWERSTA_PARAM_SETTING, &CPowerStaDemoDlg::OnBnClickedButtonPowerstaParamSetting)
//	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CPowerStaDemoDlg::OnBnClickedButtonDraw)
ON_BN_CLICKED(IDC_BUTTON1, &CPowerStaDemoDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CPowerStaDemoDlg::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON_DATA_SAVE, &CPowerStaDemoDlg::OnBnClickedButtonDataSave)
ON_WM_GETMINMAXINFO()
ON_BN_CLICKED(IDC_BUTTON_ON2, &CPowerStaDemoDlg::OnBnClickedButtonOn2)
ON_BN_CLICKED(IDC_BUTTON_OFF2, &CPowerStaDemoDlg::OnBnClickedButtonOff2)
ON_BN_CLICKED(IDC_BUTTON_ON1, &CPowerStaDemoDlg::OnBnClickedButtonOn1)
ON_BN_CLICKED(IDC_BUTTON_OFF1, &CPowerStaDemoDlg::OnBnClickedButtonOff1)
ON_BN_CLICKED(IDC_BUTTON_OFF3, &CPowerStaDemoDlg::OnBnClickedButtonOff3)
ON_BN_CLICKED(IDC_BUTTON_ON3, &CPowerStaDemoDlg::OnBnClickedButtonOn3)
END_MESSAGE_MAP()

// CPowerStaDemoDlg 消息处理程序
//对话框初始化函数


BOOL CPowerStaDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	

	m_hBrushON = CreateSolidBrush(RGB(0,0xff,0xff));  //你可以修改画刷

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CRect rect;
	// TODO: 在此添加额外的初始化代码

	//	m_HSChartCtrl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_Tab)->GetWindowRect(rect);
	GetDlgItem(IDC_STATIC_Tab)->ShowWindow(SW_HIDE);
	ScreenToClient(rect);
	m_tab.Create(CMFCTabCtrl::STYLE_3D_ONENOTE,//控件样式，如最上面所演示 
		rect,//控件区域
		this,//控件的父窗口指针
		1,//控件ID
		CMFCTabCtrl::LOCATION_TOP);//标签位置	
	m_tab.AutoSizeWindow(TRUE);//可以让对话框随tab显示区域而缩放，同时出发对话框的OnSize消息


	m_chartctrl.Create(&m_tab, rect, 2);
	CChartAxis *pAxis = NULL;
	//初始化坐标
	pAxis = m_chartctrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_chartctrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(true);
	m_pLineSerie = m_chartctrl.CreateLineSerie();
	UpdateData(FALSE);
	m_tab.AddTab(&m_chartctrl, _T("实时功率"));
	m_tab.SetActiveTab(0);//设置显示第一页；     
	m_tab.ShowWindow(SW_SHOWNORMAL);
	CRect TabBRect, TabTRect;
	m_tab.GetWindowRect(rect);
	ScreenToClient(rect);
	m_tab.GetTabArea(TabTRect, TabBRect);
	rect.top += TabTRect.Height();


	//激光器端口选择空间
	m_cComboPowerstaCommCtl.AddString(L"COM1");
	m_cComboPowerstaCommCtl.AddString(L"COM2");
	m_cComboPowerstaCommCtl.AddString(L"COM3");
	m_cComboPowerstaCommCtl.AddString(L"COM4");
	m_cComboPowerstaCommCtl.AddString(L"COM5");
	m_cComboPowerstaCommCtl.AddString(L"COM6");
	m_cComboPowerstaCommCtl.AddString(L"COM7");
	m_cComboPowerstaCommCtl.AddString(L"COM8");
	m_cComboPowerstaCommCtl.AddString(L"COM9");
	m_cComboPowerstaCommCtl.SetCurSel(2);
	
	m_cFontResult.CreateFont(35,                  // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		CLEARTYPE_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		TEXT("Arial"));           // lpszFacename
	m_cStaticPower.SetFont(&m_cFontResult);
	m_cStaticPowerAvg.SetFont(&m_cFontResult);
	m_cStaticPowerDC.SetFont(&m_cFontResult);
	pPSDlg = this;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CPowerStaDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPowerStaDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPowerStaDemoDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 1250;
	lpMMI->ptMinTrackSize.y = 772;
	lpMMI->ptMaxTrackSize.x = 1250;
	lpMMI->ptMaxTrackSize.y = 772;
}

void CPowerStaDemoDlg::PowerStaSettingEnable(BOOL bEnable)
{
	// TODO: 在此添加控件通知处理程序代码
	//GetDlgItem(IDC_SPIN_PARAM_1)->EnableWindow(bEnable);
	//GetDlgItem(IDC_EDIT_PARAM_1)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_PARAM_2)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_PARAM_3)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_PARAM_4)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_POWERSTA_SETTING)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_POWER_SET)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_POWER_SET)->EnableWindow(bEnable);

}


HBRUSH CPowerStaDemoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//pDC->SetBkMode(TRANSPARENT);
	int i,iCtrlID = pWnd->GetDlgCtrlID();

	switch (iCtrlID)
	{
	case IDC_STATIC_POWERSTA_CONNECT:
		if (m_bPowerstaConnected)
		{
			hbr = m_hBrushON;
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		break;

		case IDC_STATIC_POWERSTA_CONNECT2:
			if (m_bPowerstaConnected)
			{
				hbr = m_hBrushON;
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(255, 0, 0));
			}
			break;
		
		case IDC_STATIC_POWER_TITLE:
		
			pDC->SetTextColor(RGB(0,0,255));
			break;

		case IDC_STATIC_average:

			pDC->SetTextColor(RGB(0, 0, 255));
			break;

		case IDC_STATIC_DC:

			pDC->SetTextColor(RGB(0, 0, 255));
			break;

	}
	return hbr;
}



void CPowerStaDemoDlg::OnStaticClickedPowerStaConnect()//连接串口
{
	// TODO: 在此添加命令处理程序代码
	int num = m_cComboPowerstaCommCtl.GetCurSel();
	BOOL  bRet = FALSE;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"激光控制器连接设置成功!");
		bRet = psDisconnectDevice(m_pHandleComm);
		if (bRet)
		{
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"激光器串口关闭成功！");
			m_bPowerstaConnected = false;
		}
		else
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"激光器串口关闭失败！");

		GetDlgItem(IDC_STATIC_POWERSTA_CONNECT)->SetWindowTextW(L"连接稳功仪");
		m_bPowerstaConnected = FALSE;
		PowerStaSettingEnable(m_bPowerstaConnected);
		m_pHandleComm = NULL;
		return;

	}
	else
	{
		m_pHandleComm = psConnectDevice(num + 1, 8, callbackFunc);
		hThread_KeepListen = psKeepListen(m_pHandleComm);
		if (m_pHandleComm)
		{
			bRet=psSetPort(m_pHandleComm, CBR_115200);
			if (!bRet)
			{
				GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"通信设置失败!");
				psDisconnectDevice(m_pHandleComm);
				m_pHandleComm = NULL;
				return;
			}
			m_bPowerstaConnected = true;
			PowerStaSettingEnable(m_bPowerstaConnected);
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"激光控制器连接设置成功!");
			GetDlgItem(IDC_STATIC_POWERSTA_CONNECT)->SetWindowTextW(L"断开稳功仪");
		}
		else
		{
			GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"激光控制器连接失败!");
		}
	}
	GetDlgItem(IDC_STATIC_POWERSTA_CONNECT)->RedrawWindow();
	return;
}

void CPowerStaDemoDlg::OnClose()//Leo 20200625
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bGetData)
	{
		m_bGetData = false;
	}
	ShowWindow(SW_HIDE);
	if (m_bPowerstaConnected) OnStaticClickedPowerStaConnect();//Close comm
	m_bPowerstaConnected = false;
	CDialogEx::OnClose();
}




void CPowerStaDemoDlg::OnBnClickedButtonPowerstaParamSetting()//参数设置
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString csMsg;
	char pcData[8];
	//float *pfParam = (float *)(pcData + 2);
	DWORD *pdwParam = (DWORD *)(pcData + 2);
	unsigned short * pusChkSum= (unsigned short *)(pcData + 6);
	unsigned short * pusData = (unsigned short *)pcData;


	if (m_pHandleComm && m_bPowerstaConnected)
	{
		if (drawing)
		{
			KillTimer(2);
			AcceptData(-1);
		}
		pcData[0] = 0x17;
		pcData[1] = 0x02;
		*pdwParam = m_fParam2;
		*pusChkSum = unsigned short(pusData[0] + pusData[1] + pusData[2]);
		psWriteData(m_pHandleComm, pcData, 8);
		csMsg.Format(L"发送：参数A=%d,校验=0x%04x\n", m_fParam2, (unsigned short)(*pusChkSum & 0xffff));
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);

		pcData[1] = 0x03;
		*pdwParam = m_fParam3;
		*pusChkSum = unsigned short(pusData[0] + pusData[1] + pusData[2]);
		psWriteData(m_pHandleComm, pcData, 8);
		csMsg.Format(L"发送：参数B=%d,校验=0x%04x\n", m_fParam3, (unsigned short)(*pusChkSum & 0xffff));
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);

		pcData[1] = 0x04;
		*pdwParam = m_dwParam4;
		*pusChkSum = unsigned short(pusData[0] + pusData[1] + pusData[2]);
		psWriteData(m_pHandleComm, pcData, 8);
		csMsg.Format(L"发送：参数C=%d,校验=0x%04x\n", m_dwParam4, (unsigned short)(*pusChkSum & 0xffff));
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
		if (drawing)
		{
			SetTimer(2, 500, NULL);
			AcceptData(2);
		}
	}

}
void CPowerStaDemoDlg::OnBnClickedButtonPowerSet()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString csMsg;
	char pcData[8];
	float *pParam = (float *)(pcData + 2);
	DWORD *pdwParam = (DWORD *)(pcData + 2);
	unsigned short * pusChkSum = (unsigned short *)(pcData + 6);
	unsigned short * pusData = (unsigned short *)pcData;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		if (drawing)
		{
			KillTimer(2);//如果正在绘图，先暂停绘图。
			AcceptData(-1);
		}
		pcData[0] = 0x17;
		pcData[1] = 0x01;
		*pdwParam = round(m_fPowerSetting*exp2(23) / 5);
		*pusChkSum = unsigned short(pusData[0] + pusData[1] + pusData[2]);
		psWriteData(m_pHandleComm, pcData, 8);
		csMsg.Format(L"发送：设置功率=%f, 校验=0x%04x\n", m_fPowerSetting, (unsigned short)(*pusChkSum & 0xffff));
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
		if (drawing)
		{
			SetTimer(2, 500, NULL);////如果暂停绘图了，恢复绘图。
			AcceptData(2);
		}
	}
}

void CPowerStaDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (2 == nIDEvent)
	{
		//++m_count;
		//m_pLineSerie->ClearSerie();
		//LeftMoveArray(m_HightSpeedChartArray, m_c_arrayLength, randf(0, 10));
		//LeftMoveArray(m_X, m_c_arrayLength, m_count);
		//m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);
		++m_count;
		drawMoving();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CPowerStaDemoDlg::drawMoving()
{
	CString csMsg;
	DWORD dwRev = 0;
	LONG	lRet;
	char pcData[] = { 0x17,0x06,0x00, 0x00 ,0x00 ,0x00 ,0x1c ,0x04 };
	psWriteData(m_pHandleComm, pcData, 8);
	//AcceptData(true);
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet) {
		//	m_chartctrl.EnableRefresh(false);
		//	//画图测试
		//}
		m_chartctrl.EnableRefresh(false);
		//画图测试
		/**
		* @author hlf
		* @description 2022/3/22 把x轴改成1秒刷新2次
		*/
		m_X[m_count] = double(m_count)/2;
		m_HightSpeedChartArray[m_count] = rcv_data;
		m_pLineSerie->AddPoint(m_X[m_count], m_HightSpeedChartArray[m_count]);
		m_chartctrl.EnableRefresh(true);
		avg = DC = 0;
		CString cs = L"";
		cs.Format(L"%.6f", rcv_data);//实时功率 
		pPSDlg->m_cStaticPower.SetWindowText(cs);
		//for (int i = 0; i <= m_count; i++)//计算均值
		//{
		//	sum += m_HightSpeedChartArray[i];
		//}
		m_HightSpeedChartArray_sum += rcv_data;
		avg = m_HightSpeedChartArray_sum / m_count;
		cs.Format(L"%.6f", avg);
		pPSDlg->m_cStaticPowerAvg.SetWindowText(cs);
		DCsum = 0;
		for (int i = 1; i <= m_count; i++)//计算方差
		{
			DCsum += pow(m_HightSpeedChartArray[i] - avg, 2);
		}
		//if (m_count>=1)
		//{
		//	DCsum += pow(m_HightSpeedChartArray[m_count] - avg, 2);
		//}
		DC = pow((DCsum / m_count), 0.5);
		cs.Format(L"%.6f", DC);
		pPSDlg->m_cStaticPowerDC.SetWindowText(cs);
	}
}
/// 
/// \brief 左移数组
/// \param ptr 数组指针
/// \param data 新数值
///
void CPowerStaDemoDlg::LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i<length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}

/**
* @author hlf
* @description 2022/3/22 发送给下位机的指令：持续发送数据与暂停
* 当传入的值为1时，开启持续发送数据线程；
* 当传入的值为0时，关闭持续发送数据线程；
* 当传入的值为-1时，挂起持续发送数据线程；
* 当传入的值为2时，恢复持续发送数据线程；
*/
void CPowerStaDemoDlg::AcceptData(int StaBit)
{
	if (StaBit==1) {
		SuspendThread(hThread_KeepListen);
		hThread_SendData = psSendData(m_pHandleComm, 8);
	}
	else if (StaBit == 0) {
		SuspendThread(hThread_SendData);
		ResumeThread(hThread_KeepListen);
		//CloseHandle(hThread_SendData);
		//ExitThread((DWORD)hThread_SendData);
	}
	else if (StaBit == -1) {
		SuspendThread(hThread_SendData);
		ResumeThread(hThread_KeepListen);
	}
	else if (StaBit == 2) {
		ResumeThread(hThread_SendData);
		SuspendThread(hThread_KeepListen);
	}
}

void CPowerStaDemoDlg::OnBnClickedButton1()//接受数据并开始绘图
{
	// TODO: 在此添加控件通知处理程序代码
	AcceptData(1);
	KillTimer(2);
	ZeroMemory(&m_HightSpeedChartArray, sizeof(double)*m_c_arrayLength);
	m_count = m_c_arrayLength;
	/**
	* @author hlf
	* @description 2022/3/22 对m_HightSpeedChartArray_sum进行了初始化，防止数值初始化非0
	*/
	m_HightSpeedChartArray_sum = 0;
	m_pLineSerie->ClearSerie();
	drawing = true;
	SetTimer(2, 500, NULL);
}


void CPowerStaDemoDlg::OnBnClickedButton2()//结束绘图
{
	// TODO: 在此添加控件通知处理程序代码
	AcceptData(0);
	KillTimer(2);
	m_count = 0;
	drawing = false;
	ZeroMemory(&m_HightSpeedChartArray, sizeof(double)*m_c_arrayLength);
}


void CPowerStaDemoDlg::OnBnClickedButtonDataSave()
{
	TCHAR szFilter[] = _T("CSV文件(*.csv)|*.csv||");
	CFileDialog cFileDialog(FALSE, _T("csv"), NULL, OFN_PATHMUSTEXIST, szFilter, this);
	CString csFileName;
	if (IDOK == cFileDialog.DoModal())
	{
		csFileName = cFileDialog.GetPathName();
		TChartString fileName;
		CRect crect;
		CString w;
		int q = csFileName.Find('.');
		w = csFileName.Left(q);
		w.AppendFormat(L".png");
		fileName = w;
		crect.SetRect(0, 0, 1920, 1080);                      //set the size of the image
		m_chartctrl.SaveAsImage(fileName, crect, 32, GUID_NULL); //savethe image
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(L"曲线保存成功!");

		ofstream outFile;
		int i = 1;
		outFile.open(csFileName, ios::out); // 打开模式可省略  
		outFile << "时间/S" << ',' << "功率/mw" << ',' << "标准差" << ',' << "平均功率/mw" << ',' << "当前功率/mw" << endl;
		//m_count = 40000;
		if (m_count > 1) {
			for (i = 1; i <= m_count; i++)
			{
				outFile << m_X[i] << ',' << m_HightSpeedChartArray[i] << endl;
			}
		}
		outFile.close();
		ifstream inFile(csFileName, ios::in);
		vector<string>Content_1;//存放文件中的内容
		double tmp[3];
		tmp[0] = DC;
		tmp[1] = avg;
		tmp[2] = rcv_data;
		string str_1;
		if (inFile.fail()==0)
		{
			int j = 0;
			while (getline(inFile, str_1))
			{
				Content_1.push_back(str_1);
				j++;
			}
		}
		inFile.close();
		ofstream File_1(csFileName);
		for (int i = 0; i < Content_1.size(); i++)
		{
			if (i==1)
			{
				File_1 << Content_1[i] << ',' << tmp[0] << ',' << tmp[1] << ',' << tmp[2] << endl;
			}
			else
			{
				File_1 << Content_1[i] << endl;
			}
		}
		File_1.close();
	}
}

void CPowerStaDemoDlg::OnBnClickedButtonOn2()//打开红外开关
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		if (drawing)
		{
			KillTimer(2);
			AcceptData(-1);
		}
		if (ButtonOn3) {
			OnBnClickedButtonOff3();//关闭开关3
			//Sleep(100);
		}
		if (ButtonOn1) {
			OnBnClickedButtonOff1();//关闭开关1
			//Sleep(100);
		}
		pcData[0] = 0x0C;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0xFF;
		pcData[5] = 0x00;
		pcData[6] = 0x8C;
		pcData[7] = 0xE2;
		psWriteData(m_pHandleComm, pcData, 8);
		TRACE(pcData);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0xFF) {
		//	csMsg.Format(L"打开红外开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn2 = true;
		csMsg.Format(L"打开红外开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
		if (drawing)
		{
			SetTimer(2, 500, NULL);
			AcceptData(2);
		}
	}
}


void CPowerStaDemoDlg::OnBnClickedButtonOff2()//红外开关关闭
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG	lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		pcData[0] = 0x0C;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0x00;
		pcData[5] = 0x00;
		pcData[6] = 0xCD;
		pcData[7] = 0x12;
		psWriteData(m_pHandleComm, pcData, 8);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0x00) {
		//	csMsg.Format(L"关闭红外开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn2 = false;
		csMsg.Format(L"关闭红外开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
	}
}


void CPowerStaDemoDlg::OnBnClickedButtonOn1()//可见开
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG	lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		if (drawing)
		{
			KillTimer(2);
			AcceptData(-1);
		}
		if (ButtonOn3) {
			OnBnClickedButtonOff3();//关闭开关3
		}
		if (ButtonOn2) {
			OnBnClickedButtonOff2();//关闭开关2
		}
		pcData[0] = 0x0B;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0xFF;
		pcData[5] = 0x00;
		pcData[6] = 0x8D;
		pcData[7] = 0x55;
		psWriteData(m_pHandleComm, pcData, 8);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0xFF) {
		//	csMsg.Format(L"打开可见开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn1 = true;
		csMsg.Format(L"打开可见开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
		if (drawing)
		{
			SetTimer(2, 500, NULL);
			AcceptData(2);
		}
	}
}


void CPowerStaDemoDlg::OnBnClickedButtonOff1()//可见关
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG	lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		pcData[0] = 0x0B;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0x00;
		pcData[5] = 0x00;
		pcData[6] = 0xCC;
		pcData[7] = 0xA5;
		psWriteData(m_pHandleComm, pcData, 8);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0x00) {
		//	csMsg.Format(L"关闭可见开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn1 = false;
		csMsg.Format(L"关闭可见开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
	}
}


void CPowerStaDemoDlg::OnBnClickedButtonOff3()//关闭紫外开关
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG	lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		pcData[0] = 0x0D;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0x00;
		pcData[5] = 0x00;
		pcData[6] = 0xCC;
		pcData[7] = 0xC3;
		psWriteData(m_pHandleComm, pcData, 8);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0x00) {
		//	csMsg.Format(L"关闭紫外开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn3 = false;
		csMsg.Format(L"关闭紫外开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
	}
}


void CPowerStaDemoDlg::OnBnClickedButtonOn3()//开启紫外开关
{
	// TODO: 在此添加控件通知处理程序代码
	CString csMsg;
	char pcData[8], pcRev[8];
	DWORD dwRev = 0;
	LONG	lRet;
	if (m_pHandleComm && m_bPowerstaConnected)
	{
		if (drawing)
		{
			KillTimer(2);
			AcceptData(-1);
		}
		if (ButtonOn2) {
			OnBnClickedButtonOff2();//关闭开关2
			//Sleep(100);
		}
		if (ButtonOn1) {
			OnBnClickedButtonOff1();//关闭开关1
			//Sleep(100);
		}
		pcData[0] = 0x0D;
		pcData[1] = 0x05;
		pcData[2] = 0x00;
		pcData[3] = 0x10;
		pcData[4] = 0xFF;
		pcData[5] = 0x00;
		pcData[6] = 0x8D;
		pcData[7] = 0x33;
		psWriteData(m_pHandleComm, pcData, 8);
		//lRet = m_cComPortPowerSta.RecData(pcRev, 8, &dwRev);
		//if (lRet && pcRev[4] == 0xFF) {
		//	csMsg.Format(L"打开紫外开关\n");
		//	GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		//	TRACE(csMsg);
		//}
		ButtonOn3 = true;
		csMsg.Format(L"打开紫外开关\n");
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextW(csMsg);
		TRACE(csMsg);
		if (drawing)
		{
			SetTimer(2, 500, NULL);
			AcceptData(2);
		}
	}
}
