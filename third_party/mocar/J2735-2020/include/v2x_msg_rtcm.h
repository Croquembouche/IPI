#ifndef __V2X_MSG_RTCM_H_
#define __V2X_MSG_RTCM_H_

#include <v2x_msg_common.h>






/* AntennaOffsetSet */
typedef struct Mde_AntennaOffsetSet {
	long	 antOffsetX;
	long	 antOffsetY;
	long	 antOffsetZ;
} Mde_AntennaOffsetSet_t;

typedef struct Mde_GNSSstatus_BIT {
	#define MDE_GNSSSTATUS_BIT_MAX (8)
	uint8_t buf[MDE_GNSSSTATUS_BIT_MAX];	/* BIT STRING body */
	/*
	unavailable (0), -- Not Equipped or unavailable
	isHealthy (1),
	isMonitored (2),
	baseStationType (3), -- Set to zero if a moving base station,
	-- or if a rover device (an OBU),
	-- set to one if it is a fixed base station
	aPDOPofUnder5 (4), -- A dilution of precision greater than 5
	inViewOfUnder5 (5), -- Less than 5 satellites in view
	localCorrectionsPresent (6), -- DGPS type corrections used
	networkCorrectionsPresent (7) -- RTK type corrections used	
	*/	
	unsigned long size;	/* Size of the above buffer */
} Mde_GNSSstatus_BIT_t;

/* RTCMheader */
typedef struct Mde_rtcm_header {
	Mde_GNSSstatus_BIT_t	 status;
	Mde_AntennaOffsetSet_t	 offsetSet;
} Mde_rtcm_header_t;

typedef struct Mde_RTCMmessage {
	int size;
	#define MDE_RTCM_MESSAGE_MAX (1023)
	uint8_t buff[MDE_RTCM_MESSAGE_MAX];
}Mde_RTCMmessage_t;


/* RTCMcorrections */
typedef struct v2x_msg_rtcm 
{
	long	 msgCnt;
	long	 rev;
	int     timeStamp_active;
	long	timeStamp	/* OPTIONAL */;
	int     anchorPoint_active;
	Mde_FullPositionVector_t	anchorPoint	/* OPTIONAL */;

	int     rtcmHeader_active;
	Mde_rtcm_header_t	rtcmHeader	/* OPTIONAL */;
    uint32_t msgs_count;
	#define MSGS_COUNT_MAX (5)
    Mde_RTCMmessage_t msgs[MSGS_COUNT_MAX];
} v2x_msg_rtcm_t;

#endif
