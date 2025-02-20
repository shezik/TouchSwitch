#pragma once
#include "afxdialogex.h"


// CTouchSwitchSettingsDlg dialog

class CTouchSwitchSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTouchSwitchSettingsDlg)

public:
	CTouchSwitchSettingsDlg(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ceTargetClassGuid;
	CEdit m_ceTargetInstancePath;
	CButton m_cbAutostart;
	CButton m_cbAutoReenableDevice;
	CButton m_cbMinimizeHide;
	CComboBox m_ccbActivateMethod;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg virtual void OnCancel() override;
	// Make sure this corresponds to the drop-down list
	enum class EnumActivateMethod
	{
		LEFT_CLICK = 0,
		RIGHT_CLICK
	};

private:
	CToolTipCtrl m_cToolTipCtrl;
	void NotifyParentToDestroy(BOOL);
};

