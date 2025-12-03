#ifndef __V2X_MSG_MAP_H_
#define __V2X_MSG_MAP_H_

#include <v2x_msg_common.h>


/* IntersectionReferenceID */
typedef struct Mde_IntersectionReferenceID {
    int   region_active;
	long  region	/* OPTIONAL */;
	long  id;
	
} Mde_IntersectionReferenceID_t;


/* Dependencies */
typedef enum Mde_LaneTypeAttributes_PR {
	Mde_LaneTypeAttributes_PR_NOTHING,	/* No components present */
	Mde_LaneTypeAttributes_PR_vehicle,
	Mde_LaneTypeAttributes_PR_crosswalk,
	Mde_LaneTypeAttributes_PR_bikeLane,
	Mde_LaneTypeAttributes_PR_sidewalk,
	Mde_LaneTypeAttributes_PR_median,
	Mde_LaneTypeAttributes_PR_striping,
	Mde_LaneTypeAttributes_PR_trackedVehicle,
	Mde_LaneTypeAttributes_PR_parking
	/* Extensions may appear below */
	
} Mde_LaneTypeAttributes_PR;

/* LaneTypeAttributes */
typedef struct Mde_LaneTypeAttributes {
	Mde_LaneTypeAttributes_PR present;
    uint8_t vehicle_bits[8];
    /*
    isVehicleRevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    isVehicleFlyOverLane (1),
    -- path of lane is not at grade
    hovLaneUseOnly (2),
    restrictedToBusUse (3),
    restrictedToTaxiUse (4),
    restrictedFromPublicUse (5),
    hasIRbeaconCoverage (6),
    permissionOnRequest (7) -- e.g. to inform about a lane for e-cars
    */
    uint8_t crosswalk_bits[16];
    /*
    crosswalkRevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    bicyleUseAllowed (1),
    -- The path allows bicycle traffic,
    -- if not set, this mode is prohibited
    isXwalkFlyOverLane (2),
    -- path of lane is not at grade
    fixedCycleTime (3),
    -- ped walk phases use preset times
    -- i.e. there is not a 'push to cross' button
    biDirectionalCycleTimes (4),
    -- ped walk phases use different SignalGroupID
    -- for each direction. The first SignalGroupID
    -- in the first Connection represents 'inbound'
    -- flow (the direction of travel towards the first
    -- node point) while second SignalGroupID in the
    -- next Connection entry represents the 'outbound'
    -- flow. And use of RestrictionClassID entries
    -- in the Connect follow this same pattern in pairs.
    hasPushToWalkButton (5),
    -- Has a demand input
    audioSupport (6),
    -- audio crossing cues present
    rfSignalRequestPresent (7),
    -- Supports RF push to walk technologies
    unsignalizedSegmentsPresent (8)
    -- The lane path consists of one of more segments
    -- which are not part of a signal group ID
    -- Bits 9~15 reserved and set to zero
    */
    uint8_t	bikeLane_bits[16];
    /*
    bikeRevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    pedestrianUseAllowed (1),
    -- The path allows pedestrian traffic,
    -- if not set, this mode is prohibited
    isBikeFlyOverLane (2),
    -- path of lane is not at grade
    fixedCycleTime (3),
    -- the phases use preset times
    -- i.e. there is not a 'push to cross' button
    biDirectionalCycleTimes (4),
    -- ped walk phases use different SignalGroupID
    -- for each direction. The first SignalGroupID
    -- in the first Connection represents 'inbound'
    -- flow (the direction of travel towards the first
    -- node point) while second SignalGroupID in the
    -- next Connection entry represents the 'outbound'
    -- flow. And use of RestrictionClassID entries
    -- in the Connect follow this same pattern in pairs.
    isolatedByBarrier (5),
    unsignalizedSegmentsPresent (6)
    -- The lane path consists of one of more segments
    -- which are not part of a signal group ID
    */
    uint8_t	sidewalk_bits[16];
    /*
    sidewalk-RevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    bicyleUseAllowed (1),
    -- The path allows bicycle traffic,
    -- if not set, this mode is prohibited
    isSidewalkFlyOverLane (2),
    -- path of lane is not at grade
    walkBikes (3)
    -- bike traffic must dismount and walk
    -- Bits 4~15 reserved and set to zero    
    */
    uint8_t	median_bits[16];
    /*
    median-RevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    median (1),
    whiteLineHashing (2),
    stripedLines (3),
    doubleStripedLines (4),
    trafficCones (5),
    constructionBarrier (6),
    trafficChannels (7),
    lowCurbs (8),
    highCurbs (9)
    -- Bits 10~15 reserved and set to zero    
    */
    uint8_t	striping_bits[16];
    /*
    stripeToConnectingLanesRevocableLane (0),
    -- this lane may be activated or not activated based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    stripeDrawOnLeft (1),
    stripeDrawOnRight (2),
    -- which side of lane to mark
    stripeToConnectingLanesLeft (3),
    stripeToConnectingLanesRight (4),
    stripeToConnectingLanesAhead (5)
    -- the stripe type should be
    -- presented to the user visually
    -- to reflect stripes in the
    -- intersection for the type of
    -- movement indicated
    -- Bits 6~15 reserved and set to zero    
    */
    uint8_t	trackedVehicle[16];
    /*
    spec-RevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    spec-commuterRailRoadTrack (1),
    spec-lightRailRoadTrack (2),
    spec-heavyRailRoadTrack (3),
    spec-otherRailType (4)
    -- Bits 5~15 reserved and set to zero    
    */
    uint8_t	parking_bits[16];
    /*
    -- With bits as defined:
    -- Parking use details, note that detailed restrictions such as
    -- allowed hours are sent by way of ITIS codes in the TIM message
    parkingRevocableLane (0),
    -- this lane may be activated or not based
    -- on the current SPAT message contents
    -- if not asserted, the lane is ALWAYS present
    parallelParkingInUse (1),
    headInParkingInUse (2),
    doNotParkZone (3),
    -- used to denote fire hydrants as well as
    -- short disruptions in a parking zone
    parkingForBusUse (4),
    parkingForTaxiUse (5),
    noPublicParkingUse (6)
    -- private parking, as in front of
    -- private property
    -- Bits 7~15 reserved and set to zero
    */
} Mde_LaneTypeAttributes_t;

typedef struct Mde_LaneDirection{


} Mde_LaneDirection_t;


/* LaneAttributes */
typedef struct Mde_LaneAttributes {
    #define MDE_DIRECTIONAL_USE_MAX (2)
    uint8_t directionalUse_bits[MDE_DIRECTIONAL_USE_MAX]; 
    /*
    ingressPath (0),
    -- travel from rear of path to front
    -- is allowed
    egressPath (1)
    -- travel from front of path to rear
    -- is allowed
    -- Notes: No Travel, i.e. the
    */
    #define MDE_SHAREDWITH_MAX (2)
   	uint8_t	 sharedWith_bits[10];
    /*
    -- With bits as defined:
    overlappingLaneDescriptionProvided (0),
    -- Assert when another lane object is present to describe the
    -- path of the overlapping shared lane
    -- this construct is not used for lane objects which simply cross
    multipleLanesTreatedAsOneLane (1),
    -- Assert if the lane object path and width details represents
    -- multiple lanes within it that are not further described
    -- Various modes and type of traffic that may share this lane:
    otherNonMotorizedTrafficTypes (2), -- horse drawn etc.
    individualMotorizedVehicleTraffic (3),
    busVehicleTraffic (4),
    taxiVehicleTraffic (5),
    pedestriansTraffic (6),
    cyclistVehicleTraffic (7),
    trackedVehicleTraffic (8),
    pedestrianTraffic (9)
    */
	Mde_LaneTypeAttributes_t	 laneType;
} Mde_LaneAttributes_t;


/* ConnectingLane */
typedef struct Mde_ConnectingLane {
	long	 lane;
    int maneuver_active;
    #define MDE_MANEUVER_BITS_MAX (2)
	uint8_t	maneuver_bits[12]	/* OPTIONAL */;
    /*
    -- With bits as defined:
    -- Allowed maneuvers at path end (stop line)
    -- All maneuvers with bits not set are therefore prohibited !
    -- A value of zero shall be used for unknown, indicating no Maneuver
    maneuverStraightAllowed (0),
    -- a Straight movement is allowed in this lane
    maneuverLeftAllowed (1),
    -- a Left Turn movement is allowed in this lane
    maneuverRightAllowed (2),
    -- a Right Turn movement is allowed in this lane
    maneuverUTurnAllowed (3),
    -- a U turn movement is allowed in this lane
    maneuverLeftTurnOnRedAllowed (4),
    -- a Stop, and then proceed when safe movement
    -- is allowed in this lane
    maneuverRightTurnOnRedAllowed (5),
    -- a Stop, and then proceed when safe movement
    -- is allowed in this lane
    maneuverLaneChangeAllowed (6),
    -- a movement which changes to an outer lane
    -- on the egress side is allowed in this lane
    -- (example: left into either outbound lane)
    maneuverNoStoppingAllowed (7),
    -- the vehicle should not stop at the stop line
    -- (example: a flashing green arrow)
    yieldAllwaysRequired (8),
    -- the allowed movements above are not protected
    -- (example: an permanent yellow condition)
    goWithHalt (9),
    -- after making a full stop, may proceed
    caution (10),
    -- proceed past stop line with caution
    reserved1 (11)
    -- used to align to 12 Bit Field
    */
} Mde_ConnectingLane_t;

/* Connection */
typedef struct Mde_Connection {
	Mde_ConnectingLane_t	 connectingLane;
    int remoteIntersection_active;
	Mde_IntersectionReferenceID_t	remoteIntersection	/* OPTIONAL */;
	int signalGroup_active;
    long	signalGroup	/* OPTIONAL */;
    int userClass_active;
	long	userClass	/* OPTIONAL */;
    int connectionID_active;
	long	connectionID	/* OPTIONAL */;
} Mde_Connection_t;

/* GenericLane */
typedef struct Mde_GenericLane {
	long	 laneID;

    int name_active;
	Mde_DescriptiveName_t name	/* OPTIONAL */;

    int  ingressApproach_active;
	long ingressApproach	/* OPTIONAL */;
    int  egressApproach_active;
	long egressApproach	/* OPTIONAL */;

	Mde_LaneAttributes_t	 laneAttributes;

    int  maneuvers_active;
    #define MDE_MANEUVERS_NUM_MAX (2)
	uint8_t	maneuvers_bits[12]	/* OPTIONAL */;
    /*
    -- Allowed maneuvers at path end (stop line)
    -- All maneuvers with bits not set are therefore prohibited !
    -- A value of zero shall be used for unknown, indicating no Maneuver
    maneuverStraightAllowed (0),
    -- a Straight movement is allowed in this lane
    maneuverLeftAllowed (1),
    -- a Left Turn movement is allowed in this lane
    maneuverRightAllowed (2),
    -- a Right Turn movement is allowed in this lane
    maneuverUTurnAllowed (3),
    -- a U turn movement is allowed in this lane
    maneuverLeftTurnOnRedAllowed (4),
    -- a Stop, and then proceed when safe movement
    -- is allowed in this lane
    maneuverRightTurnOnRedAllowed (5),
    -- a Stop, and then proceed when safe movement
    -- is allowed in this lane
    maneuverLaneChangeAllowed (6),
    -- a movement which changes to an outer lane
    -- on the egress side is allowed in this lane
    -- (example: left into either outbound lane)
    maneuverNoStoppingAllowed (7),
    -- the vehicle should not stop at the stop line
    -- (example: a flashing green arrow)
    yieldAllwaysRequired (8),
    -- the allowed movements above are not protected
    -- (example: an permanent yellow condition)
    goWithHalt (9),
    -- after making a full stop, may proceed
    caution (10),
    -- proceed past stop line with caution
    reserved1 (11)
    -- used to align to 12 Bit Field    
    */
	Mde_NodeListXY_t	 nodeList;

    uint32_t connectsTo_count;
    #define MDE_CONNECTSTO_COUNT_MAX  (16)
	Mde_Connection_t	connectsTo[MDE_CONNECTSTO_COUNT_MAX]	/* OPTIONAL */;
    uint32_t overlays_count;
    #define MDE_OVERLAYS_COUNT_MAX  (5)
	long	overlays[MDE_OVERLAYS_COUNT_MAX]	/* OPTIONAL */;

} Mde_GenericLane_t;


/* SignalControlZone */
typedef struct Mde_SignalControlZone {
	Mde_RegionalExtension_124P0_t	 zone;
} Mde_SignalControlZone_t;


/* IntersectionGeometry */
typedef struct Mde_IntersectionGeometry {
    int intersection_name_active;    
	Mde_DescriptiveName_t	name/* OPTIONAL */;
	Mde_IntersectionReferenceID_t	 id;
	long	 revision;
	Mde_Position3D_t	 refPoint;
    int laneWidth_active;
	long laneWidth	/* OPTIONAL */;

    uint32_t speedLimits_count;
    #define MDE_SPEEDLIMITS_MAX (9)
	Mde_RegulatorySpeedLimit_t	speedLimits[MDE_SPEEDLIMITS_MAX]	/* OPTIONAL */;
	
    uint32_t laneSet_count;
    #define MDE_INTERSECTION_LANESET_MAX (255)
    Mde_GenericLane_t laneSet[MDE_INTERSECTION_LANESET_MAX];

    uint32_t preemptPriorityData_count;
    #define MDE_PREEMPT_PRIORITYdATA_COUNT_MAX (32)
	Mde_SignalControlZone_t	preemptPriorityData[MDE_PREEMPT_PRIORITYdATA_COUNT_MAX]	/* OPTIONAL */;
} Mde_IntersectionGeometry_t;


/* RoadSegment */
typedef struct Mde_RoadSegment {

    int name_active;
	Mde_DescriptiveName_t name	/* OPTIONAL */;

	Mde_RoadSegmentReferenceID_t	 id;
	long	 revision;
	Mde_Position3D_t	 refPoint;
    int laneWidth_active;
	long	laneWidth	/* OPTIONAL */;
	
    uint32_t speedLimits_count;
	#define MDE_ROAD_SPEEDLIMITS_MAX (9)
	Mde_RegulatorySpeedLimit_t	speedLimits[MDE_ROAD_SPEEDLIMITS_MAX];
	
    uint32_t roadLaneSet_count;
    #define MDE_ROAD_LANESET_MAX (255)
    Mde_GenericLane_t roadLaneSet[MDE_ROAD_LANESET_MAX];
} Mde_RoadSegment_t;

/* DataParameters */
typedef struct Mde_DataParameters {
    int processMethod_active;
    #define MDE_PROCESSMETHODE_MAX (255)
	Mde_IA5String_t	processMethod	/* OPTIONAL */;
    int processAgency_active;
    #define MDE_PROCESSAGENCY_MAX (255)
	Mde_IA5String_t	processAgency		/* OPTIONAL */;
    int lastCheckedDate_active;
    #define MDE_LASTCHECKDATE_MAX (255)
	Mde_IA5String_t	lastCheckedDate		/* OPTIONAL */;
    int geoidUsed_active;
    #define MDE_GEOIDUSED_MAX (255)
	Mde_IA5String_t	geoidUsed		/* OPTIONAL */;
}   Mde_DataParameters_t;

/* Dependencies */
typedef enum Mde_RestrictionUserType_PR {
	MDE_RestrictionUserType_PR_NOTHING,	/* No components present */
	MDE_RestrictionUserType_PR_basicType,
	MDE_RestrictionUserType_PR_regional
	/* Extensions may appear below */
	
} Mde_RestrictionUserType_PR;

/* RestrictionUserType */
typedef struct Mde_RestrictionUserType {
	Mde_RestrictionUserType_PR present;
    long basicType;	
} Mde_RestrictionUserType_t;

/* RestrictionClassAssignment */
typedef struct Mde_RestrictionClassAssignment {
	long	 id;
    uint32_t users_count;
    #define MDE_RESTRICTION_USER_TYPE_MAX (16)
	Mde_RestrictionUserType_t	 users[MDE_RESTRICTION_USER_TYPE_MAX];
} Mde_RestrictionClassAssignment_t;

typedef struct v2x_msg_map
{
    int     timeStamp_active;
	long	timeStamp	/* OPTIONAL */;
	long	msgIssueRevision;
    int     layerType_active;
	long	layerType	/* OPTIONAL */;
    int     layerID_active;
	long	layerID	/* OPTIONAL */;
    uint32_t intersections_count;
   // #define MDE_INTERSECTION_COUNT_MAX (32)
   #define MDE_INTERSECTION_COUNT_MAX (16)
	Mde_IntersectionGeometry_t	intersections[MDE_INTERSECTION_COUNT_MAX]	/* OPTIONAL */;
    uint32_t roadSegments_count;
   // #define MDE_ROADSEGMENTS_COUNT_MAX (32)
   #define MDE_ROADSEGMENTS_COUNT_MAX (16)
    Mde_RoadSegment_t	roadSegments[MDE_ROADSEGMENTS_COUNT_MAX]	/* OPTIONAL */;
	int dataParameters_active;
    Mde_DataParameters_t	dataParameters	/* OPTIONAL */;

    uint32_t restrictionList_count;
    #define MDE_RESTRICTIONLIST_COUNT_MAX (254)
	Mde_RestrictionClassAssignment_t	restrictionList[MDE_RESTRICTIONLIST_COUNT_MAX]	/* OPTIONAL */;
}v2x_msg_map_t;

#endif




