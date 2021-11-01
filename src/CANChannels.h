#ifndef CANChannelsH
#define CANChannelsH

//---------------------------------------------------------------------------

class CANChannels
{
public:
  CANChannels(CyDev *UsbDev, int can_id=2, int tout=1000);
	~CANChannels(void );
	BOOL Open(void);
	BOOL Close(void);
	BOOL Reset(void);
	BOOL Cmd(PUCHAR cmd,PUCHAR replybuf,int buflen);

  int can_id;

private:
	CyDev *FX2; // UsbDevice object

	int can_tout;
  int CANch; // CAN Channel number
};
#endif
