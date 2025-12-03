#ifndef __V2X_MSG_PVD_H
#define __V2X_MSG_PVD_H


/*************************************************************************/
/**************************** ProbeVehicleData_t **************************/
/*************************************************************************/




/* Dependencies */
typedef enum MDE_VehicleIdent_vehicleClass_PR {
	MDE_VehicleIdent_vehicleClass_PR_NOTHING,	/* No components present */
	MDE_VehicleIdent_vehicleClass_PR_vGroup,
	MDE_VehicleIdent_vehicleClass_PR_rGroup,
	MDE_VehicleIdent_vehicleClass_PR_rEquip
} MDE_VehicleIdent_vehicleClass_PR;

/* Dependencies */
typedef enum MDE_PVD_VehicleGroupAffected {
	MDE_PVD_MDE_PVD_VehicleGroupAffected_all_vehicles	= 9217,
	MDE_PVD_VehicleGroupAffected_bicycles	= 9218,
	MDE_PVD_VehicleGroupAffected_motorcycles	= 9219,
	MDE_PVD_VehicleGroupAffected_cars	= 9220,
	MDE_PVD_VehicleGroupAffected_light_vehicles	= 9221,
	MDE_PVD_VehicleGroupAffected_cars_and_light_vehicles	= 9222,
	MDE_PVD_VehicleGroupAffected_cars_with_trailers	= 9223,
	MDE_PVD_VehicleGroupAffected_cars_with_recreational_trailers	= 9224,
	MDE_PVD_VehicleGroupAffected_vehicles_with_trailers	= 9225,
	MDE_PVD_VehicleGroupAffected_heavy_vehicles	= 9226,
	MDE_PVD_VehicleGroupAffected_trucks	= 9227,
	MDE_PVD_VehicleGroupAffected_buses	= 9228,
	MDE_PVD_VehicleGroupAffected_articulated_buses	= 9229,
	MDE_PVD_VehicleGroupAffected_school_buses	= 9230,
	MDE_PVD_VehicleGroupAffected_vehicles_with_semi_trailers	= 9231,
	MDE_PVD_VehicleGroupAffected_vehicles_with_double_trailers	= 9232,
	MDE_PVD_VehicleGroupAffected_high_profile_vehicles	= 9233,
	MDE_PVD_VehicleGroupAffected_wide_vehicles	= 9234,
	MDE_PVD_VehicleGroupAffected_long_vehicles	= 9235,
	MDE_PVD_VehicleGroupAffected_hazardous_loads	= 9236,
	MDE_PVD_VehicleGroupAffected_exceptional_loads	= 9237,
	MDE_PVD_VehicleGroupAffected_abnormal_loads	= 9238,
	MDE_PVD_VehicleGroupAffected_convoys	= 9239,
	MDE_PVD_VehicleGroupAffected_maintenance_vehicles	= 9240,
	MDE_PVD_VehicleGroupAffected_delivery_vehicles	= 9241,
	MDE_PVD_VehicleGroupAffected_vehicles_with_even_numbered_license_plates	= 9242,
	MDE_PVD_VehicleGroupAffected_vehicles_with_odd_numbered_license_plates	= 9243,
	MDE_PVD_VehicleGroupAffected_vehicles_with_parking_permits	= 9244,
	MDE_PVD_VehicleGroupAffected_vehicles_with_catalytic_converters	= 9245,
	MDE_PVD_VehicleGroupAffected_vehicles_without_catalytic_converters	= 9246,
	MDE_PVD_VehicleGroupAffected_gas_powered_vehicles	= 9247,
	MDE_PVD_VehicleGroupAffected_diesel_powered_vehicles	= 9248,
	MDE_PVD_VehicleGroupAffected_lPG_vehicles	= 9249,
	MDE_PVD_VehicleGroupAffected_military_convoys	= 9250,
	MDE_PVD_VehicleGroupAffected_military_vehicles	= 9251
	/*
	 * Enumeration is extensible
	 */
} MDE_PVD_VehicleGroupAffected;

/* Dependencies */
typedef enum mde_pvd_ResponderGroupAffected {
	MDE_PVD_ResponderGroupAffected_emergency_vehicle_units	= 9729,
	MDE_PVD_ResponderGroupAffected_federal_law_enforcement_units	= 9730,
	MDE_PVD_ResponderGroupAffected_state_police_units	= 9731,
	MDE_PVD_ResponderGroupAffected_county_police_units	= 9732,
	MDE_PVD_ResponderGroupAffected_local_police_units	= 9733,
	MDE_PVD_ResponderGroupAffected_ambulance_units	= 9734,
	MDE_PVD_ResponderGroupAffected_rescue_units	= 9735,
	MDE_PVD_ResponderGroupAffected_fire_units	= 9736,
	MDE_PVD_ResponderGroupAffected_hAZMAT_units	= 9737,
	MDE_PVD_ResponderGroupAffected_light_tow_unit	= 9738,
	MDE_PVD_ResponderGroupAffected_heavy_tow_unit	= 9739,
	MDE_PVD_ResponderGroupAffected_freeway_service_patrols	= 9740,
	MDE_PVD_ResponderGroupAffected_transportation_response_units	= 9741,
	MDE_PVD_ResponderGroupAffected_private_contractor_response_units	= 9742
	/*
	 * Enumeration is extensible
	 */
} MDE_PVD_ResponderGroupAffected;

/* Dependencies */
typedef enum mde_pvd_IncidentResponseEquipment {
	MDE_PVD_IncidentResponseEquipment_ground_fire_suppression	= 9985,
	MDE_PVD_IncidentResponseEquipment_heavy_ground_equipment	= 9986,
	MDE_PVD_IncidentResponseEquipment_aircraft	= 9988,
	MDE_PVD_IncidentResponseEquipment_marine_equipment	= 9989,
	MDE_PVD_IncidentResponseEquipment_support_equipment	= 9990,
	MDE_PVD_IncidentResponseEquipment_medical_rescue_unit	= 9991,
	MDE_PVD_IncidentResponseEquipment_other	= 9993,
	MDE_PVD_IncidentResponseEquipment_ground_fire_suppression_other	= 9994,
	MDE_PVD_IncidentResponseEquipment_engine	= 9995,
	MDE_PVD_IncidentResponseEquipment_truck_or_aerial	= 9996,
	MDE_PVD_IncidentResponseEquipment_quint	= 9997,
	MDE_PVD_IncidentResponseEquipment_tanker_pumper_combination	= 9998,
	MDE_PVD_IncidentResponseEquipment_brush_truck	= 10000,
	MDE_PVD_IncidentResponseEquipment_aircraft_rescue_firefighting	= 10001,
	MDE_PVD_IncidentResponseEquipment_heavy_ground_equipment_other	= 10004,
	MDE_PVD_IncidentResponseEquipment_dozer_or_plow	= 10005,
	MDE_PVD_IncidentResponseEquipment_tractor	= 10006,
	MDE_PVD_IncidentResponseEquipment_tanker_or_tender	= 10008,
	MDE_PVD_IncidentResponseEquipment_aircraft_other	= 10024,
	MDE_PVD_IncidentResponseEquipment_aircraft_fixed_wing_tanker	= 10025,
	MDE_PVD_IncidentResponseEquipment_helitanker	= 10026,
	MDE_PVD_IncidentResponseEquipment_helicopter	= 10027,
	MDE_PVD_IncidentResponseEquipment_marine_equipment_other	= 10034,
	MDE_PVD_IncidentResponseEquipment_fire_boat_with_pump	= 10035,
	MDE_PVD_IncidentResponseEquipment_boat_no_pump	= 10036,
	MDE_PVD_IncidentResponseEquipment_support_apparatus_other	= 10044,
	MDE_PVD_IncidentResponseEquipment_breathing_apparatus_support	= 10045,
	MDE_PVD_IncidentResponseEquipment_light_and_air_unit	= 10046,
	MDE_PVD_IncidentResponseEquipment_medical_rescue_unit_other	= 10054,
	MDE_PVD_IncidentResponseEquipment_rescue_unit	= 10055,
	MDE_PVD_IncidentResponseEquipment_urban_search_rescue_unit	= 10056,
	MDE_PVD_IncidentResponseEquipment_high_angle_rescue	= 10057,
	MDE_PVD_IncidentResponseEquipment_crash_fire_rescue	= 10058,
	MDE_PVD_IncidentResponseEquipment_bLS_unit	= 10059,
	MDE_PVD_IncidentResponseEquipment_aLS_unit	= 10060,
	MDE_PVD_IncidentResponseEquipment_mobile_command_post	= 10075,
	MDE_PVD_IncidentResponseEquipment_chief_officer_car	= 10076,
	MDE_PVD_IncidentResponseEquipment_hAZMAT_unit	= 10077,
	MDE_PVD_IncidentResponseEquipment_type_i_hand_crew	= 10078,
	MDE_PVD_IncidentResponseEquipment_type_ii_hand_crew	= 10079,
	MDE_PVD_IncidentResponseEquipment_privately_owned_vehicle	= 10083,
	MDE_PVD_IncidentResponseEquipment_other_apparatus_resource	= 10084,
	MDE_PVD_IncidentResponseEquipment_ambulance	= 10085,
	MDE_PVD_IncidentResponseEquipment_bomb_squad_van	= 10086,
	MDE_PVD_IncidentResponseEquipment_combine_harvester	= 10087,
	MDE_PVD_IncidentResponseEquipment_construction_vehicle	= 10088,
	MDE_PVD_IncidentResponseEquipment_farm_tractor	= 10089,
	MDE_PVD_IncidentResponseEquipment_grass_cutting_machines	= 10090,
	MDE_PVD_IncidentResponseEquipment_hAZMAT_containment_tow	= 10091,
	MDE_PVD_IncidentResponseEquipment_heavy_tow	= 10092,
	MDE_PVD_IncidentResponseEquipment_light_tow	= 10094,
	MDE_PVD_IncidentResponseEquipment_flatbed_tow	= 10114,
	MDE_PVD_IncidentResponseEquipment_hedge_cutting_machines	= 10093,
	MDE_PVD_IncidentResponseEquipment_mobile_crane	= 10095,
	MDE_PVD_IncidentResponseEquipment_refuse_collection_vehicle	= 10096,
	MDE_PVD_IncidentResponseEquipment_resurfacing_vehicle	= 10097,
	MDE_PVD_IncidentResponseEquipment_road_sweeper	= 10098,
	MDE_PVD_IncidentResponseEquipment_roadside_litter_collection_crews	= 10099,
	MDE_PVD_IncidentResponseEquipment_salvage_vehicle	= 10100,
	MDE_PVD_IncidentResponseEquipment_sand_truck	= 10101,
	MDE_PVD_IncidentResponseEquipment_snowplow	= 10102,
	MDE_PVD_IncidentResponseEquipment_steam_roller	= 10103,
	MDE_PVD_IncidentResponseEquipment_swat_team_van	= 10104,
	MDE_PVD_IncidentResponseEquipment_track_laying_vehicle	= 10105,
	MDE_PVD_IncidentResponseEquipment_unknown_vehicle	= 10106,
	MDE_PVD_IncidentResponseEquipment_white_lining_vehicle	= 10107,
	MDE_PVD_IncidentResponseEquipment_dump_truck	= 10108,
	MDE_PVD_IncidentResponseEquipment_supervisor_vehicle	= 10109,
	MDE_PVD_IncidentResponseEquipment_snow_blower	= 10110,
	MDE_PVD_IncidentResponseEquipment_rotary_snow_blower	= 10111,
	MDE_PVD_IncidentResponseEquipment_road_grader	= 10112,
	MDE_PVD_IncidentResponseEquipment_steam_truck	= 10113
	/*
	 * Enumeration is extensible
	 */
} MDE_PVD_IncidentResponseEquipment;

typedef struct mde_vehicleClass {
		MDE_VehicleIdent_vehicleClass_PR present;
		union VehicleIdent_vehicleClass_u {
			long	 vGroup;
			long	 rGroup;
			long	 rEquip;
		} choice;
		
} MDE_vehicleClass_t;

/* Dependencies */
typedef enum mde_BasicVehicleRole {
	MDE_BasicVehicleRole_basicVehicle	= 0,
	MDE_BasicVehicleRole_publicTransport	= 1,
	MDE_BasicVehicleRole_specialTransport	= 2,
	MDE_BasicVehicleRole_dangerousGoods	= 3,
	MDE_BasicVehicleRole_roadWork	= 4,
	MDE_BasicVehicleRole_roadRescue	= 5,
	MDE_BasicVehicleRole_emergency	= 6,
	MDE_BasicVehicleRole_safetyCar	= 7,
	MDE_BasicVehicleRole_none_unknown	= 8,
	MDE_BasicVehicleRole_truck	= 9,
	MDE_BasicVehicleRole_motorcycle	= 10,
	MDE_BasicVehicleRole_roadSideSource	= 11,
	MDE_BasicVehicleRole_police	= 12,
	MDE_BasicVehicleRole_fire	= 13,
	MDE_BasicVehicleRole_ambulance	= 14,
	MDE_BasicVehicleRole_dot	= 15,
	MDE_BasicVehicleRole_transit	= 16,
	MDE_BasicVehicleRole_slowMoving	= 17,
	MDE_BasicVehicleRole_stopNgo	= 18,
	MDE_BasicVehicleRole_cyclist	= 19,
	MDE_BasicVehicleRole_pedestrian	= 20,
	MDE_BasicVehicleRole_nonMotorized	= 21,
	MDE_BasicVehicleRole_military	= 22
	/*
	 * Enumeration is extensible
	 */
} MDE_BasicVehicleRole;

/* Dependencies */
typedef enum mde_VehicleID_PR {
	MDE_VehicleID_PR_NOTHING,	/* No components present */
	MDE_VehicleID_PR_entityID,
	MDE_VehicleID_PR_stationID
} MDE_VehicleID_PR;

typedef struct mde_VehicleID {
		MDE_VehicleID_PR present;
		union MDE_VehicleID_u {
			char	         entityID[4];  // 4 byte
			unsigned long	 stationID;
		} choice;
		
} MDE_VehicleID;


typedef struct mde_VehicleIdent {
	    int    name_active;
		char   name[63];   // 1..63 byte
		int    vin_active;
		char   vin[17];    // 1..17 byte
		int    ownerCode_active;
		char   ownerCode[32];  // 1..32 byte
		int vehId_active;
		MDE_VehicleID vehId;
		int vehType_active;
		MDE_VehicleType vehType;
		MDE_vehicleClass_t vehClass;
		
} MDE_VehicleIdent;
#if 0
typedef struct mde_DDateTime {
	long	year	/* OPTIONAL */;
	long    month	/* OPTIONAL */;
	long    day	/* OPTIONAL */;
	long    hour	/* OPTIONAL */;
	long    minute	/* OPTIONAL */;
	long    second	/* OPTIONAL */;
	long    offset	/* OPTIONAL */;

} MDE_DDateTime_t;

/* Dependencies */
typedef enum mde_TransmissionState {
	MDE_TransmissionState_neutral	= 0,
	MDE_TransmissionState_park	= 1,
	MDE_TransmissionState_forwardGears	= 2,
	MDE_TransmissionState_reverseGears	= 3,
	MDE_TransmissionState_reserved1	= 4,
	MDE_TransmissionState_reserved2	= 5,
	MDE_TransmissionState_reserved3	= 6,
	MDE_TransmissionState_unavailable	= 7
} eMDE_TransmissionState;

typedef struct mde_TransmissionAndSpeed {
	eMDE_TransmissionState transmisson;
	long	 speed;

} MDE_TransmissionAndSpeed_t;

typedef struct mde_PositionConfidenceSet {
	Mde_PositionConfidence_e   pos;
	Mde_ElevationConfidence_e  elevation;
} MDE_PositionConfidenceSet_t;

typedef struct mde_FullPositionVector {
	MDE_DDateTime_t	utcTime	/* OPTIONAL */;
	double	 Long;
	double	 lat;
	double	 elevation	/* OPTIONAL */;
	double	 heading	/* OPTIONAL */;
	MDE_TransmissionAndSpeed_t	speedTrans	/* OPTIONAL */;
	Mde_positional_accuracy_t	posAccuracy	/* OPTIONAL */;
	MDE_TimeConfidence_e	timeConfidence	/* OPTIONAL */;
	MDE_PositionConfidenceSet_t	posConfidence	/* OPTIONAL */;
	Mde_SpeedandHeadingandThrottleConfidence_t    speedConfidence	/* OPTIONAL */;
	
} MDE_FullPositionVector_t;
#endif

/* Dependencies */
typedef enum mde_ExteriorLights {
	MDE_ExteriorLights_lowBeamHeadlightsOn	= 0,
	MDE_ExteriorLights_highBeamHeadlightsOn	= 1,
	MDE_ExteriorLights_leftTurnSignalOn	= 2,
	MDE_ExteriorLights_rightTurnSignalOn	= 3,
	MDE_ExteriorLights_hazardSignalOn	= 4,
	MDE_ExteriorLights_automaticLightControlOn	= 5,
	MDE_ExteriorLights_daytimeRunningLightsOn	= 6,
	MDE_ExteriorLights_fogLightOn	= 7,
	MDE_ExteriorLights_parkingLightsOn	= 8
} MDE_ExteriorLights_e;

typedef struct mde_VehicleSafetyExtensions {
	Mde_VehicleEventFlags_e	events	/* OPTIONAL */;
	struct PathHistory	*pathHistory	/* OPTIONAL */;
	struct PathPrediction	*pathPrediction	/* OPTIONAL */;
	MDE_ExteriorLights_e	lights	/* OPTIONAL */;
	
} MDE_VehicleSafetyExtensions_t;

typedef struct MDE_VehicleStatus {
#if 0
	ExteriorLights_t	*lights	/* OPTIONAL */;
	LightbarInUse_t	*lightBar	/* OPTIONAL */;
	struct WiperSet	*wipers	/* OPTIONAL */;
	struct BrakeSystemStatus	*brakeStatus	/* OPTIONAL */;
	BrakeAppliedPressure_t	*brakePressure	/* OPTIONAL */;
	CoefficientOfFriction_t	*roadFriction	/* OPTIONAL */;
	SunSensor_t	*sunData	/* OPTIONAL */;
	RainSensor_t	*rainData	/* OPTIONAL */;
	AmbientAirTemperature_t	*airTemp	/* OPTIONAL */;
	AmbientAirPressure_t	*airPres	/* OPTIONAL */;
	struct VehicleStatus__steering {
		SteeringWheelAngle_t	 angle;
		SteeringWheelAngleConfidence_t	*confidence	/* OPTIONAL */;
		SteeringWheelAngleRateOfChange_t	*rate	/* OPTIONAL */;
		DrivingWheelAngle_t	*wheels	/* OPTIONAL */;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *steering;
	struct VehicleStatus__accelSets {
		struct AccelerationSet4Way	*accel4way	/* OPTIONAL */;
		VerticalAccelerationThreshold_t	*vertAccelThres	/* OPTIONAL */;
		YawRateConfidence_t	*yawRateCon	/* OPTIONAL */;
		AccelerationConfidence_t	*hozAccelCon	/* OPTIONAL */;
		struct ConfidenceSet	*confidenceSet	/* OPTIONAL */;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *accelSets;
	struct VehicleStatus__object {
		ObstacleDistance_t	 obDist;
		V2X_Angle_t	 obDirect;
		DDateTime_t	 dateTime;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *object;
	struct FullPositionVector	*fullPos	/* OPTIONAL */;
	ThrottlePosition_t	*throttlePos	/* OPTIONAL */;
	struct SpeedandHeadingandThrottleConfidence	*speedHeadC	/* OPTIONAL */;
	SpeedConfidence_t	*speedC	/* OPTIONAL */;
	struct VehicleStatus__vehicleData {
		VehicleHeight_t	 height;
		BumperHeights_t	 bumpers;
		VehicleMass_t	 mass;
		TrailerWeight_t	 trailerWeight;
		VehicleType_t	 type;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *vehicleData;
	struct VehicleIdent	*vehicleIdent	/* OPTIONAL */;
	struct J1939data	*j1939data	/* OPTIONAL */;
	struct VehicleStatus__weatherReport {
		EssPrecipYesNo_t	 isRaining;
		EssPrecipRate_t	*rainRate	/* OPTIONAL */;
		EssPrecipSituation_t	*precipSituation	/* OPTIONAL */;
		EssSolarRadiation_t	*solarRadiation	/* OPTIONAL */;
		EssMobileFriction_t	*friction	/* OPTIONAL */;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *weatherReport;
	GNSSstatus_t	*gnssStatus	/* OPTIONAL */;
#endif	
} MDE_VehicleStatus_t;

typedef struct mde_pvd_Snapshot {
	Mde_FullPositionVector_t	thePosition;
	uint8_t                     safetyExt_exit;
	Mde_Vehicle_Safety_Ext_t	safetyExt	/* OPTIONAL */;
	uint8_t                 dataSet_exit;
	MDE_VehicleStatus_t	    dataSet	/* OPTIONAL */;
} MDEP_PVD_Snapshot_t;


typedef struct v2x_msg_pvd
{

    /*
     *  element: MinuteOfTheYear 
     *  desc: Time, unit:minute
     *  range: 0..527040
     */
    uint8_t                 time_stamp_exit;
    long                time_stamp;

	/*
     *  element: MinuteOfTheYear 
     *  desc: Time, unit:minute
     *  range: 0..32767
     */
    uint8_t                 segNum_exit;
    long                segNum;

	uint8_t                 probeID_exit;
	MDE_VehicleIdent        probeID;

	Mde_FullPositionVector_t startVector;

	Mde_VehicleClassification_t vehicleType;

	uint8_t                 snapshot_num;
	MDEP_PVD_Snapshot_t     snapshots[32];

} v2x_msg_pvd_t;

#endif
