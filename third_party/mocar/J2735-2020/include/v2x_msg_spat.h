#ifndef _V2X_MSG_SPAT_H_
#define _V2X_MSG_SPAT_H_

#include <v2x_msg_common.h>


typedef struct Mde_TimeChangeDetails
{
	/*
     *  element: TimeMark 
     *  desc: When this phase 1st started
     *  range: startTime:0..36001
     *  unit: 0.1s
	 */
	uint8_t startTime_is_exsit;
	uint16_t startTime;

	/*
     *  element: TimeMark 
     *  desc: Expected shortest end time
     *  range: minEndTime:0..36001
     *  unit: 0.1s
	 */
	uint16_t minEndTime;

	/*
     *  element: TimeMark 
     *  desc: Expected longest end time
     *  range: maxEndTime:0..36001
     *  unit: 0.1s
	 */
	uint8_t maxEndTime_is_exsit;
	uint16_t maxEndTime;

	/*
     *  element: TimeMark 
     *  desc: Best predicted value based on other data  
     *  range: likelyTime:0..36001
     *  unit: 0.1s
	 */
	uint8_t likelyTime_is_exsit;
	uint16_t likelyTime;

	/*
     *  element: TimeMark 
     *  desc: Applies to above time element only
     *  range: confidence:0..15
     *  	0         21%
     *  	1         36%
     *  	2         47%
     *  	3         56%
     *  	4         62%
     *  	5         68%
     *  	6         73%
     *  	7         77%
     *  	8         81%
     *  	9         85%
     *  	10        88%
     *  	11        91%
     *  	12        94%
     *  	13        96%
     *  	14        98%
     *  	15        100%
	 */
	uint8_t confidence_is_exsit;
	uint8_t confidence;

	/*
     *  element: TimeMark 
     *  desc: A rough estimate of time when this phase may next occur again
     *  range: nextTime:0..36001
     *  unit: 0.1s
	 */
	uint8_t nextTime_is_exsit;
	uint16_t nextTime;	
}Mde_TimeChangeDetails_t;

typedef struct Mde_AdvisorySpeed
{
	/*
     *  element: AdvisorySpeedType 
     *  desc: the type of advisory which this is
     *  range: 0..3
     *  	none       (0),
     *  	greenwave  (1),
     *  	ecoDrive   (2),
     *  	transit    (3),
	 */
	uint8_t type;

	/*
     *  element: SpeedAdvice 
     *  desc: the value 499 shall be used for values at or greater than 49.9 m/s. the value 500 shall be used to indicate that speed is unavailable
     *  range: 0..500
     *  unit: 0.1 m/s
	 */
	uint8_t speed_is_exsit;
	uint16_t speed;

	/*
     *  element: SpeedConfidence 
     *  desc: A confidence value for the above speed
     *  range: 0..7
     *  	unavailable (0), -- Not Equipped or unavailable
     *  	prec100ms   (1), -- 100  meters / sec
     *  	prec10ms    (2), -- 10   meters / sec
     *  	prec5ms     (3), -- 5    meters / sec
     *  	prec1ms     (4), -- 1    meters / sec
     *  	prec0-1ms   (5), -- 0.1  meters / sec
     *  	prec0-05ms  (6), -- 0.05 meters / sec
     *  	prec0-01ms  (7)  -- 0.01 meters / sec
	 */
	uint8_t confidence_is_exsit;
	uint8_t confidence;

	/*
     *  element: ZoneLength 
     *  desc: The distance indicates the region for which the advised speed  is recommended
     *  range: 0..10000
     *  unit:1m
	 */
	uint8_t distance_is_exsit;
	uint16_t distance;
	
	/*
     *  element: RestrictionClassID 
     *  desc: the vehicle types to which it applies when absent
     *  range: 0..255
     *  unit:1m
	 */
	uint8_t class_is_exsit;
	uint8_t class;
}Mde_AdvisorySpeed_t;

typedef struct Mde_MovementEvent
{
	/*
     *  element: MovementPhaseState 
     *  desc: Phase state
     *  	unavailable (0), 
     *  	dark (1),   
     *  	stop-Then-Proceed (2), 
     *  	stop-And-Remain (3),
     *  	pre-Movement (4), 
     *  	permissive-Movement-Allowed (5), 
     *  	protected-Movement-Allowed (6), 
     *  	permissive-clearance (7), 
     *  	protected-clearance (8),  
     *  	caution-Conflicting-Traffic (9)
     *  range: 0..9
	 */
	uint8_t eventState;

	uint8_t timing_is_exsit;
	Mde_TimeChangeDetails_t timing;

	/*
     *  element: AdvisorySpeed 
     *  desc: various speed advisories for use
     *  range: speed_count:0..16
	 */
	uint8_t speed_count;
	Mde_AdvisorySpeed_t speed[16];
}Mde_MovementEvent_t;

typedef struct Mde_ConnectionManeuverAssist
{
	/*
     *  element: LaneConnectionID 
     *  desc: the common connectionID used by all lanes
     *  range: 0..255
	 */
	uint8_t connectionID;

	/*
     *  element: ZoneLength 
     *  desc: The distance from the stop line to the back edge of the last vehicle in the queue as measured along the lane center line.
     *  range: queueLength:0..10000
     *  unit: 1m
	 */
	uint8_t queueLength_is_exsit;
	uint16_t queueLength;

	/*
     *  element: ZoneLength 
     *  range: queueLength:0..10000
     *  unit: 1m
	 */
	uint8_t availableStorageLength_is_exsit;
	uint16_t availableStorageLength;

	/*
     *  element: WaitOnStopline 
     *  desc: If "true", the vehicles on this specific connecting maneuver have to stop on the stop-line and not to enter the collision area
     *  range: 0..1.  (1 stand by True, or 0 stand by False)
	 */
	uint8_t waitOnStop_is_exsit;
	uint8_t	waitOnStop;
	
	/*
     *  element: PedestrianBicycleDetect 
     *  desc: true if ANY Pedestrians or Bicyclists are detected crossing the target lane or lanes
     *  range: 0..1.  (1 stand by True, or 0 stand by False)
	 */
	uint8_t pedBicycleDetect_is_exsit;
	uint8_t	pedBicycleDetect;

}Mde_ConnectionManeuverAssist_t;

typedef struct Mde_MovementState
{
	/*
     *  element: DescriptiveName 
     *  desc: uniquely defines movement by name
     *  size: 1..63
	 */
	uint8_t movementName_is_exist;
	char movementName[64];

	/*
     *  element: SignalGroupID 
     *  desc: the group id is used to map to lists of lanes (and their descriptions) 
     *  range: 0..255
	 */
	uint8_t signalGroup;

	/*
     *  element: MovementEvent list 
     *  desc: Consisting of sets of movement data with: SignalPhaseState TimeChangeDetails AdvisorySpeeds
     *  size: state_time_speed_count:1..16
	 */
	uint8_t state_time_speed_count;
	Mde_MovementEvent_t state_time_speed[16];

	/*
	 *	element: MovementEvent list 
	 *	desc: Consisting of sets of movement data with: SignalPhaseState TimeChangeDetails AdvisorySpeeds
	 *	size: maneuverAssist_count:0..16
	 */
	uint8_t maneuverAssist_count;
	Mde_ConnectionManeuverAssist_t maneuverAssist[16];
}Mde_MovementState_t;

typedef struct Mde_Intersection_State
{
	/*
     *  element: DescriptiveName 
     *  desc: human readable name for intersection. to be used only in debug mode
     *  size: 1..63
	 */
	uint8_t name_is_exist;
	char name[63];

	/*
     *  element: RoadRegulatorID 
     *  desc: a globally unique regional assignment value
     *  range: 0..65535
	 */
	uint8_t region_is_exist;
	uint16_t region;
	/*
     *  element: IntersectionID
     *  desc:  A globally unique value. intersection ID 
     *  range: 0..65535
	 */
	uint16_t id;

	/*
     *  element: MsgCount
     *  range: 0..127
	 */
	uint8_t revision;

	/*
     *  element: IntersectionStatusObject
     *  desc:  BIT STRING. Bits 14,15 reserved at this time and shall be zero
	 *		manualControlIsEnabled				  (0),
	 *		stopTimeIsActivated 				  (1),
	 *		failureFlash						  (2),
	 *		preemptIsActive 					  (3),
	 *		signalPriorityIsActive				  (4),
	 *		fixedTimeOperation					  (5),
	 *		trafficDependentOperation			  (6),
	 *		standbyOperation					  (7),
	 *		failureMode 						  (8),
	 *		off 								  (9),
	 *		recentMAPmessageUpdate				  (10),
	 *		recentChangeInMAPassignedLanesIDsUsed (11),
	 *		noValidMAPisAvailableAtThisTime 	  (12),
	 *		noValidSPATisAvailableAtThisTime	  (13)
     *  range: 0..1
	 */	
	uint8_t status[16];
	
	/*
     *  element: MinuteOfTheYear
     *  desc:  Minute of current UTC year. used only with messages to be archived 
     *  range: 0..527040
	 */
	uint8_t moy_is_exist;
	uint32_t moy;
	
	/*
     *  element: DSecond
     *  desc:  the mSec point in the current UTC minute.
     *  range: 0..65535
	 */
	uint8_t timeStamp_is_exist;
	uint16_t timeStamp;

	/*
     *  element: EnabledLaneList
     *  desc:  the mSec point in the current UTC minute.
     *  range: enabledLanes_count: 0..16
     *  		enabledLane: 0..255
	 */
	uint8_t enabledLane_count;
	uint8_t enabledLane[16];

	/*
     *  element: MovementState
     *  desc:  Each Movement is given in turn and contains its signal phase state,  mapping to the lanes it applies to, and point in time it will end
     *  range: state_count: 1..255
	 */
	uint8_t state_count;
	Mde_MovementState_t state[255];

	/*
     *  element: ManeuverAssistList
     *  desc:  Assist data
     *  range: maneuverAssist_count: 0..16
	 */
	uint8_t maneuverAssist_count;
	Mde_ConnectionManeuverAssist_t maneuverAssist[16];
	
}Mde_Intersection_State_t;

typedef struct v2x_msg_spat
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
     *  element: DescriptiveName 
     *  desc: human readable name for this collection
     *  size: 1..63
	 */
	uint8_t name_is_exist;
	char name[63];

	/*
     *  element: IntersectionStateList 
     *  desc: sets of SPAT data (one per intersection)
     *  size: 1..32
	 */
	uint8_t intersections_count;
	Mde_Intersection_State_t intersections[32];	
}v2x_msg_spat_t;


#endif
