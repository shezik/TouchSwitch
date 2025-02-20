#pragma once

#include <SetupAPI.h>
#include <atlstr.h>

// Adapted from https://stackoverflow.com/questions/1438371/win32-api-function-to-programmatically-enable-disable-device
// Thank you!

class SetupApiHelper
{
public:
	SetupApiHelper(CString, CString);
	~SetupApiHelper();
	BOOL SetDeviceEnabled(BOOL);
	BOOL GetDeviceEnabled(BOOL*);
	BOOL GetConfigValidity() const;
	BOOL GetPermissionValidity() const;

private:
	GUID m_sTargetClassGuid;
	CString m_sTargetInstancePath;
	HDEVINFO m_hDevInfo;
	SP_DEVINFO_DATA m_sDevinfoData;
	BOOL m_bHasGetPermission;
	BOOL m_bHasSetPermission;
};

