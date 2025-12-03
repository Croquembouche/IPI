#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>


void v2x_map_rx_data_dump(v2x_msg_map_t* user_map)
{
	if (NULL == user_map)
	{
		MOCAR_LOG_ERROR("usr msp is null");
		return;
	}

	if (SDK_OPTIONAL_EXSIT == user_map->timeStamp_active)
	{
		MOCAR_LOG_DEBUG_FMT("timeStamp : %ld", user_map->timeStamp);
	}
	MOCAR_LOG_DEBUG_FMT("msgIssueRevision : %ld", user_map->msgIssueRevision);
	if (SDK_OPTIONAL_EXSIT == user_map->layerType_active)
	{
		MOCAR_LOG_DEBUG_FMT("layerType : %ld", user_map->layerType);
	}
	if (SDK_OPTIONAL_EXSIT == user_map->layerID_active)
	{
		MOCAR_LOG_DEBUG_FMT("layerID : %ld", user_map->layerID);
	}
	if (user_map->intersections_count <= MDE_INTERSECTION_COUNT_MAX)
	{
		for(uint8_t i = 0; i < user_map->intersections_count; i++)
		{
			Mde_IntersectionGeometry_t * tmp_inter = &user_map->intersections[i];
			MOCAR_LOG_DEBUG_FMT("intersections[%d]: ------", i);
			
			if (SDK_OPTIONAL_EXSIT == tmp_inter->intersection_name_active)
			{
				MOCAR_LOG_DEBUG_FMT("inter name : %s", tmp_inter->name.buf);
			}
			if (SDK_OPTIONAL_EXSIT == tmp_inter->id.region_active)
			{
				MOCAR_LOG_DEBUG_FMT("region : %ld", tmp_inter->id.region);
			}
			MOCAR_LOG_DEBUG_FMT("id : %ld", tmp_inter->id.id);
			MOCAR_LOG_DEBUG_FMT("revision : %ld", tmp_inter->revision);
			MOCAR_LOG_DEBUG_FMT("lat : %ld", tmp_inter->refPoint.lat);
			MOCAR_LOG_DEBUG_FMT("Long : %ld", tmp_inter->refPoint.Long);
			if (SDK_OPTIONAL_EXSIT == tmp_inter->refPoint.elevation_active)
			{
				MOCAR_LOG_DEBUG_FMT("elevation : %ld", tmp_inter->refPoint.elevation);
			}
			if (SDK_OPTIONAL_EXSIT == tmp_inter->laneWidth_active)
			{
				MOCAR_LOG_DEBUG_FMT("laneWidth : %ld", tmp_inter->laneWidth);
			}
			if (tmp_inter->speedLimits_count <= MDE_SPEEDLIMITS_MAX)
			{
				for( uint8_t j = 0; j < tmp_inter->speedLimits_count; j++)
				{
					MOCAR_LOG_DEBUG_FMT("speedLimits type : %ld", tmp_inter->speedLimits[j].type);
					MOCAR_LOG_DEBUG_FMT("speedLimits speed : %ld", tmp_inter->speedLimits[j].speed);
				}
			}
			if (tmp_inter->laneSet_count <= MDE_INTERSECTION_LANESET_MAX)
			{
				for( uint8_t j = 0; j < tmp_inter->laneSet_count; j++)
				{
					Mde_GenericLane_t * tmp_lane = &tmp_inter->laneSet[j];
					MOCAR_LOG_DEBUG_FMT("laneSet[%d]: ------", j);

					MOCAR_LOG_DEBUG_FMT("laneID : %ld", tmp_lane->laneID);
					if (SDK_OPTIONAL_EXSIT == tmp_lane->name_active)
					{
						MOCAR_LOG_DEBUG_FMT("name : %s", tmp_lane->name.buf);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_lane->ingressApproach_active)
					{
						MOCAR_LOG_DEBUG_FMT("ingressApproach : %ld", tmp_lane->ingressApproach);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_lane->egressApproach_active)
					{
						MOCAR_LOG_DEBUG_FMT("egressApproach : %ld", tmp_lane->egressApproach);
					}
					char tmp_bit_string[40] = {0};
					for(uint8_t m = 0; m < 2; m++)
					{
						tmp_bit_string[m*2] = tmp_lane->laneAttributes.directionalUse_bits[m] + '0';
						tmp_bit_string[m*2 + 1] = ' ';
					}
					MOCAR_LOG_DEBUG_FMT("directionalUse_bits : %s", tmp_bit_string);
					memset(tmp_bit_string, 0, 40);
					for(uint8_t m = 0; m < 10; m++)
					{
						tmp_bit_string[m*2] = tmp_lane->laneAttributes.sharedWith_bits[m] + '0';
						tmp_bit_string[m*2 + 1] = ' ';
					}
					MOCAR_LOG_DEBUG_FMT("sharedWith_bits : %s", tmp_bit_string);
					
					if (Mde_LaneTypeAttributes_PR_vehicle == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 8; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.vehicle_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("vehicle_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_crosswalk == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.crosswalk_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("crosswalk_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_bikeLane == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.bikeLane_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("bikeLane_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_sidewalk == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.sidewalk_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("sidewalk_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_median == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.median_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("median_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_striping == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.striping_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("striping_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_trackedVehicle == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.trackedVehicle[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("trackedVehicle : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_parking == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.parking_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("parking_bits : %s", tmp_bit_string);
					}
					
					if (SDK_OPTIONAL_EXSIT ==  tmp_lane->maneuvers_active)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 12; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->maneuvers_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("maneuvers_bits : %s", tmp_bit_string);
					}
					
					if (Mde_NodeListXY_PR_nodes == tmp_lane->nodeList.present)
					{
						if (tmp_lane->nodeList.NodeSetXY_count <= MDE_NODELISTXY_MAX)
						{
							for (uint32_t m = 0; m < tmp_lane->nodeList.NodeSetXY_count; m++)
							{
								Mde_NodeXY_t * tmp_node = &tmp_lane->nodeList.nodes[m];

								if (Mde_NodeOffsetPointXY_PR_node_XY1 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY1 x : %ld", tmp_node->delta.node_XY1.x);
									MOCAR_LOG_DEBUG_FMT("node_XY1 y : %ld", tmp_node->delta.node_XY1.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY2 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY2 x : %ld", tmp_node->delta.node_XY2.x);
									MOCAR_LOG_DEBUG_FMT("node_XY2 y : %ld", tmp_node->delta.node_XY2.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY3 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY3 x : %ld", tmp_node->delta.node_XY3.x);
									MOCAR_LOG_DEBUG_FMT("node_XY3 y : %ld", tmp_node->delta.node_XY3.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY4 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY4 x : %ld", tmp_node->delta.node_XY4.x);
									MOCAR_LOG_DEBUG_FMT("node_XY4 y : %ld", tmp_node->delta.node_XY4.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY5 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY5 x : %ld", tmp_node->delta.node_XY5.x);
									MOCAR_LOG_DEBUG_FMT("node_XY5 y : %ld", tmp_node->delta.node_XY5.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY6 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY6 x : %ld", tmp_node->delta.node_XY6.x);
									MOCAR_LOG_DEBUG_FMT("node_XY6 y : %ld", tmp_node->delta.node_XY6.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_LatLon == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_LatLon lon : %ld", tmp_node->delta.node_LatLon.lon);
									MOCAR_LOG_DEBUG_FMT("node_LatLon lat : %ld", tmp_node->delta.node_LatLon.lat);
								}
								
								if (SDK_OPTIONAL_EXSIT == tmp_node->attributes_active)
								{
									if (tmp_node->attributes.localNode_count <= MDE_LOCALNODE_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.localNode_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("localNode[%d] : %ld", n, tmp_node->attributes.localNode[n]);
										}
									}
									if (tmp_node->attributes.disabled_count <= MDE_DISABLED_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.disabled_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("disabled[%d] : %ld", n, tmp_node->attributes.disabled[n]);
										}
									}
									if (tmp_node->attributes.enabled_count <= MDE_ENABLE_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.enabled_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("enabled[%d] : %ld", n, tmp_node->attributes.enabled[n]);
										}
									}
									if (tmp_node->attributes.data_count <= MDE_DATA_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.data_count; n++)
										{
											if (Mde_LaneDataAttribute_PR_pathEndPointAngle == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] pathEndPointAngle : %ld", n, tmp_node->attributes.data[n].choice.pathEndPointAngle);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointCenter == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointCenter : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointCenter);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointLeft == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointLeft : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointLeft);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointRight == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointRight : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointRight);
											}
											else if (Mde_LaneDataAttribute_PR_laneAngle == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneAngle : %ld", n, tmp_node->attributes.data[n].choice.laneAngle);
											}
											else if (Mde_LaneDataAttribute_PR_speedLimits == tmp_node->attributes.data[n].present)
											{
												if (tmp_node->attributes.data[n].choice.speedLimits_count <= MDE_SPEEDLIMITS_COUNT_MAX)
												{
													for( uint8_t q = 0; q < tmp_node->attributes.data[n].choice.speedLimits_count; q++)
													{
														MOCAR_LOG_DEBUG_FMT("data[%d] type : %ld", q, tmp_node->attributes.data[n].choice.speedLimits[q].type);
														MOCAR_LOG_DEBUG_FMT("data[%d] speed : %ld", q, tmp_node->attributes.data[n].choice.speedLimits[q].speed);
													}
												}
											}
										}
									}
									
									if (SDK_OPTIONAL_EXSIT == tmp_node->attributes.dWidth_active)
									{
										MOCAR_LOG_DEBUG_FMT("dWidth : %ld", tmp_node->attributes.dWidth);
									}
									if (SDK_OPTIONAL_EXSIT == tmp_node->attributes.dElevation_active)
									{
										MOCAR_LOG_DEBUG_FMT("dElevation : %ld", tmp_node->attributes.dElevation);
									}
								}
							}
						}
					}
					else if (Mde_NodeListXY_PR_computed == tmp_lane->nodeList.present)
					{
						MOCAR_LOG_DEBUG_FMT("referenceLaneId : %ld", tmp_lane->nodeList.computed.referenceLaneId);
						if (Mde_ComputedLane__offsetXaxis_PR_small == tmp_lane->nodeList.computed.present_x)
						{							
							MOCAR_LOG_DEBUG_FMT("small_x : %ld", tmp_lane->nodeList.computed.small_x);
						}
 						else if (Mde_ComputedLane__offsetXaxis_PR_large == tmp_lane->nodeList.computed.present_x)
						{
							MOCAR_LOG_DEBUG_FMT("large_x : %ld", tmp_lane->nodeList.computed.large_x);
						}
						if (Mde_ComputedLane__offsetYaxis_PR_small == tmp_lane->nodeList.computed.present_y)
						{							
							MOCAR_LOG_DEBUG_FMT("small_y : %ld", tmp_lane->nodeList.computed.small_y);
						}
 						else if (Mde_ComputedLane__offsetYaxis_PR_large == tmp_lane->nodeList.computed.present_y)
						{
							MOCAR_LOG_DEBUG_FMT("large_y : %ld", tmp_lane->nodeList.computed.large_y);
						}
						
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.rotateXY_active)
						{
							MOCAR_LOG_DEBUG_FMT("rotateXY : %ld", tmp_lane->nodeList.computed.rotateXY);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.scaleXaxis_active)
						{
							MOCAR_LOG_DEBUG_FMT("scaleXaxis : %ld", tmp_lane->nodeList.computed.scaleXaxis);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.scaleYaxis_active)
						{
							MOCAR_LOG_DEBUG_FMT("scaleYaxis : %ld", tmp_lane->nodeList.computed.scaleYaxis);
						}
					}
					
					if (tmp_lane->connectsTo_count <= MDE_CONNECTSTO_COUNT_MAX)
					{
						for( uint8_t m = 0; m < tmp_lane->connectsTo_count; m++)
						{
							MOCAR_LOG_DEBUG_FMT("lane : %ld", tmp_lane->connectsTo[m].connectingLane.lane);
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].connectingLane.maneuver_active)
							{
								memset(tmp_bit_string, 0, 40);
								for(uint8_t n = 0; n < 12; n++)
								{
									tmp_bit_string[n*2] = tmp_lane->connectsTo[m].connectingLane.maneuver_bits[n] + '0';
									tmp_bit_string[n*2 + 1] = ' ';
								}
								MOCAR_LOG_DEBUG_FMT("maneuver_bits : %s", tmp_bit_string);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].remoteIntersection_active)
							{
								if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].remoteIntersection.region_active)
								{
									MOCAR_LOG_DEBUG_FMT("remoteIntersection region: %ld", tmp_lane->connectsTo[m].remoteIntersection.region);
								}
								MOCAR_LOG_DEBUG_FMT("remoteIntersection id: %ld", tmp_lane->connectsTo[m].remoteIntersection.id);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].signalGroup_active)
							{
								MOCAR_LOG_DEBUG_FMT("signalGroup : %ld", tmp_lane->connectsTo[m].signalGroup);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].userClass_active)
							{
								MOCAR_LOG_DEBUG_FMT("userClass : %ld", tmp_lane->connectsTo[m].userClass);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].connectionID_active)
							{
								MOCAR_LOG_DEBUG_FMT("connectionID : %ld", tmp_lane->connectsTo[m].connectionID);
							}
						}
					}
					
					if (tmp_lane->overlays_count <= MDE_OVERLAYS_COUNT_MAX)
					{
						for( uint8_t n = 0; n < tmp_lane->overlays_count; n++)
						{
							MOCAR_LOG_DEBUG_FMT("overlays[%d] : %ld", n, tmp_lane->overlays[n]);
						}
					}
				}
			}
			
			if (tmp_inter->preemptPriorityData_count <= MDE_PREEMPT_PRIORITYdATA_COUNT_MAX)
			{
				for( uint8_t j = 0; j < tmp_inter->preemptPriorityData_count; j++)
				{
					MOCAR_LOG_DEBUG_FMT("regionId[%d] : %ld",j, tmp_inter->preemptPriorityData[j].zone.regionId);
				}
			}
		}
	}

	if (user_map->roadSegments_count <= MDE_ROADSEGMENTS_COUNT_MAX)
	{
		for( uint8_t i = 0; i < user_map->roadSegments_count; i++)
		{
			Mde_RoadSegment_t *tmp_road = &user_map->roadSegments[i];
			MOCAR_LOG_DEBUG_FMT("roadSegments[%d]: ------", i);

			
			if (SDK_OPTIONAL_EXSIT == tmp_road->name_active)
			{				
				MOCAR_LOG_DEBUG_FMT("roadSegments[%d] name : %s", i, tmp_road->name.buf);
			}
//////////////////////////=====================

			if (SDK_OPTIONAL_EXSIT == tmp_road->id.region_active)
			{
				MOCAR_LOG_DEBUG_FMT("region : %ld", tmp_road->id.region);
			}
			MOCAR_LOG_DEBUG_FMT("id : %ld", tmp_road->id.id);
			MOCAR_LOG_DEBUG_FMT("revision : %ld", tmp_road->revision);
			MOCAR_LOG_DEBUG_FMT("lat : %ld", tmp_road->refPoint.lat);
			MOCAR_LOG_DEBUG_FMT("Long : %ld", tmp_road->refPoint.Long);
			if (SDK_OPTIONAL_EXSIT == tmp_road->refPoint.elevation_active)
			{
				MOCAR_LOG_DEBUG_FMT("elevation : %ld", tmp_road->refPoint.elevation);
			}
			if (SDK_OPTIONAL_EXSIT == tmp_road->laneWidth_active)
			{
				MOCAR_LOG_DEBUG_FMT("laneWidth : %ld", tmp_road->laneWidth);
			}
			if (tmp_road->speedLimits_count <= MDE_SPEEDLIMITS_MAX)
			{
				for( uint8_t j = 0; j < tmp_road->speedLimits_count; j++)
				{
					MOCAR_LOG_DEBUG_FMT("speedLimits type : %ld", tmp_road->speedLimits[j].type);
					MOCAR_LOG_DEBUG_FMT("speedLimits speed : %ld", tmp_road->speedLimits[j].speed);
				}
			}
			if (tmp_road->roadLaneSet_count <= MDE_ROAD_LANESET_MAX)
			{
				for( uint8_t j = 0; j < tmp_road->roadLaneSet_count; j++)
				{
					Mde_GenericLane_t * tmp_lane = &tmp_road->roadLaneSet[j];
					MOCAR_LOG_DEBUG_FMT("roadLaneSet[%d]: ------", j);

					MOCAR_LOG_DEBUG_FMT("laneID : %ld", tmp_lane->laneID);
					if (SDK_OPTIONAL_EXSIT == tmp_lane->name_active)
					{
						MOCAR_LOG_DEBUG_FMT("name : %s", tmp_lane->name.buf);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_lane->ingressApproach_active)
					{
						MOCAR_LOG_DEBUG_FMT("ingressApproach : %ld", tmp_lane->ingressApproach);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_lane->egressApproach_active)
					{
						MOCAR_LOG_DEBUG_FMT("egressApproach : %ld", tmp_lane->egressApproach);
					}
					char tmp_bit_string[40] = {0};
					for(uint8_t m = 0; m < 2; m++)
					{
						tmp_bit_string[m*2] = tmp_lane->laneAttributes.directionalUse_bits[m] + '0';
						tmp_bit_string[m*2 + 1] = ' ';
					}
					MOCAR_LOG_DEBUG_FMT("directionalUse_bits : %s", tmp_bit_string);
					memset(tmp_bit_string, 0, 40);
					for(uint8_t m = 0; m < 10; m++)
					{
						tmp_bit_string[m*2] = tmp_lane->laneAttributes.sharedWith_bits[m] + '0';
						tmp_bit_string[m*2 + 1] = ' ';
					}
					MOCAR_LOG_DEBUG_FMT("sharedWith_bits : %s", tmp_bit_string);
					
					if (Mde_LaneTypeAttributes_PR_vehicle == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 8; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.vehicle_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("vehicle_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_crosswalk == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.crosswalk_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("crosswalk_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_bikeLane == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.bikeLane_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("bikeLane_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_sidewalk == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.sidewalk_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("sidewalk_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_median == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.median_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("median_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_striping == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.striping_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("striping_bits : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_trackedVehicle == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.trackedVehicle[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("trackedVehicle : %s", tmp_bit_string);
					}
					else if (Mde_LaneTypeAttributes_PR_parking == tmp_lane->laneAttributes.laneType.present)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 16; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->laneAttributes.laneType.parking_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("parking_bits : %s", tmp_bit_string);
					}
					
					if (SDK_OPTIONAL_EXSIT ==  tmp_lane->maneuvers_active)
					{
						memset(tmp_bit_string, 0, 40);
						for(uint8_t m = 0; m < 12; m++)
						{
							tmp_bit_string[m*2] = tmp_lane->maneuvers_bits[m] + '0';
							tmp_bit_string[m*2 + 1] = ' ';
						}
						MOCAR_LOG_DEBUG_FMT("maneuvers_bits : %s", tmp_bit_string);
					}
					if (Mde_NodeListXY_PR_nodes == tmp_lane->nodeList.present)
					{
						if (tmp_lane->nodeList.NodeSetXY_count <= MDE_NODELISTXY_MAX)
						{
							for (uint32_t m = 0; m < tmp_lane->nodeList.NodeSetXY_count; m++)
							{
								Mde_NodeXY_t * tmp_node = &tmp_lane->nodeList.nodes[m];

								if (Mde_NodeOffsetPointXY_PR_node_XY1 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY1 x : %ld", tmp_node->delta.node_XY1.x);
									MOCAR_LOG_DEBUG_FMT("node_XY1 y : %ld", tmp_node->delta.node_XY1.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY2 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY2 x : %ld", tmp_node->delta.node_XY2.x);
									MOCAR_LOG_DEBUG_FMT("node_XY2 y : %ld", tmp_node->delta.node_XY2.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY3 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY3 x : %ld", tmp_node->delta.node_XY3.x);
									MOCAR_LOG_DEBUG_FMT("node_XY3 y : %ld", tmp_node->delta.node_XY3.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY4 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY4 x : %ld", tmp_node->delta.node_XY4.x);
									MOCAR_LOG_DEBUG_FMT("node_XY4 y : %ld", tmp_node->delta.node_XY4.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY5 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY5 x : %ld", tmp_node->delta.node_XY5.x);
									MOCAR_LOG_DEBUG_FMT("node_XY5 y : %ld", tmp_node->delta.node_XY5.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_XY6 == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_XY6 x : %ld", tmp_node->delta.node_XY6.x);
									MOCAR_LOG_DEBUG_FMT("node_XY6 y : %ld", tmp_node->delta.node_XY6.y);
								}
								else if (Mde_NodeOffsetPointXY_PR_node_LatLon == tmp_node->delta.present)
								{
									MOCAR_LOG_DEBUG_FMT("node_LatLon lon : %ld", tmp_node->delta.node_LatLon.lon);
									MOCAR_LOG_DEBUG_FMT("node_LatLon lat : %ld", tmp_node->delta.node_LatLon.lat);
								}
								
								if (SDK_OPTIONAL_EXSIT == tmp_node->attributes_active)
								{
									if (tmp_node->attributes.localNode_count <= MDE_LOCALNODE_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.localNode_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("localNode[%d] : %ld", n, tmp_node->attributes.localNode[n]);
										}
									}
									if (tmp_node->attributes.disabled_count <= MDE_DISABLED_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.disabled_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("disabled[%d] : %ld", n, tmp_node->attributes.disabled[n]);
										}
									}
									if (tmp_node->attributes.enabled_count <= MDE_ENABLE_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.enabled_count; n++)
										{
											MOCAR_LOG_DEBUG_FMT("enabled[%d] : %ld", n, tmp_node->attributes.enabled[n]);
										}
									}
									if (tmp_node->attributes.data_count <= MDE_DATA_COUNT_MAX)
									{
										for( uint8_t n = 0; n < tmp_node->attributes.data_count; n++)
										{
											if (Mde_LaneDataAttribute_PR_pathEndPointAngle == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] pathEndPointAngle : %ld", n, tmp_node->attributes.data[n].choice.pathEndPointAngle);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointCenter == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointCenter : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointCenter);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointLeft == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointLeft : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointLeft);
											}
											else if (Mde_LaneDataAttribute_PR_laneCrownPointRight == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneCrownPointRight : %ld", n, tmp_node->attributes.data[n].choice.laneCrownPointRight);
											}
											else if (Mde_LaneDataAttribute_PR_laneAngle == tmp_node->attributes.data[n].present)
											{
												MOCAR_LOG_DEBUG_FMT("data[%d] laneAngle : %ld", n, tmp_node->attributes.data[n].choice.laneAngle);
											}
											else if (Mde_LaneDataAttribute_PR_speedLimits == tmp_node->attributes.data[n].present)
											{
												if (tmp_node->attributes.data[n].choice.speedLimits_count <= MDE_SPEEDLIMITS_COUNT_MAX)
												{
													for( uint8_t q = 0; q < tmp_node->attributes.data[n].choice.speedLimits_count; q++)
													{
														MOCAR_LOG_DEBUG_FMT("data[%d] type : %ld", q, tmp_node->attributes.data[n].choice.speedLimits[q].type);
														MOCAR_LOG_DEBUG_FMT("data[%d] speed : %ld", q, tmp_node->attributes.data[n].choice.speedLimits[q].speed);
													}
												}
											}
											
										}
									}
									
									if (SDK_OPTIONAL_EXSIT == tmp_node->attributes.dWidth_active)
									{
										MOCAR_LOG_DEBUG_FMT("dWidth : %ld", tmp_node->attributes.dWidth);
									}
									if (SDK_OPTIONAL_EXSIT == tmp_node->attributes.dElevation_active)
									{
										MOCAR_LOG_DEBUG_FMT("dElevation : %ld", tmp_node->attributes.dElevation);
									}
								}
							}
						}
					}
					else if (Mde_NodeListXY_PR_computed == tmp_lane->nodeList.present)
					{
						MOCAR_LOG_DEBUG_FMT("referenceLaneId : %ld", tmp_lane->nodeList.computed.referenceLaneId);
						if (Mde_ComputedLane__offsetXaxis_PR_small == tmp_lane->nodeList.computed.present_x)
						{							
							MOCAR_LOG_DEBUG_FMT("small_x : %ld", tmp_lane->nodeList.computed.small_x);
						}
 						else if (Mde_ComputedLane__offsetXaxis_PR_large == tmp_lane->nodeList.computed.present_x)
						{
							MOCAR_LOG_DEBUG_FMT("large_x : %ld", tmp_lane->nodeList.computed.large_x);
						}
						if (Mde_ComputedLane__offsetYaxis_PR_small == tmp_lane->nodeList.computed.present_y)
						{							
							MOCAR_LOG_DEBUG_FMT("small_y : %ld", tmp_lane->nodeList.computed.small_y);
						}
 						else if (Mde_ComputedLane__offsetYaxis_PR_large == tmp_lane->nodeList.computed.present_y)
						{
							MOCAR_LOG_DEBUG_FMT("large_y : %ld", tmp_lane->nodeList.computed.large_y);
						}
						
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.rotateXY_active)
						{
							MOCAR_LOG_DEBUG_FMT("rotateXY : %ld", tmp_lane->nodeList.computed.rotateXY);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.scaleXaxis_active)
						{
							MOCAR_LOG_DEBUG_FMT("scaleXaxis : %ld", tmp_lane->nodeList.computed.scaleXaxis);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_lane->nodeList.computed.scaleYaxis_active)
						{
							MOCAR_LOG_DEBUG_FMT("scaleYaxis : %ld", tmp_lane->nodeList.computed.scaleYaxis);
						}
					}
					
					if (tmp_lane->connectsTo_count <= MDE_CONNECTSTO_COUNT_MAX)
					{
						for( uint8_t m = 0; m < tmp_lane->connectsTo_count; m++)
						{
							MOCAR_LOG_DEBUG_FMT("lane : %ld", tmp_lane->connectsTo[m].connectingLane.lane);
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].connectingLane.maneuver_active)
							{
								memset(tmp_bit_string, 0, 40);
								for(uint8_t n = 0; n < 12; n++)
								{
									tmp_bit_string[n*2] = tmp_lane->connectsTo[m].connectingLane.maneuver_bits[n] + '0';
									tmp_bit_string[n*2 + 1] = ' ';
								}
								MOCAR_LOG_DEBUG_FMT("maneuver_bits : %s", tmp_bit_string);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].remoteIntersection_active)
							{
								if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].remoteIntersection.region_active)
								{
									MOCAR_LOG_DEBUG_FMT("remoteIntersection region: %ld", tmp_lane->connectsTo[m].remoteIntersection.region);
								}
								MOCAR_LOG_DEBUG_FMT("remoteIntersection id: %ld", tmp_lane->connectsTo[m].remoteIntersection.id);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].signalGroup_active)
							{
								MOCAR_LOG_DEBUG_FMT("signalGroup : %ld", tmp_lane->connectsTo[m].signalGroup);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].userClass_active)
							{
								MOCAR_LOG_DEBUG_FMT("userClass : %ld", tmp_lane->connectsTo[m].userClass);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_lane->connectsTo[m].connectionID_active)
							{
								MOCAR_LOG_DEBUG_FMT("connectionID : %ld", tmp_lane->connectsTo[m].connectionID);
							}
						}
					}
					
					if (tmp_lane->overlays_count <= MDE_OVERLAYS_COUNT_MAX)
					{
						for( uint8_t n = 0; n < tmp_lane->overlays_count; n++)
						{
							MOCAR_LOG_DEBUG_FMT("overlays[%d] : %ld", n, tmp_lane->overlays[n]);
						}
					}
				}
			}			
		}
	}

	if (SDK_OPTIONAL_EXSIT == user_map->dataParameters_active)
	{
		if (SDK_OPTIONAL_EXSIT == user_map->dataParameters.processMethod_active)
		{
			MOCAR_LOG_DEBUG_FMT("processMethod : %s", user_map->dataParameters.processMethod.buf);
		}
		if (SDK_OPTIONAL_EXSIT == user_map->dataParameters.processAgency_active)
		{
			MOCAR_LOG_DEBUG_FMT("processAgency : %s", user_map->dataParameters.processAgency.buf);
		}
		if (SDK_OPTIONAL_EXSIT == user_map->dataParameters.lastCheckedDate_active)
		{
			MOCAR_LOG_DEBUG_FMT("lastCheckedDate : %s", user_map->dataParameters.lastCheckedDate.buf);
		}
		if (SDK_OPTIONAL_EXSIT == user_map->dataParameters.geoidUsed_active)
		{
			MOCAR_LOG_DEBUG_FMT("geoidUsed : %s", user_map->dataParameters.geoidUsed.buf);
		}
	}
	
	if (user_map->restrictionList_count <= MDE_RESTRICTIONLIST_COUNT_MAX)
	{
		for( uint8_t j = 0; j < user_map->restrictionList_count; j++)
		{
			MOCAR_LOG_DEBUG_FMT("restrictionList[%d] id : %ld", j, user_map->restrictionList[j].id);
			
			if (user_map->restrictionList[j].users_count <= MDE_RESTRICTION_USER_TYPE_MAX)
			{
				for( uint8_t m = 0; m < user_map->restrictionList[j].users_count; m++)
				{
					if (MDE_RestrictionUserType_PR_basicType == user_map->restrictionList[j].users[m].present)
					{
						MOCAR_LOG_DEBUG_FMT("restrictionList[%d] users[%d] basicType : %ld", j, m, user_map->restrictionList[j].users[m].basicType);
					}
				}
			}
		}
	}

}

void v2x_user_map_recv_handle(v2x_msg_map_t* user_map, void* param)
{
    MOCAR_LOG_INFO("rxmsg-MAP: msg count ");
    fprintf(stderr, "rxmsg-MAP: msg count \n");

	v2x_map_rx_data_dump(user_map);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_map(v2x_msg_map_t* usr_map)
{
    if(NULL == usr_map)
    {
    	MOCAR_LOG_ERROR("fill map invalid param");
        return -1;
    }

    static uint8_t msgcount = 0;
	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

	usr_map->timeStamp_active = SDK_OPTIONAL_EXSIT;
	usr_map->timeStamp = minute_of_the_year(tm_time);

	usr_map->msgIssueRevision = msgcount % 128;
    msgcount++;

	usr_map->layerType_active = SDK_OPTIONAL_EXSIT;
	usr_map->layerType = 2;
	usr_map->layerID_active = SDK_OPTIONAL_EXSIT;
	usr_map->layerID = 100;
	usr_map->intersections_count = 2;
	for(uint8_t i = 0; i < usr_map->intersections_count; i++)
	{
		Mde_IntersectionGeometry_t * tmp_inter = &usr_map->intersections[i];

		tmp_inter->intersection_name_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->name.size = 15;
		char tmp_interName[15] = "__inter_name";
		memcpy(tmp_inter->name.buf, tmp_interName, strlen(tmp_interName));
		tmp_inter->name.buf[0] = i + '0';

		tmp_inter->id.region_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->id.region = 10000;
		tmp_inter->id.id = 60000;
		tmp_inter->revision = 127;
		tmp_inter->refPoint.lat = 900000000;
		tmp_inter->refPoint.Long = 1800000000;
		tmp_inter->refPoint.elevation_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->refPoint.elevation = 60000;
		tmp_inter->laneWidth_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneWidth = 30000;
		
		tmp_inter->speedLimits_count = 2;
		tmp_inter->speedLimits[0].type = 5;
		tmp_inter->speedLimits[0].speed = 8000;
		tmp_inter->speedLimits[1].type = 6;
		tmp_inter->speedLimits[1].speed = 8001;
		
		tmp_inter->laneSet_count = 1;
		tmp_inter->laneSet[0].laneID = 250;
		tmp_inter->laneSet[0].name_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].name.size = 4;
		char tmp_name[5] = "lane";
		memcpy(tmp_inter->laneSet[0].name.buf, tmp_name, strlen(tmp_name));
		tmp_inter->laneSet[0].ingressApproach_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].ingressApproach = 10;
		tmp_inter->laneSet[0].egressApproach_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].egressApproach = 11;
		
		tmp_inter->laneSet[0].laneAttributes.directionalUse_bits[1] = 1;
		tmp_inter->laneSet[0].laneAttributes.sharedWith_bits[4] = 1;
		tmp_inter->laneSet[0].laneAttributes.sharedWith_bits[5] = 1;
		tmp_inter->laneSet[0].laneAttributes.sharedWith_bits[6] = 1;
		tmp_inter->laneSet[0].laneAttributes.sharedWith_bits[7] = 1;
		tmp_inter->laneSet[0].laneAttributes.laneType.present = Mde_LaneTypeAttributes_PR_vehicle;
		tmp_inter->laneSet[0].laneAttributes.laneType.vehicle_bits[4] = 1;

		tmp_inter->laneSet[0].maneuvers_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].maneuvers_bits[10] = 1;
		
		tmp_inter->laneSet[0].nodeList.present = Mde_NodeListXY_PR_computed;
		tmp_inter->laneSet[0].nodeList.computed.referenceLaneId = 200;
		tmp_inter->laneSet[0].nodeList.computed.present_x = Mde_ComputedLane__offsetXaxis_PR_small;
		tmp_inter->laneSet[0].nodeList.computed.small_x = 2000;
		tmp_inter->laneSet[0].nodeList.computed.present_y = Mde_ComputedLane__offsetYaxis_PR_small;
		tmp_inter->laneSet[0].nodeList.computed.small_y = 2001;
		tmp_inter->laneSet[0].nodeList.computed.rotateXY_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].nodeList.computed.rotateXY = 10;
		tmp_inter->laneSet[0].nodeList.computed.scaleXaxis_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].nodeList.computed.scaleXaxis = -10;
		tmp_inter->laneSet[0].nodeList.computed.scaleYaxis_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].nodeList.computed.scaleYaxis = -11;
		
		tmp_inter->laneSet[0].connectsTo_count = 2;
		tmp_inter->laneSet[0].connectsTo[0].connectingLane.lane = 250;
		tmp_inter->laneSet[0].connectsTo[0].connectingLane.maneuver_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].connectingLane.maneuver_bits[4] = 1;
		tmp_inter->laneSet[0].connectsTo[0].connectingLane.maneuver_bits[6] = 1;
		tmp_inter->laneSet[0].connectsTo[0].remoteIntersection_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].remoteIntersection.region_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].remoteIntersection.region = 10;
		tmp_inter->laneSet[0].connectsTo[0].remoteIntersection.id = 11;
		tmp_inter->laneSet[0].connectsTo[0].signalGroup_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].signalGroup = 12;
		tmp_inter->laneSet[0].connectsTo[0].userClass_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].userClass = 13;
		tmp_inter->laneSet[0].connectsTo[0].connectionID_active = SDK_OPTIONAL_EXSIT;
		tmp_inter->laneSet[0].connectsTo[0].connectionID = 14;
		tmp_inter->laneSet[0].connectsTo[1].connectingLane.lane = 251;

		tmp_inter->laneSet[0].overlays_count = 3;
		tmp_inter->laneSet[0].overlays[0] = 10;
		tmp_inter->laneSet[0].overlays[1] = 20;
		tmp_inter->laneSet[0].overlays[2] = 30;

		/*tmp_inter->preemptPriorityData_count = 1;
		tmp_inter->preemptPriorityData[0].zone.regionId = 2;*/
		
	}
	
	usr_map->roadSegments_count = 1;
	usr_map->roadSegments[0].name_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].name.size = 5;
	char tmp_road_name[5] = "road";
	memcpy(usr_map->roadSegments[0].name.buf, tmp_road_name, 4);
	
	usr_map->roadSegments[0].id.region_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].id.region = 65;
	usr_map->roadSegments[0].id.id = 66;
	usr_map->roadSegments[0].revision = 120;
	usr_map->roadSegments[0].refPoint.lat = 900000000;
	usr_map->roadSegments[0].refPoint.Long = 1800000000;
	usr_map->roadSegments[0].refPoint.elevation_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].refPoint.elevation = 60000;
	usr_map->roadSegments[0].laneWidth_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].laneWidth = 32323;
	usr_map->roadSegments[0].speedLimits_count = 1;
	usr_map->roadSegments[0].speedLimits[0].type = 3;
	usr_map->roadSegments[0].speedLimits[0].speed = 8000;
	usr_map->roadSegments[0].roadLaneSet_count = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneID = 200;
	usr_map->roadSegments[0].roadLaneSet[0].name_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].name.size = 9;
	memcpy(usr_map->roadSegments[0].roadLaneSet[0].name.buf, "road_name", 9);
	usr_map->roadSegments[0].roadLaneSet[0].ingressApproach_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].ingressApproach = 9;
	usr_map->roadSegments[0].roadLaneSet[0].egressApproach_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].egressApproach = 8;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.directionalUse_bits[1] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.sharedWith_bits[4] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.sharedWith_bits[5] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.sharedWith_bits[6] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.sharedWith_bits[7] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.laneType.present = Mde_LaneTypeAttributes_PR_vehicle;
	usr_map->roadSegments[0].roadLaneSet[0].laneAttributes.laneType.vehicle_bits[4] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].maneuvers_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].maneuvers_bits[1] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.present = Mde_NodeListXY_PR_nodes;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.NodeSetXY_count = 2;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].delta.present = Mde_NodeOffsetPointXY_PR_node_XY2;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].delta.node_XY2.x = -1000;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].delta.node_XY2.y = 1000;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.localNode_count = 1;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.localNode[0] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.disabled_count = 2;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.disabled[0] = 2;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.disabled[1] = 2;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.enabled_count = 3;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.enabled[0] = 3;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.enabled[1] = 3;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.enabled[2] = 3;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.data_count = 1;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.data[0].present = Mde_LaneDataAttribute_PR_pathEndPointAngle;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.data[0].choice.pathEndPointAngle = 100;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.dWidth_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.dWidth = 500;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.dElevation_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[0].attributes.dElevation = 501;	
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[1].delta.present = Mde_NodeOffsetPointXY_PR_node_XY3;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[1].delta.node_XY3.x = -99;
	usr_map->roadSegments[0].roadLaneSet[0].nodeList.nodes[1].delta.node_XY3.y = 99;

	usr_map->roadSegments[0].roadLaneSet[0].connectsTo_count = 1;	
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectingLane.lane = 250;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectingLane.maneuver_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectingLane.maneuver_bits[4] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectingLane.maneuver_bits[6] = 1;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].remoteIntersection_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].remoteIntersection.region_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].remoteIntersection.region = 10;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].remoteIntersection.id = 11;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].signalGroup_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].signalGroup = 12;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].userClass_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].userClass = 13;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectionID_active = SDK_OPTIONAL_EXSIT;
	usr_map->roadSegments[0].roadLaneSet[0].connectsTo[0].connectionID = 14;
	
	usr_map->roadSegments[0].roadLaneSet[0].overlays_count = 1;
	usr_map->roadSegments[0].roadLaneSet[0].overlays[0] = 210;

	usr_map->dataParameters_active = SDK_OPTIONAL_EXSIT;
	usr_map->dataParameters.processMethod_active = SDK_OPTIONAL_EXSIT;
	usr_map->dataParameters.processMethod.size = 9;
	memcpy(usr_map->dataParameters.processMethod.buf, "moth_name", 9);
	usr_map->dataParameters.processAgency_active = SDK_OPTIONAL_EXSIT;
	usr_map->dataParameters.processAgency.size = 9;
	memcpy(usr_map->dataParameters.processAgency.buf, "agen_name", 9);
	usr_map->dataParameters.lastCheckedDate_active = SDK_OPTIONAL_EXSIT;
	usr_map->dataParameters.lastCheckedDate.size = 10;
	memcpy(usr_map->dataParameters.lastCheckedDate.buf, "2021-01-11", 10);
	usr_map->dataParameters.geoidUsed_active = SDK_OPTIONAL_EXSIT;
	usr_map->dataParameters.geoidUsed.size = 9;
	memcpy(usr_map->dataParameters.geoidUsed.buf, "blackpink", 9);

	usr_map->restrictionList_count = 1;
	usr_map->restrictionList[0].id = 250;
	usr_map->restrictionList[0].users_count = 1;
	usr_map->restrictionList[0].users[0].present = MDE_RestrictionUserType_PR_basicType;
	usr_map->restrictionList[0].users[0].basicType = 3;

    return 0;    
}

void v2x_map_tx(void)
{
    int ret = -1;
	v2x_msg_map_t * user_map = calloc(1, sizeof(v2x_msg_map_t));
	if (NULL == user_map)
	{
		MOCAR_LOG_ERROR("usr map calloc error");
		return;
	}

    ret = v2x_fill_usr_map(user_map);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("user map fill fail\n");
		free(user_map);
		user_map = NULL;
        return;
    }
	
    MOCAR_LOG_INFO("map fill success!");
    ret = mde_v2x_map_send(user_map, NULL, 32);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("map send fail\n");
		free(user_map);
		user_map = NULL;
        return;
    }
	
    MOCAR_LOG_INFO("txmsg-MAP: send msg successed, sequence ");
    fprintf(stderr, "txmsg-MAP: send msg successed, sequence \n");
	
	free(user_map);
	user_map = NULL;
}

int main(int argc, char* argv[])
{
    int ret = -1;

    ret = mde_v2x_init(0);
    if(0 != ret)
    {
        printf("cv2x init fail\n");
        return -1;    
    }

    mde_v2x_map_recv_handle_register(v2x_user_map_recv_handle); 
    MOCAR_LOG_INFO("map recv handle register\n");
    
    while(1)
    {
        v2x_map_tx();
        usleep(1000*1000);
    }
    
    return 0;
}
