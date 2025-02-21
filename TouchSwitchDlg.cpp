
// TouchSwitchDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TouchSwitch.h"
#include "TouchSwitchDlg.h"
#include "WtsApi32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTouchSwitchDlg dialog



CTouchSwitchDlg::CTouchSwitchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOUCHSWITCH_DIALOG, pParent)
	, m_pSettingsDlg(nullptr)
	, m_bIsDragging(FALSE)
	, m_hPowerNotify(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(&m_sTrayIcon, sizeof(NOTIFYICONDATA));
	m_cbDeviceEnabled.CreateSolidBrush(RGB(0, 255, 0));
	m_cbDeviceDisabled.CreateSolidBrush(RGB(255, 0, 0));
	ReadFromSettingsProfile();
	m_bLockPositionBackup = m_bLockPosition;
	m_bLockPosition = FALSE;  // Hack; GetMinMaxInfo is called before OnInitDialog so window cannot fall back to its default position
	m_pSetupApiHelper = new SetupApiHelper(m_sTargetClassGuid, m_sTargetInstancePath);
	m_bDeviceStatus = GetDeviceStatusWindows();
}

void CTouchSwitchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTouchSwitchDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	ON_WM_POWERBROADCAST()
	ON_WM_WTSSESSION_CHANGE()
	ON_WM_ENDSESSION()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCACTIVATE()
	ON_MESSAGE(WM_APP_FREE_SETTINGS_DLG, OnFreeSettingsDlg)
	ON_MESSAGE(WM_APP_TRAYICON, OnTrayIcon)
	ON_COMMAND(ID_MENU_TOGGLEDEVICE, OnMenuToggleDevice)
	ON_COMMAND(ID_MENU_LOCKPOSITION, OnMenuLockPosition)
	ON_COMMAND(ID_MENU_STAYONTOP, OnMenuStayOnTop)
	ON_COMMAND(ID_MENU_SETTINGS, OnMenuSettings)
	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, OnMenuExit)
END_MESSAGE_MAP()


// CTouchSwitchDlg message handlers

BOOL CTouchSwitchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Allow minimization
	ModifyStyle(NULL, WS_MINIMIZEBOX);

	// Restore last saved window position
	if (m_iLastX > 0 && m_iLastY > 0 && m_iLastWidth > 0 && m_iLastHeight > 0)
		MoveWindow(m_iLastX, m_iLastY, m_iLastWidth, m_iLastHeight);

	// Read back actual value of m_bLockPosition
	m_bLockPosition = m_bLockPositionBackup;

	m_hPowerNotify = RegisterSuspendResumeNotification(m_hWnd, DEVICE_NOTIFY_WINDOW_HANDLE);  // Sleep
	WTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_THIS_SESSION);  // Lock

	InitAndShowTrayIcon();  // hWnd is NULL in constructor so here we are
	ApplyWindowStayOnTop();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTouchSwitchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTouchSwitchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTouchSwitchDlg::OnDestroy()
{
	// m_hWnd is still available here, clean up
	// TODO: Seems that m_hWnd is no longer available!
	if (m_iAutoReenableDevice != BST_UNCHECKED)
		SetDeviceStatus(TRUE);
	WriteToSettingsProfile();  // Save window position
	RemoveTrayIcon();
	OnFreeSettingsDlg(FALSE, NULL);
	delete m_pSetupApiHelper;  // No need to check if is nullptr
	m_pSetupApiHelper = nullptr;
	UnregisterSuspendResumeNotification(m_hPowerNotify);
	m_hPowerNotify = nullptr;
	WTSUnRegisterSessionNotification(m_hWnd);
}

ULONG CTouchSwitchDlg::GetGestureStatus(CPoint /*ptTouch*/)
{
	return 0;  // Allow long-press gesture on touch screen devices
}

BOOL CTouchSwitchDlg::PreTranslateMessage(MSG* pMsg)
{
	// Disable ESC key
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTouchSwitchDlg::OnCancel()
{
	if (m_bMinimizeHide) {
		ShowWindow(SW_HIDE);
		return;
	}
	CDialogEx::OnCancel();
}

void CTouchSwitchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE && m_bMinimizeHide)
	{
		ShowWindow(SW_HIDE);
		return;
	}
	if (nID == SC_RESTORE && m_bLockPosition)
	{
		m_bLockPosition = FALSE;  // Hack; Restoring window with this set fails horribly
		CDialogEx::OnSysCommand(nID, lParam);
		m_bLockPosition = TRUE;
		return;
	}
	CDialogEx::OnSysCommand(nID, lParam);
}

void CTouchSwitchDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_bLockPosition)
	{
		CRect pos;
		GetWindowRect(&pos);
		lpMMI->ptMinTrackSize.x = pos.Size().cx;
		lpMMI->ptMinTrackSize.y = pos.Size().cy;
		lpMMI->ptMaxTrackSize.x = pos.Size().cx;
		lpMMI->ptMaxTrackSize.y = pos.Size().cy;
	}
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CTouchSwitchDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bLockPosition)
	{
		m_bIsDragging = FALSE;
		m_ptDragStart = point;
		SetCapture();
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CTouchSwitchDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	if (m_bIsDragging)
	{
		m_bIsDragging = FALSE;
		WriteToSettingsProfile();
	}
	else if (m_eActivateMethod == CTouchSwitchSettingsDlg::EnumActivateMethod::LEFT_CLICK)
	{
		OnMenuToggleDevice();
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CTouchSwitchDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_eActivateMethod == CTouchSwitchSettingsDlg::EnumActivateMethod::RIGHT_CLICK)
		OnMenuToggleDevice();
	else
		ShowContextMenu();
	CDialogEx::OnRButtonUp(nFlags, point);
}

void CTouchSwitchDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	const int dragThreshold = 5;  // Change drag threshold here
	if (GetCapture() == this)
	{
		int dx = point.x - m_ptDragStart.x;
		int dy = point.y - m_ptDragStart.y;

		if (m_bIsDragging)
		{
			CRect pos;
			GetWindowRect(&pos);
			MoveWindow(pos.left + dx, pos.top + dy, pos.Width(), pos.Height());
		} else if (abs(dx) > dragThreshold || abs(dy) > dragThreshold)
			m_bIsDragging = TRUE;
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

HBRUSH CTouchSwitchDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_DLG) {
		return m_bDeviceStatus ? m_cbDeviceEnabled : m_cbDeviceDisabled;
	}
	return hbr;
}

LRESULT CTouchSwitchDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONUP:
			ShowContextMenu();
			break;

		case WM_LBUTTONUP:
			// Does not follow m_bMinimizeHide
			if (!IsWindowVisible() || IsIconic()) {
				ShowWindow(SW_RESTORE);
				// SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				SetForegroundWindow();
			} else {
				ShowWindow(SW_HIDE);
			}
			break;
	}
	return TRUE;
}

BOOL CTouchSwitchDlg::InitAndShowTrayIcon()
{
	memset(&m_sTrayIcon, 0, sizeof(NOTIFYICONDATA));
	m_sTrayIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_sTrayIcon.hWnd = m_hWnd;
	m_sTrayIcon.uID = 1;
	m_sTrayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_sTrayIcon.uCallbackMessage = WM_APP_TRAYICON;
	m_sTrayIcon.hIcon = m_hIcon;
	_tcscpy_s(m_sTrayIcon.szTip, _T(APP_TITLE));
	return Shell_NotifyIcon(NIM_ADD, &m_sTrayIcon);
}

BOOL CTouchSwitchDlg::RemoveTrayIcon()
{
	return Shell_NotifyIcon(NIM_DELETE, &m_sTrayIcon);
}

BOOL CTouchSwitchDlg::ShowContextMenu()
{
	CMenu menu;
	menu.LoadMenu(IDR_TASKBAR_MENU);
	CMenu *pSubMenu = menu.GetSubMenu(0);
	
	//BOOL status = GetDeviceStatusWindows();
	pSubMenu->ModifyMenu(ID_MENU_TOGGLEDEVICE, MF_BYCOMMAND | MF_STRING, ID_MENU_TOGGLEDEVICE, \
		m_bDeviceStatus ? \
		_T("Disable device") : \
		_T("Enable device")
	);
	pSubMenu->CheckMenuItem(ID_MENU_LOCKPOSITION, MF_BYCOMMAND | (m_bLockPosition ? MF_CHECKED : MF_UNCHECKED));
	pSubMenu->CheckMenuItem(ID_MENU_STAYONTOP, MF_BYCOMMAND | (m_bStayOnTop ? MF_CHECKED : MF_UNCHECKED));

	CPoint pos;
	GetCursorPos(&pos);

	SetForegroundWindow();
	return pSubMenu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, this);
}

void CTouchSwitchDlg::OnMenuToggleDevice()
{
	SetDeviceStatus(!GetDeviceStatusWindows());
	PromptTargetDeviceValidity();
}

void CTouchSwitchDlg::OnMenuLockPosition()
{
	m_bLockPosition = !m_bLockPosition;
	WriteToSettingsProfile();
}

void CTouchSwitchDlg::OnMenuStayOnTop()
{
	m_bStayOnTop = !m_bStayOnTop;
	WriteToSettingsProfile();
	ApplyWindowStayOnTop();
}

void CTouchSwitchDlg::ApplyWindowStayOnTop()
{
	SetWindowPos((m_bStayOnTop ? &wndTopMost : &wndNoTopMost), \
		0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CTouchSwitchDlg::OnMenuSettings()
{
	if (m_pSettingsDlg)
	{
		m_pSettingsDlg->ShowWindow(SW_RESTORE);
		return;
	}
	m_pSettingsDlg = new CTouchSwitchSettingsDlg(this);
	m_pSettingsDlg->Create(IDD_SETTINGS_DIALOG, this);
	WriteToSettingsDlg();
	m_pSettingsDlg->UpdateData(FALSE);
	m_pSettingsDlg->ShowWindow(SW_SHOWNORMAL);  // Main window could be hidden when this pops up
}

void CTouchSwitchDlg::OnMenuAbout()
{
	AfxMessageBox(_T("TouchSwitch v" APP_VERSION "\nhttps://github.com/shezik/TouchSwitch"), MB_ICONASTERISK);  // this is so bad lol
}

void CTouchSwitchDlg::OnMenuExit()
{
	CDialogEx::OnCancel();
}

BOOL CTouchSwitchDlg::GetDeviceStatusWindows()
{
	BOOL enabled = FALSE;
	m_pSetupApiHelper->GetDeviceEnabled(&enabled);
	return enabled;
}

BOOL CTouchSwitchDlg::SetDeviceStatusWindows(BOOL status)
{
	return m_pSetupApiHelper->SetDeviceEnabled(status);
}

BOOL CTouchSwitchDlg::SetDeviceStatus(BOOL targetStatus)
{
	m_bDeviceStatus = targetStatus;

	if (GetDeviceStatusWindows() != targetStatus)
	{
		SetDeviceStatusWindows(targetStatus);
		m_bDeviceStatus = GetDeviceStatusWindows();  // In case the operation failed
	}

	Invalidate(TRUE);
	return m_bDeviceStatus == targetStatus;
}

LRESULT CTouchSwitchDlg::OnFreeSettingsDlg(WPARAM wParam, LPARAM lParam)
{
	if (!m_pSettingsDlg)
		return FALSE;

	if (wParam)  // doSave
	{
		CString prevTargetClassGuid = m_sTargetClassGuid;
		CString prevTargetInstancePath = m_sTargetInstancePath;
		ReadFromSettingsDlg();

		if (m_sTargetClassGuid != prevTargetClassGuid || m_sTargetInstancePath != prevTargetInstancePath)
		{
			if (m_pSetupApiHelper->GetConfigValidity() && !GetDeviceStatusWindows())
			{
				switch (AfxMessageBox(_T("Previous Target Device was disabled.\nWould you like to re-enable it now?"), MB_YESNOCANCEL | MB_ICONQUESTION))
				{
					case IDYES:
						SetDeviceStatus(TRUE);
						PromptTargetDeviceValidity();
						break;
					case IDNO:
						break;
					case IDCANCEL:
						ReadFromSettingsProfile();
						// This is too aggressive
						//WriteToSettingsDlg();
						//m_pSettingsDlg->UpdateData(FALSE);
						//m_pSettingsDlg->ShowWindow(SW_SHOWNORMAL);
						return TRUE;
				}
			}
			delete m_pSetupApiHelper;
			m_pSetupApiHelper = new SetupApiHelper(m_sTargetClassGuid, m_sTargetInstancePath);
			m_bDeviceStatus = GetDeviceStatusWindows();
			Invalidate(TRUE);
		}

		WriteToSettingsProfile();
		//PromptTargetDeviceValidity();  // Bringing up two identical dialogs consecutively is ambiguous
	}

	m_pSettingsDlg->DestroyWindow();
	delete m_pSettingsDlg;
	m_pSettingsDlg = nullptr;
	return TRUE;
}

void CTouchSwitchDlg::ReadFromSettingsDlg()
{
	if (!m_pSettingsDlg)
		return;

	m_pSettingsDlg->m_ceTargetClassGuid.GetWindowText(m_sTargetClassGuid);
	m_pSettingsDlg->m_ceTargetInstancePath.GetWindowText(m_sTargetInstancePath);
	m_bAutostart = m_pSettingsDlg->m_cbAutostart.GetCheck();
	m_iAutoReenableDevice = m_pSettingsDlg->m_cbAutoReenableDevice.GetCheck();
	m_bMinimizeHide = m_pSettingsDlg->m_cbMinimizeHide.GetCheck();
	m_eActivateMethod = static_cast<CTouchSwitchSettingsDlg::EnumActivateMethod>(m_pSettingsDlg->m_ccbActivateMethod.GetCurSel());
}

void CTouchSwitchDlg::WriteToSettingsDlg()
{
	if (!m_pSettingsDlg)
		return;

	m_pSettingsDlg->m_ceTargetClassGuid.SetWindowText(m_sTargetClassGuid);
	m_pSettingsDlg->m_ceTargetInstancePath.SetWindowText(m_sTargetInstancePath);
	m_pSettingsDlg->m_cbAutostart.SetCheck(m_bAutostart);
	m_pSettingsDlg->m_cbAutoReenableDevice.SetCheck(m_iAutoReenableDevice);
	m_pSettingsDlg->m_cbMinimizeHide.SetCheck(m_bMinimizeHide);
	m_pSettingsDlg->m_ccbActivateMethod.SetCurSel(static_cast<int>(m_eActivateMethod));
}

void CTouchSwitchDlg::ReadFromSettingsProfile()
{
	// Default values in here
	CWinApp *app = AfxGetApp();
	m_sTargetClassGuid = app->GetProfileString(_T(APP_TITLE), _T("TargetClassGuid"), _T(""));
	m_sTargetInstancePath = app->GetProfileString(_T(APP_TITLE), _T("TargetInstancePath"), _T(""));
	m_bAutostart = app->GetProfileInt(_T(APP_TITLE), _T("Autostart"), FALSE);
	m_iAutoReenableDevice = app->GetProfileInt(_T(APP_TITLE), _T("AutoReenableDevice"), BST_CHECKED);
	m_bMinimizeHide = app->GetProfileInt(_T(APP_TITLE), _T("MinimizeHide"), TRUE);
	m_eActivateMethod = static_cast<CTouchSwitchSettingsDlg::EnumActivateMethod>(app->GetProfileInt(_T(APP_TITLE), _T("ActivateMethod"), static_cast<int>(CTouchSwitchSettingsDlg::EnumActivateMethod::LEFT_CLICK)));
	m_bLockPosition = app->GetProfileInt(_T(APP_TITLE), _T("LockPosition"), FALSE);
	m_bStayOnTop = app->GetProfileInt(_T(APP_TITLE), _T("StayOnTop"), TRUE);
	m_iLastX = app->GetProfileInt(_T(APP_TITLE), _T("WindowX"), 0);
	m_iLastY = app->GetProfileInt(_T(APP_TITLE), _T("WindowY"), 0);
	m_iLastWidth = app->GetProfileInt(_T(APP_TITLE), _T("WindowWidth"), 0);
	m_iLastHeight = app->GetProfileInt(_T(APP_TITLE), _T("WindowHeight"), 0);
}

void CTouchSwitchDlg::WriteToSettingsProfile()
{
	CWinApp *app = AfxGetApp();
	app->WriteProfileString(_T(APP_TITLE), _T("TargetClassGuid"), m_sTargetClassGuid);
	app->WriteProfileString(_T(APP_TITLE), _T("TargetInstancePath"), m_sTargetInstancePath);
	app->WriteProfileInt(_T(APP_TITLE), _T("Autostart"), m_bAutostart);
	app->WriteProfileInt(_T(APP_TITLE), _T("AutoReenableDevice"), m_iAutoReenableDevice);
	app->WriteProfileInt(_T(APP_TITLE), _T("MinimizeHide"), m_bMinimizeHide);
	app->WriteProfileInt(_T(APP_TITLE), _T("ActivateMethod"), static_cast<int>(m_eActivateMethod));
	app->WriteProfileInt(_T(APP_TITLE), _T("LockPosition"), m_bLockPosition);
	app->WriteProfileInt(_T(APP_TITLE), _T("StayOnTop"), m_bStayOnTop);
	CRect pos;
	GetWindowRect(&pos);
	if (pos.left > 0 && pos.top > 0 && pos.Width() > 0 && pos.Height() > 0)  // In case the window is minimized
	{
		app->WriteProfileInt(_T(APP_TITLE), _T("WindowX"), pos.left);
		app->WriteProfileInt(_T(APP_TITLE), _T("WindowY"), pos.top);
		app->WriteProfileInt(_T(APP_TITLE), _T("WindowWidth"), pos.Width());
		app->WriteProfileInt(_T(APP_TITLE), _T("WindowHeight"), pos.Height());
	}
}

// TODO: Test this
UINT CTouchSwitchDlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData)
{
	switch (nPowerEvent) {
		case PBT_APMSUSPEND:
			if (m_iAutoReenableDevice == BST_CHECKED)
				SetDeviceStatus(TRUE);
			break;
	}
	return CDialogEx::OnPowerBroadcast(nPowerEvent, lEventData);
}

void CTouchSwitchDlg::OnSessionChange(UINT nSessionState, UINT nId)
{
	switch (nSessionState) {
		case WTS_SESSION_LOCK:
			if (m_iAutoReenableDevice == BST_CHECKED)
				SetDeviceStatus(TRUE);
			break;
	}
	CDialogEx::OnSessionChange(nSessionState, nId);
}

void CTouchSwitchDlg::OnEndSession(BOOL bEnding)
{
	if (m_iAutoReenableDevice != BST_UNCHECKED)
		SetDeviceStatus(TRUE);
	CDialogEx::OnEndSession(bEnding);
}

void CTouchSwitchDlg::OnNcLButtonDblClk(UINT, CPoint)
{
	return;  // Disable double clicking on borders
}

void CTouchSwitchDlg::OnNcCalcSize(BOOL bParam, NCCALCSIZE_PARAMS *params)
{
	// bruh can't get rid of that white bar
	if (params)
		params->rgrc[0].top -= 8;
	CDialogEx::OnNcCalcSize(bParam, params);
}

LRESULT CTouchSwitchDlg::OnNcHitTest(CPoint point)
{
	// remedy the side effect of OnNcCalcSize
	CRect rect;
	GetWindowRect(&rect);

	const int borderWidth = 8;
	if (point.y >= rect.top && point.y <= rect.top + borderWidth)
		return HTTOP;

	return CDialogEx::OnNcHitTest(point);
}

BOOL CTouchSwitchDlg::OnNcActivate(BOOL bParam)
{
	// I'm so sick of this shit. It doesn't work well even
	Invalidate(TRUE);
	return TRUE;
}

BOOL CTouchSwitchDlg::PromptTargetDeviceValidity()
{
	BOOL configValidity = m_pSetupApiHelper->GetConfigValidity();
	BOOL permissionValidity = m_pSetupApiHelper->GetPermissionValidity();

	if (!configValidity)
		AfxMessageBox(_T("Invalid Target Device configuration, or insufficient permissions.\nThis program requires elevated permissions."), MB_ICONEXCLAMATION);
	else if (!permissionValidity)
		AfxMessageBox(_T("Insufficient permissions.\nThis program requires elevated permissions."), MB_ICONEXCLAMATION);

	return configValidity && permissionValidity;
}

