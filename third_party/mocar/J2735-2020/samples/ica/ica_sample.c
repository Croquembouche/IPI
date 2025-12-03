#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>

void v2x_user_ica_recv_handle(v2x_msg_ica_t* usr_ica, void* param)
{
    printf("rxmsg-ICA: message count %ld\n", usr_ica->msg_count);
}

int v2x_fill_usr_ica(v2x_msg_ica_t* usr_ica)
{
    static uint32_t msgcount = 0;
	char tmpId[4] = {"1234"};
    //int i = 0;

    if (usr_ica == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

    usr_ica->msg_count = msgcount % 128;
    msgcount++;

	memcpy(usr_ica->temp_id, tmpId, sizeof(usr_ica->temp_id));

	usr_ica->time_stamp = 12345;

	usr_ica->part_One.longitude = 120.123123;

	usr_ica->part_One.latitude = 30.123123;
	
	usr_ica->pathHistory_is_exsit = 1;
	usr_ica->pathHistory.ph_count = 2;
	usr_ica->pathHistory.path_his[0].latOffset = 30.123123;
	usr_ica->pathHistory.path_his[0].lonOffset = 120.123123;
	usr_ica->pathHistory.path_his[0].timeOffset = 25;
	usr_ica->pathHistory.path_his[1].latOffset = 30.164289;
	usr_ica->pathHistory.path_his[1].lonOffset = 120.128932;
	usr_ica->pathHistory.path_his[1].timeOffset = 20;

	usr_ica->pathPrediction_is_exsit = 1;
	usr_ica->pathPrediction.radiusOfCurve = 4;
	usr_ica->pathPrediction.confidence = 2;
		
	usr_ica->intersectionID.region = 1;

	usr_ica->intersectionID.id = 2;

	usr_ica->laneNumber.present = MDE_ApproachOrLane_PR_lane;

	usr_ica->laneNumber.choice.lane = 2;

	usr_ica->event_is_exist = 1;

	usr_ica->eventsFlag[2] = 1;

    return 0;
}

void v2x_ica_tx(void)
{
    v2x_msg_ica_t   usr_ica;
    int ret         = -1;

    memset(&usr_ica, 0, sizeof(v2x_msg_ica_t));
    ret = v2x_fill_usr_ica(&usr_ica);
    if(0 != ret)
    {
        fprintf(stderr, "user ica fill fail\n");
        return;
    }

    ret = mde_v2x_ica_send(&usr_ica, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "ica send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-ICA: send msg successed, sequence %ld\n", usr_ica.msg_count);
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

    mde_v2x_ica_recv_handle_register(v2x_user_ica_recv_handle); 
    printf("ica recv handle register");
    
    while(1)
    {
        v2x_ica_tx();
        usleep(1000*1000);
    }
    
    return 0;
}

