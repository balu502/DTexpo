/*
 *    Cypress FX2LP USB controller library
 *    uses CyUSB.sys driver from Cypress
 *    DNA Technology
 *    ver. 0.9 27.02.2007 Control EP communication and EP6 bulk read
 *    ver. 0.91 10.03.2007 SerNum overlow error fixed
 *    ver. 0.92 02.04.2008  CkHighSpeed() added to check High Speed connection for bulk rd/wr
 */
//------------------------------------------------------

#include "CypressUsb.h"
#include<QDebug>

//------------------------------------------------------
//static GUID GUID_ADAPT_DEVICE_REQUEST_NOTIFICATION = {0xdd57f00d, 0xf1d4, 0x4b88, 0xac, 0x34, 0x28, 0xd7, 0x33, 0x28, 0xf8, 0x36};

char Str[132];
#define NOISY(_x_) printf _x_ ;
// #define SERVICE_PRG     // some debug stuff for "internal use only" programs

//---------------------------------------------------------------------------
CyDev::CyDev(HWND winHandle,GUID guid)
{
	devHandle = INVALID_HANDLE_VALUE;
	USBMutex = INVALID_HANDLE_VALUE;
  //int deviceNumber = 0; // Can be other values if more than 1 device connected to

  USBMutex=CreateMutex(NULL,FALSE,NULL/*,(const WCHAR*)"USBPortMutex"*/);
 // USBMutex=CreateMutex(NULL,FALSE,(const WCHAR*)"USBPortMutex");
	if (USBMutex==INVALID_HANDLE_VALUE){
#ifdef SERVICE_PRG
		exit(1);
#endif
	}
	Event = CreateEvent(
		NULL,	// pointer to security attributes
		FALSE,	// flag for manual-reset event
		FALSE,	// flag for initial state
    //(const WCHAR*)"Read Wait" // pointer to event-object name
    NULL 	// (const WCHAR*)"Read Wait" // pointer to event-object name
	);

	OvStr.Offset     = 0;
	OvStr.OffsetHigh = 0;
	OvStr.hEvent     = Event;
	drv_guid = guid;
	ProductID = 0;
	VendorID=0;
	ZeroMemory(SerNum,sizeof(SerNum));
	topwinHandle = winHandle;
        DoRegisterDeviceInterface(guid,&hInterfaceNotify);

	//devHandle = Open(deviceNumber);
}

CyDev::~CyDev(void)
{
  if (IsOpen())  Close();  // close USB device if it's opened
	if (hInterfaceNotify)
                UnregisterDeviceNotification(hInterfaceNotify);
  if (USBMutex!=INVALID_HANDLE_VALUE) CloseHandle(USBMutex);
}

HANDLE CyDev::Open(int deviceNumber)
{
	SP_DEVINFO_DATA devInfoData;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData;
	ULONG requiredLength = 0;
	HANDLE hDevice=INVALID_HANDLE_VALUE;

	WaitForSingleObject(USBMutex,INFINITE);

	HDEVINFO hwDeviceInfo = SetupDiGetClassDevs ( (LPGUID) &drv_guid,
                                                        NULL,
                                                        NULL,
                                                        DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

	if (hwDeviceInfo != INVALID_HANDLE_VALUE) {
		devInterfaceData.cbSize = sizeof(devInterfaceData);
		if (SetupDiEnumDeviceInterfaces ( hwDeviceInfo, 0, (LPGUID) &drv_guid,
											deviceNumber, &devInterfaceData)) {
			SetupDiGetInterfaceDeviceDetail ( hwDeviceInfo, &devInterfaceData, NULL, 0,&requiredLength, NULL);
			ULONG predictedLength = requiredLength;
			functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(predictedLength);
			functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
			devInfoData.cbSize = sizeof(devInfoData);
			if (SetupDiGetInterfaceDeviceDetail (hwDeviceInfo,&devInterfaceData,
													functionClassDeviceData,
                          predictedLength,&requiredLength,
													&devInfoData)) {
				hDevice = CreateFile (functionClassDeviceData->DevicePath,GENERIC_WRITE | GENERIC_READ,
										FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,
										OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
				free(functionClassDeviceData);
				SetupDiDestroyDeviceInfoList(hwDeviceInfo);
			}
		}
	}

	devHandle=hDevice;
	if (devHandle!=INVALID_HANDLE_VALUE) {
    	GetDeviceIDS();
      CkHighSpeed();
     DoRegisterDeviceHandle(hDevice,&hHandleNotify);
	}

	ReleaseMutex(USBMutex);
	return hDevice;
}

HANDLE CyDev::DeviceHandle(void)
{
	return devHandle;
}

BOOL CyDev::IsOpen(void)
{

/*
	UCHAR buf[100];
  if (VendRead(buf,100, 0x41, 0,0)<0) {
		Close();     // no communication, close device
		return false;
	}
	else return true;
*/
  return (devHandle!=INVALID_HANDLE_VALUE);
}

void CyDev::Close(void)
{
	int i;
	BOOL res;
        if (hHandleNotify) {
                if (!UnregisterDeviceNotification(hHandleNotify))
      i=GetLastError();
    }
  CloseHandle(devHandle);
	devHandle = INVALID_HANDLE_VALUE;
}

int CyDev::VendRead(UCHAR *buf, int bufLen, UCHAR ReqCode, WORD Value, WORD Index)
{
	int ret;
	union {
		struct {
			UCHAR Recipient:5;
			UCHAR Type:2;
			UCHAR Direction:1;
		} bmRequest;
		UCHAR bmReq;
	};

	WaitForSingleObject(USBMutex,INFINITE);

	bmRequest.Recipient = 0; // Device
	bmRequest.Type = 2; // Vendor
	bmRequest.Direction = 1; // IN command (from Device to Host)
	int iXmitBufSize = sizeof(SINGLE_TRANSFER) + bufLen; // The size of the two-part structure
	UCHAR *pXmitBuf = new UCHAR[iXmitBufSize]; // Allocate the memory
	ZeroMemory(pXmitBuf, iXmitBufSize);
	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER)pXmitBuf; // The SINGLE_TRANSFER comes first
	pTransfer->SetupPacket.bmRequest = bmReq;
	pTransfer->SetupPacket.bRequest = ReqCode;
	pTransfer->SetupPacket.wValue = Value;
	pTransfer->SetupPacket.wIndex = Index;
	pTransfer->SetupPacket.wLength = bufLen;
	pTransfer->SetupPacket.ulTimeOut = 5000 / 1000;
	//pTransfer->WaitForever = false;
	pTransfer->ucEndpointAddress = 0x00; // Control pipe
	pTransfer->IsoPacketLength = 0;
	pTransfer->BufferOffset = sizeof (SINGLE_TRANSFER);
	pTransfer->BufferLength = bufLen;
	DWORD dwReturnBytes;
	if (DeviceIoControl (devHandle, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
								pXmitBuf, iXmitBufSize,pXmitBuf, iXmitBufSize,
						&dwReturnBytes, NULL)) {
 // Copy data into buf
		UCHAR *ptr = pXmitBuf + sizeof (SINGLE_TRANSFER);
		memcpy(buf, ptr, dwReturnBytes-sizeof (SINGLE_TRANSFER));
		ret=dwReturnBytes-sizeof (SINGLE_TRANSFER);
	}
	else ret = -1; // communication error
	delete(pXmitBuf);

	ReleaseMutex(USBMutex);

	return ret;
}

int CyDev::VendWrite(UCHAR *buf, int bufLen, UCHAR ReqCode, WORD Value, WORD Index)
{
	int ret;
	union {
		struct {
			UCHAR Recipient:5;
			UCHAR Type:2;
			UCHAR Direction:1;
		} bmRequest;
		UCHAR bmReq;
	};

	WaitForSingleObject(USBMutex,INFINITE);

	bmRequest.Recipient = 0; // Device
	bmRequest.Type = 2; // Vendor
	bmRequest.Direction = 0; // OUT command (from Host to Device )
	int iXmitBufSize = sizeof(SINGLE_TRANSFER) + bufLen; // The size of the two-part structure
	UCHAR *pXmitBuf = new UCHAR[iXmitBufSize]; // Allocate the memory
	ZeroMemory(pXmitBuf, iXmitBufSize);
	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER)pXmitBuf; // The SINGLE_TRANSFER comes first
	pTransfer->SetupPacket.bmRequest = bmReq;
	pTransfer->SetupPacket.bRequest = ReqCode;
	pTransfer->SetupPacket.wValue = Value;
	pTransfer->SetupPacket.wIndex = Index;
	pTransfer->SetupPacket.wLength = bufLen;
	pTransfer->SetupPacket.ulTimeOut = 5000 / 1000;
	//pTransfer->WaitForever = false;
	pTransfer->ucEndpointAddress = 0x00; // Control pipe
	pTransfer->IsoPacketLength = 0;
	pTransfer->BufferOffset = sizeof (SINGLE_TRANSFER);
	pTransfer->BufferLength = bufLen;
	DWORD dwReturnBytes;

	// Copy data from buf

	UCHAR *ptr = pXmitBuf + sizeof (SINGLE_TRANSFER);
	memcpy(ptr, buf, bufLen);
	if (DeviceIoControl (devHandle, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
								pXmitBuf, iXmitBufSize,pXmitBuf, iXmitBufSize,
						&dwReturnBytes, NULL)) {
		ret = dwReturnBytes-sizeof (SINGLE_TRANSFER);
	}
	else ret = -1; // communication error
	delete(pXmitBuf);

	ReleaseMutex(USBMutex);

	return ret;
}

bool CyDev::BulkRead(UCHAR *buf, LONG bufLen, int tout)
{
	DWORD wRes;
	BOOL bResult;
	DWORD dwErr;
  if ( devHandle == INVALID_HANDLE_VALUE || Speed!=DEVICE_SPEED_HIGH ) return false;

	WaitForSingleObject(USBMutex,INFINITE);


	int iXmitBufSize = sizeof (SINGLE_TRANSFER);
	PUCHAR pXmitBuf = new UCHAR[iXmitBufSize];
	ZeroMemory (pXmitBuf, iXmitBufSize);
	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER) pXmitBuf;
	pTransfer->ucEndpointAddress = 0x86; // IN Endpoint 6
	pTransfer->IsoPacketLength = 0;
	pTransfer->BufferOffset = 0;
	pTransfer->BufferLength = 0;
	DWORD dwReturnBytes;
	if (DeviceIoControl (devHandle,IOCTL_ADAPT_SEND_NON_EP0_DIRECT,
					pXmitBuf, iXmitBufSize,buf, bufLen,
					&dwReturnBytes, &OvStr)) {  // ok, no wait
		delete(pXmitBuf);
		ReleaseMutex(USBMutex);
		return true;
	}
	else {
		dwErr = GetLastError();
	//return pXmitBuf;
		if (dwErr!=ERROR_IO_PENDING) {  // coomunication error
			delete(pXmitBuf);
			ReleaseMutex(USBMutex);
			return false;
		}
	}

	wRes = WaitForSingleObject(Event,tout);
	if(wRes == WAIT_OBJECT_0)
	{
		GetOverlappedResult(
		devHandle,	// handle of file, pipe, or communications device
		&OvStr,	// address of overlapped structure
		&BytesRead,	// address of actual bytes count
		FALSE 	// wait flag
		);
		delete(pXmitBuf);
		ReleaseMutex(USBMutex);
		return true;
	}
	delete(pXmitBuf);
	ReleaseMutex(USBMutex);
	return false;
}

bool CyDev::BulkWrite(UCHAR *buf, LONG bufLen, int tout)
{
  DWORD wRes;
  BOOL bResult;
  DWORD dwErr;
  if ( devHandle == INVALID_HANDLE_VALUE || Speed!=DEVICE_SPEED_HIGH ) return false;

  WaitForSingleObject(USBMutex,INFINITE);

  if ( devHandle == INVALID_HANDLE_VALUE ) return false;
  int iXmitBufSize = sizeof (SINGLE_TRANSFER);
  PUCHAR pXmitBuf = new UCHAR[iXmitBufSize];
  ZeroMemory (pXmitBuf, iXmitBufSize);
  PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER) pXmitBuf;
  pTransfer->ucEndpointAddress = 0x02; // OUT Endpoint 2
  pTransfer->IsoPacketLength = 0;
  pTransfer->BufferOffset = 0;
  pTransfer->BufferLength = 0;
  DWORD dwReturnBytes;
  if (DeviceIoControl (devHandle,IOCTL_ADAPT_SEND_NON_EP0_DIRECT,
          pXmitBuf, iXmitBufSize,buf, bufLen,
          &dwReturnBytes, &OvStr)) {  // ok, no wait
    delete(pXmitBuf);
    ReleaseMutex(USBMutex);
    return true;
  }
  else {
    dwErr = GetLastError();
  //return pXmitBuf;
    if (dwErr!=ERROR_IO_PENDING) {  // coomunication error
      delete(pXmitBuf);
      ReleaseMutex(USBMutex);
      return false;
    }
  }

  wRes = WaitForSingleObject(Event,tout);
  if(wRes == WAIT_OBJECT_0)
  {
    GetOverlappedResult(
    devHandle,	// handle of file, pipe, or communications device
    &OvStr,	// address of overlapped structure
    &BytesRead,	// address of actual bytes count
    FALSE 	// wait flag
    );
    delete(pXmitBuf);
    ReleaseMutex(USBMutex);
    return true;
  }
  delete(pXmitBuf);
  ReleaseMutex(USBMutex);
  return false;
}


BOOL CyDev::DoRegisterDeviceInterface(
	GUID InterfaceClassGuid,
	HDEVNOTIFY *hDevNotify
)
/*
Routine Description:
    Registers for notification of changes in the device interfaces for
	the specified interface class GUID.

Parameters:
    InterfaceClassGuid - The interface class GUID for the device 
        interfaces. 

    hDevNotify - Receives the device notification handle. On failure, 
		this value is NULL.

Return Value:
	If the function succeeds, the return value is TRUE.

	If the function fails, the return value is FALSE.
*/
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	DWORD Err;
	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = InterfaceClassGuid;

	*hDevNotify = RegisterDeviceNotification( topwinHandle,
		&NotificationFilter,
		DEVICE_NOTIFY_WINDOW_HANDLE
	);

	if(!*hDevNotify)
	{
		Err = GetLastError();
		LastErr = Err;
		//printf( "RegisterDeviceNotification failed: %lx.\n", Err);
		return FALSE;
	}

	return TRUE;
}

BOOL CyDev::DoRegisterDeviceHandle(HANDLE hDev, HDEVNOTIFY *hDevNotify)
{
	DEV_BROADCAST_HANDLE    filter;

	ZeroMemory (&filter,sizeof(filter)); //zero the structure
	filter.dbch_size = sizeof(filter);
	filter.dbch_devicetype = DBT_DEVTYP_HANDLE;
	filter.dbch_handle = hDev;
 
	*hDevNotify = RegisterDeviceNotification(topwinHandle, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (*hDevNotify==NULL) {
		return FALSE;
	}
	return TRUE;
}


int CyDev::GetDeviceIDS(void)
{
	int ret,serlen;
	PUSB_DEVICE_DESCRIPTOR usbdsc;
	PUSB_STRING_DESCRIPTOR strdsc;

	WaitForSingleObject(USBMutex,INFINITE);

	int iXmitBufSize = sizeof(SINGLE_TRANSFER) + sizeof(USB_DEVICE_DESCRIPTOR); // The size of the two-part structure
	UCHAR *pXmitBuf = new UCHAR[iXmitBufSize]; // Allocate the memory
	ZeroMemory(pXmitBuf, iXmitBufSize);
	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER)pXmitBuf; // The SINGLE_TRANSFER comes first
	pTransfer->SetupPacket.bmRequest = 0x80; //
	pTransfer->SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
	pTransfer->SetupPacket.wValue = (USB_DEVICE_DESCRIPTOR_TYPE<<8);
	pTransfer->SetupPacket.wIndex = 0;
	pTransfer->SetupPacket.wLength = sizeof(USB_DEVICE_DESCRIPTOR);
	pTransfer->SetupPacket.ulTimeOut = 5000 / 1000;
	//pTransfer->WaitForever = false;
	pTransfer->ucEndpointAddress = 0x00; // Control pipe
	pTransfer->IsoPacketLength = 0;
	pTransfer->BufferOffset = sizeof (SINGLE_TRANSFER);
	pTransfer->BufferLength = sizeof(USB_DEVICE_DESCRIPTOR);
	DWORD dwReturnBytes;
	if (DeviceIoControl (devHandle, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
								pXmitBuf, iXmitBufSize,pXmitBuf, iXmitBufSize,
						&dwReturnBytes, NULL)) {
// Copy data into buf
		UCHAR *ptr = pXmitBuf + sizeof (SINGLE_TRANSFER);
		usbdsc = (PUSB_DEVICE_DESCRIPTOR)ptr;
		VendorID = usbdsc->idVendor;
		ProductID = usbdsc->idProduct;
		// get serial number from USB descriptor
		pTransfer->SetupPacket.wValue =
		(USB_STRING_DESCRIPTOR_TYPE<<8)+usbdsc->iSerialNumber; ;
        DeviceIoControl (devHandle, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
								pXmitBuf, iXmitBufSize,pXmitBuf, iXmitBufSize,
						&dwReturnBytes, NULL);
		ret=dwReturnBytes-sizeof (SINGLE_TRANSFER);
		strdsc = (PUSB_STRING_DESCRIPTOR)ptr;
		serlen = strdsc->bLength - 2;
		if (serlen<sizeof(SerNum)) {
			memcpy(SerNum, strdsc->bString,serlen);
			SerNum[serlen/sizeof(WCHAR)]=0;
		}
		else {
			memcpy(SerNum, strdsc->bString, sizeof (SerNum)-1);
			SerNum[sizeof(SerNum)/sizeof(WCHAR)]=0;
		}
	}
	else ret = -1; // communication error
	delete(pXmitBuf);

	ReleaseMutex(USBMutex);

	return ret;
}
/*
 *  Remove IO request from a queue
 */
bool CyDev::BulkClear(void )
{
  DWORD dwBytes = 0;
  UCHAR Address = 0x86;

  if ( devHandle == INVALID_HANDLE_VALUE ) return false;

  DeviceIoControl(devHandle, IOCTL_ADAPT_ABORT_PIPE,
          &Address, sizeof (UCHAR),NULL, 0,
          &dwBytes, NULL);
  return true;
}


bool CyDev::BulkWriteClear(void )
{
  DWORD dwBytes = 0;
  UCHAR Address = 0x02;

  if ( devHandle == INVALID_HANDLE_VALUE ) return false;

  DeviceIoControl(devHandle, IOCTL_ADAPT_ABORT_PIPE,
          &Address, sizeof (UCHAR),NULL, 0,
          &dwBytes, NULL);
  return true;
}
/*
 *  Check if current USB connection speed is high
 */
bool CyDev::CkHighSpeed(void )
{
  DWORD dwBytes = 0;
  ULONG DevSpeed;

  if ( devHandle == INVALID_HANDLE_VALUE ) return false;

  DeviceIoControl(devHandle, IOCTL_ADAPT_GET_DEVICE_SPEED,
                 &DevSpeed, sizeof (ULONG),
                 &DevSpeed, sizeof (ULONG),
                 &dwBytes, NULL);

  Speed = DevSpeed;
  if (DevSpeed==DEVICE_SPEED_HIGH)
    return true;
  else return false;
}
