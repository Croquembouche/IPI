#ifndef __V2X_MSG_RSM_DS_H
#define __V2X_MSG_RSM_DS_H

#include <v2x_msg_common.h>

typedef struct
{
    uint8_t  ptc_type;
    uint32_t ptc_id;
    uint8_t  source_type; 
    /*
     *  element: TemporaryID 
     *  desc: This is the 8 octet random device identifier, called the TemporaryID.
     *  size: 8 byte
     */ 
    uint8_t  remote_device_id[8];
    uint8_t  remote_device_plate_num[16];
    uint32_t sec_mark;

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

    /*
     *  element: Elevation 
     *  desc: The geographic position above or below the reference ellipsoid (typically WGS-84).
     *  range: -409.5...6143.9 m
     *  lsb: 0.1m
     */   
    double                  elevation;

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
    uint8_t                 pos_confidence;

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
    uint8_t                 ele_confidence;

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
     *  desc: The vehicle speed expressed in unsigned units of 0.072 km/h. 
     *  range: 0...163.82 km/h
     *  lsb: 0.072 km/h
     */ 
    double                  speed;

    /*
     *  element: Heading 
     *  desc: The element provides the current heading of the sending device, expressed in unsigned units of
     *        0.0125 degrees from North such that 28799 such degrees represent 359.9875 degrees. 
     *  range: 0...359.9875 degrees
     *  lsb: 0.0125 degree
     */ 
    double                  heading;

    /*
     *  element: SteeringWheelAngle 
     *  desc: The angle of the driver’s steering wheel, expressed in a signed (to the right being positive) value with LSB 
     *        units of 1.5 degrees.
     *  range: -189...189 degrees
     *  lsb: 1.5 degrees
     */ 
    double                  angle;

    /*
     *  element: SpeedConfidence
     *  desc: The element is used to provide the current position confidence for speed.
     *  range: 0...7
     *  unavailable (0), -- Not Equipped or unavailable
     *  100 meters / sec (1),
     *  10 meters / sec (2),
     *  5 meters / sec (3),
     *  1 meters / sec (4),
     *  0.1 meters / sec (5),
     *  0.05 meters / sec (6),
     *  0.01 meters / sec (7),
     */
    uint8_t                 speed_confidence; 

    /*
     *  element: HeadingConfidence
     *  desc: The element is used to provide the current position confidence for heading.
     *  range: 0...7
     *  unavailable (0), -- Not Equipped or unavailable
     *  10 degrees (1),
     *  5 degrees (2),
     *  1 degrees (3),
     *  0.1 degrees (4),
     *  0.05 degrees (5),
     *  0.01 degrees (6),
     *  0.0125 degrees (7),
     */
    uint8_t                 heading_confidence; 

    /*
     *  element: HeadingConfidence
     *  desc: The element is used to provide the current position confidence for heading.
     *  range: 0...3
     *  unavailable (0), -- Not Equipped or unavailable
     *  2 degrees (1),
     *  1 degrees (2),
     *  0.2 degrees (3),
     */
    uint8_t                 steer_wheel_angle_confidence; 

    /*
     *  element: LongAcceleration
     *  desc: The element represents the signed acceleration of the vehicle along the Vehicle Longitudinal axis in units of 
     *        0.01 meters per second squared.
     *  range: -20...20 m/s^2
     *  lsb: 0.01 m/s^2
     */  
    double                  long_accel;

    /*
     *  element: LatAcceleration
     *  desc: The element represents the signed acceleration of the vehicle along the Vehicle Lateral axis in units of 
     *        0.01 meters per second squared.
     *  range: -20...20 m/s^2
     *  lsb: 0.01 m/s^2
     */ 
    double                  lat_accel;

    /*
     *  element: VerticalAcceleration
     *  desc: The element representing the signed vertical acceleration of the vehicle along the vertical axis in units of 
     *        0.02 G (where 9.80665 meters per second squared is one G, i.e., 0.02 G = 0.1962 meters per second squared).
     *  range: -24.7...24.9 m/s^2
     *  lsb: 0.1962 m/s^2
     */ 
    double                  vert_accel;

    /*
     *  element: YawRate
     *  desc: The element provides the Yaw Rate of the vehicle, a signed value (to the right being positive) expressed in 
     *        0.01 degrees per second. 
     *  range: -327.67...327.67 deg/s
     *  lsb: 0.01 deg/s
     */ 
    double                  yaw_rate;

    /*
     *  element: VehicleWidth 
     *  desc: The width of the vehicle expressed in centimeters, unsigned. The width shall be the widest point of the vehicle 
     *        with all factory installed equipment. The value zero shall be sent when data is unavailable.
     *  range: 0...10.23 m
     *  lsb: 0.01m
     */
    double                  width;

    /*
     *  element: VehicleLength 
     *  desc: The length of the vehicle measured from the edge of the front bumper to the edge of the rear bumper expressed in
     *        centimeters, unsigned. It should be noted that this value is often combined with a vehicle width value to form a 
     *        data frame.The value zero shall be sent when data is unavailable.
     *  range: 0...40.95 m
     *  lsb: 0.01m
     */ 
    double                  length;

    /*
     *  element: VehicleHeight
     *  desc: The height of the vehicle, measured from the ground to the highest surface, excluding any antenna(s).
     *  range: 0...6.35 m
     *  lsb: 0.05 m
     */ 
    double                  vehicle_height;

    /*
     *  element: BasicVehicleClass
     *  desc: The element is used to provide a common classification system.
     *  valid value see BasicVehicleClass     
     */
    uint16_t                basic_vehicle_class;
} participant_data_frame_t;

typedef struct
{
    /*
     *  element: MsgCount 
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    uint32_t                msg_count;

    /*
     *  element: TemporaryID 
     *  desc: This is the 8 octet random device identifier, called the TemporaryID.
     *  size: 8 byte
     */    
    uint8_t                 temp_id[8];

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

    /*
     *  element: Elevation 
     *  desc: The geographic position above or below the reference ellipsoid (typically WGS-84).
     *  range: -409.5...6143.9 m
     *  lsb: 0.1m
     */   
    double                  elevation;

    int                      participants_count;
    participant_data_frame_t participants[16];
} v2x_msg_rsm_t;

#endif
