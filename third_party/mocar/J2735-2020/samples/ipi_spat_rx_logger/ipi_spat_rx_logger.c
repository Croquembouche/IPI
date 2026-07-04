#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <v2x_api.h>

static volatile sig_atomic_t g_running = 1;
static uint64_t g_rx_count = 0;

static uint64_t unix_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
}

static void handle_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

static void spat_rx_cb(v2x_msg_spat_t* spat, void* param)
{
    (void)param;
    ++g_rx_count;

    int intersection_id = -1;
    int revision = -1;
    int state_count = 0;
    int signal_group = -1;
    int event_state = -1;
    const char* name = "";

    if (spat != NULL) {
        if (spat->name_is_exist == SDK_OPTIONAL_EXSIT) {
            name = spat->name;
        }
        if (spat->intersections_count > 0) {
            Mde_Intersection_State_t* inter = &spat->intersections[0];
            intersection_id = inter->id;
            revision = inter->revision;
            state_count = inter->state_count;
            if (inter->state_count > 0) {
                signal_group = inter->state[0].signalGroup;
                if (inter->state[0].state_time_speed_count > 0) {
                    event_state = inter->state[0].state_time_speed[0].eventState;
                }
            }
        }
    }

    printf("rxmsg-SPAT,count=%llu,time_ns=%llu,name=%s,intersections=%u,id=%d,revision=%d,state_count=%d,signal_group=%d,event_state=%d\n",
           (unsigned long long)g_rx_count,
           (unsigned long long)unix_time_ns(),
           name,
           spat == NULL ? 0U : (unsigned)spat->intersections_count,
           intersection_id,
           revision,
           state_count,
           signal_group,
           event_state);
    fflush(stdout);
}

int main(void)
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (mde_v2x_init(0) != 0) {
        fprintf(stderr, "mde_v2x_init failed\n");
        return 1;
    }
    if (mde_v2x_spat_recv_handle_register(spat_rx_cb) != 0) {
        fprintf(stderr, "mde_v2x_spat_recv_handle_register failed\n");
        return 1;
    }

    printf("spat_rx_logger_ready\n");
    fflush(stdout);
    while (g_running) {
        sleep(1);
    }
    return 0;
}
