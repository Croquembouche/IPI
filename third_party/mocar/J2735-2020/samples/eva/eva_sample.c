#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>

void v2x_user_eva_recv_handle(v2x_msg_eva_t* usr_eva, void* param)
{
    printf("rxmsg-EVA: message count %s\n", usr_eva->temp_id);
}

int v2x_fill_usr_eva(v2x_msg_eva_t* usr_eva)
{
    static uint32_t msgcount = 0;
	char tmpId[4] = {"1234"};
    //int i = 0;

    if (usr_eva == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

	usr_eva->temp_id_exist = SDK_OPTIONAL_EXSIT;
	memcpy(usr_eva->temp_id, tmpId, sizeof(usr_eva->temp_id));

	usr_eva->time_stamp_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->time_stamp = 12345;

	usr_eva->rsaMsg.msgCnt = msgcount % 128;
    msgcount++;

	usr_eva->rsaMsg.typeEvent = 11;

	usr_eva->rsaMsg.priority_exist = SDK_OPTIONAL_EXSIT;
    usr_eva->rsaMsg.priority = 1;

	usr_eva->rsaMsg.description_num = 1;

	memcpy(usr_eva->rsaMsg.description, "t", 1);

	usr_eva->responseType_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->responseType = MDE_ResponseType_emergency;

	usr_eva->mass_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->mass = 2.5; // 2.5 t

	usr_eva->basicType_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->basicType = MDE_VehicleType_special;

	usr_eva->vehicleType_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->vehicleType = MDE_VehicleGroupAffected_motorcycles;

	usr_eva->responseEquip_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->responseEquip = MDE_IncidentResponseEquipment_engine;

	usr_eva->responderType_exist = SDK_OPTIONAL_EXSIT;
	usr_eva->responderType = MDE_ResponderGroupAffected_emergency_vehicle_units;

    return 0;
}

void v2x_eva_tx(void)
{
    v2x_msg_eva_t   usr_eva;
    int ret         = -1;

    memset(&usr_eva, 0, sizeof(v2x_msg_eva_t));
    ret = v2x_fill_usr_eva(&usr_eva);
    if(0 != ret)
    {
        fprintf(stderr, "user eva fill fail\n");
        return;
    }

    ret = mde_v2x_eva_send(&usr_eva, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "eva send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-EVA: send msg successed, sequence %ld\n", usr_eva.rsaMsg.msgCnt);
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

    mde_v2x_eva_recv_handle_register(v2x_user_eva_recv_handle); 
    printf("eva recv handle register\n");
    while(1)
    {
        v2x_eva_tx();
        usleep(1000*1000);
    }
    
    return 0;
}

