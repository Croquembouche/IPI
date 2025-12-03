#ifndef __V2X_MSG_TIM_H_
#define __V2X_MSG_TIM_H_

#include <v2x_msg_common.h>


/* Dependencies */
typedef enum Mde_GeographicalPath__description_PR {
	Mde_GeographicalPath__description_PR_NOTHING,	/* No components present */
	Mde_GeographicalPath__description_PR_path,
	Mde_GeographicalPath__description_PR_geometry,
	Mde_GeographicalPath__description_PR_oldRegion
	/* Extensions may appear below */
	
} Mde_GeographicalPath__description_PR;

/* Dependencies */
typedef enum Mde_OffsetSystem__offset_PR {
	Mde_OffsetSystem__offset_PR_NOTHING,	/* No components present */
	Mde_OffsetSystem__offset_PR_xy,
	Mde_OffsetSystem__offset_PR_ll
} Mde_OffsetSystem__offset_PR;


/* Dependencies */
typedef enum Mde_RegionalExtension_124P0__regExtValue_PR {
	Mde_RegionalExtension_124P0__regExtValue_PR_NOTHING,	/* No components present */
	
} Mde_RegionalExtension_124P0__regExtValue_PR;



/* Dependencies */
typedef enum Mde_NodeListLL_PR {
	Mde_NodeListLL_PR_NOTHING,	/* No components present */
	Mde_NodeListLL_PR_nodes
	/* Extensions may appear below */
	
} Mde_NodeListLL_PR;


/* Dependencies */
typedef enum Mde_NodeOffsetPointLL_PR {
	Mde_NodeOffsetPointLL_PR_NOTHING,	/* No components present */
	Mde_NodeOffsetPointLL_PR_node_LL1,
	Mde_NodeOffsetPointLL_PR_node_LL2,
	Mde_NodeOffsetPointLL_PR_node_LL3,
	Mde_NodeOffsetPointLL_PR_node_LL4,
	Mde_NodeOffsetPointLL_PR_node_LL5,
	Mde_NodeOffsetPointLL_PR_node_LL6,
	Mde_NodeOffsetPointLL_PR_node_LatLon,
	Mde_NodeOffsetPointLL_PR_regional
} Mde_NodeOffsetPointLL_PR;

/* Node-LL-24B */
typedef struct Mde_Node_LL {
	long	 lon;
	long	 lat;
} Mde_Node_LL_t;


typedef struct Mde_NodeAttributeLLList {
	long	 localNode;
} Mde_NodeAttributeLLList_t;

typedef struct Mde_SegmentAttributeLLList {
	long	 segment;
} Mde_SegmentAttributeLLList_t;


typedef struct Mde_LaneDataAttributeList {
	Mde_LaneDataAttribute_PR present;
	long pathEndPointAngle;
	long laneCrownPointCenter;
	long laneCrownPointLeft;
	long laneCrownPointRight;
	long laneAngle;
	int  speedLimits_count;
	#define MDE_SPEEDLIMITS_COUNT_MAX (9)
	Mde_RegulatorySpeedLimit_t	speedLimits[MDE_SPEEDLIMITS_COUNT_MAX];	

} Mde_LaneDataAttributeList_t;



/* NodeAttributeSetLL */
typedef struct Mde_NodeAttributeSetLL {

	int localNode_count;
	#define MDE_LOCALNODE_COUNT_MAX (8)
	Mde_NodeAttributeLLList_t localNode_list[MDE_LOCALNODE_COUNT_MAX];

	int disabled_count;
	#define MDE_DISABLED_LIST_MAX (8)
    Mde_SegmentAttributeLLList_t disabled_list[MDE_DISABLED_LIST_MAX];

	int enabled_count;
	#define MDE_ENABLED_LIST_MAX (8)
    Mde_SegmentAttributeLLList_t enabled_list[MDE_ENABLED_LIST_MAX];


	uint32_t data_count;
	#define MDE_LANEDATA_LIST_MAX (8)
	Mde_LaneDataAttribute_t   data[MDE_LANEDATA_LIST_MAX];

	int dWidth_active;
	long	dWidth	/* OPTIONAL */;
	int dElevation_active;
	long	dElevation	/* OPTIONAL */;
} Mde_NodeAttributeSetLL_t;

/* NodeOffsetPointLL */
typedef struct Mde_NodeOffsetPointLL {
	Mde_NodeOffsetPointLL_PR present;
	Mde_Node_LL_t node_LL1;
	Mde_Node_LL_t node_LL2;
	Mde_Node_LL_t node_LL3;
	Mde_Node_LL_t node_LL4;
	Mde_Node_LL_t node_LL5;
	Mde_Node_LL_t node_LL6;
	Mde_Node_LLmD_64b_t node_LatLon;
	Mde_RegionalExtension_124P0_t	 regional;	
} Mde_NodeOffsetPointLL_t;

/* NodeLL */
typedef struct Mde_NodeLL {
	Mde_NodeOffsetPointLL_t	 delta;
	int attributes_acvtive;
	Mde_NodeAttributeSetLL_t	attributes	/* OPTIONAL */;

} Mde_NodeLL_t;

/* NodeListLL */
typedef struct Mde_NodeListLL {
	Mde_NodeListLL_PR present;
	uint32_t nodes_count;
	#define MDE_NODES_COUNT_MAX (63)
	Mde_NodeLL_t nodes[MDE_NODES_COUNT_MAX];
} Mde_NodeListLL_t;

/* OffsetSystem */
typedef struct Mde_OffsetSystem {
	int scale_active;
	long	scale	/* OPTIONAL */;
	Mde_OffsetSystem__offset_PR present; 
	Mde_NodeListXY_t xy;
    Mde_NodeListLL_t ll;
} Mde_OffsetSystem_t;

/* Circle */
typedef struct Mde_Circle {
	Mde_Position3D_t	 center;
	long	 radius;
	long	 units;
} Mde_Circle_t;

/* GeometricProjection */
typedef struct Mde_GeometricProjection {
	Mde_HeadingSlice_t	 direction;
	int  extent_active;
	long	extent	/* OPTIONAL */;
	int  laneWidth_active;
	long	laneWidth	/* OPTIONAL */;
	Mde_Circle_t	 circle;
} Mde_GeometricProjection_t;

/* Dependencies */
typedef enum Mde_ValidRegion__area_PR {
	Mde_ValidRegion__area_PR_NOTHING,	/* No components present */
	Mde_ValidRegion__area_PR_shapePointSet,
	Mde_ValidRegion__area_PR_circle,
	Mde_ValidRegion__area_PR_regionPointSet,
} Mde_ValidRegion__area_PR;

/* ShapePointSet */
typedef struct Mde_ShapePointSet {
	int anchor_active;
	Mde_Position3D_t	anchor	/* OPTIONAL */;
	int laneWidth_active;
	long	laneWidth	/* OPTIONAL */;
	int directionality_active;
	long	directionality	/* OPTIONAL */;

	Mde_NodeListXY_t	 nodeList;
} Mde_ShapePointSet_t;


/* RegionOffsets */
typedef struct Mde_RegionOffsets {
	long	 xOffset;
	long	 yOffset;
	int   zOffset_active;
	long	zOffset	/* OPTIONAL */;

} Mde_RegionOffsets_t;

/* RegionPointSet */
typedef struct Mde_RegionPointSet {
	int anchor_active;
	Mde_Position3D_t	anchor	/* OPTIONAL */;

	int scale_active;
	long	scale	/* OPTIONAL */;

	uint32_t nodeList_count;
	#define MDE_REGION_OFFSRTS_COUNT_MAX (64)
	Mde_RegionOffsets_t	 nodeList[MDE_REGION_OFFSRTS_COUNT_MAX];
} Mde_RegionPointSet_t;

typedef	struct Mde_ValidRegion__area 
{
	Mde_ValidRegion__area_PR present;
	Mde_ShapePointSet_t	 shapePointSet;
	Mde_Circle_t	 circle;
	Mde_RegionPointSet_t	 regionPointSet;	
} Mde_ValidRegion_area;

/* ValidRegion */
typedef struct Mde_ValidRegion {
	Mde_HeadingSlice_t	 direction;
	int  extent_active;
	long	extent	/* OPTIONAL */;
	Mde_ValidRegion_area area;
} Mde_ValidRegion_t;

typedef struct Mde_GeographicalPath__description {
	Mde_GeographicalPath__description_PR present;
	Mde_OffsetSystem_t	 path;
	Mde_GeometricProjection_t	 geometry;
	Mde_ValidRegion_t	 oldRegion;	
}Mde_GeographicalPath__description_t;


/* GeographicalPath */
typedef struct Mde_GeographicalPath {
	int name_active;
	Mde_DescriptiveName_t	name	/* OPTIONAL */;

	int id_active;
	Mde_RoadSegmentReferenceID_t	id	/* OPTIONAL */;

	int anchor_active;
	Mde_Position3D_t	anchor	/* OPTIONAL */;

	int laneWidth_active;
	long	laneWidth	/* OPTIONAL */;

	int directionality_active;
	long	directionality	/* OPTIONAL */;

	int closedPath_active;
	int	closedPath	/* OPTIONAL */;

	int direction_active;
	Mde_HeadingSlice_t	direction	/* OPTIONAL */;

	int description_active;
    Mde_GeographicalPath__description_t description;
} Mde_GeographicalPath_t;

/* Dependencies */
typedef enum Mde_TravelerDataFrame__msgId_PR {
	Mde_TravelerDataFrame__msgId_PR_NOTHING,	/* No components present */
	Mde_TravelerDataFrame__msgId_PR_furtherInfoID,
	Mde_TravelerDataFrame__msgId_PR_roadSignID
} Mde_TravelerDataFrame__msgId_PR;

typedef enum Mde_TravelerDataFrame__content_PR {
	Mde_TravelerDataFrame__content_PR_NOTHING,	/* No components present */
	Mde_TravelerDataFrame__content_PR_advisory,
	Mde_TravelerDataFrame__content_PR_workZone,
	Mde_TravelerDataFrame__content_PR_genericSign,
	Mde_TravelerDataFrame__content_PR_speedLimit,
	Mde_TravelerDataFrame__content_PR_exitService
} Mde_TravelerDataFrame__content_PR;

/* Dependencies */
typedef enum Mde_ITIScodesAndText__Member__item_PR {
	Mde_ITIScodesAndText__Member__item_PR_NOTHING,	/* No components present */
	Mde_ITIScodesAndText__Member__item_PR_itis,
	Mde_ITIScodesAndText__Member__item_PR_text
} Mde_ITIScodesAndText__Member__item_PR;




typedef struct Mde_ITIScodesAndText__Member {

	Mde_ITIScodesAndText__Member__item_PR present;
	long itis;
	#define MDE_ITISTEXT_SIZE_MAX (500)
	Mde_IA5String_t text;

} Mde_ITIScodesAndText__Member;

/* Dependencies */
typedef enum Mde_WorkZone__Member__item_PR {
	Mde_WorkZone__Member__item_PR_NOTHING,	/* No components present */
	Mde_WorkZone__Member__item_PR_itis,
	Mde_WorkZone__Member__item_PR_text
} Mde_WorkZone__Member__item_PR;


/* Forward definitions */
typedef struct Mde_WorkZone_Member {
	Mde_WorkZone__Member__item_PR present;
	long itis;
	#define MDE_ITISTEXT_PHRASE_SIZE_MAX (16)
	Mde_IA5String_t text;
} Mde_WorkZone_Member_t;

/* Dependencies */
typedef enum Mde_GenericSignage__Member__item_PR {
	Mde_GenericSignage__Member__item_PR_NOTHING,	/* No components present */
	Mde_GenericSignage__Member__item_PR_itis,
	Mde_GenericSignage__Member__item_PR_text
} Mde_GenericSignage__Member__item_PR;

typedef struct Mde_GenericSignage__Member {
	Mde_GenericSignage__Member__item_PR present;
	long itis;
	#define MDE_GENERICSIGNAGE_SIZE_MAX (16)
	Mde_IA5String_t text;	
} Mde_GenericSignage__Member_t;

/* Dependencies */
typedef enum Mde_SpeedLimit__Member__item_PR {
	Mde_SpeedLimit__Member__item_PR_NOTHING,	/* No components present */
	Mde_SpeedLimit__Member__item_PR_itis,
	Mde_SpeedLimit__Member__item_PR_text
} Mde_SpeedLimit__Member__item_PR;

typedef struct Mde_SpeedLimit__Member {
	Mde_SpeedLimit__Member__item_PR present;
	long itis;
	#define MDE_SPEEDLIMIT_SIZE_MAX (16)
	Mde_IA5String_t text;
} Mde_SpeedLimit__Member_t;

/* Dependencies */
typedef enum Mde_ExitService__Member__item_PR {
	Mde_ExitService__Member__item_PR_NOTHING,	/* No components present */
	Mde_ExitService__Member__item_PR_itis,
	Mde_ExitService__Member__item_PR_text
} Mde_ExitService__Member__item_PR;

/* Forward definitions */
typedef struct Mde_ExitService__Member {
	Mde_ExitService__Member__item_PR present;
	long itis;
	Mde_IA5String_t text;	
} Mde_ExitService__Member_t;

typedef	struct Mde_TravelerDataFrame__content {
		Mde_TravelerDataFrame__content_PR present;
		uint32_t advisory_count;
		#define MDE_ADVISORY_COUNT_MAX (100)
		Mde_ITIScodesAndText__Member advisory[MDE_ADVISORY_COUNT_MAX];
		uint32_t workZone_count;
		#define MDE_WORKZONE_COUNT_MAX (16)
		Mde_WorkZone_Member_t	 workZone[MDE_WORKZONE_COUNT_MAX];
		uint32_t genericSign_count;
		#define MDE_GENERICSIGN_COUNT_MAX (16)
		Mde_GenericSignage__Member_t genericSign[MDE_GENERICSIGN_COUNT_MAX];
		uint32_t speedLimit_count;
		#define MDE_SPEEDLIMIT_COUNT_MAX (16)
		Mde_SpeedLimit__Member_t speedLimit[MDE_SPEEDLIMIT_COUNT_MAX];
		uint32_t exitService_count;
		#define MDE_EXITSERVICE_COUNT_MAX (16)
		Mde_ExitService__Member_t exitService[MDE_EXITSERVICE_COUNT_MAX];	
	} Mde_TravelerDataFrame__content_t;

typedef struct Mde_MsgCRC_t {
	int size;
	#define MDE_MSG_CRC_MAX (2)
	uint8_t buf[MDE_MSG_CRC_MAX];	
} Mde_MsgCRC_t;

/* RoadSignID */
typedef struct Mde_RoadSignID {
	Mde_Position3D_t	 position;
	#define MDE_VIEWANGLE_BITS_MAX (2)
	Mde_HeadingSlice_t	 viewAngle;

	int  mutcdCode_active;
	long mutcdCode	/* OPTIONAL */;

	int  crc_active;
	Mde_MsgCRC_t	crc	/* OPTIONAL */;
	
} Mde_RoadSignID_t;


typedef struct Mde_FurtherInfoID {
	#define MDE_FURTHERINFO_ID_MAX (2)
	uint8_t buf[MDE_FURTHERINFO_ID_MAX];
	int size;
	
} Mde_FurtherInfoID_t;



/* TravelerDataFrame */
typedef struct Mde_TravelerDataFrame 
{
	long	 sspTimRights;
	long	 frameType;

	Mde_TravelerDataFrame__msgId_PR present;

	Mde_FurtherInfoID_t	 furtherInfoID;
	Mde_RoadSignID_t	 roadSignID;

	int   startYear_active;
	long	startYear	/* OPTIONAL */;
	long	 startTime;
	long	 duratonTime;
	long	 priority;
	long	 sspLocationRights;
	uint32_t GeographicalPath_count;
	#define MDE_GEOGRAPHICAL_PATH_COUNT_MAX (16)
	Mde_GeographicalPath_t GeographicalPath[MDE_GEOGRAPHICAL_PATH_COUNT_MAX];

	long	 sspMsgRights1;
	long	 sspMsgRights2;
	Mde_TravelerDataFrame__content_t content;	
	int url_active;
	#define MDE_URL_SHORT_MAX (15)
	Mde_IA5String_t	url	/* OPTIONAL */;

} Mde_TravelerDataFrame_t;

typedef struct Mde_UniqueMSGID {
	#define MDE_UNIQUEMSG_ID_MAX (9)
	uint8_t buf[MDE_UNIQUEMSG_ID_MAX];
	int size;
} Mde_UniqueMSGID_t;

typedef struct Mde_URL_Base {
	#define MDE_URL_BASE_MAX (45)
	uint8_t buf[MDE_URL_BASE_MAX];
	int size;
} Mde_URL_Base_t;

/* TravelerInformation */
typedef struct v2x_msg_tim 
{
	long   msgCnt;
	int    timeStamp_active;
	long   timeStamp ;
	int    packetID_active;
	Mde_UniqueMSGID_t packetID	/* OPTIONAL */;
	
	int    urlB_active;
	Mde_URL_Base_t	urlB	/* OPTIONAL */;

    uint32_t dataFrames_count;
	#define MDE_TRAVELER_DATA_FRAME_MAX (8)
	Mde_TravelerDataFrame_t	 dataFrames[MDE_TRAVELER_DATA_FRAME_MAX];
} v2x_msg_tim_t;

#endif
