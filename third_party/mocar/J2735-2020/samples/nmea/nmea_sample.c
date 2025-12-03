#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_user_nmea_recv_handle(v2x_msg_nmea_t* usr_nmea, void* param)
{
    printf("rxmsg-nmea: message \n");
	if (NULL == usr_nmea)
	{
		MOCAR_LOG_ERROR("usr nmea is null");
		return;		
	}

	if (SDK_OPTIONAL_EXSIT == usr_nmea->timeStamp_active)
	{
		MOCAR_LOG_DEBUG_FMT("timeStamp : %ld", usr_nmea->timeStamp);
	}
	
	if (SDK_OPTIONAL_EXSIT == usr_nmea->rev_active)
	{
		MOCAR_LOG_DEBUG_FMT("rev : %ld", usr_nmea->rev);
	}
	
	if (SDK_OPTIONAL_EXSIT == usr_nmea->msg_active)
	{
		MOCAR_LOG_DEBUG_FMT("msg : %ld", usr_nmea->msg);
	}
	
	if (SDK_OPTIONAL_EXSIT == usr_nmea->wdCount_active)
	{
		MOCAR_LOG_DEBUG_FMT("wdCount : %ld", usr_nmea->wdCount);
	}
	
	MOCAR_LOG_DEBUG_FMT("payload : %s", usr_nmea->payload);
	MOCAR_LOG_DEBUG_FMT("payload length : %d", usr_nmea->payload_len);

}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_nmea(v2x_msg_nmea_t* usr_nmea)
{
    if (NULL == usr_nmea)
    {
        MOCAR_LOG_ERROR_FMT("usr_nmea invaild parameter");
        return -1;
    }
	
	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

    usr_nmea->timeStamp_active = SDK_OPTIONAL_EXSIT;
    usr_nmea->timeStamp = minute_of_the_year(tm_time);

	usr_nmea->rev_active = SDK_OPTIONAL_EXSIT;
	usr_nmea->rev = 1;
	
    usr_nmea->msg_active = SDK_OPTIONAL_EXSIT;
    usr_nmea->msg = 100;
	
    usr_nmea->wdCount_active = SDK_OPTIONAL_EXSIT;
    usr_nmea->wdCount = 1000;

    usr_nmea->payload_len = 4;
	char tmp_pay[5] = "test";
	memcpy(usr_nmea->payload, tmp_pay, 4);

    return 0;
}

void v2x_nmea_tx(void)
{
    v2x_msg_nmea_t   usr_nmea;
    int ret         = -1;

    memset(&usr_nmea, 0, sizeof(v2x_msg_nmea_t));
    ret = v2x_fill_usr_nmea(&usr_nmea);
    if(0 != ret)
    {
        fprintf(stderr, "user nmea fill fail\n");
        return;
    }
    MOCAR_LOG_INFO("nmea fill success!");
    ret = mde_v2x_nmea_send(&usr_nmea, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "nmea send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-nmea: send msg successed, sequence \n");
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

    mde_v2x_nmea_recv_handle_register(v2x_user_nmea_recv_handle); 
    printf("nmea recv handle register");
    
    while(1)
    {
        v2x_nmea_tx();
        usleep(1000*1000);
    }
    
    return 0;
}
