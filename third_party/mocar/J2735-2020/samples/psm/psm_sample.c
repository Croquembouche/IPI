#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>

void v2x_user_psm_debug_print_path_history(PSM_crumbData_t   *history_pos_data)
{
    
}

void v2x_user_psm_debug_print_data(v2x_msg_psm_t* usr_psm)
{
    
}

void v2x_user_psm_recv_handle(v2x_msg_psm_t* usr_psm, void* param)
{
    printf("rxmsg-PSM: message count %d\n", usr_psm->msg_count);
    MOCAR_LOG_INFO_FMT("%d" , 1);
}

int v2x_fill_usr_psm(v2x_msg_psm_t* usr_psm)
{
    static uint8_t msgcount = 0;
    char tmpId[] = {"haoyaiyo"};
    //int i = 0;

    if (usr_psm == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

    usr_psm->msg_count = msgcount % 128;
    msgcount++;

    memcpy(usr_psm->temp_id, tmpId, sizeof(usr_psm->temp_id));

    usr_psm->basicType = PSM_PersonalDeviceUserType_aPEDESTRIAN;
    usr_psm->secMark = 2;
    usr_psm->longitude = 120.2222222;
    usr_psm->latitude = 30.5555555;
    usr_psm->posAccuracy.semimajor = 88.868;
    usr_psm->posAccuracy.semiminor = 88.898;
    usr_psm->posAccuracy.orientation = 88.888;
    usr_psm->speed = 88;
    usr_psm->heading = 99;
    usr_psm->accelSet.long_Acceleration = 8.88;
    usr_psm->accelSet.lat_Acceleration = 8.98;
    usr_psm->accelSet.vert_VerticalAcceleration = 8.68;
    usr_psm->accelSet.yaw_YawRate = 8.28;
    usr_psm->pathHistory.initialPosition.transmission_state = Mde_TransmissionState_forwardGears;
    usr_psm->pathHistory.initialPosition.position_speed_confidence.pos_confidence = Mde_PositionConfidence_a200m;
    usr_psm->pathHistory.initialPosition.position_speed_confidence.elevation_confidence = Mde_ElevationConfidence_elev_020_00;
    usr_psm->pathHistory.initialPosition.position_speed_confidence.heading_confidence = Mde_HeadingConfidence_prec0_1deg;
    usr_psm->pathHistory.initialPosition.position_speed_confidence.speed_confidence = Mde_SpeedConfidence_prec100ms;
    usr_psm->pathHistory.initialPosition.position_speed_confidence.throttle_confidence = Mde_ThrottleConfidence_prec10percent;
    usr_psm->pathHistory.history_pos_num = 2;
    usr_psm->pathHistory.history_pos_data = calloc(usr_psm->pathHistory.history_pos_num , sizeof(PSM_crumbData_t));
    if ( usr_psm->pathHistory.history_pos_data != NULL )
    {
        for (uint8_t i = 0 ; i < usr_psm->pathHistory.history_pos_num ; i++ )
        {
            usr_psm->pathHistory.history_pos_data[i].latOffset = 1;
            usr_psm->pathHistory.history_pos_data[i].lonOffset = 2;
            usr_psm->pathHistory.history_pos_data[i].elevationOffset = 3;
            usr_psm->pathHistory.history_pos_data[i].timeOffset = 4;
            usr_psm->pathHistory.history_pos_data[i].posAccuracy.semimajor = 5;
            usr_psm->pathHistory.history_pos_data[i].posAccuracy.semiminor = 6;
            usr_psm->pathHistory.history_pos_data[i].posAccuracy.orientation = 7;
        }
    }
    else
    {
        MOCAR_LOG_ERROR("calloc error");
        return -1;
    }

    usr_psm->pathPrediction.radiusOfCurve = 100;
    usr_psm->pathPrediction.confidence = 120;
    usr_psm->propulsion.present = PSM_PropelledInformation_PR_human;
    usr_psm->propulsion.choice.human = PSM_HumanPropelledType_onFoot;

    return 0;
}

void v2x_psm_tx(void)
{
    v2x_msg_psm_t   usr_psm;
    int ret         = -1;

    memset(&usr_psm, 0, sizeof(v2x_msg_psm_t));
    ret = v2x_fill_usr_psm(&usr_psm);
    if(0 != ret)
    {
        fprintf(stderr, "user psm fill fail\n");
        return;
    }

    ret = mde_v2x_psm_send(&usr_psm, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "psm send fail\n");
        return;
    }

    fprintf(stderr, "txmsg-PSM: send msg successed, sequence %u\n", usr_psm.msg_count);
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

    mde_v2x_psm_recv_handle_register(v2x_user_psm_recv_handle); 
    printf("psm recv handle register");

    while(1)
    {
        v2x_psm_tx();
        sleep(1);
    }

    return 0;
}

