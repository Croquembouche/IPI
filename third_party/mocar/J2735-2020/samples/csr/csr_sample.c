#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mocar_log.h>
#include <v2x_api.h>

void v2x_user_csr_recv_handle(v2x_msg_csr_t* usr_csr, void* param)
{
    printf("rxmsg-CSR: message count %d\n", usr_csr->msg_count);
}

int v2x_fill_usr_csr(v2x_msg_csr_t* usr_csr)
{
    static uint32_t msgcount = 0;
	char tmpId[4] = {"1234"};
    int i = 0;

    if (usr_csr == NULL)
    {
        fprintf(stderr, "invaild parameter");
        return -1;
    }

    usr_csr->msg_count = msgcount % 128;
    msgcount++;

	memcpy(usr_csr->temp_id, tmpId, sizeof(usr_csr->temp_id));

	usr_csr->time_stamp = 12345;
		
    usr_csr->n_requestedItem = 1;
    
    for(i = 0; i < usr_csr->n_requestedItem; i++)
    {
        usr_csr->requestedItem[i] = 10;     
    }

    return 0;
}

void v2x_csr_tx(void)
{
    v2x_msg_csr_t   usr_csr;
    int ret         = -1;

    memset(&usr_csr, 0, sizeof(v2x_msg_csr_t));
    ret = v2x_fill_usr_csr(&usr_csr);
    if(0 != ret)
    {
        fprintf(stderr, "user csr fill fail\n");
        return;
    }

    ret = mde_v2x_csr_send(&usr_csr, NULL, 32);
    if(0 != ret)
    {
        fprintf(stderr, "csr send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-CSR: send msg successed, sequence %u\n", usr_csr.msg_count);
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

    mde_v2x_csr_recv_handle_register(v2x_user_csr_recv_handle); 
    printf("csr recv handle register");
    
    while(1)
    {
        v2x_csr_tx();
        usleep(1000*1000);
    }
    
    return 0;
}

