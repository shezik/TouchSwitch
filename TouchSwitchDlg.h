
// TouchSwitchDlg.h : header file
//

#pragma once

#include "CTouchSwitchSettingsDlg.h"
#include "SetupApiHelper.h"

// CTouchSwitchDlg dialog
class CTouchSwitchDlg : public CDialogEx
{
// Construction
public:
	CTouchSwitchDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOUCHSWITCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg virtual void OnDestroy();
	virtual ULONG GetGestureStatus(CPoint) override;
	virtual BOOL PreTranslateMessage(MSG*);
	afx_msg virtual void OnCancel() override;
	afx_msg virtual void OnSysCommand(UINT, LPARAM);
	afx_msg virtual void OnGetMinMaxInfo(MINMAXINFO*);
	afx_msg virtual void OnLButtonDown(UINT, CPoint);
	afx_msg virtual void OnLButtonUp(UINT, CPoint);
	afx_msg virtual void OnRButtonUp(UINT, CPoint);
	afx_msg virtual void OnMouseMove(UINT, CPoint);
	afx_msg virtual HBRUSH OnCtlColor(CDC*, CWnd*, UINT);
	afx_msg LRESULT OnFreeSettingsDlg(WPARAM, LPARAM);
	afx_msg LRESULT OnTrayIcon(WPARAM, LPARAM);
	afx_msg void OnMenuLockPosition();
	afx_msg void OnMenuToggleDevice();
	afx_msg void OnMenuStayOnTop();
	afx_msg void OnMenuSettings();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	afx_msg virtual UINT OnPowerBroadcast(UINT, LPARAM);
	afx_msg virtual void OnSessionChange(UINT, UINT);
	afx_msg virtual void OnEndSession(BOOL);
	afx_msg virtual void OnNcLButtonDblClk(UINT, CPoint);
	afx_msg virtual void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
	afx_msg virtual LRESULT OnNcHitTest(CPoint);
	afx_msg virtual BOOL OnNcActivate(BOOL);
	DECLARE_MESSAGE_MAP()

private:
	CTouchSwitchSettingsDlg *m_pSettingsDlg;
	HPOWERNOTIFY m_hPowerNotify;

	NOTIFYICONDATA m_sTrayIcon;
	BOOL InitAndShowTrayIcon();
	BOOL RemoveTrayIcon();

	BOOL ShowContextMenu();

	BOOL m_bDeviceStatus;
	BOOL GetDeviceStatusWindows();
	BOOL SetDeviceStatusWindows(BOOL);
	BOOL SetDeviceStatus(BOOL);

	CBrush m_cbDeviceEnabled;
	CBrush m_cbDeviceDisabled;

	CString m_sTargetClassGuid;
	CString m_sTargetInstancePath;
	BOOL m_bAutostart;
	int m_iAutoReenableDevice;
	BOOL m_bMinimizeHide;
	CTouchSwitchSettingsDlg::EnumActivateMethod m_eActivateMethod;
	BOOL m_bStayOnTop;
	BOOL m_bLockPosition;
	BOOL m_bLockPositionBackup;
	BOOL m_bIsDragging;

	int m_iLastX;
	int m_iLastY;
	int m_iLastWidth;
	int m_iLastHeight;
	CPoint m_ptDragStart;

	void ReadFromSettingsDlg();
	void WriteToSettingsDlg();
	void ReadFromSettingsProfile();
	void WriteToSettingsProfile();
	void ApplyWindowStayOnTop();
	BOOL PromptTargetDeviceValidity();

	SetupApiHelper *m_pSetupApiHelper;
};

