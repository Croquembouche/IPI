#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_srm_rx_data_dump(v2x_msg_srm_t* usr_srm)
{
	if (NULL == usr_srm)
	{
		MOCAR_LOG_ERROR("usr srm is null");
		return;
	}

	if (SDK_OPTIONAL_EXSIT == usr_srm->timeStamp_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("timestamp : %d", usr_srm->timeStamp);
	}
	
	MOCAR_LOG_DEBUG_FMT("second : %d", usr_srm->second);
	if (SDK_OPTIONAL_EXSIT == usr_srm->sequenceNumber_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("sequenceNumber : %d", usr_srm->sequenceNumber);
	}

	for(uint8_t i = 0; i < usr_srm->requests_count; i++)
	{
		Mde_SignalRequest_t * tmp_request = &usr_srm->requests[i];
		MOCAR_LOG_DEBUG_FMT("requests_count %d : ---", i);
		
		if (SDK_OPTIONAL_EXSIT == tmp_request->request_region_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("intersection region : %d", tmp_request->request_region);
		}
		MOCAR_LOG_DEBUG_FMT("intersection id : %d", tmp_request->request_id);
		MOCAR_LOG_DEBUG_FMT("requestID : %d", tmp_request->requestID);
		MOCAR_LOG_DEBUG_FMT("requestType : %d", tmp_request->requestType);
		
		if (LANE_ID == tmp_request->inBoundLane.msg_id)
		{
			MOCAR_LOG_DEBUG_FMT("inBoundLane lane : %d", tmp_request->inBoundLane.choice.lane);
		}
		else if (APPROACH_ID == tmp_request->inBoundLane.msg_id)
		{
			MOCAR_LOG_DEBUG_FMT("inBoundLane approach: %d", tmp_request->inBoundLane.choice.approach);
		}
		else if (CONNECTION_ID == tmp_request->inBoundLane.msg_id)
		{
			MOCAR_LOG_DEBUG_FMT("inBoundLane connection: %d", tmp_request->inBoundLane.choice.connection);
		}
		else
		{
			MOCAR_LOG_ERROR_FMT("inBoundLane msg_id: %d", tmp_request->inBoundLane.msg_id);
			return;
		}

		if (SDK_OPTIONAL_EXSIT == tmp_request->outBoundLane_is_exsit)
		{
			if (LANE_ID == tmp_request->outBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("outBoundLane lane : %d", tmp_request->outBoundLane.choice.lane);
			}
			else if (APPROACH_ID == tmp_request->outBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("outBoundLane approach: %d", tmp_request->outBoundLane.choice.approach);
			}
			else if (CONNECTION_ID == tmp_request->outBoundLane.msg_id)
			{
				MOCAR_LOG_DEBUG_FMT("outBoundLane connection: %d", tmp_request->outBoundLane.choice.connection);
			}
			else
			{
				MOCAR_LOG_ERROR_FMT("error outBoundLane msg_id: %d", tmp_request->outBoundLane.msg_id);
				return;
			}
		
		}

		if (SDK_OPTIONAL_EXSIT == tmp_request->minute_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("minute : %d", tmp_request->minute);
		}
		if (SDK_OPTIONAL_EXSIT == tmp_request->second_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("second : %d", tmp_request->second);
		}
		if (SDK_OPTIONAL_EXSIT == tmp_request->duration_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("duration : %d", tmp_request->duration);
		}
	}

	if (TEMPORARY_ID == usr_srm->requestor.veh_id.msg_id)
	{
		MOCAR_LOG_DEBUG_FMT("veh_id(entityID): %s", usr_srm->requestor.veh_id.choice.entityID);
	}
	else if (STATION_ID == usr_srm->requestor.veh_id.msg_id)
	{
		MOCAR_LOG_DEBUG_FMT("veh_id(stationID): %d", usr_srm->requestor.veh_id.choice.stationID);
	}
	else
	{
		MOCAR_LOG_ERROR_FMT("error veh_id msg_id: %d", usr_srm->requestor.veh_id.msg_id);
		return;
	}
	
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.requestorType_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor role : %d", usr_srm->requestor.requestorType.role);
		
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.requestorType.subrole_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor subrole : %d", usr_srm->requestor.requestorType.subrole);
		}
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.requestorType.requestLevel_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor requestLevel : %d", usr_srm->requestor.requestorType.requestLevel);
		}
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.requestorType.iso3883_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor iso3883 : %d", usr_srm->requestor.requestorType.iso3883);
		}
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.requestorType.hpmsType_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor hpmsType : %d", usr_srm->requestor.requestorType.hpmsType);
		}
	}

	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.position_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor latitude : %d", usr_srm->requestor.position.positionLat);
		MOCAR_LOG_DEBUG_FMT("requestor longitude : %d", usr_srm->requestor.position.positionLong);
		
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.position.positionElevation_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor elevation : %d", usr_srm->requestor.position.positionElevation);
		}
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.position.heading_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor heading : %d", usr_srm->requestor.position.heading);
		}
		if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.position.transmisson_speed_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("requestor transmisson : %d", usr_srm->requestor.position.transmisson);
			MOCAR_LOG_DEBUG_FMT("requestor speed : %d", usr_srm->requestor.position.speed);
		}		
	}
	
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.name_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor name : %s", usr_srm->requestor.name);
	}
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.routeName_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor routeName : %s", usr_srm->requestor.routeName);
	}
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.transitStatus_is_exsit)
	{
		uint8_t test_status[17] = {0};
		for(uint8_t i = 0; i < 8; i++)
		{
			test_status[2*i] = 48 + usr_srm->requestor.transitStatus[i];
			test_status[2*i+1] = ' ';
		}
		MOCAR_LOG_DEBUG_FMT("requestor transitStatus : %s", test_status);
	}
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.transitOccupancy_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor transitOccupancy : %d", usr_srm->requestor.transitOccupancy);
	}
	if (SDK_OPTIONAL_EXSIT == usr_srm->requestor.transitSchedule_is_exsit)
	{
		MOCAR_LOG_DEBUG_FMT("requestor transitSchedule : %d", usr_srm->requestor.transitSchedule);
	}
	
}

void v2x_user_srm_recv_handle(v2x_msg_srm_t* usr_srm, void* param)
{
    MOCAR_LOG_INFO("rxmsg-SRM");
	
	v2x_srm_rx_data_dump(usr_srm);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_srm(v2x_msg_srm_t* usr_srm)
{
    static uint32_t     msgcount   = 0;

    if(NULL == usr_srm)
    {
        return -1;
    }

	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

	usr_srm->timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
	usr_srm->timeStamp = minute_of_the_year(tm_time);

	struct timeval tv;
	gettimeofday(&tv, NULL);	
	usr_srm->second = tv.tv_sec%60 * 1000 + tv.tv_usec / 1000;

	usr_srm->sequenceNumber_is_exist = SDK_OPTIONAL_EXSIT;
	usr_srm->sequenceNumber = msgcount;
	msgcount++;
	if (msgcount > 127)
	{
		msgcount = 0;
	}

	usr_srm->requests_count = 2;
	for(uint8_t i = 0; i < usr_srm->requests_count; i++)
	{
		Mde_SignalRequest_t * tmp_req = &usr_srm->requests[i];
		
		tmp_req->request_region_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_req->request_region = 12345;
		tmp_req->request_id = 54321;

		tmp_req->requestID = 100;
		tmp_req->requestType = 1;

		tmp_req->inBoundLane.msg_id = LANE_ID;
		tmp_req->inBoundLane.choice.lane = 250;
		
		tmp_req->outBoundLane_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_req->outBoundLane.msg_id = APPROACH_ID; 
		tmp_req->outBoundLane.choice.approach = 15;

		tmp_req->minute_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_req->minute = 1515;
		tmp_req->second_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_req->second = 1516;
		tmp_req->duration_is_exsit = SDK_OPTIONAL_EXSIT;
		tmp_req->duration = 1517;
	}
	
	usr_srm->requestor.veh_id.msg_id = TEMPORARY_ID;
	memcpy(usr_srm->requestor.veh_id.choice.entityID, "7777", 4);
	
	usr_srm->requestor.requestorType_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.requestorType.role = 10;
	usr_srm->requestor.requestorType.subrole_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.requestorType.subrole = 9;
	usr_srm->requestor.requestorType.requestLevel_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.requestorType.requestLevel = 8;
	usr_srm->requestor.requestorType.iso3883_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.requestorType.iso3883 = 7;
	usr_srm->requestor.requestorType.hpmsType_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.requestorType.hpmsType = 6;

	usr_srm->requestor.position_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.position.positionLat = 900000000;
	usr_srm->requestor.position.positionLong = 900000001;
	usr_srm->requestor.position.positionElevation_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.position.positionElevation = -4000;

	usr_srm->requestor.position.heading_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.position.heading = 20010;
	usr_srm->requestor.position.transmisson_speed_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.position.transmisson = 0;
	usr_srm->requestor.position.speed = 8000;

	usr_srm->requestor.name_is_exsit = SDK_OPTIONAL_EXSIT;
	memcpy(usr_srm->requestor.name, "shangban  chidao", 16);
	
	usr_srm->requestor.routeName_is_exsit = SDK_OPTIONAL_EXSIT;
	memcpy(usr_srm->requestor.routeName, "yilu zhixing", 12);

	usr_srm->requestor.transitStatus_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.transitStatus[0] = 1;
	usr_srm->requestor.transitStatus[2] = 1;
	usr_srm->requestor.transitStatus[4] = 1;

	usr_srm->requestor.transitOccupancy_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.transitOccupancy = 1;

	usr_srm->requestor.transitSchedule_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_srm->requestor.transitSchedule = -120;
	

    return  0;
}


void v2x_srm_tx(void)
{
    v2x_msg_srm_t   usr_srm;
    int ret         = -1;

    memset(&usr_srm, 0, sizeof(v2x_msg_srm_t));
    ret = v2x_fill_usr_srm(&usr_srm);
    if(0 != ret)
    {
        fprintf(stderr, "user srm fill fail\n");
        return;
    }

    ret = mde_v2x_srm_send(&usr_srm, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "srm send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-SRM: send msg successed\n");
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

    mde_v2x_srm_recv_handle_register(v2x_user_srm_recv_handle); 
    printf("srm recv handle register");
    
    while(1)
    {
        v2x_srm_tx();
        //usleep(100*1000);
        sleep(1);
    }
    
    return 0;
}

