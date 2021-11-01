// CAN node IDs
#define UCIO_CANID      1   // IO uC CANID
#define UCTEMP_CANID    3   // temperature controller
#define UCOPTICS_CANID  2   // optics controller
#define UCMOTOR_CANID   4   // motors controller
#define UCDISP_CANID    5   // display controller
#define UCT1_CANID      6   // temp stend controller 1
#define UCT2_CANID      7   // temp stend controller 2
#define UCT3_CANID      8   // temp stend controller 3
#define UCT4_CANID      9   // temp stend controller 4
#define UCT5_CANID      10   // temp stend controller 5

/* DT-964 CAN events */
#define CEV_CAPT      1 // picture capture
#define CEV_PICRDY    2 // picture is ready in video memory
#define CEV_PICXFER   3 // picture transfer from videomemory is in progress
#define CEV_PICTEMP   4 // temperature level with picture capturing
#define CEV_WHEELRDY  5 // filter wheel rotation is completed
#define CEV_ENDPTEMP  6 // end of temperature level with picture capturing
#define CEV_PBUTTON   7 // front panel button was pressed
#define CEV_LEDON     8 // next led is on
#define CEV_READBC    9 // read bar code 
// next group of events - temperature program execution
#define CEV_TPROGSTR  16 // start of temperature program execution
#define CEV_TPROGSTP  17 // end of temperature program execution
#define CEV_TPROGCNT  18 // restart (continue) after power failure
// SYNC events
#define CEV_SYNCREQ   32 // SYNC request - somebody wants to be sure everybody is ON
#define CEV_SYNCRPL   33 // SYNC reply - to acknowledge we are alive
#define CEV_SLEEP     34 // device sleep command
#define CEV_WAKEUP    35 // wakeup DT system
#define CEV_CONFCHG   36 // change config state 

// Fast channel event
#define CEV_FCRDY     48 // Fast channel is ready

