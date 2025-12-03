#ifndef _V2X_MSG_SRM_H_
#define _V2X_MSG_SRM_H_

#include <v2x_msg_common.h>

typedef enum Mde_IntAccessPoint_value
{
	LANE_ID = 0,
	APPROACH_ID,
	CONNECTION_ID
}Mde_IntAccessPoint_value_t;

typedef struct Mde_IntAccessPoint
{
	Mde_IntAccessPoint_value_t msg_id;
	union msg_interAccessPoint_value
	{
		/*  range: 0..255 */
		uint8_t lane;
		
		/*  range: 0..15 */
		uint8_t approach;

		/*  range: 0..255 */
		uint8_t connection;
	}choice;
}Mde_IntAccessPoint_t;


typedef struct Mde_SignalRequest
{
	/*
	 *  element: RoadRegulatorID 
	 *  desc: a globally unique regional assignment value
	 *  range: request_region:0..65535
	 */
	uint8_t request_region_is_exsit;
	uint16_t request_region;
	
	/*
	 *  element: IntersectionID 
 	 *  desc: Note that the value assigned to an intersection will be unique within a given regional ID only
	 *  range: request_region: 0..65535
	 */
	uint16_t request_id;
	
	/*
	 *  element: RequestID 
 	 *  desc: The unique requestID used by the requestor
	 *  range: 0..255
	 */	
	uint8_t requestID;

	/*
	 *  element: PriorityRequestType 
 	 *  desc: The type of request or cancel for priority or preempt use
 	 *  	priorityRequestTypeReserved (0),  
	 *  	priorityRequest             (1),   
	 *  	priorityRequestUpdate       (2), 
	 *  	priorityCancellation        (3), 
	 *  range: 0..3
	 */
	uint8_t requestType;

	Mde_IntAccessPoint_t inBoundLane;

	uint8_t outBoundLane_is_exsit;
	Mde_IntAccessPoint_t outBoundLane;

	/*
 	 *  desc: Estimated time of arrival timestamp
	 *  range: 0..527040
	 *  unit: minute
	 */
	uint8_t minute_is_exsit;
	uint32_t minute;
	
	/*
 	 *  unit: ms
	 *  range: 0..65535
	 */
	uint8_t second_is_exsit;
	uint16_t second;
	
	/*
 	 *  unit: ms
	 *  range: 0..65535
	 */
	uint8_t duration_is_exsit;
	uint16_t duration;
}Mde_SignalRequest_t;

typedef struct Mde_ReqPosition
{
	/*
	 *	desc: request vehicle latitude
	 *	range: -900000000..900000001
	 */
	int32_t positionLat;

	/*
	 *	desc: request vehicle longitude
	 *	range: -1799999999..1800000001
	 *	unit: 1/10 micro degree
	 */
	int32_t positionLong;

	/*
	 *	desc: request vehicle elevation
	 *	range: -4096..61439
	 *	unit: 0.1m
	 */
	uint8_t positionElevation_is_exsit;
	int32_t positionElevation;

	/*
	 *	desc: request vehicle heading
	 *	range: 0..28800
	 *	unit: 0.0125 degrees
	 */
	uint8_t heading_is_exsit;
	uint16_t heading;

	/*
	 *	desc: request vehicle speed
	 *	--- transmisson ---
	 *	range: transmisson: 0..7,  
	 *		neutral 	 (0), -- Neutral
	 *		park		 (1), -- Park 
	 *		forwardGears (2), -- Forward gears
	 *		reverseGears (3), -- Reverse gears 
	 *		reserved1	 (4),	   
	 *		reserved2	 (5),	   
	 *		reserved3	 (6),	   
	 *		unavailable  (7)  -- not-equipped or unavailable value,
	 *	--- speed ---
	 *	range: speed: 0..8191  
	 *	unit: speed: 0.02 m/s 
	 */
	uint8_t transmisson_speed_is_exsit;
	uint8_t transmisson;
	uint16_t speed;
}Mde_ReqPosition_t;

typedef struct Mde_ReqDes
{
	Mde_vehicle_id_t veh_id;

	uint8_t requestorType_is_exsit;
	Mde_RequestorType_t requestorType;

	uint8_t position_is_exsit;
	Mde_ReqPosition_t position;
	
	/*
	 *  desc: Request vehicle name
	 *  size: 1..63
	 */
	uint8_t name_is_exsit;
	char name[64]; 
	
	/*
	 *  desc: Request vehicle route name
	 *  size: 1..63
	 */
	uint8_t routeName_is_exsit;
	char routeName[64]; 
	
	/*
	 *  element: Bit string
	 *  desc: current vehicle state
	 *  range: 0..1
	 *  	loading     (0), -- parking and unable to move at this time
	 *  	anADAuse    (1), -- an ADA access is in progress (wheelchairs, kneeling, etc.)
	 *  	aBikeLoad   (2), -- loading of a bicycle is in progress
	 *  	doorOpen    (3), -- a vehicle door is open for passenger access
	 *  	charging    (4), -- a vehicle is connected to charging point
	 *  	atStopLine  (5)  -- a vehicle is at the stop line for the lane it is in
	 */
	uint8_t transitStatus_is_exsit;
	uint8_t transitStatus[8];

	/*
	 *  desc: current vehicle occupancy 
	 *  range: 0..7
	 *  	occupancyUnknown    (0),  
	 *  	occupancyEmpty      (1),  
	 *  	occupancyVeryLow    (2),  
	 *  	occupancyLow        (3),  
	 *  	occupancyMed        (4),  
	 *  	occupancyHigh       (5),  
	 *  	occupancyNearlyFull (6),  
	 *  	occupancyFull       (7)
	 */
	uint8_t transitOccupancy_is_exsit;
	uint8_t transitOccupancy;

	/*
	 *  desc: current vehicle schedule adherence 
	 *  range: -122 .. 121
	 *  unit: 10s
	 */
	uint8_t transitSchedule_is_exsit;
	int8_t transitSchedule;	
}Mde_ReqDes_t;

typedef struct v2x_msg_srm
{
	/*
	 *  element: timeStamp_is_exist 
	 *  desc: timeStamp is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t timeStamp_is_exist;
	/*
     *  element: MinuteOfTheYear 
     *  desc: timeStamp. the value 527040 shall be used for invalid
     *  range: 0..527040
	 */
	uint32_t timeStamp;

	/*
     *  element: DSecond 
     *  desc: timeStamp. 
     *  units: milliseconds
     *  range: 0..65535
	 */
	uint16_t second;
	
	/*
     *  element: MsgCount
     *  range: 0..127
	 */
	uint8_t sequenceNumber_is_exist;
	uint8_t sequenceNumber;

	/*
     *  element: SignalRequestList
     *  Request Data for one or more signalized intersections that support SRM dialogs
     *  range: 0..32
	 */
	uint8_t requests_count;
	Mde_SignalRequest_t requests[32];

	Mde_ReqDes_t requestor;
}v2x_msg_srm_t;



#endif

