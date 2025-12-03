#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void v2x_user_rsa_recv_handle(v2x_msg_rsa_t* usr_rsa, void* param)
{
    printf("rxmsg-RSA: message count %d\n", usr_rsa->msg_count);
    MOCAR_LOG_DEBUG_FMT("rxmsg-RSA: message count %d , typeEvent  =%d, longitude  =%lf, latitude  =%lf, transmission_state  =%d, speed  =%d, posAccuracy.semiMajor  =%d, posAccuracy.semiMinor  =%d, posAccuracy.orientation  =%d, position_speed_confidence.pos_confidence =%d, position_speed_confidence.elevation_confidence =%d, position_speed_confidence.heading_confidence =%d, position_speed_confidence.speed_confidence =%d, position_speed_confidence.throttle_confidence =%d, " ,
                   usr_rsa->msg_count , usr_rsa->typeEvent , usr_rsa->longitude ,usr_rsa->latitude ,usr_rsa->transmission_state ,usr_rsa->speed ,usr_rsa->posAccuracy.semimajor ,usr_rsa->posAccuracy.semiminor ,usr_rsa->posAccuracy.orientation ,usr_rsa->position_speed_confidence.pos_confidence,usr_rsa->position_speed_confidence.elevation_confidence,usr_rsa->position_speed_confidence.heading_confidence,usr_rsa->position_speed_confidence.speed_confidence,usr_rsa->position_speed_confidence.throttle_confidence);
}

int v2x_fill_usr_rsa(v2x_msg_rsa_t* usr_rsa)
{
    #define RTS_DESCRIPTION  "test"
    static uint32_t      msgcount   = 0;
    //int i = 0;

    if (usr_rsa == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

    usr_rsa->msg_count  = msgcount % 128;
    msgcount++;

    usr_rsa->typeEvent  = 11;
    usr_rsa->longitude  = 120.1234567;
    usr_rsa->latitude  = 30.1234567;
    usr_rsa->transmission_state  = Mde_TransmissionState_forwardGears;
    usr_rsa->speed  = 25;
    usr_rsa->posAccuracy.semimajor  = 11;
    usr_rsa->posAccuracy.semiminor  = 12;
    usr_rsa->posAccuracy.orientation  = 13;
    usr_rsa->position_speed_confidence.pos_confidence = Mde_PositionConfidence_a20m;
    usr_rsa->position_speed_confidence.elevation_confidence = Mde_ElevationConfidence_elev_020_00;
    usr_rsa->position_speed_confidence.heading_confidence = Mde_HeadingConfidence_prec10deg;
    usr_rsa->position_speed_confidence.speed_confidence = Mde_SpeedConfidence_prec10ms;
    usr_rsa->position_speed_confidence.throttle_confidence = Mde_ThrottleConfidence_prec10percent;
    return 0;
}

void v2x_rsa_tx(void)
{
    v2x_msg_rsa_t   usr_rsa;
    int ret         = -1;

    memset(&usr_rsa, 0, sizeof(v2x_msg_rsa_t));
    ret = v2x_fill_usr_rsa(&usr_rsa);
    if(0 != ret)
    {
        fprintf(stderr, "user rsa fill fail\n");
        return;
    }

    ret = mde_v2x_rsa_send(&usr_rsa, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "rsa send fail\n");
        return;
    }

    fprintf(stderr, "txmsg-RSA: send msg successed, sequence %u\n", usr_rsa.msg_count);
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

    mde_v2x_rsa_recv_handle_register(v2x_user_rsa_recv_handle); 
    printf("rsa recv handle register\n");

    while(1)
    {
        v2x_rsa_tx();
        usleep(500*1000);
    }

    return 0;
}
