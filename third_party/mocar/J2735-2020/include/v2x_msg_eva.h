#ifndef __V2X_MSG_EVA_H
#define __V2X_MSG_EVA_H


/*************************************************************************/
/**************************** EmergencyVehicleAlert_t **************************/
/*************************************************************************/

typedef enum Vehicle_Mass_level
{
	Vehicle_Mass_Level_1=4,   //4t 80*50kg  (0...80)
	Vehicle_Mass_Level_2=64,  //64t 80*50kg+(200-80)*500  (81...200)
	Vehicle_Mass_Level_3=172, //172t 80*50kg+(200-80)*500+(254-200)*2000 (201...254)
	Vehicle_Mass_Level_UNKNOW=-1,
}Vehicle_Mass_Level_t;


/* Dependencies */
typedef enum MDE_ResponseType {
	MDE_ResponseType_notInUseOrNotEquipped	= 0,
	MDE_ResponseType_emergency	= 1,
	MDE_ResponseType_nonEmergency	= 2,
	MDE_ResponseType_pursuit	= 3,
	MDE_ResponseType_stationary	= 4,
	MDE_ResponseType_slowMoving	= 5,
	MDE_ResponseType_stopAndGoMovement	= 6
	/*
	 * Enumeration is extensible
	 */
} MDE_ResponseType;


/* Dependencies */
typedef enum MDE_VehicleGroupAffected {
	MDE_VehicleGroupAffected_all_vehicles	= 9217,
	MDE_VehicleGroupAffected_bicycles	= 9218,
	MDE_VehicleGroupAffected_motorcycles	= 9219,
	MDE_VehicleGroupAffected_cars	= 9220,
	MDE_VehicleGroupAffected_light_vehicles	= 9221,
	MDE_VehicleGroupAffected_cars_and_light_vehicles	= 9222,
	MDE_VehicleGroupAffected_cars_with_trailers	= 9223,
	MDE_VehicleGroupAffected_cars_with_recreational_trailers	= 9224,
	MDE_VehicleGroupAffected_vehicles_with_trailers	= 9225,
	MDE_VehicleGroupAffected_heavy_vehicles	= 9226,
	MDE_VehicleGroupAffected_trucks	= 9227,
	MDE_VehicleGroupAffected_buses	= 9228,
	MDE_VehicleGroupAffected_articulated_buses	= 9229,
	MDE_VehicleGroupAffected_school_buses	= 9230,
	MDE_VehicleGroupAffected_vehicles_with_semi_trailers	= 9231,
	MDE_VehicleGroupAffected_vehicles_with_double_trailers	= 9232,
	MDE_VehicleGroupAffected_high_profile_vehicles	= 9233,
	MDE_VehicleGroupAffected_wide_vehicles	= 9234,
	MDE_VehicleGroupAffected_long_vehicles	= 9235,
	MDE_VehicleGroupAffected_hazardous_loads	= 9236,
	MDE_VehicleGroupAffected_exceptional_loads	= 9237,
	MDE_VehicleGroupAffected_abnormal_loads	= 9238,
	MDE_VehicleGroupAffected_convoys	= 9239,
	MDE_VehicleGroupAffected_maintenance_vehicles	= 9240,
	MDE_VehicleGroupAffected_delivery_vehicles	= 9241,
	MDE_VehicleGroupAffected_vehicles_with_even_numbered_license_plates	= 9242,
	MDE_VehicleGroupAffected_vehicles_with_odd_numbered_license_plates	= 9243,
	MDE_VehicleGroupAffected_vehicles_with_parking_permits	= 9244,
	MDE_VehicleGroupAffected_vehicles_with_catalytic_converters	= 9245,
	MDE_VehicleGroupAffected_vehicles_without_catalytic_converters	= 9246,
	MDE_VehicleGroupAffected_gas_powered_vehicles	= 9247,
	MDE_VehicleGroupAffected_diesel_powered_vehicles	= 9248,
	MDE_VehicleGroupAffected_lPG_vehicles	= 9249,
	MDE_VehicleGroupAffected_military_convoys	= 9250,
	MDE_VehicleGroupAffected_military_vehicles	= 9251
	/*
	 * Enumeration is extensible
	 */
} MDE_VehicleGroupAffected;

/* Dependencies */
typedef enum MDE_IncidentResponseEquipment {
	MDE_IncidentResponseEquipment_ground_fire_suppression	= 9985,
	MDE_IncidentResponseEquipment_heavy_ground_equipment	= 9986,
	MDE_IncidentResponseEquipment_aircraft	= 9988,
	MDE_IncidentResponseEquipment_marine_equipment	= 9989,
	MDE_IncidentResponseEquipment_support_equipment	= 9990,
	MDE_IncidentResponseEquipment_medical_rescue_unit	= 9991,
	MDE_IncidentResponseEquipment_other	= 9993,
	MDE_IncidentResponseEquipment_ground_fire_suppression_other	= 9994,
	MDE_IncidentResponseEquipment_engine	= 9995,
	MDE_IncidentResponseEquipment_truck_or_aerial	= 9996,
	MDE_IncidentResponseEquipment_quint	= 9997,
	MDE_IncidentResponseEquipment_tanker_pumper_combination	= 9998,
	MDE_IncidentResponseEquipment_brush_truck	= 10000,
	MDE_IncidentResponseEquipment_aircraft_rescue_firefighting	= 10001,
	MDE_IncidentResponseEquipment_heavy_ground_equipment_other	= 10004,
	MDE_IncidentResponseEquipment_dozer_or_plow	= 10005,
	MDE_IncidentResponseEquipment_tractor	= 10006,
	MDE_IncidentResponseEquipment_tanker_or_tender	= 10008,
	MDE_IncidentResponseEquipment_aircraft_other	= 10024,
	MDE_IncidentResponseEquipment_aircraft_fixed_wing_tanker	= 10025,
	MDE_IncidentResponseEquipment_helitanker	= 10026,
	MDE_IncidentResponseEquipment_helicopter	= 10027,
	MDE_IncidentResponseEquipment_marine_equipment_other	= 10034,
	MDE_IncidentResponseEquipment_fire_boat_with_pump	= 10035,
	MDE_IncidentResponseEquipment_boat_no_pump	= 10036,
	MDE_IncidentResponseEquipment_support_apparatus_other	= 10044,
	MDE_IncidentResponseEquipment_breathing_apparatus_support	= 10045,
	MDE_IncidentResponseEquipment_light_and_air_unit	= 10046,
	MDE_IncidentResponseEquipment_medical_rescue_unit_other	= 10054,
	MDE_IncidentResponseEquipment_rescue_unit	= 10055,
	MDE_IncidentResponseEquipment_urban_search_rescue_unit	= 10056,
	MDE_IncidentResponseEquipment_high_angle_rescue	= 10057,
	MDE_IncidentResponseEquipment_crash_fire_rescue	= 10058,
	MDE_IncidentResponseEquipment_bLS_unit	= 10059,
	MDE_IncidentResponseEquipment_aLS_unit	= 10060,
	MDE_IncidentResponseEquipment_mobile_command_post	= 10075,
	MDE_IncidentResponseEquipment_chief_officer_car	= 10076,
	MDE_IncidentResponseEquipment_hAZMAT_unit	= 10077,
	MDE_IncidentResponseEquipment_type_i_hand_crew	= 10078,
	MDE_IncidentResponseEquipment_type_ii_hand_crew	= 10079,
	MDE_IncidentResponseEquipment_privately_owned_vehicle	= 10083,
	MDE_IncidentResponseEquipment_other_apparatus_resource	= 10084,
	MDE_IncidentResponseEquipment_ambulance	= 10085,
	MDE_IncidentResponseEquipment_bomb_squad_van	= 10086,
	MDE_IncidentResponseEquipment_combine_harvester	= 10087,
	MDE_IncidentResponseEquipment_construction_vehicle	= 10088,
	MDE_IncidentResponseEquipment_farm_tractor	= 10089,
	MDE_IncidentResponseEquipment_grass_cutting_machines	= 10090,
	MDE_IncidentResponseEquipment_hAZMAT_containment_tow	= 10091,
	MDE_IncidentResponseEquipment_heavy_tow	= 10092,
	MDE_IncidentResponseEquipment_light_tow	= 10094,
	MDE_IncidentResponseEquipment_flatbed_tow	= 10114,
	MDE_IncidentResponseEquipment_hedge_cutting_machines	= 10093,
	MDE_IncidentResponseEquipment_mobile_crane	= 10095,
	MDE_IncidentResponseEquipment_refuse_collection_vehicle	= 10096,
	MDE_IncidentResponseEquipment_resurfacing_vehicle	= 10097,
	MDE_IncidentResponseEquipment_road_sweeper	= 10098,
	MDE_IncidentResponseEquipment_roadside_litter_collection_crews	= 10099,
	MDE_IncidentResponseEquipment_salvage_vehicle	= 10100,
	MDE_IncidentResponseEquipment_sand_truck	= 10101,
	MDE_IncidentResponseEquipment_snowplow	= 10102,
	MDE_IncidentResponseEquipment_steam_roller	= 10103,
	MDE_IncidentResponseEquipment_swat_team_van	= 10104,
	MDE_IncidentResponseEquipment_track_laying_vehicle	= 10105,
	MDE_IncidentResponseEquipment_unknown_vehicle	= 10106,
	MDE_IncidentResponseEquipment_white_lining_vehicle	= 10107,
	MDE_IncidentResponseEquipment_dump_truck	= 10108,
	MDE_IncidentResponseEquipment_supervisor_vehicle	= 10109,
	MDE_IncidentResponseEquipment_snow_blower	= 10110,
	MDE_IncidentResponseEquipment_rotary_snow_blower	= 10111,
	MDE_IncidentResponseEquipment_road_grader	= 10112,
	MDE_IncidentResponseEquipment_steam_truck	= 10113
	/*
	 * Enumeration is extensible
	 */
} MDE_IncidentResponseEquipment;

/* Dependencies */
typedef enum MDE_ResponderGroupAffected {
	MDE_ResponderGroupAffected_emergency_vehicle_units	= 9729,
	MDE_ResponderGroupAffected_federal_law_enforcement_units	= 9730,
	MDE_ResponderGroupAffected_state_police_units	= 9731,
	MDE_ResponderGroupAffected_county_police_units	= 9732,
	MDE_ResponderGroupAffected_local_police_units	= 9733,
	MDE_ResponderGroupAffected_ambulance_units	= 9734,
	MDE_ResponderGroupAffected_rescue_units	= 9735,
	MDE_ResponderGroupAffected_fire_units	= 9736,
	MDE_ResponderGroupAffected_hAZMAT_units	= 9737,
	MDE_ResponderGroupAffected_light_tow_unit	= 9738,
	MDE_ResponderGroupAffected_heavy_tow_unit	= 9739,
	MDE_ResponderGroupAffected_freeway_service_patrols	= 9740,
	MDE_ResponderGroupAffected_transportation_response_units	= 9741,
	MDE_ResponderGroupAffected_private_contractor_response_units	= 9742
	/*
	 * Enumeration is extensible
	 */
} MDE_ResponderGroupAffected;

//typedef long mde_description_t;

typedef struct Mde_RoadSideAlert {
	long	 msgCnt;
	uint8_t  timeStamp_exist;
	long	 timeStamp	/* OPTIONAL */;
	/*
	 *	element: ITIS.ITIScodes
	 *	range: 0..65535
	 */
	long	 typeEvent;
	uint8_t        description_num;
	/*
  	 *	element: ITIS.ITIScodes
     *  desc:  Up to eight ITIS code set entries to further describe the event, give advice, or any other ITIS codes
	 *	range: 0..65535
	 */
	long           description[8];
	uint8_t        priority_exist;
	unsigned char  priority	/* OPTIONAL */;
	uint8_t        heading_exist;
	unsigned char  heading[2]	/* OPTIONAL */;
	uint8_t        extent_exist;
	long	       extent	/* OPTIONAL */;
	uint8_t        position_exist;
	Mde_FullPositionVector_t position	/* OPTIONAL */;
	uint8_t        furtherInfoID_exist;
	unsigned char  furtherInfoID[2]	/* OPTIONAL */;

} MDE_RoadSideAlert_t;



/*
typedef struct Mde_PrivilegedEvents {
	long	         sspRights;
	unsigned char	 event;
} MDE_PrivilegedEvents_t;

typedef struct Mde_EmergencyDetails {
	long	 sspRights;
	long	 sirenUse;
	long	 lightsUse;
	long	 multi;
	uint8_t                 events_exist;
	MDE_PrivilegedEvents_t	events	;
	uint8_t        responseType_exist;
	long	       responseType	;
} MDE_EmergencyDetails_t;
*/

typedef struct v2x_msg_eva
{

    /*
     *  element: TemporaryID 
     *  desc: This is the 4 octet random device identifier, called the TemporaryID.
     *  size: 4 byte
     */ 
    uint8_t                 temp_id_exist;
    char                    temp_id[4];

    /*
     *  element: MinuteOfTheYear 
     *  desc: Time, unit:minute
     *  range: 0..527040
     */
    uint8_t                 time_stamp_exist;
    uint16_t                time_stamp;

	MDE_RoadSideAlert_t     rsaMsg;
	
	uint8_t                 responseType_exist;
	MDE_ResponseType        responseType;

	uint8_t                 details_exist;
	Mde_EmergencyDetails_t  details;
	/* 
	* Values 000 to 080 in steps of 50kg
	* Values 081 to 200 in steps of 500kg
	* Values 201 to 253 in steps of 2000kg
	* 81 represents   4500 kg 
	* 181 represents  54500 kg
	* range: INTEGER (0..255) 
	* xxxx kg > (0..255)
	* unit: t
	*/
	uint8_t                 mass_exist;
	double                  mass;

	uint8_t                 basicType_exist;
	MDE_VehicleType         basicType;

	uint8_t                  vehicleType_exist;
	MDE_VehicleGroupAffected vehicleType;	

	uint8_t                       responseEquip_exist;
	MDE_IncidentResponseEquipment responseEquip;

	uint8_t                       responderType_exist;
	MDE_ResponderGroupAffected    responderType;
	

} v2x_msg_eva_t;

#endif
