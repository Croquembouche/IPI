#ifndef _V2X_MSG_PDM_H_
#define _V2X_MSG_PDM_H_

typedef enum Mde_term_value_t
{
	TERM_TIME = 0,
	TERM_DISTANCE
}Mde_term_value_t;

typedef struct Mde_term
{
	Mde_term_value_t msg_id;
	union msg_term_value
	{
		/*
	     *  desc:  Terminate this management process 
	     *  range: 1..1800
	     *  lsb: 1 sec
		 */
		uint16_t time;
		
		/*
	     *  desc:  Terminate management process  
	     *  range: 1..30000
	     *  lsb: meter
		 */
		uint16_t distance;
	}choice;
}Mde_term_t;

typedef struct Mde_SnapshotTime
{
	/*
     *  range: 0..31
     *  lsb: 1.00 m/s
	 */
	uint8_t speed1;
	/*
     *  range: 0..61
     *  lsb: 1 seconds
	 */	
	uint8_t	time1;

	/*
     *  range: 0..31
     *  lsb: 1.00 m/s
	 */
	uint8_t speed2;
	/*
     *  range: 0..61
     *  lsb: 1 seconds
	 */	
	uint8_t	time2;	
}Mde_SnapshotTime_t;

typedef struct Mde_SnapshotDistance
{
	/*
     *  range: 0..1023
     *  lsb: 1.00 meters
	 */	
	uint16_t distance1;
	/*
     *  range: 0..31
     *  lsb: 1.00 m/s
	 */
	uint8_t speed1;
	
	/*
     *  range: 0..1023
     *  lsb: 1.00 meters
	 */	
	uint16_t distance2;
	/*
     *  range: 0..31
     *  lsb: 1.00 m/s
	 */
	uint8_t speed2;
	
}Mde_SnapshotDistance_t;


typedef struct Mde_snapshot
{
	Mde_term_value_t msg_id;
	union msg_snapshot_value
	{
		Mde_SnapshotTime_t snapshotTime;
		Mde_SnapshotDistance_t snapshotDistance;
	}choice;
	
}Mde_snapshot_t;

typedef struct Mde_VehicleStatusRequest
{
	/*
     *  range: 0..28
     *  	unknown            (0),
     *  	lights             (1),  -- Exterior Lights
     *  	wipers             (2),  -- Wipers
     *  	brakes             (3),  -- Brake Applied                
     *  	stab               (4),  -- Stability Control        
     *  	trac               (5),  -- Traction Control        
     *  	abs                (6),  -- Anti-Lock Brakes        
     *  	sunS               (7),  -- Sun Sensor        
     *  	rainS              (8),  -- Rain Sensor        
     *  	airTemp            (9),  -- Air Temperature    
     *  	steering           (10),
     *  	vertAccelThres     (11), -- Wheel that Exceeded the
     *  	vertAccel          (12), -- Vertical g Force Value  
     *  	hozAccelLong       (13), -- Longitudinal Acceleration        
     *  	hozAccelLat        (14), -- Lateral Acceleration        
     *  	hozAccelCon        (15), -- Acceleration Confidence 
     *  	accel4way          (16),
     *  	confidenceSet      (17),
     *  	obDist             (18), -- Obstacle Distance        
     *  	obDirect           (19), -- Obstacle Direction        
     *  	yaw                (20), -- Yaw Rate        
     *   	yawRateCon         (21), -- Yaw Rate Confidence
     *  	dateTime           (22), -- complete time
     *  	fullPos            (23), -- complete set of time and
                             -- position, speed, heading
     *  	position2D         (24), -- lat, long
     *  	position3D         (25), -- lat, long, elevation
     *  	vehicle            (26), -- height, mass, type
     *  	speedHeadC         (27), 
     *  	speedC             (28),
	 */
	uint8_t dataType;

	/*
     *  range: 1..15
	 */
	uint8_t subType_is_exsit;
	uint8_t subType;
	
	/*
     *  range: -32767..32767
	 */
	uint8_t sendOnLessThenValue_is_exsit;
	int16_t sendOnLessThenValue;
	
	/*
     *  range: -32767..32767
	 */
	uint8_t sendOnMoreThenValue_is_exsit;
	int16_t sendOnMoreThenValue;
	
	/*
     *  range: 0..1
	 */
	uint8_t sendAll_is_exsit;
	uint8_t sendAll;
}Mde_VehicleStatusRequest_t;

typedef struct v2x_msg_pdm
{
	/*
     *  element: MinuteOfTheYear
     *  desc:  the mSec point in the current UTC minute.
     *  range: 0..527040
     *  lsb: minute
	 */
	uint8_t timeStamp_is_exist;
	uint32_t timeStamp;

	/*
     *  desc:  Sample Starting Point
     *  range: 0..255
	 */
	uint8_t sampleStart;

	/*
     *  desc:  Sample Ending Point
     *  range: 0..255
	 */
	uint8_t sampleEnd;

	/*
      *  desc:  Each bit 22.5 degree starting from North and moving Eastward (clockwise) as one bit
     *  range: 0..1
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
	 */
	uint8_t directions[16];

	Mde_term_t term;

	Mde_snapshot_t snapshot;
	
	/*
     *  desc:  Time Interval at which to send snapshots
     *  range: 0..61
     *  unit: second
	 */
	uint8_t txInterval;

	/*
     *  range: dataElements_count : 0..32
	 */
	uint8_t dataElements_count;
	Mde_VehicleStatusRequest_t dataElements[32];	
}v2x_msg_pdm_t;


#endif
