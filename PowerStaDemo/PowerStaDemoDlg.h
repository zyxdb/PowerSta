#pragma once
// PowerStaDemoDlg.h : 头文件
//


#include "Commons.h"
#include <afx.h>
#include "afxwin.h"
#include <string>
#include <atlimage.h>
#include "afxcmn.h"
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartTitle.h"
#include "ChartCtrl\ChartAxisLabel.h"
#include "ChartCtrl\ChartLineSerie.h"


//
using namespace std;
#define MENUID_DATACOLLECT  WM_USER+10
#define MENUID_DATAANALYSIS WM_USER+11
#define MENUID_RESPONSETIME WM_USER+12
#define MENUID_POWERADAPTOR WM_USER+13
#define MENUID_ABOUT WM_USER+14
#define REFRESH_VIEW WM_USER+100
#define WM_ERROR     WM_USER+101
#define RES_CYCLE 7 

typedef enum {
	Data_Laserwavelength = 0,
	Data_Modulationfreq,
	Data_Current,
	Data_AmplitudeSource,
	Data_AmplitudeNormalized,
	Data_Amplitude,
	Data_Phase,
	Data_X,
	Data_Y,
	Data_Total
}DATA_TYPE;

typedef struct  _SpectrumData
{
	double dbLaserwavelength;
	double dbModulationfreq;
	double dbCurrent;
	double dbAmplitudeSource;
	double dbAmplitudeNormalized;
	double dbAmplitude;
	double dbPhase;
	double dbX;
	double dbY;
	double dbYhead;
	double dbYmid;
	double dbYend;
}SpectrumData, *PSpectrumData;


// CPowerStaDemoDlg 对话框
class CPowerStaDemoDlg : public CDialogEx
{
// 构造
public:
	CPowerStaDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPowerStaDemoDlg();
// 对话框数据
	enum { IDD = IDD_POWERSTADEMO_DIALOG};

protected:
	// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnDeltaposSpinParam_1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void	PowerStaSettingEnable(BOOL bEnable=TRUE);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStaticClickedPowerStaConnect();
	afx_msg void OnClose();
	CStatic m_cStaticPower;
	afx_msg void OnBnClickedButtonPowerSet();
	afx_msg void OnBnClickedButtonPowerstaParamSetting();
	int   m_iWorkItem;

public:
	HICON m_hIcon;
	CComboBox m_cComboPowerstaCommCtl;
	Cport	m_cComPortPowerSta;
	CString pcCommName;
	bool	m_bGetData;
	bool	m_bPowerstaConnected;
	PVOID	m_pHandleComm;
	CFont	m_cFontResult;
	HBRUSH  m_hBrushON;
	double m_HightSpeedChartArray_sum = 0;
//	float			m_fParam1;
	DWORD			m_fParam2;
	DWORD			m_fParam3;
	DWORD			m_dwParam4;
	double m_fPowerSetting;
	//CMFCTabCtrl m_tab;

	CChartCtrl m_chartctrl;
	CChartLineSerie* m_pLineSerie;
	time_t m_timeStart;
	HANDLE  hThread_SendData = NULL;
	HANDLE  hThread_KeepListen = 0;
	bool ButtonOn1 = false;
	bool ButtonOn2 = false;
	bool ButtonOn3 = false;
private:
	void drawMoving();
	double m_HightSpeedChartArray[14450];
	double m_X[14450];
	int m_count = -1;
	double DCsum = 0;
	const size_t m_c_arrayLength;
	void LeftMoveArray(double* ptr, size_t length, double data);
	void AcceptData(int StaBit);
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

public:
//	afx_msg void OnBnClickedButtonDraw();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonDataSave();
	// 平均功率
	CStatic m_cStaticPowerAvg;
	// 标准差
	CStatic m_cStaticPowerDC;
	double DC, avg;
	afx_msg void OnBnClickedButtonOn2();
	afx_msg void OnBnClickedButtonOff2();
	afx_msg void OnBnClickedButtonOn1();
	afx_msg void OnBnClickedButtonOff1();
	afx_msg void OnBnClickedButtonOff3();
	afx_msg void OnBnClickedButtonOn3();
	bool drawing = false;
};
