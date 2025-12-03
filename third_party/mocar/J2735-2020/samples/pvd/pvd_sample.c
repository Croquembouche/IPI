#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>
#include <assert.h>

void v2x_user_pvd_recv_handle(v2x_msg_pvd_t* usr_pvd, void* param)
{
    printf("rxmsg-PVD: message segNum %ld\n", usr_pvd->segNum);
}

int v2x_fill_usr_pvd(v2x_msg_pvd_t* usr_pvd)
{
    static uint32_t msgcount = 0;
	//char vinTmp[4] = {"1234"};

    if (usr_pvd == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

	usr_pvd->time_stamp_exit = 1;
	usr_pvd->time_stamp = 12345;

	usr_pvd->segNum_exit = 1;
    usr_pvd->segNum = msgcount % 128;
    msgcount++;

	usr_pvd->probeID_exit = 1;
	usr_pvd->probeID.name_active = 1;
	memcpy(usr_pvd->probeID.name, "test_pvd_tx", 11);
	
	usr_pvd->probeID.vin_active = 1;
	memcpy(usr_pvd->probeID.vin, "v12345678765", 12);

	usr_pvd->probeID.ownerCode_active = 1;
	memcpy(usr_pvd->probeID.ownerCode, "huali_123456", 12);

	usr_pvd->probeID.vehId_active= 1;
	usr_pvd->probeID.vehId.present = MDE_VehicleID_PR_entityID;
	//usr_pvd->probeID.vehId.choice.stationID = 2;
	memcpy(usr_pvd->probeID.vehId.choice.entityID, "hl12", 4);

	usr_pvd->probeID.vehType_active= 1;
	usr_pvd->probeID.vehType = MDE_VehicleType_car;

	usr_pvd->probeID.vehClass.present = MDE_VehicleIdent_vehicleClass_PR_vGroup;
	usr_pvd->probeID.vehClass.choice.vGroup = MDE_PVD_VehicleGroupAffected_cars;

	usr_pvd->startVector.utcTime_active = 1;
	usr_pvd->startVector.utcTime.year_is_exsit = 1;
	usr_pvd->startVector.utcTime.year = 2021;
	usr_pvd->startVector.utcTime.month_is_exsit = 1;
	usr_pvd->startVector.utcTime.month = 1;
	usr_pvd->startVector.utcTime.day_is_exsit = 1;
	usr_pvd->startVector.utcTime.day = 29;

	usr_pvd->startVector.heading_active = 1;
	usr_pvd->startVector.heading = 86;
	usr_pvd->startVector.lat = 30;
	usr_pvd->startVector.Long = 120;

	usr_pvd->startVector.speed_active = 1;
	usr_pvd->startVector.speed.transmisson = 2;
	usr_pvd->startVector.speed.speed = 25;

	usr_pvd->vehicleType.keyType_is_exsit = 1;
	usr_pvd->vehicleType.keyType = 2;
	usr_pvd->vehicleType.role_is_exsit = 1;
	usr_pvd->vehicleType.role = 4;
	usr_pvd->vehicleType.iso3883_is_exsit = 1;
	usr_pvd->vehicleType.iso3883 = 40;

	usr_pvd->snapshot_num = 1;
	usr_pvd->snapshots[0].thePosition.lat = 30;
	usr_pvd->snapshots[0].thePosition.Long = 121;
	usr_pvd->snapshots[0].thePosition.speed_active = 1;
	usr_pvd->snapshots[0].thePosition.speed.transmisson = 3;
	usr_pvd->snapshots[0].thePosition.speed.speed = 23;

    return 0;
}

void v2x_pvd_tx(void)
{
    v2x_msg_pvd_t   usr_pvd;
    int ret         = -1;

    memset(&usr_pvd, 0, sizeof(v2x_msg_pvd_t));
    ret = v2x_fill_usr_pvd(&usr_pvd);
    if(0 != ret)
    {
        fprintf(stderr, "user pvd fill fail\n");
        return;
    }

    ret = mde_v2x_pvd_send(&usr_pvd, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "pvd send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-PVD: send msg successed, sequence %ld\n", usr_pvd.segNum);
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

    mde_v2x_pvd_recv_handle_register(v2x_user_pvd_recv_handle); 
    printf("pvd recv handle register");
    
    while(1)
    {
        v2x_pvd_tx();
        usleep(1000*1000);
    }
    
    return 0;
}

