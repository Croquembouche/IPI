#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_spat_rx_data_dump(v2x_msg_spat_t* usr_spat)
{
	if (NULL == usr_spat)
	{
		MOCAR_LOG_ERROR("usr spat us null");
		return;
	}

	if (SDK_OPTIONAL_EXSIT == usr_spat->timeStamp_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("usr_spat time stamp: %d", usr_spat->timeStamp);
	}
	if (SDK_OPTIONAL_EXSIT == usr_spat->name_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("usr_spat name: %s", usr_spat->name);
	}

	for(uint8_t i = 0; i < usr_spat->intersections_count; i++)
	{
		Mde_Intersection_State_t * tmp_state = &usr_spat->intersections[i];
		
		if (SDK_OPTIONAL_EXSIT == tmp_state->name_is_exist)
		{
			MOCAR_LOG_DEBUG_FMT("intersection name: %s", tmp_state->name);
		}
		if (SDK_OPTIONAL_EXSIT == tmp_state->region_is_exist)
		{
			MOCAR_LOG_DEBUG_FMT("intersection region: %d", tmp_state->region);
		}
		MOCAR_LOG_DEBUG_FMT("intersection id: %d", tmp_state->id);
		MOCAR_LOG_DEBUG_FMT("intersection revision: %d", tmp_state->revision);

		uint8_t test_status[33] = {0};
		for(uint8_t m = 0; m < 16; m++)
		{
			test_status[2*m] = 48 + tmp_state->status[m];
			test_status[2*m+1] = ' ';
		}
		MOCAR_LOG_DEBUG_FMT("intersection status: %s", test_status);
		if (SDK_OPTIONAL_EXSIT == tmp_state->moy_is_exist)
		{
			MOCAR_LOG_DEBUG_FMT("intersection moy: %d", tmp_state->moy);
		}
		if (SDK_OPTIONAL_EXSIT == tmp_state->timeStamp_is_exist)
		{
			MOCAR_LOG_DEBUG_FMT("intersection timeStamp: %d", tmp_state->timeStamp);
		}
		if ((1 <= tmp_state->enabledLane_count) && ( tmp_state->enabledLane_count <= 16))
		{
			for(uint8_t m = 0; m <  tmp_state->enabledLane_count; m++)
			{
				MOCAR_LOG_DEBUG_FMT("intersection enabledLane[%d]: %d", m, tmp_state->enabledLane[m]);
			}
		}
		if ((1 <= tmp_state->state_count) && ( tmp_state->state_count <= 255))
		{
			for(uint8_t m = 0; m < tmp_state->state_count; m++)
			{
				Mde_MovementState_t * tmp_move_state = &tmp_state->state[m];
				
				if (SDK_OPTIONAL_EXSIT == tmp_move_state->movementName_is_exist)
				{
					MOCAR_LOG_DEBUG_FMT("movement_state name: %s", tmp_move_state->movementName);
				}
				MOCAR_LOG_DEBUG_FMT("movement_state signalGroup: %d", tmp_move_state->signalGroup);
				
				if ((1 <= tmp_move_state->state_time_speed_count) && ( tmp_move_state->state_time_speed_count <= 16))
				{
					for(uint8_t j = 0; j < tmp_move_state->state_time_speed_count; j++)
					{
						Mde_MovementEvent_t * tmp_movement_event = &tmp_move_state->state_time_speed[j];
						
						MOCAR_LOG_DEBUG_FMT("movement_event eventState: %d", tmp_movement_event->eventState);
						if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing_is_exsit)
						{
							if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing.startTime_is_exsit)
							{
								MOCAR_LOG_DEBUG_FMT("movement_event startTime: %d", tmp_movement_event->timing.startTime);
							}
							MOCAR_LOG_DEBUG_FMT("movement_event minEndTime: %d", tmp_movement_event->timing.minEndTime);
							if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing.maxEndTime_is_exsit)
							{
								MOCAR_LOG_DEBUG_FMT("movement_event maxEndTime: %d", tmp_movement_event->timing.maxEndTime);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing.likelyTime_is_exsit)
							{
								MOCAR_LOG_DEBUG_FMT("movement_event likelyTime: %d", tmp_movement_event->timing.likelyTime);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing.confidence_is_exsit)
							{
								MOCAR_LOG_DEBUG_FMT("movement_event confidence: %d", tmp_movement_event->timing.confidence);
							}
							if (SDK_OPTIONAL_EXSIT == tmp_movement_event->timing.nextTime_is_exsit)
							{
								MOCAR_LOG_DEBUG_FMT("movement_event nextTime: %d", tmp_movement_event->timing.nextTime);
							}
						}

						if ((1 <= tmp_movement_event->speed_count) && (tmp_movement_event->speed_count <= 16))
						{
							for(uint8_t k = 0; k < tmp_movement_event->speed_count; k++)
							{
								Mde_AdvisorySpeed_t * tmp_speed = &tmp_movement_event->speed[k];
								
								MOCAR_LOG_DEBUG_FMT("speed type: %d", tmp_speed->type);
								if (SDK_OPTIONAL_EXSIT == tmp_speed->speed_is_exsit)
								{
									MOCAR_LOG_DEBUG_FMT("speed: %d", tmp_speed->speed);
								}
								if (SDK_OPTIONAL_EXSIT == tmp_speed->confidence_is_exsit)
								{
									MOCAR_LOG_DEBUG_FMT("confidence: %d", tmp_speed->confidence);
								}
								if (SDK_OPTIONAL_EXSIT == tmp_speed->distance_is_exsit)
								{
									MOCAR_LOG_DEBUG_FMT("distance: %d", tmp_speed->distance);
								}
								if (SDK_OPTIONAL_EXSIT == tmp_speed->class_is_exsit)
								{
									MOCAR_LOG_DEBUG_FMT("class: %d", tmp_speed->class);
								}
							}
						}
					}
				}
				
				if ((1 <= tmp_move_state->maneuverAssist_count) && ( tmp_move_state->maneuverAssist_count <= 16))
				{
					for(uint8_t k = 0; k < tmp_move_state->maneuverAssist_count; k++)
					{
						Mde_ConnectionManeuverAssist_t * tmp_mane = &tmp_move_state->maneuverAssist[k];
						
						MOCAR_LOG_DEBUG_FMT("maneuverAssist connectionID: %d", tmp_mane->connectionID);
						if (SDK_OPTIONAL_EXSIT == tmp_mane->queueLength_is_exsit)
						{
							MOCAR_LOG_DEBUG_FMT("maneuverAssist queueLength: %d", tmp_mane->queueLength);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_mane->availableStorageLength_is_exsit)
						{
							MOCAR_LOG_DEBUG_FMT("maneuverAssist availableStorageLength: %d", tmp_mane->availableStorageLength);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_mane->waitOnStop_is_exsit)
						{
							MOCAR_LOG_DEBUG_FMT("maneuverAssist waitOnStop: %d", tmp_mane->waitOnStop);
						}
						if (SDK_OPTIONAL_EXSIT == tmp_mane->pedBicycleDetect_is_exsit)
						{
							MOCAR_LOG_DEBUG_FMT("maneuverAssist pedBicycleDetect: %d", tmp_mane->pedBicycleDetect);
						}
					}
				}
			}
		}

		if ((1 <= tmp_state->maneuverAssist_count) && ( tmp_state->maneuverAssist_count <= 16))
		{
			for(uint8_t k = 0; k < tmp_state->maneuverAssist_count; k++)
			{
				Mde_ConnectionManeuverAssist_t * tmp_mane = &tmp_state->maneuverAssist[k];
				
				MOCAR_LOG_DEBUG_FMT("maneuverAssist connectionID: %d", tmp_mane->connectionID);
				if (SDK_OPTIONAL_EXSIT == tmp_mane->queueLength_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("maneuverAssist queueLength: %d", tmp_mane->queueLength);
				}
				if (SDK_OPTIONAL_EXSIT == tmp_mane->availableStorageLength_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("maneuverAssist availableStorageLength: %d", tmp_mane->availableStorageLength);
				}
				if (SDK_OPTIONAL_EXSIT == tmp_mane->waitOnStop_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("maneuverAssist waitOnStop: %d", tmp_mane->waitOnStop);
				}
				if (SDK_OPTIONAL_EXSIT == tmp_mane->pedBicycleDetect_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("maneuverAssist pedBicycleDetect: %d", tmp_mane->pedBicycleDetect);
				}
			}
		}
	}
}

void v2x_user_spat_recv_handle(v2x_msg_spat_t* usr_spat, void* param)
{
	if (NULL == usr_spat)
	{
    	MOCAR_LOG_ERROR("usr spat is NULL");
		return;
	}

    MOCAR_LOG_INFO_FMT("rxmsg-SPAT: name %s", usr_spat->name);
	
	v2x_spat_rx_data_dump(usr_spat);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_spat(v2x_msg_spat_t* usr_spat)
{
    static uint32_t     msgcount   = 0;
    struct tm*          utc_time = NULL;
    time_t              time_now;
    int                 moy = 0;

    if(NULL == usr_spat)
    {
        return -1;
    }
	
	usr_spat->timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
    time_now = time((time_t*)NULL);
    utc_time = localtime(&time_now);
    moy = minute_of_the_year(utc_time);
    usr_spat->timeStamp = moy;


	usr_spat->name_is_exist = SDK_OPTIONAL_EXSIT;
	char spat_name[5] = "Spat";
	memcpy(usr_spat->name, spat_name, 4);
	
	usr_spat->intersections_count = 2;
	for(uint8_t i = 0; i < usr_spat->intersections_count; i++)
	{
		usr_spat->intersections[i].name_is_exist = SDK_OPTIONAL_EXSIT;
		char tmp_name[10] = {0};
		memcpy(tmp_name, "inter", 5);
		char tmp_idx = i + '0';
		memcpy(tmp_name+5, &tmp_idx, 1);
		memcpy(usr_spat->intersections[i].name, tmp_name, 6);

		usr_spat->intersections[i].region_is_exist = SDK_OPTIONAL_EXSIT;
		usr_spat->intersections[i].region = i;
		
		usr_spat->intersections[i].id = i+1;
		
		usr_spat->intersections[i].revision = msgcount++;
		if (msgcount > 127)
		{
			msgcount = 0;
		}
		
		usr_spat->intersections[i].status[1] = 1;
		usr_spat->intersections[i].status[5] = 1;

		usr_spat->intersections[i].moy_is_exist = SDK_OPTIONAL_EXSIT;
		usr_spat->intersections[i].moy = 3;

		usr_spat->intersections[i].timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
		usr_spat->intersections[i].timeStamp = 4;

		usr_spat->intersections[i].enabledLane_count = 3;
		usr_spat->intersections[i].enabledLane[0] = 2;
		usr_spat->intersections[i].enabledLane[1] = 22;
		usr_spat->intersections[i].enabledLane[2] = 222;
		usr_spat->intersections[i].enabledLane[3] = 223;  // there should not show up

		Mde_Intersection_State_t * tmp_inter = &usr_spat->intersections[i];

		tmp_inter->state_count = 2;
		for(uint8_t j = 0; j < tmp_inter->state_count; j++)
		{
			Mde_MovementState_t * tmp_movestate = &tmp_inter->state[j];
			tmp_movestate->movementName_is_exist = SDK_OPTIONAL_EXSIT;
			char tmp_move_name[20] = {0};
			memcpy(tmp_move_name, "movementName", 12);
			char tmp_idx2 = j + '0';
			memcpy(tmp_move_name+12, &tmp_idx2, 1);
			memcpy(tmp_movestate->movementName, tmp_move_name, 13);

			tmp_movestate->signalGroup = 2;

			tmp_movestate->state_time_speed_count = 1;
			tmp_movestate->state_time_speed[0].eventState = 3;
			tmp_movestate->state_time_speed[0].timing_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.startTime_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.startTime = 4;
			tmp_movestate->state_time_speed[0].timing.minEndTime = 5;
			tmp_movestate->state_time_speed[0].timing.maxEndTime_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.maxEndTime = 6;
			tmp_movestate->state_time_speed[0].timing.likelyTime_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.likelyTime = 7;
			tmp_movestate->state_time_speed[0].timing.confidence_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.confidence = 8;
			tmp_movestate->state_time_speed[0].timing.nextTime_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].timing.nextTime = 9;

			tmp_movestate->state_time_speed[0].speed_count = 1;
			tmp_movestate->state_time_speed[0].speed[0].type = 1;
			tmp_movestate->state_time_speed[0].speed[0].speed_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].speed[0].speed = 2;
			tmp_movestate->state_time_speed[0].speed[0].confidence_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].speed[0].confidence = 3;
			tmp_movestate->state_time_speed[0].speed[0].distance_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].speed[0].distance = 4;
			tmp_movestate->state_time_speed[0].speed[0].class_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->state_time_speed[0].speed[0].class = 5;

			tmp_movestate->maneuverAssist_count = 1;
			tmp_movestate->maneuverAssist[0].connectionID = 1;
			tmp_movestate->maneuverAssist[0].queueLength_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->maneuverAssist[0].queueLength = 2;
			tmp_movestate->maneuverAssist[0].availableStorageLength_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->maneuverAssist[0].availableStorageLength = 3;
			tmp_movestate->maneuverAssist[0].waitOnStop_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->maneuverAssist[0].waitOnStop = 1;
			tmp_movestate->maneuverAssist[0].pedBicycleDetect_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_movestate->maneuverAssist[0].pedBicycleDetect = 0;
		}

		tmp_inter->maneuverAssist_count = 3;
		for(uint8_t k = 0; k < tmp_inter->maneuverAssist_count; k++)
		{
			tmp_inter->maneuverAssist[k].connectionID = 111;
			tmp_inter->maneuverAssist[k].queueLength_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_inter->maneuverAssist[k].queueLength = 222;
			tmp_inter->maneuverAssist[k].availableStorageLength_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_inter->maneuverAssist[k].availableStorageLength = 333;
			tmp_inter->maneuverAssist[k].waitOnStop_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_inter->maneuverAssist[k].waitOnStop = 0;
			tmp_inter->maneuverAssist[k].pedBicycleDetect_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_inter->maneuverAssist[k].pedBicycleDetect = 1;
		}
	}
	
    return  0;
}


void v2x_spat_tx(void)
{
	int ret = -1;
	v2x_msg_spat_t * usr_spat = calloc(1, sizeof(v2x_msg_spat_t));
	if (NULL == usr_spat)
	{
    	MOCAR_LOG_ERROR("spat usr_spat calloc fail");
		return ;
	}
	
    ret = v2x_fill_usr_spat(usr_spat);
    if(0 != ret)
    {
        fprintf(stderr, "user spat fill fail\n");

		free(usr_spat);
		usr_spat = NULL;
        return;
    }
	
    ret = mde_v2x_spat_send(usr_spat, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "spat send fail\n");
		free(usr_spat);
		usr_spat = NULL;
        return;   
    }

    fprintf(stderr, "txmsg-SPAT: send msg successed, sequence %s\n", usr_spat->name);
	free(usr_spat);
	usr_spat = NULL;
    
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
	
    mde_v2x_spat_recv_handle_register(v2x_user_spat_recv_handle); 
    printf("spat recv handle register");

	
    while(1)
    {
        v2x_spat_tx();
        //usleep(100*1000);
        sleep(1);
    }
    
    return 0;
}
