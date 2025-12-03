#ifndef __V2X_MSG_ICA_H
#define __V2X_MSG_ICA_H



/*************************************************************************/
/**************************** IntersectionCollision_t **************************/
/*************************************************************************/

#define COARSE_HEADING_FACTOR   (1.00 / 1.50)

#define EVENTS_BYTE_LEN 2

#define                     eventHazardLights 0x80
#define                     eventHazardLights_byte 0
#define                     eventStopLineViolation 0x40 /* Intersection Violation */
#define                     eventStopLineViolation_byte 0
#define                     eventABSactivated 0x20
#define                     eventABSactivated_byte 0
#define                     eventTractionControlLoss 0x10
#define                     eventTractionControlLoss_byte 0
#define                     eventStabilityControlactivated 0x08
#define                     eventStabilityControlactivated_byte 0
#define                     eventHazardousMaterials 0x04
#define                     eventHazardousMaterials_byte 0
#define                     eventReserved1 0x02
#define                     eventReserved1_byte 0
#define                     eventHardBraking 0x01
#define                     eventHardBraking_byte 0
#define                     eventLightsChanged 0x80
#define                     eventLightsChanged_byte 1
#define                     eventWipersChanged 0x40
#define                     eventWipersChanged_byte 1
#define                     eventFlatTire 0x20
#define                     eventFlatTire_byte 1
#define                     eventDisabledVehicle 0x10 /* The DisabledVehicle DF may also be sent */
#define                     eventDisabledVehicle_byte 1
#define                     eventAirBagDeployment 0x08
#define                     eventAirBagDeployment_byte 1

#define WHEEL_BRAKES_BYTE_LEN 1

#define                     wheelBrakesLeftFront 0x80
#define                     wheelBrakesLeftFront_byte 0
#define                     wheelBrakesLeftRear 0x40 /* Intersection Violation */
#define                     wheelBrakesLeftRear_byte 0
#define                     wheelBrakesRightFront 0x20
#define                     wheelBrakesRightFront_byte 0
#define                     wheelBrakesRightRear 0x10
#define                     wheelBrakesRightRear_byte 0


/* PathPrediction */
typedef struct MDE_PathPrediction {
	long	 radiusOfCurve;
	long	 confidence;
	
} MDE_PathPrediction_t;

typedef struct MDE_Intersection_ReferenceID 
{
	long          region;
	long	      id;
} MDE_Intersection_ReferenceID_t;

/* Dependencies */
typedef enum approachOrLane_PR {
	MDE_ApproachOrLane_PR_NOTHING,	/* No components present */
	MDE_ApproachOrLane_PR_approach,
	MDE_ApproachOrLane_PR_lane
} MDE_ApproachOrLane_PR;

/* ApproachOrLane */
typedef struct mde_approachOrLane {
	MDE_ApproachOrLane_PR present;
	union MDE_ApproachOrLane_u {
		long	 approach;
		long	 lane;
	} choice;
	
} MDE_ApproachOrLane_t;

/*typedef struct MDE_path_history
{
    double lat_offset;
    double long_offset;
    double elev_offset;
    int    time_offset;
    double positional_accuracy;
    double heading;
    double speed;
} MDE_path_history_t;*/


typedef struct bsm_core_data
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
	
}MDE_bsm_core_data_t;


typedef struct v2x_msg_ica
{
	/*
     *  element: MsgCount 
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    long                   msg_count;

    /*
     *  element: TemporaryID 
     *  desc: This is the 4 octet random device identifier, called the TemporaryID.
     *  size: 4 byte
     */    
    char                    temp_id[4];

	/*
     *  element: MinuteOfTheYear 
     *  desc: Time, unit:minute
     *  range: 0..527040
     */
    uint16_t                time_stamp;

	MDE_bsm_core_data_t     part_One;

	/*
     *  element: pathHistory_is_exsit 
     *  desc: pathHistory is exsit. 0 stand for not exsit, 1 stand for exsit.
     *  range: 0..1
     */
	uint8_t                 pathHistory_is_exsit;	
	Mde_Path_History_t      pathHistory;

	/*
	  *  element: pathPrediction_is_exsit 
	  *  desc: pathPrediction is exsit. 0 stand for not exsit, 1 stand for exsit.
	  *  range: 0..1
	  */
	uint8_t                 pathPrediction_is_exsit;
	Mde_Path_Prediction_t   pathPrediction;

	MDE_Intersection_ReferenceID_t   intersectionID;
	
	MDE_ApproachOrLane_t   laneNumber;	

	/*
     *  element: event_is_exist 
     *  desc: events is exsit. 0 stand for not exsit, 1 stand for exsit.
     *  range: 0..1
     */
    uint8_t               event_is_exist;
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
    uint8_t                eventsFlag[13];
	
	//RegionalExtension_124P0_t regional[4];
} v2x_msg_ica_t;

#endif
