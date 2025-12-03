#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_pdm_rx_data_dump(v2x_msg_pdm_t* usr_pdm)
{
	if (NULL == usr_pdm)
	{
		MOCAR_LOG_ERROR("usr pdm is null");
		return;
	}

	if (SDK_OPTIONAL_EXSIT == usr_pdm->timeStamp_is_exist)
	{
		MOCAR_LOG_DEBUG_FMT("timestamp : %d", usr_pdm->timeStamp);
	}

	MOCAR_LOG_DEBUG_FMT("sampleStart : %d", usr_pdm->sampleStart);
	MOCAR_LOG_DEBUG_FMT("sampleEnd : %d", usr_pdm->sampleEnd);

	char tmp_dir[40] = {0};
	for(uint8_t i = 0; i < 16; i++)
	{
		tmp_dir[i*2] = usr_pdm->directions[i] + '0';
		tmp_dir[i*2 + 1] = ' ';
	}
	MOCAR_LOG_DEBUG_FMT("directions : %s", tmp_dir);

	if (TERM_TIME == usr_pdm->term.msg_id)
	{
		MOCAR_LOG_DEBUG_FMT("term time : %d", usr_pdm->term.choice.time);
	}
	else if (TERM_DISTANCE == usr_pdm->term.msg_id)
	{
		MOCAR_LOG_DEBUG_FMT("term distance : %d", usr_pdm->term.choice.distance);
	}
	else
	{
		MOCAR_LOG_ERROR_FMT("error pdm term msg_id : %d", usr_pdm->term.msg_id);
	}

	if (TERM_TIME == usr_pdm->snapshot.msg_id)
	{
		MOCAR_LOG_DEBUG_FMT("snapshotTime speed1 : %d", usr_pdm->snapshot.choice.snapshotTime.speed1);
		MOCAR_LOG_DEBUG_FMT("snapshotTime time1 : %d", usr_pdm->snapshot.choice.snapshotTime.time1);
		MOCAR_LOG_DEBUG_FMT("snapshotTime speed2 : %d", usr_pdm->snapshot.choice.snapshotTime.speed2);
		MOCAR_LOG_DEBUG_FMT("snapshotTime time2 : %d", usr_pdm->snapshot.choice.snapshotTime.time2);
	}
	else if (TERM_DISTANCE == usr_pdm->snapshot.msg_id)
	{		
		MOCAR_LOG_DEBUG_FMT("snapshotDistance distance1 : %d", usr_pdm->snapshot.choice.snapshotDistance.distance1);
		MOCAR_LOG_DEBUG_FMT("snapshotDistance speed1 : %d", usr_pdm->snapshot.choice.snapshotDistance.speed1);
		MOCAR_LOG_DEBUG_FMT("snapshotDistance distance2 : %d", usr_pdm->snapshot.choice.snapshotDistance.distance2);
		MOCAR_LOG_DEBUG_FMT("snapshotDistance speed2 : %d", usr_pdm->snapshot.choice.snapshotDistance.speed2);
	}
	else
	{
		MOCAR_LOG_ERROR_FMT("error pdm snapshot msg_id : %d", usr_pdm->snapshot.msg_id);
	}
	
	MOCAR_LOG_DEBUG_FMT("txInterval : %d", usr_pdm->txInterval);

	for(uint8_t i = 0; i < usr_pdm->dataElements_count; i++)
	{		
		MOCAR_LOG_DEBUG_FMT("dataElements[%d] dataType : %d", i, usr_pdm->dataElements[i].dataType);
		if (SDK_OPTIONAL_EXSIT == usr_pdm->dataElements[i].subType_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("dataElements[%d] subType : %d", i, usr_pdm->dataElements[i].subType);
		}
		if (SDK_OPTIONAL_EXSIT == usr_pdm->dataElements[i].sendOnLessThenValue_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("dataElements[%d] sendOnLessThenValue : %d", i, usr_pdm->dataElements[i].sendOnLessThenValue);
		}
		if (SDK_OPTIONAL_EXSIT == usr_pdm->dataElements[i].sendOnMoreThenValue_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("dataElements[%d] sendOnMoreThenValue : %d", i, usr_pdm->dataElements[i].sendOnMoreThenValue);
		}
		if (SDK_OPTIONAL_EXSIT == usr_pdm->dataElements[i].sendAll_is_exsit)
		{
			MOCAR_LOG_DEBUG_FMT("dataElements[%d] sendAll : %d", i, usr_pdm->dataElements[i].sendAll);
		}
	}

}

void v2x_user_pdm_recv_handle(v2x_msg_pdm_t* usr_pdm, void* param)
{
    printf("rxmsg-PDM\n");
	
	v2x_pdm_rx_data_dump(usr_pdm);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_pdm(v2x_msg_pdm_t* usr_pdm)
{
    if(NULL == usr_pdm)
    {
        return -1;
    }

	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

	usr_pdm->timeStamp_is_exist = SDK_OPTIONAL_EXSIT;
	usr_pdm->timeStamp = minute_of_the_year(tm_time);

	usr_pdm->sampleStart = 10;
	usr_pdm->sampleEnd = 20;
	usr_pdm->directions[0] = 1;
	usr_pdm->directions[1] = 1;
	usr_pdm->directions[2] = 1;
	usr_pdm->directions[3] = 1;

	usr_pdm->term.msg_id = TERM_TIME;
	usr_pdm->term.choice.time = 1000;

	usr_pdm->snapshot.msg_id = TERM_DISTANCE;
	usr_pdm->snapshot.choice.snapshotDistance.distance1 = 1000;
	usr_pdm->snapshot.choice.snapshotDistance.speed1 = 30;
	usr_pdm->snapshot.choice.snapshotDistance.distance2 = 1001;
	usr_pdm->snapshot.choice.snapshotDistance.speed2 = 31;

	usr_pdm->txInterval = 60;

	
	usr_pdm->dataElements_count = 3;
	for(uint8_t i = 0; i < usr_pdm->dataElements_count; i++)
	{
		usr_pdm->dataElements[i].dataType = 25;
		usr_pdm->dataElements[i].subType_is_exsit = SDK_OPTIONAL_EXSIT;
		usr_pdm->dataElements[i].subType = 2;
		usr_pdm->dataElements[i].sendOnLessThenValue_is_exsit = SDK_OPTIONAL_EXSIT;
		usr_pdm->dataElements[i].sendOnLessThenValue = 3;
		usr_pdm->dataElements[i].sendOnMoreThenValue_is_exsit = SDK_OPTIONAL_EXSIT;
		usr_pdm->dataElements[i].sendOnMoreThenValue = 4;
		usr_pdm->dataElements[i].sendAll_is_exsit = SDK_OPTIONAL_EXSIT;
		usr_pdm->dataElements[i].sendAll = 1;		
	}
	
    return  0;
}


void v2x_pdm_tx(void)
{
    v2x_msg_pdm_t   usr_pdm;
    int ret         = -1;

    memset(&usr_pdm, 0, sizeof(v2x_msg_pdm_t));
    ret = v2x_fill_usr_pdm(&usr_pdm);
    if(0 != ret)
    {
        fprintf(stderr, "user pdm fill fail\n");
        return;
    }

    ret = mde_v2x_pdm_send(&usr_pdm, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "pdm send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-PDM: send msg successed\n");
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

    mde_v2x_pdm_recv_handle_register(v2x_user_pdm_recv_handle); 
    printf("pdm recv handle register");
    
    while(1)
    {
        v2x_pdm_tx();
        //usleep(100*1000);
        sleep(1);
    }
    
    return 0;
}


