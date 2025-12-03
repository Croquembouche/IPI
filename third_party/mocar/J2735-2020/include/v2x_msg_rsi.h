#ifndef __V2X_MSG_RSI_DS_H
#define __V2X_MSG_RSI_DS_H

#include <v2x_msg_common.h>

typedef struct
{
    double latitude;
    double longitude;
    double elevation;
} path_point_frame_t;

typedef unsigned short  Mde_EventType;

typedef enum Mde_EventSource {
    Mde_EventSource_unknown = 0,     /* 0 Unknown */
    Mde_police = 1,      /* 1 traffic police */
    Mde_government = 2,  /* 2 govenment */
    Mde_meteorological = 3, /* 3 meteorological department */
    Mde_internet = 4,    /* 4 internet services */
    Mde_detection = 5   /* 5 local detection */
} Mde_EventSource;

typedef struct Mde_PositionOffsetLL 
{
    /*
    *    #       define      position_LL1_chosen 1
    *    #       define      position_LL2_chosen 2
    *    #       define      position_LL3_chosen 3
    *    #       define      position_LL4_chosen 4
    *    #       define      position_LL5_chosen 5
    *    #       define      position_LL6_chosen 6
    *    #       define      position_LatLon_chosen 7
    */
    unsigned short  choice;
    double lon;
    double lat;
} Mde_PositionOffsetLL_t;

typedef struct Mde_VerticalOffset 
{
    /*
    *    #       define      offset1_chosen 1
    *    #       define      offset2_chosen 2
    *    #       define      offset3_chosen 3
    *    #       define      offset4_chosen 4
    *    #       define      offset5_chosen 5
    *    #       define      offset6_chosen 6
    *    #       define      elevation_chosen 7
    */
    unsigned short  choice;
    /* Vertical Offset */
	/* All below in steps of 10cm above or below the reference ellipsoid */
     /* to choose, set choice to offset1_chosen */
                              /* with a range of +- 6.3 meters vertical */
     /* to choose, set choice to offset2_chosen */
                              /* with a range of +- 12.7 meters vertical */
     /* to choose, set choice to offset3_chosen */
                              /* with a range of +- 25.5 meters vertical */
      /* to choose, set choice to offset4_chosen */
                              /* with a range of +- 51.1 meters vertical */
      /* to choose, set choice to offset5_chosen */
                              /* with a range of +- 102.3 meters vertical */
     /* to choose, set choice to offset6_chosen */
                              /* with a range of +- 204.7 meters vertical */
      /* to choose, set choice to
                                 * elevation_chosen */
                                /* with a range of -409.5 to + 6143.9 meters */
    short offset;  //offset1-6
    int elevation; //offset 7
} Mde_VerticalOffset_t;


typedef struct Mde_PositionOffsetLLV {
    Mde_PositionOffsetLL_t offsetLL;
		/* offset in lon/lat */
    Mde_VerticalOffset_t  offsetV;  
		/* offset in elevation */
} Mde_PositionOffsetLLV_t;

typedef struct Mde_Description {
    unsigned short  choice; //1=textString_chosen  2=textGB2312_chosen 

    unsigned short  length;
    char            *value; /*1: textString_chosen  ASCII text  2:text using Chinese-character encoding GB2312-80*/

} Mde_Description_t;

	/* Traffic event type according to China GB/T 29100-2012 */
typedef struct Mde_RSITimeDetails { 
    unsigned int startTime;       // MinuteOfTheYear;
    unsigned int endTime;  
		/* Exact or estimated end time */
    unsigned int  endTimeConfidence;  //参考BSM Mde_time_confidence 
} Mde_RSITimeDetails_t;


typedef struct Mde_RSIPriority {
    unsigned short  length;
    unsigned char   value[1];
} Mde_RSIPriority_t;

typedef struct Mde_ReferencePath 
{
    unsigned int    PathPoint_Count;
    Mde_PositionOffsetLLV_t* activePath;
		/* RSI is active for vehicles within this path */
		/* Points are listed from upstream to downstream */
		/* along the vehicle drive direction. */
		/* One path includes at least 1 points. */
		/* A path with only 1 point means a round alert area */
    unsigned short          pathRadius;
		/* The biggest distance away from the alert path */
		/* within which the warning is active. */
} Mde_ReferencePath_t;

typedef struct Mde_NodeReferenceID 
{
    unsigned short  region;  
		/* a globally unique regional assignment value */
		/* typical assigned to a regional DOT authority */
		/* the value zero shall be used for testing needs */
    unsigned short  id;
		/* a unique mapping to the node */
		/* in question within the above region of use		*/
} Mde_NodeReferenceID_t;


typedef struct Mde_ReferenceLanes 
{
    unsigned short  length;  /* number of significant bits */
    unsigned char   *value;
} Mde_ReferenceLanes_t;


typedef struct Mde_ReferenceLink {
		/* this Link is from Node of upstreamNodeId to Node of downstreamNodeId */
    Mde_NodeReferenceID_t upstreamNodeId;
    Mde_NodeReferenceID_t downstreamNodeId;
    Mde_ReferenceLanes_t  referenceLanes; 
		/* Provide reference lanes if is necessary */
		/* Refer to all lanes if this data is not given */
} Mde_ReferenceLink_t;


/* Road Traffic Event List */
typedef struct mde_rtedata {
    unsigned short      rteId;/* local ID of this rte information set by RSU */
		
    Mde_EventType       eventType;/* Type of event, according to China GB/T 29100-2012 */
		
    Mde_EventSource     eventSource;
    
    Mde_PositionOffsetLLV_t eventPos;  
    
    unsigned short          eventRadius;   /* Path point list for RSA message */
    
    Mde_Description_t     description;  /* Additional description to this event */
		
    Mde_RSITimeDetails_t  timeDetails;  /* Start time or end time when this event is active */
		
    Mde_RSIPriority_t     priority;  
    
	/* the urgency of this RSI data, a relative */
	/* degree of merit compared with other RSI data */
	/* Related paths of this traffic event */
	unsigned int            referencePath_count;	
	Mde_ReferencePath_t*    referencePath_value;

	unsigned int  referenceLink_count;	
	Mde_ReferenceLink_t* referenceLink_value;

	/* Related links of this traffic event */
    unsigned short      eventConfidence;  
	/* indicate the event confidence set by event source */
	/* the probability/confidence of the detected event */
	/* being truly extent at a certain place, */
	/* to help vehicle determine whether to trust the received information. */
} mde_rtedata_t;


typedef struct mde_rtsdata {
    unsigned short  rtsId;/* local ID of this rts information set by RSU */

	/*Set bit to 1 if the related lane is effective  Support maximum 15 lanes */	
    unsigned short        signType;/* Type of sign, according to China GB 5768.2  */
		
    Mde_PositionOffsetLLV_t signPos;  
    
    Mde_Description_t     description; 
    
    Mde_RSITimeDetails_t  timeDetails;  
    
    Mde_RSIPriority_t     priority;  
    
    unsigned int            referencePath_count;	
	Mde_ReferencePath_t*    referencePath_value;

	unsigned int  referenceLink_count;	
	Mde_ReferenceLink_t* referenceLink_value;

} mde_rtsdata_t;


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

    uint32_t                time_stamp; //moy

#if 0    
    double                  unix_time;
    uint8_t                 rsi_info_id;
    uint32_t                alert_type;
    char                    description[256];                    
    uint8_t                 priority;
#endif
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
#if 0
    int                     pp_count;
    path_point_frame_t      path_points[32];
    uint32_t                alert_radius;
#endif
    uint32_t                rtedata_count;
    mde_rtedata_t*          rtedata; 

    uint32_t                rtsdata_count;
    mde_rtsdata_t*          rtsdata; 
} v2x_msg_rsi_t;

#endif
