
// TouchSwitch.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTouchSwitchApp:
// See TouchSwitch.cpp for the implementation of this class
//

class CTouchSwitchApp : public CWinApp
{
public:
	CTouchSwitchApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()

private:
	CString GetExecutablePath();
};

extern CTouchSwitchApp theApp;


// User-defined messages
//

#define APP_TITLE "TouchSwitch"
#define APP_VERSION "1.0.0.2"  // Remember to update value in RC!
#define WM_APP_TRAYICON (WM_USER + 1)
#define WM_APP_FREE_SETTINGS_DLG (WM_USER + 2)
