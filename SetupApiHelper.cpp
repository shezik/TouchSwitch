#include "pch.h"
#include "SetupApiHelper.h"
#include <combaseapi.h>
#include <cfgmgr32.h>
#include <RegStr.h>

SetupApiHelper::SetupApiHelper(CString targetClassGuid, CString targetInstancePath)
	: m_hDevInfo(INVALID_HANDLE_VALUE)
	, m_sTargetInstancePath(targetInstancePath)
	, m_bHasGetPermission(TRUE)
	, m_bHasSetPermission(TRUE)
{
	ZeroMemory(&m_sDevinfoData, sizeof(SP_DEVINFO_DATA));

	if (CLSIDFromString(targetClassGuid, &m_sTargetClassGuid) != S_OK) {
		m_sTargetClassGuid = GUID_NULL;
		return;
	}

	m_hDevInfo = SetupDiGetClassDevs(&m_sTargetClassGuid, NULL, NULL, DIGCF_PRESENT);
	if (m_hDevInfo == INVALID_HANDLE_VALUE)
		return;

	SP_DEVINFO_DATA devinfoData{};
	devinfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; \
		SetupDiEnumDeviceInfo(m_hDevInfo, i, &devinfoData); \
		devinfoData.cbSize = sizeof(SP_DEVINFO_DATA), i++)
	{
		TCHAR thisInstanceId[MAX_DEVICE_ID_LEN];
		if (!SetupDiGetDeviceInstanceId(m_hDevInfo, &devinfoData, thisInstanceId, MAX_DEVICE_ID_LEN, NULL))
			continue;

		if (m_sTargetInstancePath == thisInstanceId)
		{
			m_sDevinfoData = devinfoData;  // Relax, no allocations inside.

			BOOL enabled;
			m_bHasGetPermission = GetDeviceEnabled(&enabled);
			if (m_bHasGetPermission)
				m_bHasSetPermission = SetDeviceEnabled(enabled);
			return;
		}
	}
	// No matches found, do nothing.
}

SetupApiHelper::~SetupApiHelper()
{
	if (m_hDevInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(m_hDevInfo);
		m_hDevInfo = INVALID_HANDLE_VALUE;
	}
}

BOOL SetupApiHelper::SetDeviceEnabled(BOOL status)
{
	if (!m_sDevinfoData.cbSize)
		return FALSE;

	SP_PROPCHANGE_PARAMS params{};
	params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	params.StateChange = status ? DICS_ENABLE : DICS_DISABLE;
	params.Scope = DICS_FLAG_GLOBAL;
	params.HwProfile = 0;
	if (!SetupDiSetClassInstallParams(m_hDevInfo, &m_sDevinfoData, reinterpret_cast<PSP_CLASSINSTALL_HEADER>(&params), sizeof(params)))
	{
		//DWORD err = GetLastError();
		m_bHasSetPermission = FALSE;
		return FALSE;
	}

	if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, m_hDevInfo, &m_sDevinfoData))
	{
		//DWORD err = GetLastError();
		m_bHasSetPermission = FALSE;
		return FALSE;  // GetLastError could say something like 0xe0000231 NotDisableable here.
	}

	m_bHasSetPermission = TRUE;
	return TRUE;
}

BOOL SetupApiHelper::GetDeviceEnabled(BOOL *enabled)
{
	if (!m_sDevinfoData.cbSize)
		return FALSE;

	DWORD configFlags;
	if (!SetupDiGetDeviceRegistryProperty(m_hDevInfo, &m_sDevinfoData, SPDRP_CONFIGFLAGS, NULL, (PBYTE)&configFlags, sizeof(configFlags), NULL))
	{
		m_bHasGetPermission = FALSE;
		return FALSE;
	}

	*enabled = !(configFlags & CONFIGFLAG_DISABLED);

	m_bHasGetPermission = TRUE;
	return TRUE;
}

BOOL SetupApiHelper::GetConfigValidity() const
{
	return m_sDevinfoData.cbSize;
}

BOOL SetupApiHelper::GetPermissionValidity() const
{
	return m_bHasGetPermission && m_bHasSetPermission;
}

