#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>

unsigned int g_vehicle_id = -1;

#define BSM_OPTIONAL_CONF	("./bsm_optional.conf")

static int8_t g_veh_safety_ext = 0;
static int8_t g_spe_veh_ext = 1;
static int8_t g_sup_veh_ext = 0;

void v2x_bsm_rx_data_dump(const v2x_msg_bsm_t* user_bsm)
{
	if (NULL == user_bsm)
	{
		MOCAR_LOG_ERROR_FMT("Dump param is null");
		return;
	}

    unsigned int vhId = 0;
    vhId = *((unsigned int *)(user_bsm->temp_id));

    MOCAR_LOG_INFO_FMT("rxmsg-BSM: vehicle id %u, message count %d", vhId, user_bsm->msg_count);

    MOCAR_LOG_DEBUG_FMT("sec mark: %d", user_bsm->sec_mark);
    MOCAR_LOG_DEBUG_FMT("latitude: %f", user_bsm->latitude);
    MOCAR_LOG_DEBUG_FMT("longitude: %f", user_bsm->longitude);
    MOCAR_LOG_DEBUG_FMT("elevation: %f", user_bsm->elevation);
    MOCAR_LOG_DEBUG_FMT("semimajor: %d", user_bsm->pos_accuracy.semimajor);
    MOCAR_LOG_DEBUG_FMT("semiminor: %d", user_bsm->pos_accuracy.semiminor);
    MOCAR_LOG_DEBUG_FMT("orientation: %d", user_bsm->pos_accuracy.orientation);

    MOCAR_LOG_DEBUG_FMT("transmission_state: %d", user_bsm->transmission_state);
    MOCAR_LOG_DEBUG_FMT("speed: %d", user_bsm->speed);
    MOCAR_LOG_DEBUG_FMT("heading: %d", user_bsm->heading);
    MOCAR_LOG_DEBUG_FMT("angle: %d", user_bsm->angle);
    MOCAR_LOG_DEBUG_FMT("long_accel: %d", user_bsm->long_accel);
    MOCAR_LOG_DEBUG_FMT("lat_accel: %d", user_bsm->lat_accel);
    MOCAR_LOG_DEBUG_FMT("vert_accel: %d", user_bsm->vert_accel);
    MOCAR_LOG_DEBUG_FMT("yaw_rate: %d", user_bsm->yaw_rate);

    MOCAR_LOG_DEBUG_FMT("wheel_brakes: %d %d %d %d %d ", user_bsm->wheel_brakes[0], user_bsm->wheel_brakes[1], user_bsm->wheel_brakes[2],
									 user_bsm->wheel_brakes[3], user_bsm->wheel_brakes[4]);
    MOCAR_LOG_DEBUG_FMT("traction: %d", user_bsm->traction);
    MOCAR_LOG_DEBUG_FMT("abs: %d", user_bsm->abs);
    MOCAR_LOG_DEBUG_FMT("scs: %d", user_bsm->scs);
    MOCAR_LOG_DEBUG_FMT("brake_boost: %d", user_bsm->brake_boost);
    MOCAR_LOG_DEBUG_FMT("aux_brakes: %d", user_bsm->aux_brakes);
    MOCAR_LOG_DEBUG_FMT("width: %d", user_bsm->width);
    MOCAR_LOG_DEBUG_FMT("length: %d", user_bsm->length);

	//partII
	MOCAR_LOG_DEBUG_FMT("======  veh safety ext count: %d  ======", user_bsm->veh_safety_ext_count);
	for(uint8_t i = 0; i < user_bsm->veh_safety_ext_count; i++)
	{
		MOCAR_LOG_DEBUG_FMT("veh safety ext[%d] is following: -----", i);
		if (user_bsm->veh_safe_ext[i].event_is_exist == SDK_OPTIONAL_EXSIT)
		{
			uint8_t show_buf[30] = {0};
			for(uint8_t j = 0; j < 13; j++)
			{
				show_buf[j*2] = 48 + (user_bsm->veh_safe_ext[i].events[j]);
				show_buf[j*2+1] = ' ';
			}
			MOCAR_LOG_DEBUG_FMT("event is %s", show_buf);
		}
		
		if (user_bsm->veh_safe_ext[i].pathHistory_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition_is_exist)
			{
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime_is_exist)
				{
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.year_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_year: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.year);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.month_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_month: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.month);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.day_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_day: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.day);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.hour_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_hour: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.hour);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.minute_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_minute: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.minute);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.second_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_second: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.second);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.offset_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("utcTime_offset: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.utcTime.offset);
					}
				}
				
				MOCAR_LOG_DEBUG_FMT("initialPosition long: %f", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.Long);
				MOCAR_LOG_DEBUG_FMT("initialPosition lat: %f", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.lat);
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.elevation_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition elevation: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.elevation);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.heading_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition heading: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.heading);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.transmission_Speed_are_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition transmisson: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.transmisson);
					MOCAR_LOG_DEBUG_FMT("initialPosition speed: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.speed);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.posAccuracy_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition semimajor: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.posAccuracy.semimajor);
					MOCAR_LOG_DEBUG_FMT("initialPosition semiminor: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.posAccuracy.semiminor);
					MOCAR_LOG_DEBUG_FMT("initialPosition orientation: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.posAccuracy.orientation);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.timeConfidence_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition timeConfidence: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.timeConfidence);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.pos_elevation_are_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition posConfidence pos: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.pos);
					MOCAR_LOG_DEBUG_FMT("initialPosition posConfidence elevation: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.posC_elevation);
				}
				if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.initialPosition.speedConfidence_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("initialPosition heading: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.speed_heading_throttle.heading);
					MOCAR_LOG_DEBUG_FMT("initialPosition speed: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.speed_heading_throttle.speed);
					MOCAR_LOG_DEBUG_FMT("initialPosition throttle: %d", user_bsm->veh_safe_ext[i].pathHistory.initialPosition.speed_heading_throttle.throttle);
				}
			}

			if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.currGNSSsts_is_exist)
			{
				uint8_t show_buf[30] = {0};
				for(uint8_t j = 0; j < 8; j++)
				{
					show_buf[j*2] = 48 + (user_bsm->veh_safe_ext[i].pathHistory.currGNSSstatus[j]);
					show_buf[j*2+1] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("currGNSSstatus is %s", show_buf);
			}

			if ((1 <= user_bsm->veh_safe_ext[i].pathHistory.ph_count) && (user_bsm->veh_safe_ext[i].pathHistory.ph_count <= 23))
			{
				MOCAR_LOG_DEBUG_FMT("pathHistory count: %d", user_bsm->veh_safe_ext[i].pathHistory.ph_count);
				for(uint8_t j = 0; j < user_bsm->veh_safe_ext[i].pathHistory.ph_count; j++)
				{
					MOCAR_LOG_DEBUG_FMT("pathHistory[%d] is following: -----", j);
					MOCAR_LOG_DEBUG_FMT("pathHistory latOffset is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].latOffset);
					MOCAR_LOG_DEBUG_FMT("pathHistory lonOffset is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].lonOffset);
					MOCAR_LOG_DEBUG_FMT("pathHistory elevationOffset is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].elevationOffset);
					MOCAR_LOG_DEBUG_FMT("pathHistory timeOffset is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].timeOffset);
					
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.path_his[j].speed_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("pathHistory speed is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].speed);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.path_his[j].posAccuracy_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("pathHistory posAccuracy semimajor is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].posAccuracy.semimajor);
						MOCAR_LOG_DEBUG_FMT("pathHistory posAccuracy semiminor is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].posAccuracy.semiminor);
						MOCAR_LOG_DEBUG_FMT("pathHistory posAccuracy orientation is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].posAccuracy.orientation);
					}
					if (SDK_OPTIONAL_EXSIT == user_bsm->veh_safe_ext[i].pathHistory.path_his[j].heading_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("pathHistory heading is %d", user_bsm->veh_safe_ext[i].pathHistory.path_his[j].heading);
					}
				}
			}
		}

		if (user_bsm->veh_safe_ext[i].pathPrediction_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("pathPrediction radiusOfCurve is %d", user_bsm->veh_safe_ext[i].pathPrediction.radiusOfCurve);
			MOCAR_LOG_DEBUG_FMT("pathPrediction confidence is %d", user_bsm->veh_safe_ext[i].pathPrediction.confidence);
		}
		
		if (user_bsm->veh_safe_ext[i].ExteriorLights_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			uint8_t show_buf[30] = {0};
			for(uint8_t j = 0; j < 9; j++)
			{
				show_buf[j*2] = 48 + (user_bsm->veh_safe_ext[i].lights[j]);
				show_buf[j*2+1] = ' ';
			}
			MOCAR_LOG_DEBUG_FMT("lights is %s", show_buf);			
		}		
	}

	MOCAR_LOG_DEBUG_FMT("======  spe_veh_ext count: %d  ======", user_bsm->spe_veh_ext_count);
	for (uint8_t i = 0; i < user_bsm->spe_veh_ext_count; i++)
	{
		MOCAR_LOG_DEBUG_FMT("spe_veh_ext[%d] is following: -----", i);
		if (user_bsm->spe_veh_ext[i].vehicleAlerts_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("sspRights is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.sspRights);
			MOCAR_LOG_DEBUG_FMT("sirenUse is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.sirenUse);
			MOCAR_LOG_DEBUG_FMT("lightsUse is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.lightsUse);
			MOCAR_LOG_DEBUG_FMT("multi is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.multi);
			
			if (user_bsm->spe_veh_ext[i].vehicleAlerts.sspRights_events_are_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("events sspRights is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.events_sspRights);
				
				uint8_t show_buf[30] = {0};
				for(uint8_t j = 0; j < 6; j++)
				{
					show_buf[j*2] = 48 + (user_bsm->spe_veh_ext[i].vehicleAlerts.events[j]);
					show_buf[j*2+1] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("events is %s", show_buf);				
			}

			if (user_bsm->spe_veh_ext[i].vehicleAlerts.responseType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("responseType is %d", user_bsm->spe_veh_ext[i].vehicleAlerts.responseType);
			}
		}
		
		if (user_bsm->spe_veh_ext[i].description_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("typeEvent is %d", user_bsm->spe_veh_ext[i].description.typeEvent);

			if ((1<= user_bsm->spe_veh_ext[i].description.description_size) && (user_bsm->spe_veh_ext[i].description.description_size <= 8))
			{
				uint8_t show_buf[30] = {0};
				for(uint8_t j = 0; j < user_bsm->spe_veh_ext[i].description.description_size; j++)
				{
					memcpy(&show_buf[j*3], &user_bsm->spe_veh_ext[i].description.description[j], sizeof(uint16_t));
					show_buf[j*3+2] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("%d count. description is: %s", user_bsm->spe_veh_ext[i].description.description_size, show_buf);	
			}

			if (user_bsm->spe_veh_ext[i].description.priority_size == 1)
			{
				MOCAR_LOG_DEBUG_FMT("priority is: %d", user_bsm->spe_veh_ext[i].description.priority[0]);	
			}
			
			if (user_bsm->spe_veh_ext[i].description.heading_is_exsit == 1)
			{
				uint8_t show_buf[30] = {0};
				for(uint8_t j = 0; j < 16; j++)
				{
					show_buf[j*2] = 48 + (user_bsm->spe_veh_ext[i].description.heading[j]);
					show_buf[j*2+1] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("heading is: %s", show_buf);
			}

			if (user_bsm->spe_veh_ext[i].description.extent_is_exsit == 1)
			{
				MOCAR_LOG_DEBUG_FMT("extent is %d", user_bsm->spe_veh_ext[i].description.extent);
			}			
		}
		
		if (user_bsm->spe_veh_ext[i].trailers_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("sspRights is %d", user_bsm->spe_veh_ext[i].trailers.sspRights);
			MOCAR_LOG_DEBUG_FMT("pivotOffset is %d", user_bsm->spe_veh_ext[i].trailers.pivotOffset);
			MOCAR_LOG_DEBUG_FMT("pivotAngle is %d", user_bsm->spe_veh_ext[i].trailers.pivotAngle);
			MOCAR_LOG_DEBUG_FMT("pivots is %d", user_bsm->spe_veh_ext[i].trailers.pivots);

			if ((1 <= user_bsm->spe_veh_ext[i].trailers.units_size) && (user_bsm->spe_veh_ext[i].trailers.units_size <= 8))
			{
				for (uint8_t j = 0; j < user_bsm->spe_veh_ext[i].trailers.units_size; j++)
				{
					MOCAR_LOG_DEBUG_FMT("units[%d] is following: -----", j);
					MOCAR_LOG_DEBUG_FMT("isDolly is %d", user_bsm->spe_veh_ext[i].trailers.units[j].isDolly);
					MOCAR_LOG_DEBUG_FMT("width is %d", user_bsm->spe_veh_ext[i].trailers.units[j].width);
					MOCAR_LOG_DEBUG_FMT("length is %d", user_bsm->spe_veh_ext[i].trailers.units[j].length);
					
					if (user_bsm->spe_veh_ext[i].trailers.units[j].height_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("height is %d", user_bsm->spe_veh_ext[i].trailers.units[j].height);
					}
					if (user_bsm->spe_veh_ext[i].trailers.units[j].mass_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("mass is %d", user_bsm->spe_veh_ext[i].trailers.units[j].mass);
					}
					if (user_bsm->spe_veh_ext[i].trailers.units[j].bumperHeights_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("bumperHeights_front is %d", user_bsm->spe_veh_ext[i].trailers.units[j].bumperHeights_front);
						MOCAR_LOG_DEBUG_FMT("bumperHeights_rear is %d", user_bsm->spe_veh_ext[i].trailers.units[j].bumperHeights_rear);
					}
					if (user_bsm->spe_veh_ext[i].trailers.units[j].centerOfGravity_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("centerOfGravity is %d", user_bsm->spe_veh_ext[i].trailers.units[j].centerOfGravity);
					}
					
					MOCAR_LOG_DEBUG_FMT("frontPivot pivotOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].frontPivot.pivotOffset);
					MOCAR_LOG_DEBUG_FMT("frontPivot pivotAngle is %d", user_bsm->spe_veh_ext[i].trailers.units[j].frontPivot.pivotAngle);
					MOCAR_LOG_DEBUG_FMT("frontPivot pivots is %d", user_bsm->spe_veh_ext[i].trailers.units[j].frontPivot.pivots);

					if (user_bsm->spe_veh_ext[i].trailers.units[j].rearPivot_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("rear pivotOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].rearPivot.pivotOffset);
						MOCAR_LOG_DEBUG_FMT("rear pivotAngle is %d", user_bsm->spe_veh_ext[i].trailers.units[j].rearPivot.pivotAngle);
						MOCAR_LOG_DEBUG_FMT("rear pivots is %d", user_bsm->spe_veh_ext[i].trailers.units[j].rearPivot.pivots);
					}
					if (user_bsm->spe_veh_ext[i].trailers.units[j].rearWheelOffset_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("rearWheelOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].rearWheelOffset);
					}
					
					MOCAR_LOG_DEBUG_FMT("positionOffset_x is %d", user_bsm->spe_veh_ext[i].trailers.units[j].positionOffset_x);
					MOCAR_LOG_DEBUG_FMT("positionOffset_y is %d", user_bsm->spe_veh_ext[i].trailers.units[j].positionOffset_y);

					if (user_bsm->spe_veh_ext[i].trailers.units[j].elevationOffset_is_exsit == SDK_OPTIONAL_EXSIT)
					{
						MOCAR_LOG_DEBUG_FMT("elevationOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].elevationOffset);
					}

					if ((1 <= user_bsm->spe_veh_ext[i].trailers.units[j].crumbData_size) && (user_bsm->spe_veh_ext[i].trailers.units[j].crumbData_size <= 23))
					{
						for (uint8_t k = 0; k < user_bsm->spe_veh_ext[i].trailers.units[j].crumbData_size; k++)	
						{
							MOCAR_LOG_DEBUG_FMT("crumbData[%d] is following: -----", k);
							MOCAR_LOG_DEBUG_FMT("pivotAngle is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].pivotAngle);
							MOCAR_LOG_DEBUG_FMT("timeOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].timeOffset);
							MOCAR_LOG_DEBUG_FMT("positionOffset_x is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].positionOffset_x);
							MOCAR_LOG_DEBUG_FMT("positionOffset_y is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].positionOffset_y);
							
							if (user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].elevationOffset_is_exsit == SDK_OPTIONAL_EXSIT)
							{
								MOCAR_LOG_DEBUG_FMT("elevationOffset is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].elevationOffset);
							}
							if (user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].heading_is_exsit == SDK_OPTIONAL_EXSIT)
							{
								MOCAR_LOG_DEBUG_FMT("heading is %d", user_bsm->spe_veh_ext[i].trailers.units[j].crumbData[k].heading);
							}
						}
					}
				}
				
			}
			
		}
		
	}

	MOCAR_LOG_DEBUG_FMT("======  sup_veh_ext count: %d  ======", user_bsm->sup_veh_ext_count);
	for (uint8_t i = 0; i < user_bsm->sup_veh_ext_count; i++)
	{
		MOCAR_LOG_DEBUG_FMT("sup_veh_ext[%d] is following: -----", i);
		if (user_bsm->sup_veh_ext[i].classification_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("classification is %d", user_bsm->sup_veh_ext[i].classification);
		}
		if (user_bsm->sup_veh_ext[i].classDetails_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			if (user_bsm->sup_veh_ext[i].classDetails.keyType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("keyType is %d", user_bsm->sup_veh_ext[i].classDetails.keyType);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.role_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("role is %d", user_bsm->sup_veh_ext[i].classDetails.role);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.iso3883_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("iso3883 is %d", user_bsm->sup_veh_ext[i].classDetails.iso3883);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.hpmsType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("hpmsType is %d", user_bsm->sup_veh_ext[i].classDetails.hpmsType);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.vehicleType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("vehicleType is %d", user_bsm->sup_veh_ext[i].classDetails.vehicleType);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.responseEquip_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("responseEquip is %d", user_bsm->sup_veh_ext[i].classDetails.responseEquip);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.responderType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("responderType is %d", user_bsm->sup_veh_ext[i].classDetails.responderType);
			}
			if (user_bsm->sup_veh_ext[i].classDetails.fuelType_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("fuelType is %d", user_bsm->sup_veh_ext[i].classDetails.fuelType);
			}
		}
		
		if (user_bsm->sup_veh_ext[i].vehicleData_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			if (user_bsm->sup_veh_ext[i].vehicleData.height_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("height is %d", user_bsm->sup_veh_ext[i].vehicleData.height);
			}
			if (user_bsm->sup_veh_ext[i].vehicleData.bumpers_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("bumpers_front is %d", user_bsm->sup_veh_ext[i].vehicleData.bumpers_front);
				MOCAR_LOG_DEBUG_FMT("bumpers_rear is %d", user_bsm->sup_veh_ext[i].vehicleData.bumpers_rear);
			}
			if (user_bsm->sup_veh_ext[i].vehicleData.mass_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("mass is %d", user_bsm->sup_veh_ext[i].vehicleData.mass);
			}
			if (user_bsm->sup_veh_ext[i].vehicleData.trailerWeight_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("trailerWeight is %d", user_bsm->sup_veh_ext[i].vehicleData.trailerWeight);
			}			
		}
		
		if (user_bsm->sup_veh_ext[i].weatherReport_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("isRaining is %d", user_bsm->sup_veh_ext[i].weatherReport.isRaining);
			if (user_bsm->sup_veh_ext[i].weatherReport.rainRate_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("rainRate is %d", user_bsm->sup_veh_ext[i].weatherReport.rainRate);
			}
			if (user_bsm->sup_veh_ext[i].weatherReport.precipSituation_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("precipSituation is %d", user_bsm->sup_veh_ext[i].weatherReport.precipSituation);
			}
			if (user_bsm->sup_veh_ext[i].weatherReport.solarRadiation_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("solarRadiation is %d", user_bsm->sup_veh_ext[i].weatherReport.solarRadiation);
			}
			if (user_bsm->sup_veh_ext[i].weatherReport.friction_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("friction is %d", user_bsm->sup_veh_ext[i].weatherReport.friction);
			}
			if (user_bsm->sup_veh_ext[i].weatherReport.roadFriction_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("roadFriction is %d", user_bsm->sup_veh_ext[i].weatherReport.roadFriction);
			}
		}

		if (user_bsm->sup_veh_ext[i].weatherProbe_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			if (user_bsm->sup_veh_ext[i].weatherProbe.airTemp_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("airTemp is %d", user_bsm->sup_veh_ext[i].weatherProbe.airTemp);
			}
			if (user_bsm->sup_veh_ext[i].weatherProbe.airPressure_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("airPressure is %d", user_bsm->sup_veh_ext[i].weatherProbe.airPressure);
			}
			if (user_bsm->sup_veh_ext[i].weatherProbe.rainRates_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("wiperSet_statusFront is %d", user_bsm->sup_veh_ext[i].weatherProbe.rainRates.statusFront);
				MOCAR_LOG_DEBUG_FMT("wiperSet_rateFront is %d", user_bsm->sup_veh_ext[i].weatherProbe.rainRates.rateFront);
				
				if (user_bsm->sup_veh_ext[i].weatherProbe.rainRates.statusRear_is_exsit == SDK_OPTIONAL_EXSIT)
				{
					MOCAR_LOG_DEBUG_FMT("wiperSet_statusRear is %d", user_bsm->sup_veh_ext[i].weatherProbe.rainRates.statusRear);
				}
				if (user_bsm->sup_veh_ext[i].weatherProbe.rainRates.rateRear_is_exsit == SDK_OPTIONAL_EXSIT)
				{
					MOCAR_LOG_DEBUG_FMT("wiperSet_rateRear is %d", user_bsm->sup_veh_ext[i].weatherProbe.rainRates.rateRear);
				}
			}
		}

		if (user_bsm->sup_veh_ext[i].obstacle_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("obDist is %d", user_bsm->sup_veh_ext[i].obstacle.obDist);
			MOCAR_LOG_DEBUG_FMT("obDirect is %d", user_bsm->sup_veh_ext[i].obstacle.obDirect);
			
			if (user_bsm->sup_veh_ext[i].obstacle.description_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("description is %d", user_bsm->sup_veh_ext[i].obstacle.description);
			}
			if (user_bsm->sup_veh_ext[i].obstacle.locationDetails_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("locationDetails is %d", user_bsm->sup_veh_ext[i].obstacle.locationDetails);
			}
			
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.year_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_year: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.year);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.month_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_month: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.month);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.day_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_day: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.day);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.hour_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_hour: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.hour);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.minute_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_minute: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.minute);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.second_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_second: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.second);
			}
			if (SDK_OPTIONAL_EXSIT == user_bsm->sup_veh_ext[i].obstacle.dateTime.offset_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_offset: %d", user_bsm->sup_veh_ext[i].obstacle.dateTime.offset);
			}

			/////////////////////////////////////
			if (user_bsm->sup_veh_ext[i].obstacle.vertEvent_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				uint8_t show_buf[30] = {0};
				for(uint8_t j = 0; j < 5; j++)
				{
					show_buf[j*2] = 48 + (user_bsm->sup_veh_ext[i].obstacle.vertEvent[j]);
					show_buf[j*2+1] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("vertEvent is: %s", show_buf);
			}
		}

		if (user_bsm->sup_veh_ext[i].status_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			MOCAR_LOG_DEBUG_FMT("statusDetails: %d", user_bsm->sup_veh_ext[i].status.statusDetails);
			
			if (user_bsm->sup_veh_ext[i].status.locationDetails_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				MOCAR_LOG_DEBUG_FMT("locationDetails: %d", user_bsm->sup_veh_ext[i].status.locationDetails);
			}
		}

		if ((1 <= user_bsm->sup_veh_ext[i].speedReports_size) && (user_bsm->sup_veh_ext[i].speedReports_size <= 20))
		{
			uint8_t show_buf[50] = {0};
			for(uint8_t j = 0; j < user_bsm->sup_veh_ext[i].speedReports_size; j++)
			{
				show_buf[j*2] = 48 + (user_bsm->sup_veh_ext[i].speedReports[j]);
				show_buf[j*2+1] = ' ';
			}
			MOCAR_LOG_DEBUG_FMT("speedReports is: %s", show_buf);			
		}
		
		if (user_bsm->sup_veh_ext[i].theRTCM_is_exsit == SDK_OPTIONAL_EXSIT)
		{
			if (user_bsm->sup_veh_ext[i].theRTCM.rtcmHeader_is_exsit == SDK_OPTIONAL_EXSIT)
			{
				uint8_t show_buf[50] = {0};
				for(uint8_t j = 0; j < 8; j++)
				{
					show_buf[j*2] = 48 + (user_bsm->sup_veh_ext[i].theRTCM.rtcmHeader.status[j]);
					show_buf[j*2+1] = ' ';
				}
				MOCAR_LOG_DEBUG_FMT("rtcmHeader_status is: %s", show_buf);
				
				MOCAR_LOG_DEBUG_FMT("antOffsetX: %d", user_bsm->sup_veh_ext[i].theRTCM.rtcmHeader.antOffsetX);
				MOCAR_LOG_DEBUG_FMT("antOffsetY: %d", user_bsm->sup_veh_ext[i].theRTCM.rtcmHeader.antOffsetY);
				MOCAR_LOG_DEBUG_FMT("antOffsetZ: %d", user_bsm->sup_veh_ext[i].theRTCM.rtcmHeader.antOffsetZ);
			}

			if ((1 <= user_bsm->sup_veh_ext[i].theRTCM.msgs_size) && (user_bsm->sup_veh_ext[i].theRTCM.msgs_size <= 5))
			{
				for(uint8_t j = 0; j < user_bsm->sup_veh_ext[i].theRTCM.msgs_size; j++)
				{
					MOCAR_LOG_DEBUG_FMT("theRTCM_msg[%d]: %s", j, user_bsm->sup_veh_ext[i].theRTCM.msgs[j]);
				}
			}
		}
	}

}

void v2x_user_bsm_recv_handle(v2x_msg_bsm_t* user_bsm, void* param)
{

	unsigned int vhId = 0;
    vhId = *((unsigned int *)(user_bsm->temp_id));
	
	printf("rxmsg-BSM: receive msg success, vhId:%u, sequence %u\n", vhId, user_bsm->msg_count);
	v2x_bsm_rx_data_dump(user_bsm);
}

int v2x_fill_usr_bsm(v2x_msg_bsm_t* usr_bsm)
{
    static uint32_t msgcount = 0;
    
    if (NULL == usr_bsm)
    {
        MOCAR_LOG_ERROR_FMT("invaild parameter");
        return -1;
    }
     
    usr_bsm->msg_count = msgcount;
    msgcount++;
    if(msgcount > 127)
    {
        msgcount = 0;   
    }

    memcpy(usr_bsm->temp_id, &g_vehicle_id, sizeof(usr_bsm->temp_id));

    time_t bsm_time   = time(NULL);
    struct tm* utc_time  = localtime(&bsm_time);
    //usr_bsm->unix_time = time(NULL);
    //bsm_time = (time_t)usr_bsm->unix_time;
    //utc_time = localtime(&bsm_time);
    usr_bsm->sec_mark = (utc_time->tm_sec + bsm_time - (uint32_t)bsm_time) * 1000;

    usr_bsm->latitude  = 2;  //30.1234567;
    usr_bsm->longitude = 3;  //121.1234567;
    usr_bsm->elevation = 4;  //60;

	usr_bsm->pos_accuracy.semimajor = 5;
	usr_bsm->pos_accuracy.semiminor = 6;
	usr_bsm->pos_accuracy.orientation = 7;
	
    usr_bsm->transmission_state = 7; // unavailable

    usr_bsm->speed   = 8;   
    usr_bsm->heading = 9;
    usr_bsm->angle   = -10;

    usr_bsm->long_accel = 11;
    usr_bsm->lat_accel  = 12;
    usr_bsm->vert_accel = 13;
    usr_bsm->yaw_rate   = 14;

    usr_bsm->wheel_brakes[WHEEL_UNAVAILABLE] = 0;
    usr_bsm->wheel_brakes[WHEEL_LEFT_FRONT]  = 0;
    usr_bsm->wheel_brakes[WHEEL_LEFT_REAR]   = 0;
    usr_bsm->wheel_brakes[WHEEL_RIGHT_FRONT] = 0;
    usr_bsm->wheel_brakes[WHEEL_RIGHT_REAR]  = 1;

    usr_bsm->traction          = 1;
    usr_bsm->abs               = 2;
    usr_bsm->scs			   = 3;
    usr_bsm->brake_boost       = 0;
    usr_bsm->aux_brakes        = 1;

    usr_bsm->width               = 1010;
    usr_bsm->length              = 1020;

	// ===============  part II ===============
	
	if (1 == g_veh_safety_ext)
	{
    MOCAR_LOG_DEBUG("[SDK] add part II VEH_SAFETY_EXT");
	usr_bsm->veh_safety_ext_count = 2;

	/********************** veh_safety_ext[0] : ******************************/
	/********************** all optional value is exsit **********************/
	Mde_Vehicle_Safety_Ext_t * tmp_veh_saf_ext = &usr_bsm->veh_safe_ext[0];
	tmp_veh_saf_ext->event_is_exist = 1;
	tmp_veh_saf_ext->events[0] = 1;
	tmp_veh_saf_ext->events[2] = 1;

	tmp_veh_saf_ext->pathHistory_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_veh_saf_ext->pathHistory.initialPosition_is_exist = SDK_OPTIONAL_EXSIT;
		{
			tmp_veh_saf_ext->pathHistory.initialPosition.utcTime_is_exist = SDK_OPTIONAL_EXSIT;
			{
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.year_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.year = 2020;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.month_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.month = 9;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.day_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.day = 20;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.hour_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.hour = 10;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.minute_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.minute = 11;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.second_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.second = 12;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.offset_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.initialPosition.utcTime.offset = 13;
			}

			tmp_veh_saf_ext->pathHistory.initialPosition.Long = 1800000000;			
			tmp_veh_saf_ext->pathHistory.initialPosition.lat = 900000000;
			tmp_veh_saf_ext->pathHistory.initialPosition.elevation_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.elevation = 60000;
			tmp_veh_saf_ext->pathHistory.initialPosition.heading_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.heading = 28800;
			
			tmp_veh_saf_ext->pathHistory.initialPosition.transmission_Speed_are_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.transmisson = 1;
			tmp_veh_saf_ext->pathHistory.initialPosition.speed = 100;

			tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy_is_exsit = SDK_OPTIONAL_EXSIT;
			{
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.semimajor = 20;
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.semiminor = 50;
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.orientation = 1000;
			}

			tmp_veh_saf_ext->pathHistory.initialPosition.timeConfidence_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.timeConfidence = 30;

			tmp_veh_saf_ext->pathHistory.initialPosition.pos_elevation_are_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.pos = 3;
			tmp_veh_saf_ext->pathHistory.initialPosition.posC_elevation = 2;

			tmp_veh_saf_ext->pathHistory.initialPosition.speedConfidence_is_exsit = SDK_OPTIONAL_EXSIT;
			{
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.heading = 1;
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.speed = 2;
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.throttle =3;
			}
		}

		tmp_veh_saf_ext->pathHistory.currGNSSsts_is_exist = SDK_OPTIONAL_EXSIT;
		tmp_veh_saf_ext->pathHistory.currGNSSstatus[5] = 1;
		tmp_veh_saf_ext->pathHistory.currGNSSstatus[6] = 1;

		tmp_veh_saf_ext->pathHistory.ph_count = 3;
		{
			for (uint8_t i = 0; i < 3; i++)
			{
				tmp_veh_saf_ext->pathHistory.path_his[i].latOffset = 130000;
				tmp_veh_saf_ext->pathHistory.path_his[i].lonOffset = 130101;
				tmp_veh_saf_ext->pathHistory.path_his[i].elevationOffset = -2000;
				tmp_veh_saf_ext->pathHistory.path_his[i].timeOffset = 2000;
				tmp_veh_saf_ext->pathHistory.path_his[i].speed_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.path_his[i].speed = 100;
				
				tmp_veh_saf_ext->pathHistory.path_his[i].posAccuracy_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.path_his[i].posAccuracy.semimajor = 200;
				tmp_veh_saf_ext->pathHistory.path_his[i].posAccuracy.semiminor = 250;
				tmp_veh_saf_ext->pathHistory.path_his[i].posAccuracy.orientation = 350;

				tmp_veh_saf_ext->pathHistory.path_his[i].heading_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_veh_saf_ext->pathHistory.path_his[i].heading = 240;
			}
		}
	}

	tmp_veh_saf_ext->pathPrediction_is_exsit = SDK_OPTIONAL_EXSIT;
	tmp_veh_saf_ext->pathPrediction.radiusOfCurve = 10;
	tmp_veh_saf_ext->pathPrediction.confidence = 20;

	tmp_veh_saf_ext->ExteriorLights_is_exsit = SDK_OPTIONAL_EXSIT;
	tmp_veh_saf_ext->lights[2] = 1;
	tmp_veh_saf_ext->lights[3] = 1;
	tmp_veh_saf_ext->lights[8] = 1;


	/********************** veh_safety_ext[1] : ******************************/
	/********************** some optional value is exsit *********************/
	tmp_veh_saf_ext = &usr_bsm->veh_safe_ext[1];

	tmp_veh_saf_ext->event_is_exist = 0;
	tmp_veh_saf_ext->events[0] = 1;
	tmp_veh_saf_ext->events[2] = 1;

	tmp_veh_saf_ext->pathHistory_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_veh_saf_ext->pathHistory.initialPosition_is_exist = SDK_OPTIONAL_EXSIT;
		{
			tmp_veh_saf_ext->pathHistory.initialPosition.Long = 1800000000;			
			tmp_veh_saf_ext->pathHistory.initialPosition.lat = 900000000;
			
			tmp_veh_saf_ext->pathHistory.initialPosition.heading_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.heading = 28800;
			
			tmp_veh_saf_ext->pathHistory.initialPosition.transmission_Speed_are_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.initialPosition.transmisson = 1;
			tmp_veh_saf_ext->pathHistory.initialPosition.speed = 100;

			tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy_is_exsit = SDK_OPTIONAL_EXSIT;
			{
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.semimajor = 20;
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.semiminor = 50;
				tmp_veh_saf_ext->pathHistory.initialPosition.posAccuracy.orientation = 1000;
			}

			tmp_veh_saf_ext->pathHistory.initialPosition.speedConfidence_is_exsit = SDK_OPTIONAL_EXSIT;
			{
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.heading = 1;
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.speed = 2;
				tmp_veh_saf_ext->pathHistory.initialPosition.speed_heading_throttle.throttle =3;
			}
		}
		
		tmp_veh_saf_ext->pathHistory.ph_count = 1;
		{
			uint8_t i = 0;
			tmp_veh_saf_ext->pathHistory.path_his[i].latOffset = 130000;
			tmp_veh_saf_ext->pathHistory.path_his[i].lonOffset = 130101;
			tmp_veh_saf_ext->pathHistory.path_his[i].elevationOffset = -2000;
			tmp_veh_saf_ext->pathHistory.path_his[i].timeOffset = 2000;

			tmp_veh_saf_ext->pathHistory.path_his[i].heading_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_veh_saf_ext->pathHistory.path_his[i].heading = 240;
		}
	}

	tmp_veh_saf_ext->pathPrediction_is_exsit = SDK_OPTIONAL_EXSIT;
	tmp_veh_saf_ext->pathPrediction.radiusOfCurve = 10;
	tmp_veh_saf_ext->pathPrediction.confidence = 20;	
	
	}

	
	if (1 == g_spe_veh_ext)
	{
    MOCAR_LOG_DEBUG("[SDK] add part II SPE_VEH_EXT");
	usr_bsm->spe_veh_ext_count = 2;

	/********************** spe_veh_ext[0] : ******************************/
	/********************** all optional value is exsit **********************/
	Mde_Special_Vehicle_Ext_t * tmp_spe_veh = &usr_bsm->spe_veh_ext[0];
	tmp_spe_veh->vehicleAlerts_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_spe_veh->vehicleAlerts.sspRights = 1;
		tmp_spe_veh->vehicleAlerts.sirenUse = 2;
		tmp_spe_veh->vehicleAlerts.lightsUse = 3;
		tmp_spe_veh->vehicleAlerts.multi = 2;

		tmp_spe_veh->vehicleAlerts.sspRights_events_are_exsit = SDK_OPTIONAL_EXSIT;
		tmp_spe_veh->vehicleAlerts.events_sspRights = 4;
		tmp_spe_veh->vehicleAlerts.events[2] = 1;
		tmp_spe_veh->vehicleAlerts.events[3] = 1;
		tmp_spe_veh->vehicleAlerts.events[4] = 1;
		tmp_spe_veh->vehicleAlerts.events[5] = 1;
		tmp_spe_veh->vehicleAlerts.responseType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_spe_veh->vehicleAlerts.responseType = 6;
	}

	tmp_spe_veh->description_is_exsit = SDK_OPTIONAL_EXSIT;
	{	
		tmp_spe_veh->description.typeEvent = 10000;
		tmp_spe_veh->description.description_size = 2;
		tmp_spe_veh->description.description[0] = 60000;
		tmp_spe_veh->description.description[1] = 60001;
		
		tmp_spe_veh->description.priority_size = 1;
		tmp_spe_veh->description.priority[0] = 2;
		
		tmp_spe_veh->description.heading_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_spe_veh->description.heading[0] = 1;
		tmp_spe_veh->description.heading[1] = 1;
		tmp_spe_veh->description.heading[2] = 1;
		tmp_spe_veh->description.heading[3] = 1;
		tmp_spe_veh->description.heading[8] = 1;
		tmp_spe_veh->description.heading[9] = 1;
		tmp_spe_veh->description.heading[10] = 1;
		tmp_spe_veh->description.heading[11] = 1;

		tmp_spe_veh->description.extent_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_spe_veh->description.extent = 10;
	}

	tmp_spe_veh->trailers_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_spe_veh->trailers.sspRights = 1;
		tmp_spe_veh->trailers.pivotOffset = 2;
		tmp_spe_veh->trailers.pivotAngle = 3;
		tmp_spe_veh->trailers.pivots = 1;

		tmp_spe_veh->trailers.units_size = 2;
		for (uint8_t i = 0; i < tmp_spe_veh->trailers.units_size; i++)
		{
			tmp_spe_veh->trailers.units[i].isDolly = 1;
			tmp_spe_veh->trailers.units[i].width = 2;
			tmp_spe_veh->trailers.units[i].length = 3;
			tmp_spe_veh->trailers.units[i].height_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].height = 4;
			tmp_spe_veh->trailers.units[i].mass_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].mass = 5;
			tmp_spe_veh->trailers.units[i].bumperHeights_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].bumperHeights_front = 6;
			tmp_spe_veh->trailers.units[i].bumperHeights_rear = 7;
			tmp_spe_veh->trailers.units[i].centerOfGravity_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].centerOfGravity = 8;
			
			tmp_spe_veh->trailers.units[i].frontPivot.pivotOffset = 9;
			tmp_spe_veh->trailers.units[i].frontPivot.pivotAngle = 10;
			tmp_spe_veh->trailers.units[i].frontPivot.pivots = 1;

			tmp_spe_veh->trailers.units[i].rearPivot_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].rearPivot.pivotOffset = 2;
			tmp_spe_veh->trailers.units[i].rearPivot.pivotAngle = 3;
			tmp_spe_veh->trailers.units[i].rearPivot.pivots = 1;

			tmp_spe_veh->trailers.units[i].rearWheelOffset_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].rearWheelOffset = 2;

			tmp_spe_veh->trailers.units[i].positionOffset_x = 3;
			tmp_spe_veh->trailers.units[i].positionOffset_y = 4;

			tmp_spe_veh->trailers.units[i].elevationOffset_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].elevationOffset = 5;

			tmp_spe_veh->trailers.units[i].crumbData_size = 1;
			{
				tmp_spe_veh->trailers.units[i].crumbData[0].pivotAngle = 6;
				tmp_spe_veh->trailers.units[i].crumbData[0].timeOffset = 7;
				tmp_spe_veh->trailers.units[i].crumbData[0].positionOffset_x = 8;
				tmp_spe_veh->trailers.units[i].crumbData[0].positionOffset_y = 9;
				tmp_spe_veh->trailers.units[i].crumbData[0].elevationOffset_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_spe_veh->trailers.units[i].crumbData[0].elevationOffset = 10;
				tmp_spe_veh->trailers.units[i].crumbData[0].heading_is_exsit = SDK_OPTIONAL_EXSIT;
				tmp_spe_veh->trailers.units[i].crumbData[0].heading = 11;
			}
		}
	}
	
	/********************** spe_veh_ext[1] : ******************************/
	/********************** some optional value is exsit **********************/
	tmp_spe_veh = &usr_bsm->spe_veh_ext[1];

	tmp_spe_veh->vehicleAlerts_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_spe_veh->vehicleAlerts.sspRights = 1;
		tmp_spe_veh->vehicleAlerts.sirenUse = 2;
		tmp_spe_veh->vehicleAlerts.lightsUse = 3;
		tmp_spe_veh->vehicleAlerts.multi = 2;

		tmp_spe_veh->vehicleAlerts.responseType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_spe_veh->vehicleAlerts.responseType = 6;
	}

	tmp_spe_veh->trailers_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_spe_veh->trailers.sspRights = 1;
		tmp_spe_veh->trailers.pivotOffset = 2;
		tmp_spe_veh->trailers.pivotAngle = 3;
		tmp_spe_veh->trailers.pivots = 1;

		tmp_spe_veh->trailers.units_size = 2;
		for (uint8_t i = 0; i < tmp_spe_veh->trailers.units_size; i++)
		{
			tmp_spe_veh->trailers.units[i].isDolly = 1;
			tmp_spe_veh->trailers.units[i].width = 2;
			tmp_spe_veh->trailers.units[i].length = 3;
			tmp_spe_veh->trailers.units[i].height_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].height = 4;
			tmp_spe_veh->trailers.units[i].mass_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].mass = 5;
			tmp_spe_veh->trailers.units[i].bumperHeights_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].bumperHeights_front = 6;
			tmp_spe_veh->trailers.units[i].bumperHeights_rear = 7;
			tmp_spe_veh->trailers.units[i].centerOfGravity_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].centerOfGravity = 8;
			
			tmp_spe_veh->trailers.units[i].frontPivot.pivotOffset = 9;
			tmp_spe_veh->trailers.units[i].frontPivot.pivotAngle = 10;
			tmp_spe_veh->trailers.units[i].frontPivot.pivots = 1;

			tmp_spe_veh->trailers.units[i].rearPivot_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].rearPivot.pivotOffset = 2;
			tmp_spe_veh->trailers.units[i].rearPivot.pivotAngle = 3;
			tmp_spe_veh->trailers.units[i].rearPivot.pivots = 1;

			tmp_spe_veh->trailers.units[i].rearWheelOffset_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].rearWheelOffset = 2;

			tmp_spe_veh->trailers.units[i].positionOffset_x = 3;
			tmp_spe_veh->trailers.units[i].positionOffset_y = 4;

			tmp_spe_veh->trailers.units[i].elevationOffset_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_spe_veh->trailers.units[i].elevationOffset = 5;

		}
		
	}
	}

	
	if (1 == g_sup_veh_ext)
	{
    MOCAR_LOG_DEBUG("[SDK] add part II SUP_VEH_EXT");
	usr_bsm->sup_veh_ext_count = 2;
	
	/********************** sup_veh_ext[0] : ******************************/
	/********************** all optional value is exsit **********************/
	
	Mde_Sup_Veh_Ext_t * tmp_sup_veh = &usr_bsm->sup_veh_ext[0];

	tmp_sup_veh->classification_is_exsit = SDK_OPTIONAL_EXSIT;
	tmp_sup_veh->classification = 1;
	
	tmp_sup_veh->classDetails_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->classDetails.keyType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.keyType = 2;

		tmp_sup_veh->classDetails.role_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.role = 3;

		tmp_sup_veh->classDetails.iso3883_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.iso3883 = 4;

		tmp_sup_veh->classDetails.hpmsType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.hpmsType = 5;

		tmp_sup_veh->classDetails.vehicleType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.vehicleType = 9229;

		tmp_sup_veh->classDetails.responseEquip_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.responseEquip = 10000;

		tmp_sup_veh->classDetails.responderType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.responderType = 9737;

		tmp_sup_veh->classDetails.fuelType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.fuelType = 0;
	}

	tmp_sup_veh->vehicleData_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->vehicleData.height_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.height = 10;

		tmp_sup_veh->vehicleData.bumpers_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.bumpers_front = 20;
		tmp_sup_veh->vehicleData.bumpers_rear = 30;

		tmp_sup_veh->vehicleData.mass_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.mass = 40;

		tmp_sup_veh->vehicleData.trailerWeight_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.trailerWeight = 50;
	}

	tmp_sup_veh->weatherReport_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->weatherReport.isRaining = 1;
		tmp_sup_veh->weatherReport.rainRate_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.rainRate = 10;
		
		tmp_sup_veh->weatherReport.precipSituation_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.precipSituation = 15;

		tmp_sup_veh->weatherReport.solarRadiation_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.solarRadiation = 20;

		tmp_sup_veh->weatherReport.friction_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.friction = 30;

		tmp_sup_veh->weatherReport.roadFriction_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.roadFriction = 40;
	}

	tmp_sup_veh->weatherProbe_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->weatherProbe.airTemp_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherProbe.airTemp = 50;

		tmp_sup_veh->weatherProbe.airPressure_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherProbe.airPressure = 60;

		tmp_sup_veh->weatherProbe.rainRates_is_exsit = SDK_OPTIONAL_EXSIT;
		{
			tmp_sup_veh->weatherProbe.rainRates.statusFront = 6;
			tmp_sup_veh->weatherProbe.rainRates.rateFront = 5;
			
			tmp_sup_veh->weatherProbe.rainRates.statusRear_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sup_veh->weatherProbe.rainRates.statusRear = 4;

			tmp_sup_veh->weatherProbe.rainRates.rateRear_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sup_veh->weatherProbe.rainRates.rateRear = 3;
		}
	}

	tmp_sup_veh->obstacle_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->obstacle.obDist = 2;
		tmp_sup_veh->obstacle.obDirect = 1;

		tmp_sup_veh->obstacle.description_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.description = 525;

		tmp_sup_veh->obstacle.locationDetails_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.locationDetails = 8000;

		tmp_sup_veh->obstacle.dateTime.year_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.year = 2020;
		tmp_sup_veh->obstacle.dateTime.month_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.month = 10;
		tmp_sup_veh->obstacle.dateTime.day_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.day = 1;
		tmp_sup_veh->obstacle.dateTime.hour_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.hour = 10;
		tmp_sup_veh->obstacle.dateTime.minute_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.minute = 1;
		tmp_sup_veh->obstacle.dateTime.second_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.second = 10;
		tmp_sup_veh->obstacle.dateTime.offset_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.offset = 1;

		tmp_sup_veh->obstacle.vertEvent_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.vertEvent[4] = 1;
	}
	
	tmp_sup_veh->status_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->status.statusDetails = 540;
		tmp_sup_veh->status.locationDetails_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->status.locationDetails = 8001;
	}

	tmp_sup_veh->speedReports_size = 10;
	tmp_sup_veh->speedReports[0] = 1;
	tmp_sup_veh->speedReports[1] = 2;	
	tmp_sup_veh->speedReports[2] = 3;	
	tmp_sup_veh->speedReports[3] = 4;	
	tmp_sup_veh->speedReports[4] = 5;	
	tmp_sup_veh->speedReports[5] = 6;	
	tmp_sup_veh->speedReports[6] = 7;	
	tmp_sup_veh->speedReports[7] = 8;	
	tmp_sup_veh->speedReports[8] = 9;	
	tmp_sup_veh->speedReports[9] = 10;

	tmp_sup_veh->theRTCM_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->theRTCM.rtcmHeader_is_exsit = SDK_OPTIONAL_EXSIT;
		{
			tmp_sup_veh->theRTCM.rtcmHeader.status[2] = 1;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetX = -100;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetY = -200;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetZ = -300;
		}
		
		tmp_sup_veh->theRTCM.msgs_size = 2;
		uint8_t test_msg[20] = "this is msg 1";
		memcpy(tmp_sup_veh->theRTCM.msgs[0], test_msg, strlen((char *)test_msg));
		uint8_t test_msg2[20] = "this is msg 2";
		memcpy(tmp_sup_veh->theRTCM.msgs[1], test_msg2, strlen((char *)test_msg));
	}
	
	/********************** sup_veh_ext[1] : ******************************/
	/********************** some optional value is exsit **********************/
	tmp_sup_veh = &usr_bsm->sup_veh_ext[1];
	
	tmp_sup_veh->classification_is_exsit = SDK_OPTIONAL_EXSIT;
	tmp_sup_veh->classification = 1;
	
	tmp_sup_veh->classDetails_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->classDetails.keyType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.keyType = 2;

		tmp_sup_veh->classDetails.vehicleType_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.vehicleType = 9229;

		tmp_sup_veh->classDetails.responseEquip_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->classDetails.responseEquip = 10000;

	}

	tmp_sup_veh->vehicleData_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->vehicleData.height_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.height = 10;

		tmp_sup_veh->vehicleData.mass_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->vehicleData.mass = 40;
	}

	tmp_sup_veh->weatherReport_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->weatherReport.isRaining = 1;
		tmp_sup_veh->weatherReport.rainRate_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.rainRate = 10;

		tmp_sup_veh->weatherReport.friction_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.friction = 30;

		tmp_sup_veh->weatherReport.roadFriction_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherReport.roadFriction = 40;
	}

	tmp_sup_veh->weatherProbe_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->weatherProbe.airTemp_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherProbe.airTemp = 50;

		tmp_sup_veh->weatherProbe.airPressure_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->weatherProbe.airPressure = 60;

		tmp_sup_veh->weatherProbe.rainRates_is_exsit = SDK_OPTIONAL_EXSIT;
		{
			tmp_sup_veh->weatherProbe.rainRates.statusFront = 6;
			tmp_sup_veh->weatherProbe.rainRates.rateFront = 5;
		
			tmp_sup_veh->weatherProbe.rainRates.rateRear_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sup_veh->weatherProbe.rainRates.rateRear = 3;
		}
	}

	tmp_sup_veh->obstacle_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->obstacle.obDist = 2;
		tmp_sup_veh->obstacle.obDirect = 1;

		tmp_sup_veh->obstacle.dateTime.year_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.year = 2020;
		tmp_sup_veh->obstacle.dateTime.month_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.month = 10;
		tmp_sup_veh->obstacle.dateTime.day_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.day = 1;
		tmp_sup_veh->obstacle.dateTime.hour_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.hour = 10;
		tmp_sup_veh->obstacle.dateTime.minute_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.minute = 1;
		tmp_sup_veh->obstacle.dateTime.second_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.dateTime.second = 10;

		tmp_sup_veh->obstacle.vertEvent_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sup_veh->obstacle.vertEvent[4] = 1;
	}
	
	tmp_sup_veh->status_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->status.statusDetails = 540;
	}

	tmp_sup_veh->theRTCM_is_exsit = SDK_OPTIONAL_EXSIT;
	{
		tmp_sup_veh->theRTCM.rtcmHeader_is_exsit = SDK_OPTIONAL_EXSIT;
		{
			tmp_sup_veh->theRTCM.rtcmHeader.status[2] = 1;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetX = -100;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetY = -200;
			tmp_sup_veh->theRTCM.rtcmHeader.antOffsetZ = -300;
		}
		
		tmp_sup_veh->theRTCM.msgs_size = 2;
		uint8_t test_msg[20] = "this is msg 1";
		memcpy(tmp_sup_veh->theRTCM.msgs[0], test_msg, strlen((char *)test_msg));
		uint8_t test_msg2[20] = "this is msg 2";
		memcpy(tmp_sup_veh->theRTCM.msgs[1], test_msg2, strlen((char *)test_msg));
	}
	}

    return 0;
}

void v2x_bsm_tx(void)
{
    v2x_msg_bsm_t   user_bsm;
    int ret         = -1;

    memset(&user_bsm, 0, sizeof(user_bsm));
    ret = v2x_fill_usr_bsm(&user_bsm);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("user bsm fill fail");
        return;
    }

    ret = mde_v2x_bsm_send(&user_bsm, NULL, 32);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("bsm send fail");
        return;   
    }

    MOCAR_LOG_INFO_FMT("txmsg-BSM: send msg successed, sequence %u", user_bsm.msg_count);
    printf("txmsg-BSM: send msg successed, sequence %u\n", user_bsm.msg_count);
}

int read_bsm_optional_conf(void)
{
	FILE * fp = fopen(BSM_OPTIONAL_CONF, "r");
	if (NULL == fp)
	{
		MOCAR_LOG_ERROR("[SDK]open bsm optional conf fail");
		return -1;
	}

	fseek(fp, 0, SEEK_SET);

	char buff[256] = {0};
	char * tmp_name = NULL;
	char * tmp_value = NULL;

	g_veh_safety_ext = 0;
	g_spe_veh_ext = 0;
	g_sup_veh_ext = 0;
	
	while( fgets(buff, 256, fp))
	{
		tmp_name = strtok(buff, "=");
		if (NULL != tmp_name)
		{
			tmp_value = strtok(NULL, "=");

			if (NULL == tmp_value)
			{
				MOCAR_LOG_ERROR("[SDK]bsm optional conf format fail");
				return -1;
			}
			//set global optional value
			if ( !strncmp(tmp_name, "VEH_SAFETY_EXT", 14) && (atoi(tmp_value) == 1))
			{
				MOCAR_LOG_INFO("[SDK] bsm conf VEH_SAFETY_EXT = 1");
				g_veh_safety_ext = 1;
			}
			if ( !strncmp(tmp_name, "SPE_VEH_EXT", 11) && (atoi(tmp_value) == 1))
			{
				MOCAR_LOG_INFO("[SDK] bsm conf SPE_VEH_EXT = 1");
				g_spe_veh_ext = 1;
			}
			if ( !strncmp(tmp_name, "SUP_VEH_EXT", 11) && (atoi(tmp_value) == 1))
			{
				MOCAR_LOG_INFO("[SDK] bsm conf SUP_VEH_EXT = 1");
				g_sup_veh_ext = 1;
			}
			else
			{
				MOCAR_LOG_INFO_FMT("[SDK]conf_name = %s, value = %d", tmp_name, atoi(tmp_value));
			}
		}
		
	}

	return 0;
}

int main(int argc, char* argv[])
{
    int ret = -1;
    int ch  = -1;
    //timer_t bsm_send_timer;

    while((ch = getopt(argc, argv, "n:h")) != -1)
    {
        switch (ch)
        {
            case 'n':
                 g_vehicle_id = atoi(optarg);
                 break;
            default:
                 printf("Please set correct vechile id!  format: ./bsm_sample -n  vechile id\n");
                 return 0;
        }
    }

    if(g_vehicle_id < 0)
    {
        printf("Please set correct vehicle id\n");
        return -1;
    }

    ret = mde_v2x_init(0);
    if(0 != ret)
    {
        printf("cv2x init fail\n");
        return -1;    
    }

	ret = read_bsm_optional_conf();
	if (0 != ret)
	{
	    MOCAR_LOG_INFO_FMT("bsm optional conf read fail");
		return -1;		
	}
	
    mde_v2x_bsm_recv_handle_register(v2x_user_bsm_recv_handle);
    MOCAR_LOG_INFO_FMT("bsm recv handle register");
    
    while(1)
    {
        v2x_bsm_tx();
        sleep(2);
    }
    
    return 0;
}
