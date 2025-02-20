// CTouchSwitchSettingsDlg.cpp : implementation file
//

#include "pch.h"
#include "TouchSwitch.h"
#include "afxdialogex.h"
#include "CTouchSwitchSettingsDlg.h"


// CTouchSwitchSettingsDlg dialog

IMPLEMENT_DYNAMIC(CTouchSwitchSettingsDlg, CDialogEx)

CTouchSwitchSettingsDlg::CTouchSwitchSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTINGS_DIALOG, pParent)
{

}

BOOL CTouchSwitchSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cToolTipCtrl.Create(this);
	m_cToolTipCtrl.SendMessage(TTM_SETMAXTIPWIDTH, 0, 512);
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_EDIT1), _T("Found in Device Manager, formatted as {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"));
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_EDIT2), _T("Found in Device Manager, usually looks like file path"));
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_CHECK1), _T("This feature has yet to be implemented!"));  // TODO
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_CHECK2), _T("Unchecked:\tNever re-enable device automatically\nPartially checked:\tRe-enable device on program exit\nFully checked:\tOn system lock / sleep as well"));
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_CHECK3), _T("Hide widget to taskbar when minimized / closed"));
	m_cToolTipCtrl.AddTool(GetDlgItem(IDC_COMBO1), _T("If \"Right click\" is selected, right click on tray icon for menu."));
	m_cToolTipCtrl.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTouchSwitchSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg)
		m_cToolTipCtrl.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTouchSwitchSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ceTargetClassGuid);
	DDX_Control(pDX, IDC_EDIT2, m_ceTargetInstancePath);
	DDX_Control(pDX, IDC_CHECK1, m_cbAutostart);
	DDX_Control(pDX, IDC_CHECK2, m_cbAutoReenableDevice);
	DDX_Control(pDX, IDC_CHECK3, m_cbMinimizeHide);
	DDX_Control(pDX, IDC_COMBO1, m_ccbActivateMethod);
}


BEGIN_MESSAGE_MAP(CTouchSwitchSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTouchSwitchSettingsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTouchSwitchSettingsDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTouchSwitchSettingsDlg message handlers

void CTouchSwitchSettingsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	NotifyParentToDestroy(TRUE);
}

void CTouchSwitchSettingsDlg::OnBnClickedCancel()
{
	OnCancel();
}

// Handle Alt+F4 or ESC stuff
void CTouchSwitchSettingsDlg::OnCancel()
{
	NotifyParentToDestroy(FALSE);
}

// Call DestroyWindow and delete in parent procedure
void CTouchSwitchSettingsDlg::NotifyParentToDestroy(BOOL doSave)
{
	CWnd *pParent = GetParent();
	if (pParent)
		pParent->SendMessage(WM_APP_FREE_SETTINGS_DLG, doSave, NULL);
}

