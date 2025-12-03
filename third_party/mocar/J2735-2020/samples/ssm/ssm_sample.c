#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_ssm_rx_data_dump(v2x_msg_ssm_t* usr_ssm)
{
	if (NULL == usr_ssm)
	{
		MOCAR_LOG_ERROR("usr ssm is null");
		return;
	}

	if (SDK_OPTIONAL_EXSIT == usr_ssm->timeStamp_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("timestamp : %d", usr_ssm->timeStamp);
	}
	
	MOCAR_LOG_DEBUG_FMT("second : %d", usr_ssm->second);
	if (SDK_OPTIONAL_EXSIT == usr_ssm->sequenceNumber_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("sequenceNumber : %d", usr_ssm->sequenceNumber);
	}

	for(uint8_t i = 0; i < usr_ssm->SignalStatus_count; i++)
	{
		Mde_SignalStatus_t * tmp_sig = &usr_ssm->SignalStatus[i];

		MOCAR_LOG_DEBUG_FMT("SignalStatus_count %d : ---", i);

		MOCAR_LOG_DEBUG_FMT("sequenceNumber : %d", tmp_sig->sequenceNumber);

		if (SDK_OPTIONAL_EXSIT == tmp_sig->region_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("intersection region : %d", tmp_sig->region);
		}
		MOCAR_LOG_DEBUG_FMT("intersection id : %d", tmp_sig->intersection_id);

		for(uint8_t j = 0; j < tmp_sig->sigStatus_count; j++)
		{
			Mde_SignalStatusPackage_t * tmp_sts = &tmp_sig->sigStatus[j];
			
			if (SDK_OPTIONAL_EXSIT == tmp_sts->requester_is_exsit)
			{
				if (TEMPORARY_ID == tmp_sts->requester.id.msg_id)
				{
					MOCAR_LOG_DEBUG_FMT("veh_id(entityID): %s", tmp_sts->requester.id.choice.entityID);
				}
				else if (STATION_ID == tmp_sts->requester.id.msg_id)
				{
					MOCAR_LOG_DEBUG_FMT("veh_id(stationID): %d", tmp_sts->requester.id.choice.stationID);
				}
				else
				{
					MOCAR_LOG_ERROR_FMT("error veh_id msg_id: %d", tmp_sts->requester.id.msg_id);
					return;
				}
				
				MOCAR_LOG_DEBUG_FMT("request: %d", tmp_sts->requester.request);
				MOCAR_LOG_DEBUG_FMT("sequenceNumber: %d", tmp_sts->requester.sequenceNumber);
				
				if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.role_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("role: %d", tmp_sts->requester.role);
				}
				
				if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.typeData_is_exsit)
				{
					MOCAR_LOG_DEBUG_FMT("requestor role : %d", tmp_sts->requester.typeData.role);
					
					if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.typeData.subrole_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("requestor subrole : %d", tmp_sts->requester.typeData.subrole);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.typeData.requestLevel_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("requestor requestLevel : %d", tmp_sts->requester.typeData.requestLevel);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.typeData.iso3883_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("requestor iso3883 : %d", tmp_sts->requester.typeData.iso3883);
					}
					if (SDK_OPTIONAL_EXSIT == tmp_sts->requester.typeData.hpmsType_is_exsit)
					{
						MOCAR_LOG_DEBUG_FMT("requestor hpmsType : %d", tmp_sts->requester.typeData.hpmsType);
					}
				}
			}
			
			if (LANE_ID == tmp_sts->inBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("inBoundLane lane : %d", tmp_sts->inBoundLane.choice.lane);
			}
			else if (APPROACH_ID == tmp_sts->inBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("inBoundLane approach: %d", tmp_sts->inBoundLane.choice.approach);
			}
			else if (CONNECTION_ID == tmp_sts->inBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("inBoundLane connection: %d", tmp_sts->inBoundLane.choice.connection);
			}
			else
			{
				MOCAR_LOG_ERROR_FMT("inBoundLane msg_id: %d", tmp_sts->inBoundLane.msg_id);
				return;
			}

			if (SDK_OPTIONAL_EXSIT == tmp_sts->outBoundLane_is_exsit)
			{
				if (LANE_ID == tmp_sts->outBoundLane.msg_id)
				{
					MOCAR_LOG_DEBUG_FMT("outBoundLane lane : %d", tmp_sts->outBoundLane.choice.lane);
				}
				else if (APPROACH_ID == tmp_sts->outBoundLane.msg_id)
				{
					MOCAR_LOG_DEBUG_FMT("outBoundLane approach: %d", tmp_sts->outBoundLane.choice.approach);
				}
				else if (CONNECTION_ID == tmp_sts->outBoundLane.msg_id)
				{
					MOCAR_LOG_DEBUG_FMT("outBoundLane connection: %d", tmp_sts->outBoundLane.choice.connection);
				}
				else
				{
					MOCAR_LOG_ERROR_FMT("error outBoundLane msg_id: %d", tmp_sts->outBoundLane.msg_id);
					return;
				}
			}

			if (SDK_OPTIONAL_EXSIT == tmp_sts->minute_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("minute : %d", tmp_sts->minute);
			}
			if (SDK_OPTIONAL_EXSIT == tmp_sts->second_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("second : %d", tmp_sts->second);
			}
			if (SDK_OPTIONAL_EXSIT == tmp_sts->duration_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("duration : %d", tmp_sts->duration);
			}
			
			MOCAR_LOG_DEBUG_FMT("status : %d", tmp_sts->status);

		}
	}
}

void v2x_user_ssm_recv_handle(v2x_msg_ssm_t* usr_ssm, void* param)
{
    MOCAR_LOG_INFO("rxmsg-SSM");
	
	v2x_ssm_rx_data_dump(usr_ssm);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_ssm(v2x_msg_ssm_t* usr_ssm)
{
    static uint32_t     msgcount   = 0;

    if(NULL == usr_ssm)
    {
        return -1;
    }

	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

	usr_ssm->timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
	usr_ssm->timeStamp = minute_of_the_year(tm_time);

	struct timeval tv;
	gettimeofday(&tv, NULL);	
	usr_ssm->second = tv.tv_sec%60 * 1000 + tv.tv_usec / 1000;

	usr_ssm->sequenceNumber_is_exist = SDK_OPTIONAL_EXSIT;
	usr_ssm->sequenceNumber = msgcount;
	msgcount++;
	if (msgcount > 127)
	{
		msgcount = 0;
	}

	usr_ssm->SignalStatus_count = 2;
	for(uint8_t i = 0; i < usr_ssm->SignalStatus_count; i++)
	{
		Mde_SignalStatus_t * tmp_sig = &usr_ssm->SignalStatus[i];

		tmp_sig->sequenceNumber = i;
		tmp_sig->region_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_sig->region = 12345;
		tmp_sig->intersection_id = 54321;

		tmp_sig->sigStatus_count = 2;
		for(uint8_t j = 0; j < tmp_sig->sigStatus_count; j++)
		{
			Mde_SignalStatusPackage_t * tmp_sts = &tmp_sig->sigStatus[j];

			tmp_sts->requester_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.id.msg_id = TEMPORARY_ID;
			memcpy(tmp_sts->requester.id.choice.entityID, "7777", 4);
			tmp_sts->requester.request = 200;
			tmp_sts->requester.sequenceNumber = j;
			tmp_sts->requester.role_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.role = 10;
			tmp_sts->requester.typeData_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.typeData.role = 10;
			tmp_sts->requester.typeData.subrole_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.typeData.subrole = 9;
			tmp_sts->requester.typeData.requestLevel_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.typeData.requestLevel = 8;
			tmp_sts->requester.typeData.iso3883_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.typeData.iso3883 = 7;
			tmp_sts->requester.typeData.hpmsType_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->requester.typeData.hpmsType = 6;
			
			tmp_sts->inBoundLane.msg_id = LANE_ID;
			tmp_sts->inBoundLane.choice.lane = 250;
			
			tmp_sts->outBoundLane_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->outBoundLane.msg_id = APPROACH_ID; 
			tmp_sts->outBoundLane.choice.approach = 15;
			
			tmp_sts->minute_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->minute = 1515;
			tmp_sts->second_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->second = 1516;
			tmp_sts->duration_is_exsit = SDK_OPTIONAL_EXSIT;
			tmp_sts->duration = 1517;

			tmp_sts->status = 7;
		}		
	}
	
    return  0;
}


void v2x_ssm_tx(void)
{
    v2x_msg_ssm_t   usr_ssm;
    int ret         = -1;

    memset(&usr_ssm, 0, sizeof(v2x_msg_ssm_t));
    ret = v2x_fill_usr_ssm(&usr_ssm);
    if(0 != ret)
    {
        fprintf(stderr, "user ssm fill fail\n");
        return;
    }

    ret = mde_v2x_ssm_send(&usr_ssm, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "ssm send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-SSM: send msg successed\n");
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

    mde_v2x_ssm_recv_handle_register(v2x_user_ssm_recv_handle); 
    printf("ssm recv handle register");
    
    while(1)
    {
        v2x_ssm_tx();
        //usleep(100*1000);
        sleep(1);
    }
    
    return 0;
}

