#ifndef __V2X_MSG_NMEA_H_
#define __V2X_MSG_NMEA_H_

#include <v2x_msg_common.h>

typedef struct v2x_msg_nmea 
{
	int  timeStamp_active;
	long	timeStamp	/* OPTIONAL */;
	int  rev_active;
	long rev	/* OPTIONAL */;
	int  msg_active;
	long	msg	/* OPTIONAL */;
	int  wdCount_active;
	long	wdCount	/* OPTIONAL */;

	int payload_len;
	#define MDE_NMEA_PAYLOAD_MAX (1023)
	uint8_t	 payload[MDE_NMEA_PAYLOAD_MAX];
} v2x_msg_nmea_t;

#endif
