#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>

void v2x_custom_recv_handle(char* buffer, int len)
{
    printf("rxmsg-CUS: len = %d --- %s\n", len, buffer);
}

void v2x_custom_tx(void)
{
    char buffer[1024] = "Hello, Huali";
    int ret = -1;

    ret = mde_v2x_custom_send(buffer, strlen(buffer), 0);
    if(0 != ret)
    {
        fprintf(stderr, "custom send fail\n");
        return;   
    }

    fprintf(stderr, "txmsg-CUS: send msg successed!\n");
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

    mde_v2x_custom_recv_handle_register(v2x_custom_recv_handle); 
    printf("custon recv handle register");
    
    while(1)
    {
        v2x_custom_tx();
        usleep(100*1000);
    }
    
    return 0;
}
