#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_rtcm_rx_data_dump(v2x_msg_rtcm_t* usr_rtcm)
{
	if (NULL == usr_rtcm)
	{
		MOCAR_LOG_ERROR("usr rtcm is null");
		return;
	}

	MOCAR_LOG_DEBUG_FMT("msgCnt : %ld", usr_rtcm->msgCnt);
	MOCAR_LOG_DEBUG_FMT("rev : %ld", usr_rtcm->rev);

	if (SDK_OPTIONAL_EXSIT == usr_rtcm->timeStamp_active)
	{
		MOCAR_LOG_DEBUG_FMT("timestamp : %ld", usr_rtcm->timeStamp);
	}

	if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint_active)
	{
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime_active)
		{
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.year_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_year: %d", usr_rtcm->anchorPoint.utcTime.year);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.month_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_month: %d", usr_rtcm->anchorPoint.utcTime.month);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.day_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_day: %d", usr_rtcm->anchorPoint.utcTime.day);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.hour_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_hour: %d", usr_rtcm->anchorPoint.utcTime.hour);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.minute_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_minute: %d", usr_rtcm->anchorPoint.utcTime.minute);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.second_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_second: %d", usr_rtcm->anchorPoint.utcTime.second);
			}
			if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.utcTime.offset_is_exsit)
			{
				MOCAR_LOG_DEBUG_FMT("dateTime_offset: %d", usr_rtcm->anchorPoint.utcTime.offset);
			}
		}
		
		MOCAR_LOG_DEBUG_FMT("Long : %ld", usr_rtcm->anchorPoint.Long);
		MOCAR_LOG_DEBUG_FMT("lat : %ld", usr_rtcm->anchorPoint.lat);

		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.elevation_active)
		{
			MOCAR_LOG_DEBUG_FMT("elevation : %ld", usr_rtcm->anchorPoint.elevation);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.heading_active)
		{
			MOCAR_LOG_DEBUG_FMT("heading : %ld", usr_rtcm->anchorPoint.heading);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.speed_active)
		{
			MOCAR_LOG_DEBUG_FMT("transmisson : %ld", usr_rtcm->anchorPoint.speed.transmisson);
			MOCAR_LOG_DEBUG_FMT("speed : %ld", usr_rtcm->anchorPoint.speed.speed);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.posAccuracy_active)
		{
			MOCAR_LOG_DEBUG_FMT("semiMajor : %ld", usr_rtcm->anchorPoint.posAccuracy.semiMajor);
			MOCAR_LOG_DEBUG_FMT("semiMinor : %ld", usr_rtcm->anchorPoint.posAccuracy.semiMinor);
			MOCAR_LOG_DEBUG_FMT("orientation : %ld", usr_rtcm->anchorPoint.posAccuracy.orientation);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.timeConfidence_active)
		{
			MOCAR_LOG_DEBUG_FMT("timeConfidence : %ld", usr_rtcm->anchorPoint.timeConfidence);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.posConfidence_active)
		{
			MOCAR_LOG_DEBUG_FMT("pos : %ld", usr_rtcm->anchorPoint.posConfidence.pos);
			MOCAR_LOG_DEBUG_FMT("elevation : %ld", usr_rtcm->anchorPoint.posConfidence.elevation);
		}
		if (SDK_OPTIONAL_EXSIT == usr_rtcm->anchorPoint.speedConfidence_active)
		{
			MOCAR_LOG_DEBUG_FMT("heading : %d", usr_rtcm->anchorPoint.speedConfidence.heading);
			MOCAR_LOG_DEBUG_FMT("speed : %d", usr_rtcm->anchorPoint.speedConfidence.speed);
			MOCAR_LOG_DEBUG_FMT("throttle : %d", usr_rtcm->anchorPoint.speedConfidence.throttle);
		}
	}

	if (SDK_OPTIONAL_EXSIT == usr_rtcm->rtcmHeader_active)
	{
		char tmp_bit_string[40] = {0};
		for(uint8_t m = 0; m < MDE_GNSSSTATUS_BIT_MAX; m++)
		{
			tmp_bit_string[m*2] = usr_rtcm->rtcmHeader.status.buf[m] + '0';
			tmp_bit_string[m*2 + 1] = ' ';
		}
		MOCAR_LOG_DEBUG_FMT("rtcmHeader status : %s", tmp_bit_string);
		MOCAR_LOG_DEBUG_FMT("antOffsetX : %ld", usr_rtcm->rtcmHeader.offsetSet.antOffsetX);
		MOCAR_LOG_DEBUG_FMT("antOffsetY : %ld", usr_rtcm->rtcmHeader.offsetSet.antOffsetY);
		MOCAR_LOG_DEBUG_FMT("antOffsetZ : %ld", usr_rtcm->rtcmHeader.offsetSet.antOffsetZ);
	}

	MOCAR_LOG_DEBUG_FMT("msgs_count : %d", usr_rtcm->msgs_count);
	if (usr_rtcm->msgs_count <= MSGS_COUNT_MAX)
	{
		for (uint8_t i = 0; i < usr_rtcm->msgs_count; i++)
		{
			MOCAR_LOG_DEBUG_FMT("msgs[%d] : %s", i, usr_rtcm->msgs[i].buff);
		}
	}
}

void v2x_user_rtcm_recv_handle(v2x_msg_rtcm_t* usr_rtcm, void* param)
{
    MOCAR_LOG_INFO("rxmsg-rtcm: message count \n");

	v2x_rtcm_rx_data_dump(usr_rtcm);
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int v2x_fill_usr_rtcm(v2x_msg_rtcm_t* usr_rtcm)
{
    if (usr_rtcm == NULL)
    {
        MOCAR_LOG_ERROR("usr rtcm invaild parameter");
        return -1;
    }
	
    static uint8_t msgcount = 0;
	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);
	
    usr_rtcm->msgCnt = msgcount % 128;
    msgcount++;

	usr_rtcm->rev = 2;
	
    usr_rtcm->timeStamp_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->timeStamp = minute_of_the_year(tm_time);

	
    usr_rtcm->anchorPoint_active = SDK_OPTIONAL_EXSIT;
	
	usr_rtcm->anchorPoint.utcTime_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.year_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.year = 2021;
	usr_rtcm->anchorPoint.utcTime.month_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.month = 1;
	usr_rtcm->anchorPoint.utcTime.day_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.day = 13;
	usr_rtcm->anchorPoint.utcTime.hour_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.hour = 18;
	usr_rtcm->anchorPoint.utcTime.minute_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.minute = 59;
	usr_rtcm->anchorPoint.utcTime.second_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.second = 60000;
	usr_rtcm->anchorPoint.utcTime.offset_is_exsit = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.utcTime.offset = 800;
	
	usr_rtcm->anchorPoint.Long = 18;
	usr_rtcm->anchorPoint.lat = 19;
	usr_rtcm->anchorPoint.elevation_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.elevation = -20;
	usr_rtcm->anchorPoint.heading_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.heading = 21;
	usr_rtcm->anchorPoint.speed_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.speed.transmisson = 3;
	usr_rtcm->anchorPoint.speed.speed = 8000;
	usr_rtcm->anchorPoint.posAccuracy_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.posAccuracy.semiMajor = 150;
	usr_rtcm->anchorPoint.posAccuracy.semiMinor = 200;
	usr_rtcm->anchorPoint.posAccuracy.orientation = 250;
	usr_rtcm->anchorPoint.timeConfidence_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.timeConfidence = 15;
	usr_rtcm->anchorPoint.posConfidence_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.posConfidence.pos = 10;
	usr_rtcm->anchorPoint.posConfidence.elevation = 11;
	usr_rtcm->anchorPoint.speedConfidence_active = SDK_OPTIONAL_EXSIT;
	usr_rtcm->anchorPoint.speedConfidence.heading = 5;
	usr_rtcm->anchorPoint.speedConfidence.speed = 6;
	usr_rtcm->anchorPoint.speedConfidence.throttle = 1;

	usr_rtcm->rtcmHeader_active = SDK_OPTIONAL_EXSIT;
    usr_rtcm->rtcmHeader.status.size = 1;
    usr_rtcm->rtcmHeader.status.buf[0] = 1;
	
    usr_rtcm->rtcmHeader.offsetSet.antOffsetX = -2000;
	usr_rtcm->rtcmHeader.offsetSet.antOffsetY = -200;
	usr_rtcm->rtcmHeader.offsetSet.antOffsetZ = -500;

    usr_rtcm->msgs_count = 5;
	char tmp_msg[20] = "i: this is test msg";
	
    usr_rtcm->msgs[0].size = 20;
	memcpy(usr_rtcm->msgs[0].buff, tmp_msg, sizeof(tmp_msg));
    usr_rtcm->msgs[0].buff[0] = 0 + '0';
    usr_rtcm->msgs[1].size = 20;
	memcpy(usr_rtcm->msgs[1].buff, tmp_msg, sizeof(tmp_msg));
    usr_rtcm->msgs[1].buff[0] = 1 + '0';
    usr_rtcm->msgs[2].size = 20;
	memcpy(usr_rtcm->msgs[2].buff, tmp_msg, sizeof(tmp_msg));
    usr_rtcm->msgs[2].buff[0] = 2 + '0';
    usr_rtcm->msgs[3].size = 20;
	memcpy(usr_rtcm->msgs[3].buff, tmp_msg, sizeof(tmp_msg));
    usr_rtcm->msgs[3].buff[0] = 3 + '0';
    usr_rtcm->msgs[4].size = 20;
	memcpy(usr_rtcm->msgs[4].buff, tmp_msg, sizeof(tmp_msg));
    usr_rtcm->msgs[4].buff[0] = 4 + '0';
 
    return 0;
}

void v2x_rtcm_tx(void)
{
    v2x_msg_rtcm_t   usr_rtcm;
    int ret         = -1;

    memset(&usr_rtcm, 0, sizeof(v2x_msg_rtcm_t));
    ret = v2x_fill_usr_rtcm(&usr_rtcm);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("user rtcm fill fail\n");
        return;
    }
    MOCAR_LOG_INFO("rtcm fill success!");
    ret = mde_v2x_rtcm_send(&usr_rtcm, NULL, 32);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("rtcm send fail\n");
        return;   
    }

    MOCAR_LOG_INFO("txmsg-rtcm: send msg successed, sequence \n");
}

int main(int argc, char* argv[])
{
    int ret = -1;

    ret = mde_v2x_init(0);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("cv2x init fail\n");
        return -1;    
    }

    mde_v2x_rtcm_recv_handle_register(v2x_user_rtcm_recv_handle); 
    MOCAR_LOG_INFO("rtcm recv handle register");
    
    while(1)
    {
        v2x_rtcm_tx();
        usleep(1000*1000);
    }
    
    return 0;
}
