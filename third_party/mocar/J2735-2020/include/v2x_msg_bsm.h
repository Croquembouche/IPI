#ifndef __V2X_MSG_BSM_H
#define __V2X_MSG_BSM_H

#include <v2x_msg_common.h>
#include <stdbool.h>


/*************************************************************************/
/**************************** BasicVehicleClass **************************/
/*************************************************************************/

#define BRAKE_STATUS_NUM 5

enum
{
    WHEEL_UNAVAILABLE,
    WHEEL_LEFT_FRONT,
    WHEEL_LEFT_REAR,
    WHEEL_RIGHT_FRONT,
    WHEEL_RIGHT_REAR
};

/*************************************************************************/
/**************************** BSM Part II content ************************/
/*************************************************************************/

typedef enum partII_Id
{
	VEHICLE_SAFETY_EXTENSIONS,
	SPECIAL_VEHICLE_EXTENSIONS,
	SUPPLEMENTAL_VEHICLE_EXTENSIONS
}partII_Id_t;

/*
enum Mde_VehicleEventFlags
{
	EVENT_HAZARDLIGHTS,
	EVENT_STOPLINEVIOLATION,
	EVENT_ABSACTIVATED,
	EVENT_TRACTIONCONTROLLOSS,
	EVENT_STABILITYCONTROLACTIVATED,
	EVENT_HAZARDOUSMATERIALS,
	EVENT_RESERVED1,
	EVENT_HARDBRAKING,
	EVENT_LIGHTSCHANGED,
	EVENT_WIPERSCHANGED,
	EVENT_FLATTIRE,
	EVENT_DISABLEDVEHICLE,
	EVENT_AIRBAGDEPLOYMENT,
	EVENT_MAX
};
*/






typedef struct Mde_Event_Description
{
	/*
	 *	element: ITIS.ITIScodes
	 *	range: 0..65535
	 */
	uint16_t typeEvent;


	/*
     *  desc:  description is optional , description_size = 0 stand for no element description
	 *	range: 0..8
	 */
	uint8_t description_size;
	/*
  	 *	element: ITIS.ITIScodes
     *  desc:  Up to eight ITIS code set entries to further describe the event, give advice, or any other ITIS codes
	 *	range: 0..65535
	 */
	uint16_t description[8];  /*optional ,    size = 0 stand for no this element*/


	/*
     *  desc:  priority is optional , priority_size = 0 stand for no element priority
	 *	range: 0..1
	 */
	uint8_t priority_size;
	/*
     *  desc:  Follow definition notes on setting these bits
	 */
	uint8_t priority[1];

	uint8_t heading_is_exsit;
    /*
     *  element: HeadingSlice[16]
     *  	from000-0to022-5degrees  (0),
     *  	from022-5to045-0degrees  (1), 
     *  	from045-0to067-5degrees  (2), 
     *  	from067-5to090-0degrees  (3), 
     *  	from090-0to112-5degrees  (4), 
     *  	from112-5to135-0degrees  (5), 
     *  	from135-0to157-5degrees  (6), 
     *  	from157-5to180-0degrees  (7), 
     *  	from180-0to202-5degrees  (8), 
     *  	from202-5to225-0degrees  (9), 
     *  	from225-0to247-5degrees  (10),
     *  	from247-5to270-0degrees  (11),
     *  	from270-0to292-5degrees  (12),
     *  	from292-5to315-0degrees  (13),
     *  	from315-0to337-5degrees  (14),
     *  	from337-5to360-0degrees  (15) 
     *  range: 0...1
     */
	uint8_t heading[16];

	uint8_t extent_is_exsit;
	/*
	 *  element: Extent
	 *	range: 0..15
	 *  	useInstantlyOnly	 (0),
	 *  	useFor3meters		 (1),
	 *  	useFor10meters		 (2),
	 *  	useFor50meters		 (3),
	 *  	useFor100meters 	 (4),
	 *  	useFor500meters 	 (5),
	 *  	useFor1000meters	 (6),
	 *  	useFor5000meters	 (7),
	 *  	useFor10000meters	 (8),
	 *  	useFor50000meters	 (9),
	 *  	useFor100000meters	 (10),
	 *  	useFor500000meters	 (11),
	 *  	useFor1000000meters  (12),
	 *  	useFor5000000meters  (13),
	 *  	useFor10000000meters (14),
	 *  	forever 			 (15)  -- very wide area
	 */
	uint8_t extent;

	// NOTICE : 
	// Here is ptional regional. But I have no idea to fill it.So default null
	
}Mde_Event_Description_t;

typedef struct Mde_PivotPointDescription
{	
	/*
	 *	range: -1024..1023
	 */
	int16_t pivotOffset;

	/*
	 *	range: 0..28800
	 *  LSB: 0.0125 degrees
	 */
	uint16_t pivotAngle;
		
	/*
	 *	range: This machine can rotate around the front pivot connection point, is true
	 */
	bool pivots;
}Mde_PivotPointDescription_t;

typedef struct Mde_TrailerHistoryPoint
{
	/*
	 *	range: 0..28800
	 *  LSB: 0.0125 degrees
	 */
	uint16_t pivotAngle;
	
	/*
	 *	range: 1..65535
	 *		a value of 65534 to be used for 655.34 seconds or greater
	 *		a value of 65535 to be unavailable
	 *  LSB: 10 mSec
	 */
	uint16_t timeOffset;


	/*
	 *	range: -2048..2047
	 */
	int16_t positionOffset_x;
		
	/*
	 *	range: -2048..2047
	 */
	int16_t	positionOffset_y;

	uint8_t elevationOffset_is_exsit;
	/*
	 *	range: -64..63
	 *	LSB: 10 cm
	 *		value  63 to be used for  63 or greater
	 *		value -63 to be used for -63 or greater
	 *		value -64 to be unavailable
	 */	
	int8_t elevationOffset;

	uint8_t heading_is_exsit;
	/*
	 *	range: 0..240
	 *	LSB: 1.5 degrees 
	 *		the value 240 shall be used for unavailable
	 */	
	uint8_t heading;	
}Mde_TrailerHistoryPoint_t;



typedef struct Mde_TrailerUnitDescription
{
	/*
	 *	true: When false indicates a trailer unit
	 */
	bool isDolly;

	/*
	 *	range: 0..1023
	 *  LSB: 1 cm with a range of >10 meters
	 */
	uint16_t width;

	/*
	 *	range: 0.. 4095
	 *  LSB: 1 cm with a range of >40 meters
	 */	
	uint16_t length;

	uint8_t height_is_exsit;
	/*
	 *  desc: the height of the vehicle
	 *	range: 0..127
	 *  LSB: units of 5 cm, range to 6.35 meters
	 */	
	uint8_t height;

	uint8_t mass_is_exsit;
	/*
	 *  desc: the height of the vehicle
	 *	range: 0..255
	 *  	the value 0 shall be uaed for an unknown mass value
	 *  	the value 255 shall be used any mass larger than 127,500kg
	 *  LSB: steps of 500 kg (~1100 lbs)
	 */	
	uint8_t mass;


	uint8_t bumperHeights_is_exsit;
	/*
	 *	range: 0..127
	 *  units: 0.01 meters from ground surface.
	 */		
	uint8_t bumperHeights_front;
	/*
	 *	range: 0..127
	 *  units: 0.01 meters from ground surface.
	 */		
	uint8_t bumperHeights_rear;

	
	uint8_t centerOfGravity_is_exsit;
	/*
	 *  desc: the height of the vehicle
	 *	range: 0..127
	 *  LSB: units of 5 cm, range to 6.35 meters
	 */	
	uint8_t centerOfGravity;


	Mde_PivotPointDescription_t frontPivot;

	uint8_t rearPivot_is_exsit;
	Mde_PivotPointDescription_t rearPivot;

	uint8_t rearWheelOffset_is_exsit;
	/*
	 *  desc: the effective center-line of the wheel set
	 *	range: -2048..2047
	 */	
	int16_t rearWheelOffset;

	/*
	 *	range: -2048..2047
	 */
	int16_t positionOffset_x;
		
	/*
	 *	range: -2048..2047
	 */
	int16_t	positionOffset_y;


	uint8_t elevationOffset_is_exsit;
	/*
	 *  desc: value  63 to be used for  63 or greater
	 *  		value -63 to be used for -63 or greater
	 *  		value -64 to be unavailable
	 *	range: -64..63
	 *  units: 10 cm
	 */	
	int8_t elevationOffset;

	/*
	 *	range: 0..23
	 *		crumbData is optional value
	 *		when crumbData_size = 0 stand for have not crumbData
	 */
	uint8_t crumbData_size;
	Mde_TrailerHistoryPoint_t crumbData[23];
}Mde_TrailerUnitDescription_t;


typedef struct Mde_TrailerData
{	
	/*
	 *	range: 0..31
	 */
	uint8_t sspRights;

	/*
	 *	range: -1024..1023
	 *  unit:  1cm
	 */	
	int16_t pivotOffset;

	/*
	 *	range: 0..28800
	 *  LSB: 0.0125 degrees
	 */
	uint16_t pivotAngle;
	
	/*
	 *	true: The machine can rotate around the pivot connection point
	 */
	bool pivots;
	
	/*
	 *	desc: units size.
	 *  range: 1..8
	 */	
	uint8_t units_size;
	
	Mde_TrailerUnitDescription_t units[8];
}Mde_TrailerData_t;


typedef struct Mde_Special_Vehicle_Ext
{
	uint8_t vehicleAlerts_is_exsit;
	Mde_EmergencyDetails_t vehicleAlerts;

	uint8_t description_is_exsit;
	Mde_Event_Description_t description;

	uint8_t trailers_is_exsit;
	Mde_TrailerData_t trailers;
}Mde_Special_Vehicle_Ext_t;

typedef struct Mde_VehicleData
{
	uint8_t height_is_exsit;
	/*
	 *  range: 0..127
	 *  LSB: 5cm
	 */
	uint8_t height;

	uint8_t bumpers_is_exsit;
	/*
	 *  range: 0..127
	 *  LSB: 0.01 meters from ground surface
	 */
	uint8_t bumpers_front;
	/*
	 *  range: 0..127
	 *  LSB: 0.01 meters from ground surface
	 */
	uint8_t bumpers_rear;

	uint8_t mass_is_exsit;
	/*
	 *  range: 0..255
	 *  	Values 000 to 080 in steps of 50kg
	 *  	Values 081 to 200 in steps of 500kg
	 *  	Values 201 to 253 in steps of 2000kg
	 *  	The Value 254 shall be used for weights above 170000 kg
	 *  	The Value 255 shall be used when the value is unknown or unavailable
	 *  unit: 500kg
	 */
	uint8_t mass;

	uint8_t trailerWeight_is_exsit;
	/*
	 *  range: 0..64255
	 *  unit: 2kg
	 */
	uint16_t trailerWeight;
}Mde_VehicleData_t;

typedef struct Mde_WeatherReport
{
	/*
	 *  range: 1..3
	 *		precip (1), noPrecip (2), error (3)
	 */
	uint8_t isRaining;

	uint8_t rainRate_is_exsit;
	/*
	 *  range: 0..65535
	 */
	uint16_t rainRate;

	uint8_t precipSituation_is_exsit;
	/*
	 *  range: 1..15
	 *  	other (1), 
	 *  	unknown (2), 
	 *  	noPrecipitation (3), 
	 *  	unidentifiedSlight (4), 
	 *  	unidentifiedModerate (5), 
	 *  	unidentifiedHeavy (6), 
	 *  	snowSlight (7), 
	 *  	snowModerate (8), 
	 *  	snowHeavy (9), 
	 *  	rainSlight (10), 
	 *  	rainModerate (11),  
	 *  	rainHeavy (12), 
	 *  	frozenPrecipitationSlight (13), 
	 *  	frozenPrecipitationModerate (14),
	 *  	frozenPrecipitationHeavy (15)
	 */
	uint8_t precipSituation;
		
	uint8_t solarRadiation_is_exsit;
	/*
	 *  range: 0..65535
	 */
	uint16_t solarRadiation;
		
	uint8_t friction_is_exsit;
	/*
	 *  range: 0..101
	 */
	uint8_t friction;
	
	uint8_t roadFriction_is_exsit;
	/*
	 *  range: 0..50
	 *  	where 0 = 0.00 micro (frictionless), also used when data is unavailable 
	 *  	and  50 = 1.00 micro, in steps of 0.02
	 */
	uint8_t roadFriction;
}Mde_WeatherReport_t;

typedef struct Mde_WiperSet
{	
	/*
	 *  range:0..6
	 *  	 unavailable         (0), -- Not Equipped with wiper status or wiper status is unavailable
	 *  	 off                 (1),  
	 *  	 intermittent        (2), 
	 *  	 low                 (3),
	 *  	 high                (4),
	 *  	 washerInUse         (5), -- washing solution being used
	 *  	 automaticPresent    (6), -- Auto wiper equipped
	 */
	uint8_t statusFront;

	/*
	 *  range: 0..127
	 *  units: sweeps per minute
	 */
	uint8_t rateFront;


	uint8_t statusRear_is_exsit;
	/*
	 *  range:0..6
	 *  	 unavailable         (0), -- Not Equipped with wiper status or wiper status is unavailable
	 *  	 off                 (1),  
	 *  	 intermittent        (2), 
	 *  	 low                 (3),
	 *  	 high                (4),
	 *  	 washerInUse         (5), -- washing solution being used
	 *  	 automaticPresent    (6), -- Auto wiper equipped
	 */
	uint8_t	statusRear;

	uint8_t rateRear_is_exsit;
	/*
	 *  range: 0..127
	 *  units: sweeps per minute
	 */
	uint8_t rateRear;
}Mde_WiperSet_t;

typedef struct Mde_WeatherProbe
{
	uint8_t airTemp_is_exsit;
	/*
	 *  range:0..191
	 *  	The value 191 shall indicate an unknown value
	 */
	uint8_t airTemp;

	uint8_t airPressure_is_exsit;
	/*
	 *  range:0..255
	 */
	uint8_t airPressure;


	uint8_t rainRates_is_exsit;
	Mde_WiperSet_t rainRates;	
}Mde_WeatherProbe_t;

typedef struct Mde_ObstacleDetection
{
	/*
	 *  range: 0..32767
	 *  LSB: meters
	 */
	uint16_t obDist;

	/*
	 *  range: 0..28800
	 *  LSB: 0.0125 degrees
	 */
	uint16_t obDirect;

	uint8_t description_is_exsit;
	/*
	 *  range: 523..541
	 */
	uint16_t description;

	uint8_t locationDetails_is_exsit;
	/*
	 *  range: 7937..8005
	 */
	uint16_t locationDetails;

	Mde_DDateTime_t dateTime;

	uint8_t vertEvent_is_exsit;
	/*
	 * 	desc:  Any wheels which have exceeded the acceleration point
	 * 		notEquipped (0), -- Not equipped or off
	 * 		leftFront   (1), -- Left Front Event
	 * 		leftRear    (2), -- Left Rear Event
	 * 		rightFront  (3), -- Right Front Event
	 * 		rightRear   (4)  -- Right Rear Event
	 *  range: 0..1
	 */
	uint8_t vertEvent[5];
}Mde_ObstacleDetection_t;

typedef struct Mde_DisabledVehicle
{
	/*
	 *  range: 523..541
	 *  	Codes 532 to 541, as taken from J2540:
	 *  	 -- Disabled, etc.
	 *  		stalled-vehicle (532),
	 *  	  	abandoned-vehicle (533),
	 *  	  	disabled-vehicle (534),
	 *  	  	disabled-truck (535),
	 *  	  	disabled-semi-trailer (536), -^- Alt: disabled
	 *  	 -- tractor-trailer
	 *  	  	disabled-bus (537),
	 *  	   	disabled-train (538),
	 *  	   	vehicle-spun-out (539),
	 *  	   	vehicle-on-fire (540),
	 *  	   	vehicle-in-water (541),
	 */
	uint16_t statusDetails;

	uint8_t locationDetails_is_exsit;
	/*
	 *  range: 7937..8005
	 */
	uint16_t locationDetails;
}Mde_DisabledVehicle_t;

typedef struct Mde_RTCMheader
{
	/*
	 *  desc:  
	 *  	unavailable               (0), -- Not Equipped or unavailable
	 *  	isHealthy                 (1),
	 *  	isMonitored               (2),
	 *  	baseStationType           (3), -- Set to zero if a moving base station,
	 *  	                          -- or if a rover device (an OBU),
	 *  	                          -- set to one if it is a fixed base station 
	 *  	aPDOPofUnder5             (4), -- A dilution of precision greater than 5
	 *  	inViewOfUnder5            (5), -- Less than 5 satellites in view
	 *  	localCorrectionsPresent   (6), -- DGPS type corrections used
	 *  	networkCorrectionsPresent (7)  -- RTK type corrections used
	 *  range: 0..1
	 */
	uint8_t status[8];
	
	/*
	 *  range: -2048..2047
	 *  unit: 0.01m
	 */
	int16_t antOffsetX;
	/*
	 *  range: -256..255
	 *  unit: 0.01m
	 */	
	int16_t antOffsetY;
	/*
	 *  range: -512..511
	 *  unit: 0.01m
	 */	
 	int16_t antOffsetZ;
}Mde_RTCMheader_t;

typedef struct Mde_RTCMPackage
{
	uint8_t rtcmHeader_is_exsit;
	Mde_RTCMheader_t rtcmHeader;

	/*
	 *  desc: msgs[1023] size.
	 *  range: 1..5
	 */
	uint8_t msgs_size;
	/*
	 *  element: msgs[1023]
	 */
	uint8_t msgs[5][1023];
}Mde_RTCMPackage_t;

typedef struct Mde_Sup_Veh_Ext
{
	uint8_t classification_is_exsit;
	/*
	 *  range: 0..255
	 */
	uint8_t classification;

	uint8_t classDetails_is_exsit;
	Mde_VehicleClassification_t classDetails;

	uint8_t vehicleData_is_exsit;
	Mde_VehicleData_t vehicleData;

	uint8_t weatherReport_is_exsit;
	Mde_WeatherReport_t weatherReport;

	uint8_t weatherProbe_is_exsit;
	Mde_WeatherProbe_t weatherProbe;

	uint8_t obstacle_is_exsit;
	Mde_ObstacleDetection_t obstacle;

	uint8_t status_is_exsit;
	Mde_DisabledVehicle_t status;

	/*
	 *  desc: speedReports size.
	 *  	speedReports is optional value. speedReports_size = 0 stand for have no speedReports
	 *  range: 0..20
	 */
	uint8_t speedReports_size;
	/*
	 *  range: 0..31
	 *  unit: 1.00 m/s
	 *  	The value 30 shall be used for speeds of 30 m/s or greater (67.1 mph)
	 *  	The value 31 shall indicate that the speed is unavailable
	 */
	uint8_t speedReports[20];

	uint8_t theRTCM_is_exsit;
	Mde_RTCMPackage_t theRTCM;

	//regional is OPTIONAL value
	//I have no idea to fill it, so default null
}Mde_Sup_Veh_Ext_t;













typedef enum 
{
    Mde_TimeConfidence_unavailable = 0, /* Not Equipped or unavailable */
    Mde_time_100_000 = 1, /* Better than 100 Seconds */
    Mde_time_050_000 = 2, /* Better than 50 Seconds */
    Mde_time_020_000 = 3, /* Better than 20 Seconds */
    Mde_time_010_000 = 4, /* Better than 10 Seconds */
    Mde_time_002_000 = 5, /* Better than 2 Seconds */
    Mde_time_001_000 = 6, /* Better than 1 Second */
    Mde_time_000_500 = 7, /* Better than 0.5 Seconds */
    Mde_time_000_200 = 8, /* Better than 0.2 Seconds */
    Mde_time_000_100 = 9, /* Better than 0.1 Seconds */
    Mde_time_000_050 = 10, /* Better than 0.05 Seconds */
    Mde_time_000_020 = 11, /* Better than 0.02 Seconds */
    Mde_time_000_010 = 12, /* Better than 0.01 Seconds */
    Mde_time_000_005 = 13, /* Better than 0.005 Seconds */
    Mde_time_000_002 = 14, /* Better than 0.002 Seconds */
    Mde_time_000_001 = 15, /* Better than 0.001 Seconds */
		/* Better than one millisecond */
    Mde_time_000_000_5 = 16, /* Better than 0.000,5 Seconds */
    Mde_time_000_000_2 = 17, /* Better than 0.000,2 Seconds */
    Mde_time_000_000_1 = 18, /* Better than 0.000,1 Seconds */
    Mde_time_000_000_05 = 19, /* Better than 0.000,05 Seconds */
    Mde_time_000_000_02 = 20, /* Better than 0.000,02 Seconds */
    Mde_time_000_000_01 = 21, /* Better than 0.000,01 Seconds */
    Mde_time_000_000_005 = 22, /* Better than 0.000,005 Seconds */
    Mde_time_000_000_002 = 23, /* Better than 0.000,002 Seconds */
    Mde_time_000_000_001 = 24, /* Better than 0.000,001 Seconds */
		/* Better than one micro second */
    Mde_time_000_000_000_5 = 25, /* Better than 0.000,000,5 Seconds */
    Mde_time_000_000_000_2 = 26, /* Better than 0.000,000,2 Seconds */
    Mde_time_000_000_000_1 = 27, /* Better than 0.000,000,1 Seconds */
    Mde_time_000_000_000_05 = 28, /* Better than 0.000,000,05 Seconds */
    Mde_time_000_000_000_02 = 29, /* Better than 0.000,000,02 Seconds */
    Mde_time_000_000_000_01 = 30, /* Better than 0.000,000,01 Seconds */
    Mde_time_000_000_000_005 = 31, /* Better than 0.000,000,005 Seconds */
    Mde_time_000_000_000_002 = 32, /* Better than 0.000,000,002 Seconds */
    Mde_time_000_000_000_001 = 33, /* Better than 0.000,000,001 Seconds */
		/* Better than one nano second */
    Mde_time_000_000_000_000_5 = 34, /* Better than 0.000,000,000,5 Seconds */
    Mde_time_000_000_000_000_2 = 35, /* Better than 0.000,000,000,2 Seconds */
    Mde_time_000_000_000_000_1 = 36, /* Better than 0.000,000,000,1 Seconds */
    Mde_time_000_000_000_000_05 = 37, /* Better than 0.000,000,000,05 Seconds */
    Mde_time_000_000_000_000_02 = 38, /* Better than 0.000,000,000,02 Seconds */
    Mde_time_000_000_000_000_01 = 39 /* Better than 0.000,000,000,01 Seconds */
} Mde_time_confidence;

typedef enum 
{
    Mde_notInUseOrNotEquipped = 0,
    Mde_emergency = 1, /* active service call at emergency level */
    Mde_nonEmergency = 2, /* also used when returning from service call */
    Mde_pursuit = 3, /* sender driving may be erratic */
    Mde_stationary = 4, /* sender is not moving, stopped along roadside */
    Mde_slowMoving = 5, /* such a litter trucks, etc. */
    Mde_stopAndGoMovement = 6 /* such as school bus or garbage truck */
} Mde_response_type;

typedef enum 
{
    Mde_SirenInUse_unavailable = 0, /* Not Equipped or unavailable */
    Mde_SirenInUse_notInUse = 1,
    Mde_SirenInUse_inUse = 2,
    Mde_SirenInUse_reserved = 3 /* for future use */
} Mde_siren_inuse;

typedef enum 
{
    Mde_LightbarInUse_unavailable = 0, /* Not Equipped or unavailable */
    Mde_LightbarInUse_notInUse = 1, /* none active */
    Mde_LightbarInUse_inUse = 2,
    Mde_yellowCautionLights = 3,
    Mde_schooldBusLights = 4,
    Mde_arrowSignsActive = 5,
    Mde_slowMovingVehicle = 6,
    Mde_freqStops = 7
} Mde_lightbar_inuse;

/* imports and exports */
	/* type assignments	*/
typedef struct Mde_vehicle_emergency_extensions {
    Mde_response_type    response_type;  /* optional; set in bit_mask
                                    * responseType_present if present */
    Mde_siren_inuse      siren_use;  /* optional; set in bit_mask sirenUse_present if
                                * present */
    Mde_lightbar_inuse   lights_use;  /* optional; set in bit_mask lightsUse_present
                                 * if present */
} Mde_vehicle_emergency_extensions_t;


typedef struct v2x_msg_bsm
{
    /*
     *  element: MsgCount 
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    uint8_t                msg_count;

    /*
     *  element: TemporaryID 
     *  desc: This is the 8 octet random device identifier, called the TemporaryID.
     *  size: 4 byte
     */    
    char                    temp_id[4];

    /*
     *  element: DSecond 
     *  desc: Time, unit:ms
     *  range: 0..65535
     */
    uint16_t                sec_mark;

    /*
     *  element: Latitude 
     *  desc: latitude
     *  range: -900000000...900000001
     */
	double 				latitude;

    /*
     *  element: Longitude 
     *  desc: longitude
     *  range: -1799999999...1800000001
     */
	double				longitude;
	
    /*
     *  element: Elevation 
     *  desc: elevation,unit:0.1m
     *  range: -4096…61439
     */
	double				elevation;

	Mde_positional_accuracy_t pos_accuracy;

    /*
     *  element: TransmissionState 
     *  desc: The element is used to provide the current state of the vehicle transmission.
     *  range: 0...7
     *  neutral (0), -- Neutral
     *  park (1), -- Park
     *  forwardGears (2), -- Forward gears
     *  reverseGears (3), -- Reverse gears
     *  reserved1 (4),
     *  reserved2 (5),
     *  reserved3 (6),
     *  unavailable (7) -- not-equipped or unavailable value
     */      
    uint32_t                transmission_state;

    /*
     *  element: Speed 
     *  desc: The vehicle speed expressed in unsigned.
     *  range: 0…8191
     *  lsb: 0.02 m/s
     */ 
    uint16_t                  speed;

    /*
     *  element: Heading 
     *  desc: The element provides the current heading of the sending device, expressed in unsigned units of
     *        0.0125 degrees from North such that 28799 such degrees represent 359.9875 degrees. 
     *  range: 0…28800
     *  lsb: 0.0125 deg
     */ 
    uint16_t                  heading;
	
    /*
     *  element: SteeringWheelAngle 
     *  desc: The angle of the driver’s steering wheel, expressed in a signed (to the right being positive) value with LSB 
     *        units of 1.5 degrees.Positive on the right
     *  range: -126..127
     *  lsb: units of 1.5 degrees
     */ 
    int8_t                  angle;


    /*
     *  element: Long Acceleration
     *  desc: The element represents the signed acceleration of the vehicle along the Vehicle Longitudinal axis in units of 
     *        0.01 meters per second squared.
     *  range: -2000..2001.Forward is positive
     *  lsb: 0.01 m/s^2
     */  
    int16_t                  long_accel;

    /*
     *  element: Lat Acceleration
     *  desc: The element represents the signed acceleration of the vehicle along the Vehicle Lateral axis in units of 
     *        0.01 meters per second squared.
     *  range: -2000..2001.Positive to the right
     *  lsb: 0.01 m/s^2
     */ 
    int16_t                  lat_accel;

    /*
     *  element: VerticalAcceleration
     *  desc: The element representing the signed vertical acceleration of the vehicle along the vertical axis in units of 
     *        0.02 G (where 9.80665 meters per second squared is one G, i.e., 0.02 G = 0.1962 meters per second squared).
     *  range: -127..127.Up is positive
     *  lsb: 0.02 G
     */ 
    int8_t                  vert_accel;

    /*
     *  element: YawRate
     *  desc: The element provides the Yaw Rate of the vehicle, a signed value (to the right being positive) expressed in 
     *        0.01 degrees per second. 
     *  range: -32767..32767. Positive clockwise
     *  lsb: units of 0.01 degrees per second (signed)
     */ 
    int16_t                  yaw_rate;
    

    /*
     *  element: BrakeAppliedStatus[5]
     *  desc: The element indicates independently for each of four wheels whether braking is currently active.
     *  	unavailable (BrakeAppliedStatus[0]), -- When set, the brake applied status is unavailable
     *  	leftFront (BrakeAppliedStatus[1]), -- Left Front Active
     *  	leftRear (BrakeAppliedStatus[2]), -- Left Rear Active
     *  	rightFront(BrakeAppliedStatus[3]), -- Right Front Active
     *  	rightRear(BrakeAppliedStatus[4]), -- Right Rear Active
     *  range: 0...1
     */
    uint8_t                 wheel_brakes[BRAKE_STATUS_NUM];

    /*
     *  element: TractionControlStatus
     *  desc: The element reflects the status of the vehicle traction control system.
     *  range: 0...3
     *  unavailable (0), -- Not Equipped with traction control or traction control status is unavailable
     *  off (1), -- traction control is Off
     *  on (2), -- traction control is On (but not Engaged)
     *  engaged(3), -- traction control is Engaged
     */     
    uint8_t                 traction;

    /*
     *  element: AntiLockBrakeStatus
     *  desc: The element reflects the status of the vehicle ABS.
     *  range: 0...3
     *  unavailable (0), -- Vehicle Not Equipped with ABS Brakes or ABS Brakes status is unavailable
     *  off (1), -- Vehicle's ABS are Off
     *  on (2), -- Vehicle's ABS are On ( but not Engaged )
     *  engaged(3), -- Vehicle's ABS control is Engaged on any wheel
     */  
    uint8_t                     abs;

    /*
     *  element: StabilityControlStatus
     *  desc: The element reflects the current state of the stability control system.
     *  range: 0...3
     *  unavailable (0), -- Not Equipped with SC or SC status is unavailable
     *  off (1), -- Off
     *  on (2), -- On or active (but not engaged)
     *  engaged(3), -- stability control is Engaged
     */  
    uint8_t                     scs;

    /*
     *  element: BrakeBoostApplied
     *  desc: The element reflects the current state of the brake boost system.
     *  range: 0...2
     *  unavailable (0), --Vehicle not equipped with brake boost or brake boost data is unavailable
     *  off (1), -- Vehicle's brake boost is off
     *  on (2), -- Vehicle's brake boost is on (applied)
     */
    uint8_t                brake_boost;

    /*
     *  element: AuxiliaryBrakeStatus
     *  desc: The element reflects the status of the auxiliary brakes of the vehicle.
     *  range: 0...3
     *  unavailable (0), -- Vehicle Not Equipped with Aux Brakes or Aux Brakes status is unavailable
     *  off (1), -- Vehicle's Aux Brakes are Off
     *  on (2), -- Vehicle's Aux Brakes are On ( Engaged )
     *  reserved (3), 
     */
    uint8_t                aux_brakes;
	
    /*
     *  element: VehicleWidth 
     *  desc: The width of the vehicle expressed in centimeters, unsigned. The width shall be the widest point of the vehicle 
     *        with all factory installed equipment. The value zero shall be sent when data is unavailable.
     *  range: 0..1023
     *  lsb: units are 1 cm with a range of >10 meters
     */
    uint16_t                width;

    /*
     *  element: VehicleLength 
     *  desc: The length of the vehicle measured from the edge of the front bumper to the edge of the rear bumper expressed in
     *        centimeters, unsigned. It should be noted that this value is often combined with a vehicle width value to form a 
     *        data frame.The value zero shall be sent when data is unavailable.
     *  range: 0.. 4095
     *  lsb: units of 1 cm with a range of >40 meters
     */ 
    uint16_t                length;


	// Part II Content
	
    /*
     *  element: Part II Content 
     *  desc: In a given message there may be multiple extensions present 
     *  		but at most one instance of each extension type.
     *  range: 0 <= (veh_safety_ext_count + spe_veh_ext_count + sup_veh_ext_count) <= 8
     *  lsb: units of 1 cm with a range of >40 meters
     */
	uint8_t 				veh_safety_ext_count;
	Mde_Vehicle_Safety_Ext_t veh_safe_ext[8];
	
	uint8_t 				spe_veh_ext_count;
	Mde_Special_Vehicle_Ext_t spe_veh_ext[8];

	uint8_t 				sup_veh_ext_count;
	Mde_Sup_Veh_Ext_t		sup_veh_ext[8];

	// regional is optional value, I have no idea what it is. So default null
} v2x_msg_bsm_t;

#endif
