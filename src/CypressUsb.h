//---------------------------------------------------------------------------
#ifndef CypressUsbH
#define CypressUsbH

//---------------------------------------------------------------------------

#define _WIN32_WINNT 0x0500
#define WINVER 0x0500

#include <windows.h>
#include <winuser.h>
#include <wtypes.h>
#include <initguid.h>

extern "C" {
// Declare the C libraries use
#include <basetyps.h>
#include <stdio.h>
#include <setupapi.h>
#include <dbt.h>
}

#include "cyioctl.h"

static GUID CYUSBDRV_GUID={0x7F84D80A, 0x1966, 0x43ce, 0xAD, 0x65, 0xBD, 0xEE, 0x69, 0xD3, 0xC7, 0xD7};

class CyDev
{
public:
        CyDev(HWND winHandle,GUID guid=CYUSBDRV_GUID);
	~CyDev(void );
	HANDLE Open(int);
	void Close(void);
	HANDLE DeviceHandle(void);
	BOOL IsOpen(void);
	int VendRead(UCHAR *buf, int bufLen, UCHAR ReqCode, WORD Value, WORD Index);
	int VendWrite(UCHAR *buf, int bufLen, UCHAR ReqCode, WORD Value, WORD Index);
	void CyReset(void);
	void CyReconnect(void);
	bool BulkRead(UCHAR *buf, LONG bufLen, int tout=5000);
  bool BulkWrite(UCHAR *buf, LONG bufLen, int tout=5000);
	bool BulkClear(void);
  bool BulkWriteClear(void);
  bool CkHighSpeed(void);
	//void Err(char *msg, char *hdr = "Message");
	USHORT VendorID;
	USHORT ProductID;
	WCHAR SerNum[10];
  ULONG Speed; // 0 - unknown, 1 LOW_FULL, 2 HIGH
  HANDLE topwinHandle;
private:
        BOOL DoRegisterDeviceInterface(GUID,HDEVNOTIFY *);
        BOOL DoRegisterDeviceHandle(HANDLE, HDEVNOTIFY *);
	int GetDeviceIDS(void);
	DWORD BytesWritten,BytesRead;
	HANDLE devHandle;
	HDEVNOTIFY hInterfaceNotify;
	HDEVNOTIFY hHandleNotify;
	DWORD OvrFlg;
	_OVERLAPPED OvStr;
	HANDLE Event;
	int WrSucc,RdSucc,NbRead;
	HANDLE USBMutex;
	GUID drv_guid;
	DWORD LastErr;
};
#endif

