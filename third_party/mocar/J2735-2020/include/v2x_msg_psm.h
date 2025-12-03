#ifndef _V2X_MSG_PSM_H_
#define _V2X_MSG_PSM_H_

#include <v2x_msg_common.h>

typedef enum PSM_PersonalDeviceUserType {
	PSM_PersonalDeviceUserType_unavailable	= 0,
	PSM_PersonalDeviceUserType_aPEDESTRIAN	= 1,
	PSM_PersonalDeviceUserType_aPEDALCYCLIST	= 2,
	PSM_PersonalDeviceUserType_aPUBLICSAFETYWORKER	= 3,
	PSM_PersonalDeviceUserType_anANIMAL	= 4
	/*
	 * Enumeration is extensible
	 */
} PSM_PersonalDeviceUserType_e;



typedef struct PSM_initialPosition
{
    Mde_Transmission_State_e                transmission_state;
    Mde_position_speed_confidence_t    position_speed_confidence;
}PSM_initialPosition_t;

typedef struct PSM_crumbData
{
    int16_t    latOffset;
    int16_t    lonOffset;
    int16_t    elevationOffset;
    int16_t    timeOffset;
    Mde_positional_accuracy_t    posAccuracy;
}PSM_crumbData_t;

typedef struct PSM_PathHistory
{
    PSM_initialPosition_t       initialPosition;
    uint8_t                          history_pos_num;
    PSM_crumbData_t           *history_pos_data;
}PSM_PathHistory_t;







typedef enum PSM_HumanPropelledType {
	PSM_HumanPropelledType_unavailable	= 0,
	PSM_HumanPropelledType_otherTypes	= 1,
	PSM_HumanPropelledType_onFoot	= 2,
	PSM_HumanPropelledType_skateboard	= 3,
	PSM_HumanPropelledType_pushOrKickScooter	= 4,
	PSM_HumanPropelledType_wheelchair	= 5
	/*
	 * Enumeration is extensible
	 */
} PSM_HumanPropelledType_e;


typedef enum PSM_AnimalPropelledType {
	PSM_AnimalPropelledType_unavailable	= 0,
	PSM_AnimalPropelledType_otherTypes	= 1,
	PSM_AnimalPropelledType_animalMounted	= 2,
	PSM_AnimalPropelledType_animalDrawnCarriage	= 3
	/*
	 * Enumeration is extensible
	 */
} PSM_AnimalPropelledType_e;



typedef enum PSM_MotorizedPropelledType {
	PSM_MotorizedPropelledType_unavailable	= 0,
	PSM_MotorizedPropelledType_otherTypes	= 1,
	PSM_MotorizedPropelledType_wheelChair	= 2,
	PSM_MotorizedPropelledType_bicycle	= 3,
	PSM_MotorizedPropelledType_scooter	= 4,
	PSM_MotorizedPropelledType_selfBalancingDevice	= 5
	/*
	 * Enumeration is extensible
	 */
} PSM_MotorizedPropelledType_e;


typedef enum PSM_PropelledInformation_PR {
	PSM_PropelledInformation_PR_NOTHING,	/* No components present */
	PSM_PropelledInformation_PR_human,
	PSM_PropelledInformation_PR_animal,
	PSM_PropelledInformation_PR_motor
} PSM_PropelledInformation_PR;


typedef struct PSM_PropelledInformation {
	PSM_PropelledInformation_PR present;
	union PSM_PropelledInformation_u {
		PSM_HumanPropelledType_e	 human;
		PSM_AnimalPropelledType_e	 animal;
		PSM_MotorizedPropelledType_e	 motor;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
} PSM_PropelledInformation_t;



typedef struct v2x_msg_psm
{
    PSM_PersonalDeviceUserType_e        basicType;

    /*
    second mark
    (0..65535) -- units of milliseconds
    */
    uint16_t secMark;

    /*
     *  element: MsgCount
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    uint16_t                msg_count;

    /*
     *  element: TemporaryID
     *  desc: This is the 4 octet random device identifier, called the TemporaryID.
     *  size: 4 byte
     */
    char                    temp_id[VEH_ID_LEN];

    /*
     *  element: Longitude
     *  desc: The geographic longitude of an object.
     *  range: -180...180 degrees
     *  lsb: 0.1 micro degree
     */
    double                  longitude;

    /*
     *  element: Latitude
     *  desc: The geographic latitude of an object.
     *  range: -90...90 degrees
     *  lsb: 0.1 micro degree
     */
    double                  latitude;

    Mde_positional_accuracy_t    posAccuracy;

    /*
     *  element: Speed
     *  desc: The vehicle speed expressed in unsigned.
     *  range: 0…8191
     *  lsb: 0.02 m/s
     */
    uint16_t                  speed;

    float                       heading;

    Mde_AccelerationSet4Way_t       accelSet;

    PSM_PathHistory_t               pathHistory;

    Mde_Path_Prediction_t           pathPrediction;

    PSM_PropelledInformation_t      propulsion;
}v2x_msg_psm_t;


#endif
