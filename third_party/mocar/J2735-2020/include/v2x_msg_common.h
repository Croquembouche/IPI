#ifndef _V2X_MSG_COMMON_H_
#define _V2X_MSG_COMMON_H_

#include <stdint.h>
#define VEH_ID_LEN 4
//#define VEH_ID_LEN 8

#define LAT_FACTOR              (1.00 / 0.0000001)
#define LONG_FACTOR             (1.00 / 0.0000001)
#define ELE_FACTOR              (1.00 / 0.10)
#define HEADING_FACTOR          (1.00 / 0.0125)
#define POS_ACCURACY_FACTOR_0   (1.00 / 0.05)
#define POS_ACCURACY_FACTOR_1   (1.00 / 0.05)
#define POS_ACCURACY_FACTOR_2   (65535.00 / 360.00)
#define LONGACCEL_FACTOR        (1.00 / 0.01)
#define LATACCEL_FACTOR         (1.00 / 0.01)
#define VERTACCEL_FACTOR        (1.00 / 0.1962)
#define YAW_FACTOR              (1.00 / 0.01)
#define SPEED_FACTOR            (1.00 / 0.02)
#define ANGLE_FACTOR            (1.00 / 1.50)
#define WIDTH_FACTOR            (1.00 / 0.01)
#define LENGTH_FACTOR           (1.00 / 0.01)
#define VEH_HEIGHT_FACTOR       (1.00 / 0.05)

#define TEST_MAKE_SUCCESS		(0U) 
#define STING_TO_BIT(x) (x/8 + ((x % 8)>0 ? 1:0))



enum SDK_Optional_Exsit
{
	SDK_OPTIONAL_NO_EXSIT = 0,
	SDK_OPTIONAL_EXSIT = 1
};

typedef enum Mde_vehicle_id_type
{
	TEMPORARY_ID = 0,
	STATION_ID
}Mde_vehicle_id_type_t;

typedef struct Mde_vehicle_id
{
	/*
	 *  range:  entityID = 0, stationID =1
	 */
	Mde_vehicle_id_type_t msg_id;
	union msg_vehicle_id
	{
		/*
		 *  desc: Temporary id
		 *  size: 4
		 */
		char entityID[5];
		
		/*
		 *  desc: Vehicle id
		 *  range: 0..4294967295
		 */
		uint32_t stationID;
	}choice;
}Mde_vehicle_id_t;

/* Dependencies */
typedef enum mde_TimeConfidence {
	MDE_TimeConfidence_unavailable	= 0,
	MDE_TimeConfidence_time_100_000	= 1,
	MDE_TimeConfidence_time_050_000	= 2,
	MDE_TimeConfidence_time_020_000	= 3,
	MDE_TimeConfidence_time_010_000	= 4,
	MDE_TimeConfidence_time_002_000	= 5,
	MDE_TimeConfidence_time_001_000	= 6,
	MDE_TimeConfidence_time_000_500	= 7,
	MDE_TimeConfidence_time_000_200	= 8,
	MDE_TimeConfidence_time_000_100	= 9,
	MDE_TimeConfidence_time_000_050	= 10,
	MDE_TimeConfidence_time_000_020	= 11,
	MDE_TimeConfidence_time_000_010	= 12,
	MDE_TimeConfidence_time_000_005	= 13,
	MDE_TimeConfidence_time_000_002	= 14,
	MDE_TimeConfidence_time_000_001	= 15,
	MDE_TimeConfidence_time_000_000_5	= 16,
	MDE_TimeConfidence_time_000_000_2	= 17,
	MDE_TimeConfidence_time_000_000_1	= 18,
	MDE_TimeConfidence_time_000_000_05	= 19,
	MDE_TimeConfidence_time_000_000_02	= 20,
	MDE_TimeConfidence_time_000_000_01	= 21,
	MDE_TimeConfidence_time_000_000_005	= 22,
	MDE_TimeConfidence_time_000_000_002	= 23,
	MDE_TimeConfidence_time_000_000_001	= 24,
	MDE_TimeConfidence_time_000_000_000_5	= 25,
	MDE_TimeConfidence_time_000_000_000_2	= 26,
	MDE_TimeConfidence_time_000_000_000_1	= 27,
	MDE_TimeConfidence_time_000_000_000_05	= 28,
	MDE_TimeConfidence_time_000_000_000_02	= 29,
	MDE_TimeConfidence_time_000_000_000_01	= 30,
	MDE_TimeConfidence_time_000_000_000_005	= 31,
	MDE_TimeConfidence_time_000_000_000_002	= 32,
	MDE_TimeConfidence_time_000_000_000_001	= 33,
	MDE_TimeConfidence_time_000_000_000_000_5	= 34,
	MDE_TimeConfidence_time_000_000_000_000_2	= 35,
	MDE_TimeConfidence_time_000_000_000_000_1	= 36,
	MDE_TimeConfidence_time_000_000_000_000_05	= 37,
	MDE_TimeConfidence_time_000_000_000_000_02	= 38,
	MDE_TimeConfidence_time_000_000_000_000_01	= 39
} MDE_TimeConfidence_e;

/* Dependencies */
typedef enum Mde_VehicleEventFlags {
	Mde_VehicleEventFlags_eventHazardLights	= 0,
	Mde_VehicleEventFlags_eventStopLineViolation	= 1,
	Mde_VehicleEventFlags_eventABSactivated	= 2,
	Mde_VehicleEventFlags_eventTractionControlLoss	= 3,
	Mde_VehicleEventFlags_eventStabilityControlactivated	= 4,
	Mde_VehicleEventFlags_eventHazardousMaterials	= 5,
	Mde_VehicleEventFlags_eventReserved1	= 6,
	Mde_VehicleEventFlags_eventHardBraking	= 7,
	Mde_VehicleEventFlags_eventLightsChanged	= 8,
	Mde_VehicleEventFlags_eventWipersChanged	= 9,
	Mde_VehicleEventFlags_eventFlatTire	= 10,
	Mde_VehicleEventFlags_eventDisabledVehicle	= 11,
	Mde_VehicleEventFlags_eventAirBagDeployment	= 12
} Mde_VehicleEventFlags_e;

typedef struct Mde_VehicleClassification
{
	uint8_t keyType_is_exsit;
	/*
	 *  range: 0..255
	 */
	uint8_t keyType;

	uint8_t role_is_exsit;
	/*
	 *  element: BasicVehicleRole
	 *  desc: Values used in the EU and in the US
	 *  range: 0..22
	 *  	basicVehicle     (0), -- Light duty passenger vehicle type
	 *  	publicTransport  (1), -- Used in EU for Transit us
	 *  	specialTransport (2), -- Used in EU (e.g. heavy load)
	 *  	dangerousGoods   (3), -- Used in EU for any HAZMAT
	 *  	roadWork         (4), -- Used in EU for State and Local DOT uses
	 *  	roadRescue       (5), -- Used in EU and in the US to include tow trucks.
	 *  	emergency        (6), -- Used in EU for Police, Fire and Ambulance units
	 *  	safetyCar        (7), -- Used in EU for Escort vehicles
	 *  						-- Begin US unique numbering
	 *  	none-unknown     (8), -- added to follow current SAE style guidelines
	 *  	truck            (9), -- Heavy trucks with additional BSM rights and obligations
	 *  	motorcycle      (10), --
	 *  	roadSideSource  (11), -- For infrastructure generated calls such as
	 *  		              -- fire house, rail infrastructure, roadwork site, etc.
	 *  	police          (12), --
	 *  	fire            (13), --
	 *  	ambulance       (14), -- (does not include private para-transit etc.)
	 *  	dot             (15), -- all roadwork vehicles
	 *  	transit         (16), -- all transit vehicles 
	 *  	slowMoving      (17), -- to also include oversize etc.
	 *  	stopNgo         (18), -- to include trash trucks, school buses and others
	 *  		             -- that routinely disturb the free flow of traffic
	 *  	cyclist         (19), --
	 *  	pedestrian      (20), -- also includes those with mobility limitations
	 *  	nonMotorized    (21), -- other, horse drawn, etc.
	 *  	military        (22), --
	 */
	uint8_t role;

	uint8_t iso3883_is_exsit;
	/*
	 *  range: 0..100
	 */
	uint8_t iso3883;

	uint8_t hpmsType_is_exsit;
	/*
	 *  range: 0..15
	 *  	none                 (0),  -- Not Equipped, Not known or unavailable
	 *  	unknown              (1),  -- Does not fit any other category    
	 *  	special              (2),  -- Special use    
	 *  	moto                 (3),  -- Motorcycle    
	 *  	car                  (4),  -- Passenger car    
	 *  	carOther             (5),  -- Four tire single units    
	 *  	bus                  (6),  -- Buses    
	 *  	axleCnt2             (7),  -- Two axle, six tire single units    
	 *  	axleCnt3             (8),  -- Three axle, single units    
	 *  	axleCnt4             (9),  -- Four or more axle, single unit    
	 *  	axleCnt4Trailer      (10), -- Four or less axle, single trailer   
	 *  	axleCnt5Trailer      (11), -- Five or less axle, single trailer   
	 *  	axleCnt6Trailer      (12), -- Six or more axle, single trailer    
	 *  	axleCnt5MultiTrailer (13), -- Five or less axle, multi-trailer    
	 *  	axleCnt6MultiTrailer (14), -- Six axle, multi-trailer    
	 *  	axleCnt7MultiTrailer (15),  -- Seven or more axle, multi-tra
	 */
	uint8_t hpmsType;

	uint8_t vehicleType_is_exsit;
	/*
	 *  range: 9217..9251
	 *  	all-vehicles							   (9217),	
	 *  	bicycles								   (9218),	
	 *  	motorcycles 							   (9219),	-- to include mopeds as well
	 *  	cars									   (9220),	-- (remapped from ERM value of zero)
	 *  	light-vehicles							   (9221),	
	 *  	cars-and-light-vehicles 				   (9222),	
	 *  	cars-with-trailers						   (9223),	
	 *  	cars-with-recreational-trailers 		   (9224),	
	 *  	vehicles-with-trailers					   (9225),	
	 *  	heavy-vehicles							   (9226),	
	 *  	trucks									   (9227),	
	 *  	buses									   (9228),	
	 *  	articulated-buses						   (9229),	
	 *  	school-buses							   (9230),	
	 *  	vehicles-with-semi-trailers 			   (9231),	
	 *  	vehicles-with-double-trailers			   (9232),	-- Alternative Rendering: western doubles
	 *  	high-profile-vehicles					   (9233),	
	 *  	wide-vehicles							   (9234),	
	 *  	long-vehicles							   (9235),	
	 *  	hazardous-loads 						   (9236),	
	 *  	exceptional-loads						   (9237),	
	 *  	abnormal-loads							   (9238),	
	 *  	convoys 								   (9239),	
	 *  	maintenance-vehicles					   (9240),	
	 *  	delivery-vehicles						   (9241),	
	 *  	vehicles-with-even-numbered-license-plates (9242),	
	 *  	vehicles-with-odd-numbered-license-plates  (9243),	
	 *  	vehicles-with-parking-permits			   (9244),	
	 *  	vehicles-with-catalytic-converters		   (9245),	
	 *  	vehicles-without-catalytic-converters	   (9246),	
	 *  	gas-powered-vehicles					   (9247),	
	 *  	diesel-powered-vehicles 				   (9248),	
	 *  	lPG-vehicles							   (9249),	-- The L is lower case here
	 *  	military-convoys						   (9250),	
	 *  	military-vehicles						   (9251),	
	 */
	uint16_t vehicleType;

	uint8_t responseEquip_is_exsit;
	/*
	 *  range: 9985..10113
	 */
	uint16_t responseEquip;

	uint8_t responderType_is_exsit;
	/*
	 *  range: 9729..9742
	 *  	emergency-vehicle-units 		  (9729),  -- Default, to be used when one of the below does not fit better
	 *  	federal-law-enforcement-units	  (9730),  
	 *  	state-police-units				  (9731),  
	 *  	county-police-units 			  (9732),  -- Hint: also sheriff response units
	 *  	local-police-units				  (9733),  
	 *  	ambulance-units 				  (9734),  
	 *  	rescue-units					  (9735),  
	 *  	fire-units						  (9736),  
	 *  	hAZMAT-units					  (9737),  
	 *  	light-tow-unit					  (9738),  
	 *  	heavy-tow-unit					  (9739),  
	 *  	freeway-service-patrols 		  (9740),  
	 *  	transportation-response-units	  (9741),  
	 *  	private-contractor-response-units (9742),  
	 */
	uint16_t responderType;

	uint8_t fuelType_is_exsit;
	/*
	 *  range: 0..15
	 *  	unknownFuel   FuelType::=  0 -- Gasoline Powered
	 *  	gasoline	  FuelType::=  1 
	 *  	ethanol 	  FuelType::=  2 -- Including blends
	 *  	diesel		  FuelType::=  3 -- All types
	 *  	electric	  FuelType::=  4  
	 *  	hybrid		  FuelType::=  5 -- All types 
	 *  	hydrogen	  FuelType::=  6  
	 *  	natGasLiquid  FuelType::=  7 -- Liquefied
	 *  	natGasComp	  FuelType::=  8 -- Compressed
	 *  	propane 	  FuelType::=  9  
	 */
	uint8_t fuelType;
	
	// NOTICE : 
	// Here is ptional regional. But I have no idea to fill it.So default null
}Mde_VehicleClassification_t;

/* Dependencies */
typedef enum mde_VehicleType {
	MDE_VehicleType_none	= 0,
	MDE_VehicleType_unknown	= 1,
	MDE_VehicleType_special	= 2,
	MDE_VehicleType_moto	= 3,
	MDE_VehicleType_car	= 4,
	MDE_VehicleType_carOther	= 5,
	MDE_VehicleType_bus	= 6,
	MDE_VehicleType_axleCnt2	= 7,
	MDE_VehicleType_axleCnt3	= 8,
	MDE_VehicleType_axleCnt4	= 9,
	MDE_VehicleType_axleCnt4Trailer	= 10,
	MDE_VehicleType_axleCnt5Trailer	= 11,
	MDE_VehicleType_axleCnt6Trailer	= 12,
	MDE_VehicleType_axleCnt5MultiTrailer	= 13,
	MDE_VehicleType_axleCnt6MultiTrailer	= 14,
	MDE_VehicleType_axleCnt7MultiTrailer	= 15
	/*
	 * Enumeration is extensible
	 */
} MDE_VehicleType;


typedef struct Mde_RequestorType
{
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
	uint8_t role;

	/*
	 *  desc: A local list with role based items 
	 *  range: 0..15
	 *  	requestSubRoleUnKnown    (0),
	 *  	requestSubRole1          (1), -- The first type of sub role
	 *  	requestSubRole2          (2), -- The values here shall be assigned
	 *  	requestSubRole3          (3), -- Meanings based on regional needs
	 *  	requestSubRole4          (4), -- to refine and expand the basic
	 *  	requestSubRole5          (5), -- roles which are defined elsewhere
	 *  	requestSubRole6          (6),
	 *  	requestSubRole7          (7),
	 *  	requestSubRole8          (8),
	 *  	requestSubRole9          (9),
	 *  	requestSubRole10        (10),
	 *  	requestSubRole11        (11),
	 *  	requestSubRole12        (12),
	 *  	requestSubRole13        (13),
	 *  	requestSubRole14        (14), -- The last type of sub role
	 *  	requestSubRoleReserved  (15)  -- Reserved for future use
	 */
	uint8_t subrole_is_exsit;
	uint8_t subrole;

	/*
	 *  desc: a level of importance in the Priority Scheme
	 *  range: 0..15
	 *  	requestImportanceLevelUnKnown	(0),
	 *  	requestImportanceLevel1 	   (1), -- The least important request
	 *  	requestImportanceLevel2 	   (2), -- The values here shall be assigned
	 *  	requestImportanceLevel3 	   (3), -- Meanings based on regional needs
	 *  	requestImportanceLevel4 	   (4), -- for each of the basic roles which
	 *  	requestImportanceLevel5 	   (5), -- are defined elsewhere
	 *  	requestImportanceLevel6 	   (6),
	 *  	requestImportanceLevel7 	   (7),
	 *  	requestImportanceLevel8 	   (8),
	 *  	requestImportanceLevel9 	   (9),
	 *  	requestImportanceLevel10	  (10),
	 *  	requestImportanceLevel11	  (11),
	 *  	requestImportanceLevel12	  (12),
	 *  	requestImportanceLevel13	  (13),
	 *  	requestImportanceLevel14	  (14), -- The most important request
	 *  	requestImportanceReserved	  (15)	-- Reserved for future use
	 */
	uint8_t requestLevel_is_exsit;
	uint8_t requestLevel;
	
	/*
	 *  des: Additional classification details
	 *  range: 0..100
	 */
	uint8_t iso3883_is_exsit;
	uint8_t	iso3883;

	/*
	 *  des: HPMS classification types
	 *  range: 0..15
	 *  	none                 (0),  -- Not Equipped, Not known or unavailable
	 *  	unknown              (1),  -- Does not fit any other category    
	 *  	special              (2),  -- Special use    
	 *  	moto                 (3),  -- Motorcycle    
	 *  	car                  (4),  -- Passenger car    
	 *  	carOther             (5),  -- Four tire single units    
	 *  	bus                  (6),  -- Buses    
	 *  	axleCnt2             (7),  -- Two axle, six tire single units    
	 *  	axleCnt3             (8),  -- Three axle, single units    
	 *  	axleCnt4             (9),  -- Four or more axle, single unit    
	 *  	axleCnt4Trailer      (10), -- Four or less axle, single trailer    
	 *  	axleCnt5Trailer      (11), -- Five or less axle, single trailer    
	 *  	axleCnt6Trailer      (12), -- Six or more axle, single trailer    
	 *  	axleCnt5MultiTrailer (13), -- Five or less axle, multi-trailer    
	 *  	axleCnt6MultiTrailer (14), -- Six axle, multi-trailer    
	 *  	axleCnt7MultiTrailer (15),  -- Seven or more axle, multi-trailer   
	 */
	uint8_t hpmsType_is_exsit;
	uint8_t hpmsType;
}Mde_RequestorType_t;

typedef struct Mde_EmergencyDetails
{
	/*
	 *  range: 0..31
	 */
	uint8_t sspRights;
	
	/*
	 *	element: SirenInUse
	 *	   unavailable   (0), -- Not Equipped or unavailable
	 *	   notInUse      (1),  
	 *	   inUse         (2),  
	 *	   reserved      (3)  -- for future use
	 *	range: 0..3
	 */
	uint8_t sirenUse;

	/*
	 *	element: LightbarInUse
	 *	   unavailable         (0),  -- Not Equipped or unavailable
	 *	   notInUse            (1),  -- none active
	 *	   inUse               (2),  
	 *	   yellowCautionLights (3),  
	 *	   schooldBusLights    (4),  
	 *	   arrowSignsActive    (5),  
	 *	   slowMovingVehicle   (6),   
	 *	   freqStops           (7)   
	 *	range: 0..7
	 */
	uint8_t lightsUse;

	/*
	 *	element: MultiVehicleResponse
	 *	   unavailable   (0), -- Not Equipped or unavailable
	 *	   singleVehicle (1),
	 *	   multiVehicle  (2),  
	 *	   reserved      (3)  -- for future use
	 *	range: 0..3
	 */
	 uint8_t multi;


	uint8_t sspRights_events_are_exsit;
	/*
	 *	range: 0..31
	 */
	uint8_t events_sspRights;

	/*
	 *	element: PrivilegedEventFlags
	 *  desc: These values require a suitable SSP to be sent
	 *		 peUnavailable				  (0), -- Not Equipped or unavailable
	 *		 peEmergencyResponse		  (1),
	 *		 	-- The vehicle is a properly authorized public safety vehicle, 
	 *		 	-- is engaged in a service call, and is currently moving 
	 *		 	-- or is within the roadway.  Note that lights and sirens 
	 *		 	-- may not be evident during any given response call   
	 *		 -- Emergency and Non Emergency Lights related
	 *		 peEmergencyLightsActive	  (2),
	 *		 peEmergencySoundActive 	  (3),
	 *		 peNonEmergencyLightsActive   (4),
	 *		 peNonEmergencySoundActive	  (5)
	 *	range: 0..1
	 */	
	uint8_t events[6];

	uint8_t responseType_is_exsit;
	/*
	 *	element: ResponseType
	 *		 notInUseOrNotEquipped		(0),			
	 *		 emergency					(1),  -- active service call at emergency level 		  
	 *		 nonEmergency				(2),  -- also used when returning from service call 		 
	 *		 pursuit					(3),  -- sender driving may be erratic 
	 *		 stationary 				(4),  -- sender is not moving, stopped along roadside
	 *		 slowMoving 				(5),  -- such a mowers, litter trucks, etc.
	 *		 stopAndGoMovement			(6),  -- such as school bus or garbage truck 
	 *	range: 0..6
	 */
	uint8_t responseType;
}Mde_EmergencyDetails_t;

typedef enum MKE_PositionOffsetLL_TYPE
{
    Mde_position_LL1_chosen = 1,
    Mde_position_LL2_chosen = 2,
    Mde_position_LL3_chosen = 3,
    Mde_position_LL4_chosen = 4,
    Mde_position_LL5_chosen = 5,
    Mde_position_LL6_chosen = 6,
    Mde_position_LatLon_chosen = 7
}MKE_PositionOffsetLL_TYPE;

typedef enum MKE_VerticalOffset_TYPE
{
    Mde_offset1_chosen = 1,
    Mde_offset2_chosen = 2,
    Mde_offset3_chosen = 3,
    Mde_offset4_chosen = 4,
    Mde_offset5_chosen = 5,
    Mde_offset6_chosen = 6,
    Mde_elevation_chosen = 7
}MKE_VerticalOffset_TYPE;

typedef struct _mde_pos_3d_
{
    MKE_PositionOffsetLL_TYPE   offsetLL_type;
    double                      latitude;
    double                      longitude;
    MKE_VerticalOffset_TYPE     offsetV_type;
    double                      elevation;
}mde_pos_3d_t;

typedef struct mde_reference_id 
{
	unsigned short          region;
	unsigned short	        id;
} mde_reference_id_t;

typedef struct Mde_Path_Prediction
{
    /*
     *  element: RadiusOfCurvature
     *  desc: A straight path to use value of 32767.
     *  range: -32767..32767
     *  LSB: 10cm
     */
	int16_t radiusOfCurve;

    /*
     *  element: Confidence
     *  range: 0..200
     *  LSB: 0.5 percent
     */
	uint8_t confidence;
}Mde_Path_Prediction_t;









// add by panfei
/* Dependencies */
typedef enum Mde_Transmission_State {
	Mde_TransmissionState_neutral,
	Mde_TransmissionState_park	= 1,
	Mde_TransmissionState_forwardGears	= 2,
	Mde_TransmissionState_reverseGears	= 3,
	Mde_TransmissionState_reserved1	= 4,
	Mde_TransmissionState_reserved2	= 5,
	Mde_TransmissionState_reserved3	= 6,
	Mde_TransmissionState_unavailable	= 7
}Mde_Transmission_State_e;

typedef enum Mde_Position_Confidence {
	Mde_PositionConfidence_unavailable	= 0,
	Mde_PositionConfidence_a500m	= 1,
	Mde_PositionConfidence_a200m	= 2,
	Mde_PositionConfidence_a100m	= 3,
	Mde_PositionConfidence_a50m	= 4,
	Mde_PositionConfidence_a20m	= 5,
	Mde_PositionConfidence_a10m	= 6,
	Mde_PositionConfidence_a5m	= 7,
	Mde_PositionConfidence_a2m	= 8,
	Mde_PositionConfidence_a1m	= 9,
	Mde_PositionConfidence_a50cm	= 10,
	Mde_PositionConfidence_a20cm	= 11,
	Mde_PositionConfidence_a10cm	= 12,
	Mde_PositionConfidence_a5cm	= 13,
	Mde_PositionConfidence_a2cm	= 14,
	Mde_PositionConfidence_a1cm	= 15
} Mde_PositionConfidence_e;

typedef enum Mde_Elevation_Confidence {
	Mde_ElevationConfidence_unavailable	= 0,
	Mde_ElevationConfidence_elev_500_00	= 1,
	Mde_ElevationConfidence_elev_200_00	= 2,
	Mde_ElevationConfidence_elev_100_00	= 3,
	Mde_ElevationConfidence_elev_050_00	= 4,
	Mde_ElevationConfidence_elev_020_00	= 5,
	Mde_ElevationConfidence_elev_010_00	= 6,
	Mde_ElevationConfidence_elev_005_00	= 7,
	Mde_ElevationConfidence_elev_002_00	= 8,
	Mde_ElevationConfidence_elev_001_00	= 9,
	Mde_ElevationConfidence_elev_000_50	= 10,
	Mde_ElevationConfidence_elev_000_20	= 11,
	Mde_ElevationConfidence_elev_000_10	= 12,
	Mde_ElevationConfidence_elev_000_05	= 13,
	Mde_ElevationConfidence_elev_000_02	= 14,
	Mde_ElevationConfidence_elev_000_01	= 15
} Mde_ElevationConfidence_e;

/* Dependencies */
typedef enum Mde_Heading_Confidence {
	Mde_HeadingConfidence_unavailable	= 0,
	Mde_HeadingConfidence_prec10deg	= 1,
	Mde_HeadingConfidence_prec05deg	= 2,
	Mde_HeadingConfidence_prec01deg	= 3,
	Mde_HeadingConfidence_prec0_1deg	= 4,
	Mde_HeadingConfidence_prec0_05deg	= 5,
	Mde_HeadingConfidence_prec0_01deg	= 6,
	Mde_HeadingConfidence_prec0_0125deg	= 7
} Mde_HeadingConfidence_e;

typedef enum Mde_Speed_Confidence {
	Mde_SpeedConfidence_unavailable	= 0,
	Mde_SpeedConfidence_prec100ms	= 1,
	Mde_SpeedConfidence_prec10ms	= 2,
	Mde_SpeedConfidence_prec5ms	= 3,
	Mde_SpeedConfidence_prec1ms	= 4,
	Mde_SpeedConfidence_prec0_1ms	= 5,
	Mde_SpeedConfidence_prec0_05ms	= 6,
	Mde_SpeedConfidence_prec0_01ms	= 7
} Mde_SpeedConfidence_e;

typedef enum Mde_Throttle_Confidence {
	Mde_ThrottleConfidence_unavailable	= 0,
	Mde_ThrottleConfidence_prec10percent	= 1,
	Mde_ThrottleConfidence_prec1percent	= 2,
	Mde_ThrottleConfidence_prec0_5percent	= 3
} Mde_ThrottleConfidence_e;

typedef struct Mde_positional_accuracy {
    /*
        semi-major axis accuracy at one standard dev
        range 0-12.7 meter, LSB = .05m
        254 = any value equal or greater than 12.70 meter
        255 = unavailable semi-major axis value
    */
	uint16_t	 semimajor;

    /*
    semi-minor axis accuracy at one standard dev
    range 0-12.7 meter, LSB = .05m
    254 = any value equal or greater than 12.70 meter
    255 = unavailable semi-minor axis value
    */
	uint16_t	 semiminor;

    /*
    orientation of semi-major axis
    relative to true north (0~359.9945078786 degrees)
    LSB units of 360/65535 deg  = 0.0054932479
    a value of 0 shall be 0 degrees
    a value of 1 shall be 0.0054932479 degrees
    a value of 65534 shall be 359.9945078786 deg
    a value of 65535 shall be used for orientation unavailable
    */
	uint16_t	 orientation;
} Mde_positional_accuracy_t;


typedef struct Mde_position_speed_confidence
{
    /*
     *  element: ElevationConfidence
     *  desc: The element is used to provide the current position confidence for elevation.
     *  range: 0...15
     *  unavailable (0), -- Not Equipped or unavailable
     *  500m (1),
     *  200m (2),
     *  100m (3),
     *  50m (4),
     *  20m (5),
     *  10m (6),
     *  5m (7),
     *  2m (8),
     *  1m (9),
     *  0.50m (10),
     *  0.20m (11),
     *  0.10m (12),
     *  0.05m (13),
     *  0.02m (14),
     *  0.01m (15),
     */
    Mde_PositionConfidence_e        pos_confidence;

    /*
     *  element: PositionConfidence
     *  desc: The element is used to provide the current position confidence for both horizontal directions.
     *  range: 0...15
     *  unavailable (0), -- Not Equipped or unavailable
     *  500m (1),
     *  200m (2),
     *  100m (3),
     *  50m (4),
     *  20m (5),
     *  10m (6),
     *  5m (7),
     *  2m (8),
     *  1m (9),
     *  0.50m (10),
     *  0.20m (11),
     *  0.10m (12),
     *  0.05m (13),
     *  0.02m (14),
     *  0.01m (15),
     */
    Mde_ElevationConfidence_e      elevation_confidence;

    /*
	 *  element: HeadingConfidence
	 *  range: 0-7
	 *  	unavailable   (0), -- B'000  Not Equipped or unavailable
	 *  	prec10deg     (1), -- B'010  10     degrees
	 *  	prec05deg     (2), -- B'011  5      degrees
	 *  	prec01deg     (3), -- B'100  1      degrees
	 *  	prec0-1deg    (4), -- B'101  0.1    degrees
	 *  	prec0-05deg   (5), -- B'110  0.05   degrees
	 *  	prec0-01deg   (6), -- B'110  0.01   degrees
	 *  	prec0-0125deg (7)  -- B'111  0.0125 degrees, aligned with heading LSB
	 */
    Mde_HeadingConfidence_e heading_confidence;

    /*
	 *  element: SpeedConfidence
	 *  range: 0-7
	 *  	unavailable (0), -- Not Equipped or unavailable
	 *  	prec100ms   (1), -- 100  meters / sec
	 *  	prec10ms    (2), -- 10   meters / sec
	 *  	prec5ms     (3), -- 5    meters / sec
	 *  	prec1ms     (4), -- 1    meters / sec
	 *  	prec0-1ms   (5), -- 0.1  meters / sec
	 *  	prec0-05ms  (6), -- 0.05 meters / sec
	 *  	prec0-01ms  (7)  -- 0.01 meters / sec
	 */
    Mde_SpeedConfidence_e speed_confidence;

    /*
	  *  element: ThrottleConfidence
	  *  range: 0-3
	  * 	 unavailable	 (0), -- B'00  Not Equipped or unavailable
	  * 	 prec10percent	 (1), -- B'01  10  percent Confidence level
	  * 	 prec1percent	 (2), -- B'10  1   percent Confidence level
	  * 	 prec0-5percent  (3)  -- B'11  0.5 percent Confidence level
	  */
    Mde_ThrottleConfidence_e throttle_confidence;
}Mde_position_speed_confidence_t;

typedef struct Mde_AccelerationSet4Way
{
    float   long_Acceleration;   //Along the Vehicle Longitudinal axis
    float   lat_Acceleration;   //Along the Vehicle Lateral axis
    float   vert_VerticalAcceleration;  //Along the Vehicle Vertical axis
    float   yaw_YawRate;
}Mde_AccelerationSet4Way_t;

typedef struct Mde_DDateTime
{
	/*
	 *  element: utcTime_is_exist 
	 *  desc: utcTime is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t year_is_exsit;
	/*
	 *  range: 0..4095
	 */
	uint16_t year;

	
	uint8_t month_is_exsit;
	/*
	 *  range: 0..12
	 */	
	uint8_t month;


	uint8_t day_is_exsit;
	/*
	 *  range: 0..31
	 */	
	uint8_t day;


	uint8_t hour_is_exsit;
	/*
	 *  range: 0..31
	 */	
	uint8_t hour;


	uint8_t minute_is_exsit;
	/*
	 *  range: 0..60
	 */	
	uint8_t minute;

	
	uint8_t second_is_exsit;
	/*
	 *  range: 0..65535
	 *  units: milliseconds
	 */	
	uint16_t second;

	
	uint8_t offset_is_exsit;
	/*
	 *  range: -840..840
	 *  units: minutes from UTC time
	 */
	int16_t offset;
}Mde_DDateTime_t;

typedef struct mde_SpeedandHeadingandThrottleConfidence {
	Mde_HeadingConfidence_e   heading;
	Mde_SpeedConfidence_e     speed;
	Mde_ThrottleConfidence_e  throttle;
} Mde_SpeedandHeadingandThrottleConfidence_t;

typedef struct Mde_Full_Position_Vector
{
	/*
	 *  element: utcTime_is_exist 
	 *  desc: utcTime is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t utcTime_is_exist;

	Mde_DDateTime_t utcTime;

	/*
	 *  element: Longitude 
	 *  desc: Providing a range of plus-minus 180 degrees.
	 *  range: -1799999999..1800000001
	 *  LSB: 1/10 micro degree
	 */
	double Long;

	/*
	 *  element: Latitude 
	 *  desc: Providing a range of plus-minus 90 degrees.
	 *  range: -900000000..900000001
	 *  LSB: 1/10 micro degree
	 */
	double lat;


	/*
	 *  element: elevation_is_exsit 
	 *  desc: elevation is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t elevation_is_exsit;

	/*
	 *  element: Elevation 
	 *  desc: Providing a range of -409.5 to + 6143.9 meters. The value -4096 shall be used when Unknown is to be sent
	 *  range: -4096..61439
	 *  units: 10 cm
	 */
	int32_t elevation;

	
	/*
	 *  element: heading_is_exsit 
	 *  desc: heading is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t heading_is_exsit;

	/*
	 *  element: Heading 
	 *  desc: A range of 0 to 359.9875 degrees
	 *  range: 0..28800
	 *  LSB: 0.0125 degrees
	 */
	uint16_t heading;

	/*
	 *  element: transmission_Speed_are_exsit 
	 *  desc: transmission and Speed are exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t transmission_Speed_are_exsit;

    /*
     *  element: TransmissionState 
     *  desc: Transmission state. Any related speed is relative to the vehicle reference frame used.
     *  range: 0..7
     * 		neutral      (0), -- Neutral
     * 		park         (1), -- Park 
     * 		forwardGears (2), -- Forward gears
     * 		reverseGears (3), -- Reverse gears 
     * 		reserved1    (4),      
     * 		reserved2    (5),      
     * 		reserved3    (6),      
     * 		unavailable  (7)  -- not-equipped or unavailable value,
     */
	uint8_t transmisson;

    /*
     *  element: Velocity 
     *  desc: Speed. The value 8191 indicates that velocity is unavailable
     *  range: 0..8191
     *  Units: 0.02 m/s
     */
	uint16_t speed;

	/*
	 *  element: posAccuracy_is_exsit 
	 *  desc: posAccuracy is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t posAccuracy_is_exsit;
	
	Mde_positional_accuracy_t posAccuracy;

	/*
	 *  element: timeConfidence_is_exsit 
	 *  desc: timeConfidence is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t timeConfidence_is_exsit;

    /*
     *  element: TimeConfidence 
     *  desc: time Confidence.
     *  range: 0..39
     * 		unavailable              (0), -- Not Equipped or unavailable
     * 		time-100-000             (1), -- Better than  100 Seconds
     * 		time-050-000             (2), -- Better than  50 Seconds
     * 		time-020-000             (3), -- Better than  20 Seconds
     * 		time-010-000             (4), -- Better than  10 Seconds
     * 		time-002-000             (5), -- Better than  2 Seconds
     * 		time-001-000             (6), -- Better than  1 Second
     * 		time-000-500             (7), -- Better than  0.5 Seconds
     * 		time-000-200             (8), -- Better than  0.2 Seconds
     * 		time-000-100             (9), -- Better than  0.1 Seconds
     * 		time-000-050            (10), -- Better than  0.05 Seconds
     * 		time-000-020            (11), -- Better than  0.02 Seconds
     * 		time-000-010            (12), -- Better than  0.01 Seconds
     * 		time-000-005            (13), -- Better than  0.005 Seconds
     * 		time-000-002            (14), -- Better than  0.002 Seconds
     * 		time-000-001            (15), -- Better than  0.001 Seconds
     * 		                             -- Better than  one millisecond
     * 		time-000-000-5          (16), -- Better than  0.000,5 Seconds
     * 		time-000-000-2          (17), -- Better than  0.000,2 Seconds
     * 		time-000-000-1          (18), -- Better than  0.000,1 Seconds
     * 		time-000-000-05         (19), -- Better than  0.000,05 Seconds
     * 		time-000-000-02         (20), -- Better than  0.000,02 Seconds
     * 		time-000-000-01         (21), -- Better than  0.000,01 Seconds
     * 		time-000-000-005        (22), -- Better than  0.000,005 Seconds
     * 		time-000-000-002        (23), -- Better than  0.000,002 Seconds
     * 		time-000-000-001        (24), -- Better than  0.000,001 Seconds 
     * 		                             -- Better than  one micro second
     * 		time-000-000-000-5      (25), -- Better than  0.000,000,5 Seconds
     * 		time-000-000-000-2      (26), -- Better than  0.000,000,2 Seconds
     * 		time-000-000-000-1      (27), -- Better than  0.000,000,1 Seconds
     * 		time-000-000-000-05     (28), -- Better than  0.000,000,05 Seconds
     * 		time-000-000-000-02     (29), -- Better than  0.000,000,02 Seconds
     * 		time-000-000-000-01     (30), -- Better than  0.000,000,01 Seconds
     * 		time-000-000-000-005    (31), -- Better than  0.000,000,005 Seconds
     * 		time-000-000-000-002    (32), -- Better than  0.000,000,002 Seconds
     * 		time-000-000-000-001    (33), -- Better than  0.000,000,001 Seconds
     * 		                             -- Better than  one nano second
     * 		time-000-000-000-000-5  (34), -- Better than  0.000,000,000,5 Seconds
     * 		time-000-000-000-000-2  (35), -- Better than  0.000,000,000,2 Seconds
     * 		time-000-000-000-000-1  (36), -- Better than  0.000,000,000,1 Seconds
     * 		time-000-000-000-000-05 (37), -- Better than  0.000,000,000,05 Seconds
     * 		time-000-000-000-000-02 (38), -- Better than  0.000,000,000,02 Seconds
     * 		time-000-000-000-000-01 (39)  -- Better than  0.000,000,000,01 Seconds 
     */
	MDE_TimeConfidence_e timeConfidence;


	/*
	 *  element: pos_elevation_are_exsit 
	 *  desc: pos and elevation are exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t pos_elevation_are_exsit;
	
    /*
     *  element: PositionConfidence 
     *  desc: Pos Confidence.
     *  range: 0..15
     *  	unavailable (0),  -- B'0000  Not Equipped or unavailable
     *  	a500m   (1), -- B'0001  500m  or about 5 * 10 ^ -3 decimal degrees
     *  	a200m   (2), -- B'0010  200m  or about 2 * 10 ^ -3 decimal degrees
     *  	a100m   (3), -- B'0011  100m  or about 1 * 10 ^ -3 decimal degrees
     *  	a50m    (4), -- B'0100  50m   or about 5 * 10 ^ -4 decimal degrees 
     *  	a20m    (5), -- B'0101  20m   or about 2 * 10 ^ -4 decimal degrees 
     *  	a10m    (6), -- B'0110  10m   or about 1 * 10 ^ -4 decimal degrees 
     *  	a5m     (7), -- B'0111  5m    or about 5 * 10 ^ -5 decimal degrees 
     *  	a2m     (8), -- B'1000  2m    or about 2 * 10 ^ -5 decimal degrees 
     *  	a1m     (9), -- B'1001  1m    or about 1 * 10 ^ -5 decimal degrees 
     *  	a50cm  (10), -- B'1010  0.50m or about 5 * 10 ^ -6 decimal degrees 
     *  	a20cm  (11), -- B'1011  0.20m or about 2 * 10 ^ -6 decimal degrees 
     *  	a10cm  (12), -- B'1100  0.10m or about 1 * 10 ^ -6 decimal degrees 
     *  	a5cm   (13), -- B'1101  0.05m or about 5 * 10 ^ -7 decimal degrees 
     *  	a2cm   (14), -- B'1110  0.02m or about 2 * 10 ^ -7 decimal degrees 
     *  	a1cm   (15)  -- B'1111  0.01m or about 1 * 10 ^ -7 decimal degrees 
     */
	uint8_t pos;

    /*
     *  element: ElevationConfidence 
     *  desc: Elevation Confidence.
     *  range: 0..15
     *  	unavailable (0),  -- B'0000  Not Equipped or unavailable
     *  	elev-500-00 (1),  -- B'0001  (500 m)
     *  	elev-200-00 (2),  -- B'0010  (200 m)
     *  	elev-100-00 (3),  -- B'0011  (100 m)
     *  	elev-050-00 (4),  -- B'0100  (50 m)
     *  	elev-020-00 (5),  -- B'0101  (20 m)
     *  	elev-010-00 (6),  -- B'0110  (10 m)
     *  	elev-005-00 (7),  -- B'0111  (5 m)
     *  	elev-002-00 (8),  -- B'1000  (2 m)
     *  	elev-001-00 (9),  -- B'1001  (1 m)
     *  	elev-000-50 (10), -- B'1010  (50 cm)
     *  	elev-000-20 (11), -- B'1011  (20 cm)
     *  	elev-000-10 (12), -- B'1100  (10 cm)
     *  	elev-000-05 (13), -- B'1101  (5 cm)
     *  	elev-000-02 (14), -- B'1110  (2 cm)
     *  	elev-000-01 (15)  -- B'1111  (1 cm)
     */
	uint8_t posC_elevation;


	/*
	 *  element: speedConfidence_is_exsit 
	 *  desc: speedConfidence is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t speedConfidence_is_exsit;

	Mde_SpeedandHeadingandThrottleConfidence_t speed_heading_throttle;
	
}Mde_Full_Position_Vector_t;

typedef struct Mde_Path_History_Point
{
    /*
     *  element: OffsetLL-B18 
     *  desc: 
     * 		The value +131071 shall be used for values >= than +0.0131071 degrees
     * 		The value -131071 shall be used for values <= than -0.0131071 degrees
     * 		The value -131072 shall be used unknown
     *  range: -131072..131071
     *  LSB:  0.1 microdegrees (unless a zoom is employed)
     */
	int32_t latOffset;
	
    /*
     *  element: OffsetLL-B18 
     *  desc: 
     * 		The value +131071 shall be used for values >= than +0.0131071 degrees
     * 		The value -131071 shall be used for values <= than -0.0131071 degrees
     * 		The value -131072 shall be used unknown
     *  range: -131072..131071
     *  LSB:  0.1 microdegrees (unless a zoom is employed)
     */
	int32_t lonOffset;

    /*
     *  element: OffsetLL-B18 
     *  desc: 
     * 		value -2047 to be used for -2047 or greater
     * 		value  2047 to be used for  2047 or greater
     * 		value -2048 to be unavailable
     *  range: -2048..2047
     *  LSB:  10 cm
     */
	int16_t elevationOffset;

    /*
     *  element: TimeOffset
     *  desc: 
     * 		a value of 65534 to be used for 655.34 seconds or greater
     * 		a value of 65535 to be unavailable
     *  range: 1..65535
     *  LSB:  10 mSec
     */
	uint16_t timeOffset;

	uint8_t speed_is_exsit;		
	/*
	 *	element: Speed 
	 *	desc: Speed. The value 8191 indicates that velocity is unavailable
	 *	range: 0..8191
	 *	Units: 0.02 m/s
	 */
	uint16_t speed;


	uint8_t posAccuracy_is_exsit;	
	Mde_positional_accuracy_t posAccuracy;

	uint8_t heading_is_exsit;
	/*
	 *  element: CoarseHeading 
	 *  desc: the value 240 shall be used for unavailable
	 *  range: 0..240
	 *  LSB: 1.5 degrees
	 */
	uint16_t heading;
}Mde_Path_History_Point_t;


typedef struct Mde_Path_History
{
	/*
	 *  element: initialPosition_is_exist 
	 *  desc: initialPosition is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t initialPosition_is_exist;
	
	/*
	 *  element: currGNSSsts_is_exist 
	 *  desc: currGNSSsts is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	Mde_Full_Position_Vector_t initialPosition;


	/*
	 *  element: currGNSSsts_is_exist 
	 *  desc: currGNSSsts is exsit. 0 stand for not exsit, 1 stand for exsit.
	 *  range: 0..1
	 */
	uint8_t currGNSSsts_is_exist;

    /*
     *  element: VehicleEventFlags 
     *  desc: Vehicle event flags
     * 		unavailable			   (0), -- Not Equipped or unavailable
     * 		isHealthy				   (1),
     * 		isMonitored			   (2),
     * 		baseStationType		   (3), -- Set to zero if a moving base station,
     * 									-- or if a rover device (an OBU),
     * 									-- set to one if it is a fixed base station 
     * 		aPDOPofUnder5			   (4), -- A dilution of precision greater than 5
     * 		inViewOfUnder5 		   (5), -- Less than 5 satellites in view
     * 		localCorrectionsPresent   (6), -- DGPS type corrections used
     * 		networkCorrectionsPresent (7)	-- RTK type corrections used
     *  range: 0..1
     */
	uint8_t currGNSSstatus[8];


	/*
	 *  element: ph_count
	 *  desc: path_his count.
	 *  range: 1..23
	 */
	uint8_t ph_count;

	Mde_Path_History_Point_t path_his[23];	
}Mde_Path_History_t;


typedef struct Mde_HeadingSlice {
	int size;
	#define MDE_HEADING_SLICE_BUFF_MAX (16)
	uint8_t buf[MDE_HEADING_SLICE_BUFF_MAX];
	/*
	-- Each bit 22.5 degree starting from
	-- North and moving Eastward (clockwise) as one bit
	-- a value of noHeading means no bits set, while a
	-- a value of allHeadings means all bits would be set

	from000-0to022-5degrees (0),
	from022-5to045-0degrees (1),
	from045-0to067-5degrees (2),
	from067-5to090-0degrees (3),

	from090-0to112-5degrees (4), 
	from112-5to135-0degrees (5),
	from135-0to157-5degrees (6),
	from157-5to180-0degrees (7),

	from180-0to202-5degrees (8),
	from202-5to225-0degrees (9),
	from225-0to247-5degrees (10),
	from247-5to270-0degrees (11),

	from270-0to292-5degrees (12),
	from292-5to315-0degrees (13),
	from315-0to337-5degrees (14),
	from337-5to360-0degrees (15) 	
	*/
} Mde_HeadingSlice_t;


/* TransmissionAndSpeed */
typedef struct Mde_TransmissionAndSpeed {
	long	 transmisson;
	long	 speed;
} Mde_TransmissionAndSpeed_t;

/* PositionalAccuracy */
typedef struct Mde_PositionalAccuracy_t {
	long	 semiMajor;
	long	 semiMinor;
	long	 orientation;
} Mde_PositionalAccuracy_t;


/* PositionConfidenceSet */
typedef struct Mde_PositionConfidenceSet {
	long	 pos;
	long	 elevation;
} Mde_PositionConfidenceSet_t;



/* FullPositionVector */
typedef struct Mde_FullPositionVector {
	int utcTime_active;
	Mde_DDateTime_t	utcTime	/* OPTIONAL */;
	long	 Long;
	long	 lat;

	int elevation_active;
	long	elevation	/* OPTIONAL */;
	int heading_active;
	long	heading	/* OPTIONAL */;

	int speed_active;
	Mde_TransmissionAndSpeed_t	speed	/* OPTIONAL */;

	int posAccuracy_active;
	Mde_PositionalAccuracy_t	posAccuracy	/* OPTIONAL */;

	int timeConfidence_active;
	long	timeConfidence	/* OPTIONAL */;

	int posConfidence_active;
	Mde_PositionConfidenceSet_t	posConfidence	/* OPTIONAL */;

	int speedConfidence_active;
	Mde_SpeedandHeadingandThrottleConfidence_t	speedConfidence	/* OPTIONAL */;
} Mde_FullPositionVector_t;

typedef struct Mde_Vehicle_Safety_Ext
{
    /*
     *  element: event_is_exist 
     *  desc: events is exsit. 0 stand for not exsit, 1 stand for exsit.
     *  range: 0..1
     */
     uint8_t event_is_exist;

    /*
     *  element: VehicleEventFlags 
     *  desc: Vehicle event flags
     *  	eventHazardLights               (0),
     *  	eventStopLineViolation          (1), -- Intersection Violation   
     *  	eventABSactivated               (2),
     *  	eventTractionControlLoss        (3),
     *  	eventStabilityControlactivated  (4),
     *  	eventHazardousMaterials         (5),
     *  	eventReserved1                  (6),   
     *  	eventHardBraking                (7),
     *  	eventLightsChanged              (8),
     *  	eventWipersChanged              (9),
     *  	eventFlatTire                   (10),
     *  	eventDisabledVehicle            (11), -- The DisabledVehicle DF may also be sent
     *  	eventAirBagDeployment           (12)
     *  range: 0..1
     */
     uint8_t events[13];


    /*
     *  element: pathHistory_is_exsit 
     *  desc: pathHistory is exsit. 0 stand for not exsit, 1 stand for exsit.
     *  range: 0..1
     */
	 uint8_t pathHistory_is_exsit;
	
	 Mde_Path_History_t pathHistory;

	 
	 /*
	  *  element: pathPrediction_is_exsit 
	  *  desc: pathPrediction is exsit. 0 stand for not exsit, 1 stand for exsit.
	  *  range: 0..1
	  */
	 uint8_t pathPrediction_is_exsit;

	 Mde_Path_Prediction_t pathPrediction;

	 
	 /*
	  *  element: ExteriorLights_is_exsit 
	  *  desc: ExteriorLights is exsit. 0 stand for not exsit, 1 stand for exsit.
	  *  range: 0..1
	  */
	 uint8_t ExteriorLights_is_exsit;
	 
	 /*
	  *  element: ExteriorLights 
	  *  desc: Exterior Lights. All lights off is indicated by no bits set.
	  * 	lowBeamHeadlightsOn		(0), 
	  * 	highBeamHeadlightsOn	(1), 
	  * 	leftTurnSignalOn		(2), 
	  * 	rightTurnSignalOn 		(3), 
	  * 	hazardSignalOn			(4), 
	  * 	automaticLightControlOn	(5),
	  * 	daytimeRunningLightsOn	(6), 
	  * 	fogLightOn				(7), 
	  * 	parkingLightsOn			(8) 
	  *  range: 0..1
	  */
	 uint8_t lights[9]; 
}Mde_Vehicle_Safety_Ext_t;


// Position3D 
typedef struct Mde_Position3D {
	long   lat;
	long   Long;
    int    elevation_active;
	long   elevation;	// OPTIONAL ;
} Mde_Position3D_t;



typedef enum Mde_NodeListXY_PR {
	Mde_NodeListXY_PR_NOTHING,	// No components present 
	Mde_NodeListXY_PR_nodes,
	Mde_NodeListXY_PR_computed
	// Extensions may appear below 
} Mde_NodeListXY_PR;


// Dependencies 
typedef enum Mde_NodeOffsetPointXY_PR {
	Mde_NodeOffsetPointXY_PR_NOTHING,	// No components present 
	Mde_NodeOffsetPointXY_PR_node_XY1,
	Mde_NodeOffsetPointXY_PR_node_XY2,
	Mde_NodeOffsetPointXY_PR_node_XY3,
	Mde_NodeOffsetPointXY_PR_node_XY4,
	Mde_NodeOffsetPointXY_PR_node_XY5,
	Mde_NodeOffsetPointXY_PR_node_XY6,
	Mde_NodeOffsetPointXY_PR_node_LatLon,
	Mde_NodeOffsetPointXY_PR_regional
} Mde_NodeOffsetPointXY_PR;

// Node-XY-20b 
typedef struct Mde_Node_XY {
	long	 x;
	long	 y;
} Mde_Node_XY_t;

// Node-LLmD-64b 
typedef struct Mde_Node_LLmD_64b {
	long	 lon;
	long	 lat;
} Mde_Node_LLmD_64b_t;

//RegionalExtension 
typedef struct Mde_RegionalExtension_124P0 {
	long	 regionId;
} Mde_RegionalExtension_124P0_t;

// NodeOffsetPointXY 
typedef struct Mde_NodeOffsetPointXY {
	Mde_NodeOffsetPointXY_PR present;
	Mde_Node_XY_t node_XY1;
	Mde_Node_XY_t node_XY2;
	Mde_Node_XY_t node_XY3;
	Mde_Node_XY_t node_XY4;
	Mde_Node_XY_t node_XY5;
	Mde_Node_XY_t node_XY6;
	Mde_Node_LLmD_64b_t node_LatLon;
	Mde_RegionalExtension_124P0_t	 regional;
} Mde_NodeOffsetPointXY_t;

// Dependencies 
typedef enum Mde_LaneDataAttribute_PR {
	Mde_LaneDataAttribute_PR_NOTHING,	// No components present 
	Mde_LaneDataAttribute_PR_pathEndPointAngle,
	Mde_LaneDataAttribute_PR_laneCrownPointCenter,
	Mde_LaneDataAttribute_PR_laneCrownPointLeft,
	Mde_LaneDataAttribute_PR_laneCrownPointRight,
	Mde_LaneDataAttribute_PR_laneAngle,
	Mde_LaneDataAttribute_PR_speedLimits,
	Mde_LaneDataAttribute_PR_regional
	// Extensions may appear below 
	
} Mde_LaneDataAttribute_PR;


typedef struct Mde_RegulatorySpeedLimit {
	long	 type;
	long	 speed;
} Mde_RegulatorySpeedLimit_t;

// LaneDataAttribute 
typedef struct Mde_LaneDataAttribute {
	Mde_LaneDataAttribute_PR present;
	struct Mde_LaneDataAttribute_u {
		long	 pathEndPointAngle;
		long	 laneCrownPointCenter;
		long	 laneCrownPointLeft;
		long	 laneCrownPointRight;
		long	 laneAngle;
		int      speedLimits_count;
		#define MDE_SPEEDLIMITS_COUNT_MAX (9)
		Mde_RegulatorySpeedLimit_t	 speedLimits[MDE_SPEEDLIMITS_COUNT_MAX];
	} choice;
	
} Mde_LaneDataAttribute_t;


// NodeAttributeSetXY 
typedef struct Mde_NodeAttributeSetXY {
	uint32_t localNode_count;
	#define MDE_LOCALNODE_COUNT_MAX (8)
	long	 localNode[MDE_LOCALNODE_COUNT_MAX];

	uint32_t disabled_count;
	#define MDE_DISABLED_COUNT_MAX (8)
	long     disabled[8];

	uint32_t enabled_count;
	#define MDE_ENABLE_COUNT_MAX (8)
	long     enabled[MDE_ENABLE_COUNT_MAX];

	uint32_t data_count;
	#define MDE_DATA_COUNT_MAX (8)
	Mde_LaneDataAttribute_t   data[MDE_DATA_COUNT_MAX];

    int  dWidth_active;
	long dWidth;	// OPTIONAL ;
	
    int  dElevation_active;
	long	dElevation;	// OPTIONAL ;
} Mde_NodeAttributeSetXY_t;

// NodeXY 
typedef struct Mde_NodeXY {
	Mde_NodeOffsetPointXY_t	 delta;
    int attributes_active;
	Mde_NodeAttributeSetXY_t attributes;	//OPTIONAL ;
} Mde_NodeXY_t;

// Dependencies 
typedef enum Mde_ComputedLane__offsetXaxis_PR {
	Mde_ComputedLane__offsetXaxis_PR_NOTHING,	// No components present 
	Mde_ComputedLane__offsetXaxis_PR_small,
	Mde_ComputedLane__offsetXaxis_PR_large
} Mde_ComputedLane__offsetXaxis_PR;

typedef enum Mde_ComputedLane__offsetYaxis_PR {
	Mde_ComputedLane__offsetYaxis_PR_NOTHING,	// No components present 
	Mde_ComputedLane__offsetYaxis_PR_small,
	Mde_ComputedLane__offsetYaxis_PR_large
} Mde_ComputedLane__offsetYaxis_PR;

/* ComputedLane */
typedef struct Mde_ComputedLane {
	long	 referenceLaneId;
	Mde_ComputedLane__offsetXaxis_PR present_x;
	long	 small_x;
	long	 large_x;
	Mde_ComputedLane__offsetYaxis_PR present_y;
	long	 small_y;
	long	 large_y;
    int     rotateXY_active;     
	long	rotateXY;	// OPTIONAL ;
    int     scaleXaxis_active;  
	long	scaleXaxis;	// OPTIONAL ;
    int     scaleYaxis_active;  
	long	scaleYaxis;	// OPTIONAL ;
} Mde_ComputedLane_t;

/* NodeListXY */
typedef struct Mde_NodeListXY {
	Mde_NodeListXY_PR present;
	uint32_t NodeSetXY_count;
    #define MDE_NODELISTXY_MAX (63)
	Mde_NodeXY_t nodes[MDE_NODELISTXY_MAX];
	Mde_ComputedLane_t computed;	
} Mde_NodeListXY_t;

// RoadSegmentReferenceID 
typedef struct Mde_RoadSegmentReferenceID {
    int   region_active;
	long  region;	//OPTIONAL ;
	long  id;
} Mde_RoadSegmentReferenceID_t;

typedef struct Mde_DescriptiveName {
	int size;
	#define MDE_DESCRIPTIVE_NAME_MAX (63)
	uint8_t buf[MDE_DESCRIPTIVE_NAME_MAX];	
} Mde_DescriptiveName_t;

typedef struct Mde_IA5String {
	#define MDE_IA5STRING_MAX (500)
	uint8_t buf[MDE_IA5STRING_MAX];	
	int size;	
} Mde_IA5String_t;


#endif

