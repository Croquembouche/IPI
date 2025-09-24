#define uint64_t mocar_uint64_t
#define int64_t mocar_int64_t
#define uint32_t mocar_uint32_t
#define int32_t mocar_int32_t
#define uint16_t mocar_uint16_t
#define int16_t mocar_int16_t
#include <v2x_api.h>
#undef uint64_t
#undef int64_t
#undef uint32_t
#undef int32_t
#undef uint16_t
#undef int16_t

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#define DEFAULT_LOG_CONFIG "../third_party/mocar/samples/mocar_log.conf"
#define MODULE_NAME "ipi_demo"

#define MAX_BSM_MSG_COUNT 128
#define MAP_LANES 2
#define MAP_NODE_POINTS 4
#define SPAT_PHASES 2

typedef void (*timer_func)(union sigval sig);

static int create_timer(timer_t* timer_id, timer_func func, void* param)
{
    struct sigevent sev;
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = param;
    sev.sigev_notify_function = func;

    if (timer_create(CLOCK_REALTIME, &sev, timer_id) < 0) {
        perror("timer_create");
        return -1;
    }
    return 0;
}

static int set_timer(timer_t timer_id, int period_ms)
{
    struct itimerspec its;
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = period_ms / 1000;
    its.it_value.tv_nsec = (period_ms % 1000) * 1000000L;
    its.it_interval = its.it_value;

    if (timer_settime(timer_id, 0, &its, NULL) < 0) {
        perror("timer_settime");
        return -1;
    }
    return 0;
}

// ---------------------------------------------------------------------
// BSM

static void fill_bsm(v2x_msg_bsm_t* msg)
{
    static unsigned int counter = 0;
    memset(msg, 0, sizeof(*msg));
    msg->msgcount = counter++ % MAX_BSM_MSG_COUNT;
    msg->temp_id = 0x01020304;
    msg->unix_time = time(NULL);
    msg->latitude = 37.336500;
    msg->longitude = -121.889900;
    msg->elevation = 30.0;
    msg->speed = 12.5 * 3.6;
    msg->heading = 90.0;
    msg->longaccel = 0.2;
    msg->lataccel = 0.1;
    msg->vertaccel = 0.05;
    msg->yawrate = 0.01;
    msg->positionalaccuracy[0] = 1.0;
    msg->positionalaccuracy[1] = 1.0;
    msg->positionalaccuracy[2] = 0.0;
    msg->transmissionstate = 2; // forward gears
    msg->vehicletype = 4;
    msg->width = 2.0;
    msg->length = 4.5;
}

static void bsm_tx_handler(union sigval sig)
{
    (void)sig;
    v2x_msg_bsm_t msg;
    fill_bsm(&msg);
    if (mde_v2x_send_bsm(&msg) != 0) {
        fprintf(stderr, "[BSM] send failed\n");
        return;
    }
    fprintf(stdout, "[BSM] sent msgCount=%u\n", msg.msgcount);
}

static void bsm_rx_handler(v2x_msg_bsm_t* msg)
{
    if (!msg) return;
    fprintf(stdout, "[BSM] recv tempId=0x%08x lat=%.6f lon=%.6f speed=%.2f\n",
            msg->temp_id, msg->latitude, msg->longitude, msg->speed);
}

// ---------------------------------------------------------------------
// MAP

static int fill_map(v2x_msg_map_t* map)
{
    int i = 0, j = 0;
    memset(map, 0, sizeof(*map));
    map->msg_revision = 1;
    map->n_geo_descr = 1;
    map->isec_geo_descr = calloc(map->n_geo_descr, sizeof(mde_isec_geo_descr_t));
    if (!map->isec_geo_descr) return -1;

    for (i = 0; i < map->n_geo_descr; ++i) {
        mde_isec_geo_descr_t* descr = &map->isec_geo_descr[i];
        descr->intersection_id = 101 + i;
        descr->msg_revision = 1;
        descr->ref_pt.latitude = 37.3365;
        descr->ref_pt.longitude = -121.8899;
        descr->ref_pt.elevation = 30.0;
        descr->lanewidth = 3.5;
        descr->n_lanelist = MAP_LANES;
        descr->lanelist = calloc(descr->n_lanelist, sizeof(mde_lanelist_t));
        if (!descr->lanelist) return -1;
        for (j = 0; j < descr->n_lanelist; ++j) {
            mde_lanelist_t* lane = &descr->lanelist[j];
            lane->lane_id = j + 1;
            lane->phase = 1;
            lane->man = MDE_MAN_STRAIGHT_ALLOWED;
            lane->signal_group_id = j + 1;
            memset(&lane->lane_attr, 0, sizeof(lane->lane_attr));
            lane->n_nodepoint = MAP_NODE_POINTS;
            lane->node_point = calloc(lane->n_nodepoint, sizeof(mde_node_point_t));
            if (!lane->node_point) return -1;
            for (int k = 0; k < lane->n_nodepoint; ++k) {
                mde_node_point_t* node = &lane->node_point[k];
                node->node_id.node.latitude = descr->ref_pt.latitude + k * 1e-5;
                node->node_id.node.longitude = descr->ref_pt.longitude + k * 1e-5;
                node->node_id.node.elevation = descr->ref_pt.elevation;
                node->node_id.lanewidth = descr->lanewidth;
                memset(&node->nodeattr, 0, sizeof(node->nodeattr));
            }
        }
    }

    return 0;
}

static void map_tx_handler(union sigval sig)
{
    (void)sig;
    v2x_msg_map_t map;
    if (fill_map(&map) != 0) {
        fprintf(stderr, "[MAP] fill failed\n");
        mde_v2x_msg_map_free(&map);
        return;
    }
    if (mde_v2x_send_map(&map) != 0) {
        fprintf(stderr, "[MAP] send failed\n");
        mde_v2x_msg_map_free(&map);
        return;
    }
    fprintf(stdout, "[MAP] sent intersection=%u\n", map.isec_geo_descr[0].intersection_id);
    mde_v2x_msg_map_free(&map);
}

static void map_rx_handler(v2x_msg_map_t* map)
{
    if (!map || map->n_geo_descr <= 0) return;
    fprintf(stdout, "[MAP] recv intersection=%u lanes=%d\n",
            map->isec_geo_descr[0].intersection_id,
            map->isec_geo_descr[0].n_lanelist);
}

// ---------------------------------------------------------------------
// SPaT

static int fill_spat(v2x_msg_spat_t* spat)
{
    int i;
    memset(spat, 0, sizeof(*spat));
    spat->n_intersections = 1;
    spat->Intersection_list = calloc(spat->n_intersections, sizeof(mde_Intersection_t));
    if (!spat->Intersection_list) return -1;

    mde_Intersection_t* intersection = &spat->Intersection_list[0];
    intersection->intersection_id = 101;
    intersection->msgCount = 1;
    intersection->intersection_status = Mde_ISEC_STATUS_FIXED_TIME_OPERATION;
    intersection->num_movement_states = SPAT_PHASES;
    intersection->movement_states = calloc(intersection->num_movement_states, sizeof(mde_MovementState_t));
    if (!intersection->movement_states) return -1;

    for (i = 0; i < SPAT_PHASES; ++i) {
        mde_MovementState_t* state = &intersection->movement_states[i];
        state->signal_group_id = i + 1;
        state->num_movement_event = 1;
        state->movement_event = calloc(1, sizeof(mde_MovementEventList_t));
        if (!state->movement_event) return -1;
        state->movement_event[0].phase_state = (i % 2 == 0) ?
            Mde_PHASE_STATE_PROTECTED_MOVEMENT_ALLOWED :
            Mde_PHASE_STATE_STOP_AND_REMAIN;
        state->movement_event[0].time_change_info.likely_time = 5000;
    }

    return 0;
}

static void spat_tx_handler(union sigval sig)
{
    (void)sig;
    v2x_msg_spat_t spat;
    if (fill_spat(&spat) != 0) {
        fprintf(stderr, "[SPaT] fill failed\n");
        mde_v2x_msg_spat_free(&spat);
        return;
    }
    if (mde_v2x_send_spat(&spat) != 0) {
        fprintf(stderr, "[SPaT] send failed\n");
        mde_v2x_msg_spat_free(&spat);
        return;
    }
    fprintf(stdout, "[SPaT] sent intersection=%ld\n", spat.Intersection_list[0].intersection_id);
    mde_v2x_msg_spat_free(&spat);
}

static void spat_rx_handler(v2x_msg_spat_t* spat)
{
    if (!spat) return;
    fprintf(stdout, "[SPaT] recv intersections=%d\n", spat->n_intersections);
}

// ---------------------------------------------------------------------
// SRM

static void fill_srm(v2x_msg_srm_t* srm)
{
    memset(srm, 0, sizeof(*srm));
    srm->bit_mask = 0x20;
    srm->second = 100;
    srm->requestor.bit_mask = 0x80;
    srm->requestor.id_choice = 2;
    srm->requestor.id.station_id = 0x1234;
    srm->requestor.requestor_type_bit_mask = 0x40;
    srm->requestor.requestor_type.role = BASIC_VEHICLE_TYPE_PUBLIC_TRANSPORT;
    srm->requestor.requestor_type.req_importance_level = 14;
    srm->srm_list_count = 1;
    srm->srmlist[0].signal_request.iid = 101;
    srm->srmlist[0].signal_request.request_id = 42;
    srm->srmlist[0].signal_request.req_type = SRM_PRIO_REQ_TYPE_REQ;
    srm->srmlist[0].signal_request.inbound_accesspoint_choice = 1;
    srm->srmlist[0].signal_request.in_bound_access_point.lane_id = 1;
}

static void srm_tx_handler(union sigval sig)
{
    (void)sig;
    v2x_msg_srm_t srm;
    fill_srm(&srm);
    if (mde_v2x_send_srm(&srm) != 0) {
        fprintf(stderr, "[SRM] send failed\n");
        return;
    }
    fprintf(stdout, "[SRM] sent requestId=%d\n", srm.srmlist[0].signal_request.request_id);
}

static void srm_rx_handler(v2x_msg_srm_t* srm)
{
    if (!srm) return;
    fprintf(stdout, "[SRM] recv requests=%d\n", srm->srm_list_count);
}

// ---------------------------------------------------------------------
// SSM

static void fill_ssm(v2x_msg_ssm_t* ssm)
{
    memset(ssm, 0, sizeof(*ssm));
    ssm->second = 100;
    ssm->sequenceNumber = 1;
    ssm->sslist_len = 1;
    ssm->sslist[0].id.id = 101;
    ssm->sslist[0].sequenceNumber = 1;
    ssm->sslist[0].ss_pkg_list_len = 1;
    ssm->sslist[0].ss_pkg_list[0].in_bound_on_choice = 1;
    ssm->sslist[0].ss_pkg_list[0].in_bound_access_point.lane_id = 1;
    ssm->sslist[0].ss_pkg_list[0].prio_resp = Mde_PRIORITIZATION_RESPONSE_GRANTED;
    ssm->sslist[0].ss_pkg_list[0].signal_request.vehicle_id.station_id = 0x1234;
}

static void ssm_tx_handler(union sigval sig)
{
    (void)sig;
    v2x_msg_ssm_t ssm;
    fill_ssm(&ssm);
    if (mde_v2x_send_ssm(&ssm) != 0) {
        fprintf(stderr, "[SSM] send failed\n");
        return;
    }
    fprintf(stdout, "[SSM] sent sequence=%u\n", ssm.sequenceNumber);
}

static void ssm_rx_handler(v2x_msg_ssm_t* ssm)
{
    if (!ssm) return;
    fprintf(stdout, "[SSM] recv list_len=%d\n", ssm->sslist_len);
}

// ---------------------------------------------------------------------

int main(void)
{
    const char* log_cfg = getenv("MOCAR_LOG_CFG");
    if (!log_cfg) {
        log_cfg = DEFAULT_LOG_CONFIG;
    }

    if (mde_v2x_init(log_cfg, MODULE_NAME) != 0) {
        fprintf(stderr, "Failed to initialise Mocar SDK\n");
        return -1;
    }

    if (mde_v2x_register_bsm(bsm_rx_handler) != 0) fprintf(stderr, "register BSM failed\n");
    if (mde_v2x_register_map(map_rx_handler) != 0) fprintf(stderr, "register MAP failed\n");
    if (mde_v2x_register_spat(spat_rx_handler) != 0) fprintf(stderr, "register SPaT failed\n");
    if (mde_v2x_register_srm(srm_rx_handler) != 0) fprintf(stderr, "register SRM failed\n");
    if (mde_v2x_register_ssm(ssm_rx_handler) != 0) fprintf(stderr, "register SSM failed\n");

    timer_t bsm_timer, map_timer, spat_timer, srm_timer, ssm_timer;

    if (create_timer(&bsm_timer, bsm_tx_handler, NULL) == 0) set_timer(bsm_timer, 1000);
    if (create_timer(&map_timer, map_tx_handler, NULL) == 0) set_timer(map_timer, 5000);
    if (create_timer(&spat_timer, spat_tx_handler, NULL) == 0) set_timer(spat_timer, 1000);
    if (create_timer(&srm_timer, srm_tx_handler, NULL) == 0) set_timer(srm_timer, 2000);
    if (create_timer(&ssm_timer, ssm_tx_handler, NULL) == 0) set_timer(ssm_timer, 2000);

    fprintf(stdout, "Mocar IPI demo running (Ctrl+C to exit)\n");
    while (1) {
        pause();
    }

    return 0;
}
