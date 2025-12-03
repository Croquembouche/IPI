#ifndef __V2X_MSG_RSA_H
#define __V2X_MSG_RSA_H

#include "v2x_msg_common.h"

/*************************************************************************/
/***************************** Road Side Alert ***************************/
/*************************************************************************/

typedef struct v2x_msg_rsa
{
    /*
     *  element: MsgCount
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    uint16_t                   msg_count;

    /*
	 *	element: ITIS.ITIScodes
	 *	range: 0..65535
	 */
    uint16_t                   typeEvent;

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
    Mde_Transmission_State_e                transmission_state;

    /*
     *  element: Speed
     *  desc: The vehicle speed expressed in unsigned.
     *  range: 0…8191
     *  lsb: 0.02 m/s
     */
    uint16_t                  speed;

    /*
    typedef struct PositionalAccuracy {
        //semi-major axis accuracy at one standard dev
        //range 0-12.7 meter, LSB = .05m
        //254 = any value equal or greater than 12.70 meter
        //255 = unavailable semi-major axis value
        	SemiMajorAxisAccuracy_t	 semiMajor;

        //semi-minor axis accuracy at one standard dev
        //range 0-12.7 meter, LSB = .05m
        //254 = any value equal or greater than 12.70 meter
        //255 = unavailable semi-minor axis value
        	SemiMinorAxisAccuracy_t	 semiMinor;

        //orientation of semi-major axis
        //relative to true north (0~359.9945078786 degrees)
        //LSB units of 360/65535 deg  = 0.0054932479
        //a value of 0 shall be 0 degrees
        //a value of 1 shall be 0.0054932479 degrees
        //a value of 65534 shall be 359.9945078786 deg
        //a value of 65535 shall be used for orientation unavailable
        	SemiMajorAxisOrientation_t	 orientation;
    } PositionalAccuracy_t;
    */
    Mde_positional_accuracy_t    posAccuracy;

    Mde_position_speed_confidence_t     position_speed_confidence;
} v2x_msg_rsa_t;

#endif
