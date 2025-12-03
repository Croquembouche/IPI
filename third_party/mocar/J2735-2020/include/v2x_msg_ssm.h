#ifndef _V2X_MSG_SSM_H_
#define _V2X_MSG_SSM_H_

#include <v2x_msg_common.h>

typedef struct Mde_SignalRequesterInfo
{
	Mde_vehicle_id_t id;

	/*
     *  element: RequestID
     *  range: 0..255
	 */
	uint8_t request;

	/*
     *  element: MsgCount
     *  range: 0..127
	 */
	uint8_t sequenceNumber;

	/*
	 *  desc: Basic role of this user at this time
	 *  range: 0..22
 	 *  		-- Values used in the EU and in the US
	 *  	basicVehicle     (0), -- Light duty passenger vehicle type
	 *  	publicTransport  (1), -- Used in EU for Transit us
	 *  	specialTransport (2), -- Used in EU (e.g. heavy load)
	 *  	dangerousGoods   (3), -- Used in EU for any HAZMAT
	 *  	roadWork         (4), -- Used in EU for State and Local DOT uses
	 *  	roadRescue       (5), -- Used in EU and in the US to include tow trucks.
	 *  	emergency        (6), -- Used in EU for Police, Fire and Ambulance units
	 *  	safetyCar        (7), -- Used in EU for Escort vehicles
	 *  		-- Begin US unique numbering
	 *  	none-unknown     (8), -- added to follow current SAE style guidelines
	 *  	truck            (9), -- Heavy trucks with additional BSM rights and obligations
	 *  	motorcycle      (10), --
	 *  	roadSideSource  (11), -- For infrastructure generated calls such as fire house, rail infrastructure, roadwork site, etc.
	 *  	police          (12), --
	 *  	fire            (13), --
	 *  	ambulance       (14), -- (does not include private para-transit etc.)
	 *  	dot             (15), -- all roadwork vehicles
	 *  	transit         (16), -- all transit vehicles 
	 *  	slowMoving      (17), -- to also include oversize etc.
	 *  	stopNgo         (18), -- to include trash trucks, school buses and others that routinely disturb the free flow of traffic
	 *  	cyclist         (19), --
	 *  	pedestrian      (20), -- also includes those with mobility limitations
	 *  	nonMotorized    (21), -- other, horse drawn, etc.
	 *  	military        (22), --
	 */
	uint8_t role_is_exsit;
	uint8_t role;

	uint8_t typeData_is_exsit;
	Mde_RequestorType_t typeData;	
}Mde_SignalRequesterInfo_t;


typedef struct Mde_SignalStatusPackage
{
	uint8_t requester_is_exsit;
	Mde_SignalRequesterInfo_t requester;

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

	/*
	 *  range: 0..7
	 *  	unknown 		  (0),
	 *  	requested		  (1),
	 *  	processing		  (2),
	 *  	watchOtherTraffic (3),
	 *  	granted 		  (4),
	 *  	rejected		  (5),
	 *  	maxPresence 	  (6),
	 *  	reserviceLocked   (7),
	*/
	uint8_t status;		
}Mde_SignalStatusPackage_t;

typedef struct Mde_SignalStatus
{
	/*
     *  element: MsgCount
     *  range: 0..127
	 */
	uint8_t sequenceNumber;

	/*
	 *  element: RoadRegulatorID 
	 *  desc: a globally unique regional assignment value
	 *  range: request_region:0..65535
	 */
	uint8_t region_is_exsit;
	uint16_t region;
	
	/*
	 *  element: IntersectionID 
 	 *  desc: Note that the value assigned to an intersection will be unique within a given regional ID only
	 *  range: request_region: 0..65535
	 */
	uint16_t intersection_id;

	/*
     *  element: SignalStatusPackage
     *  range: sigStatus_count : 1..32
	 */
	uint8_t sigStatus_count;
	Mde_SignalStatusPackage_t sigStatus[32];
}Mde_SignalStatus_t;

typedef struct v2x_msg_ssm
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
     *  element: SignalStatus
     *  range: SignalStatus_count : 1..32
	 */	
	uint8_t SignalStatus_count;
	Mde_SignalStatus_t SignalStatus[32];
}v2x_msg_ssm_t;





#endif

