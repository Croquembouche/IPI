#ifndef _V2X_API_H_
#define _V2X_API_H_

#include <v2x_msg_bsm.h>
#include <v2x_msg_map.h>
#include <v2x_msg_spat.h>
#include <v2x_msg_srm.h>
#include <v2x_msg_ssm.h>
#include <v2x_msg_rsi.h>
#include <v2x_msg_rsm.h>
#include <v2x_msg_csr.h>
#include <v2x_msg_eva.h>
#include <v2x_msg_ica.h>
#include <v2x_msg_pvd.h>
#include <v2x_msg_rsa.h>
#include <v2x_msg_psm.h>
#include <v2x_msg_pdm.h>
#include <v2x_msg_tim.h>
#include <v2x_msg_rtcm.h>
#include <v2x_msg_nmea.h>

typedef void (*bsm_recv_cb) (v2x_msg_bsm_t* user_bsm, void* param);
typedef void (*map_recv_cb) (v2x_msg_map_t* user_map, void* param);
typedef void (*spat_recv_cb) (v2x_msg_spat_t* user_spat, void* param);
typedef void (*srm_recv_cb) (v2x_msg_srm_t* user_srm, void* param);
typedef void (*ssm_recv_cb) (v2x_msg_ssm_t* user_ssm, void* param);
typedef void (*rsi_recv_cb) (v2x_msg_rsi_t* user_rsi, void* param);
typedef void (*rsm_recv_cb) (v2x_msg_rsm_t* user_rsm, void* param);
typedef void (*csr_recv_cb) (v2x_msg_csr_t* user_csr, void* param);
typedef void (*eva_recv_cb) (v2x_msg_eva_t* user_eva, void* param);
typedef void (*ica_recv_cb) (v2x_msg_ica_t* user_ica, void* param);
typedef void (*pvd_recv_cb) (v2x_msg_pvd_t* user_pvd, void* param);
typedef void (*rsa_recv_cb) (v2x_msg_rsa_t* user_rsa, void* param);
typedef void (*psm_recv_cb) (v2x_msg_psm_t* user_psm , void* param);
typedef void (*pdm_recv_cb) (v2x_msg_pdm_t* user_pdm , void* param);
typedef void (*tim_recv_cb) (v2x_msg_tim_t* user_tim, void* param);
typedef void (*rtcm_recv_cb) (v2x_msg_rtcm_t* user_rtcm, void* param);
typedef void (*nmea_recv_cb) (v2x_msg_nmea_t* user_nmea, void* param);
typedef void (*custom_recv_cb) (char* buffer, int len);

int mde_cv2x_debug_init(int debug_state);
//cv2x init
//security: 1 enable, 0 disable
int mde_v2x_init(int security_state);

//bsm recv 
int mde_v2x_bsm_recv_handle_register(bsm_recv_cb bsm_cb);

//map recv
int mde_v2x_map_recv_handle_register(map_recv_cb map_cb);

int mde_v2x_tim_recv_handle_register(tim_recv_cb tim_cb);

int mde_v2x_nmea_recv_handle_register(nmea_recv_cb nmea_cb);

int mde_v2x_rtcm_recv_handle_register(rtcm_recv_cb rtcm_cb);
//spat recv
int mde_v2x_spat_recv_handle_register(spat_recv_cb spat_cb);

int mde_v2x_srm_recv_handle_register(srm_recv_cb srm_cb);

int mde_v2x_ssm_recv_handle_register(ssm_recv_cb ssm_cb);
//rsm recv
int mde_v2x_rsm_recv_handle_register(rsm_recv_cb rsm_cb);

//rsi recv
int mde_v2x_rsi_recv_handle_register(rsi_recv_cb rsi_cb);

//csr recv
int mde_v2x_csr_recv_handle_register(csr_recv_cb csr_cb);

//eva recv
int mde_v2x_eva_recv_handle_register(eva_recv_cb eva_cb);

//ica recv
int mde_v2x_ica_recv_handle_register(ica_recv_cb ica_cb);

//pvd recv
int mde_v2x_pvd_recv_handle_register(pvd_recv_cb pvd_cb);

//custom recv
int mde_v2x_custom_recv_handle_register(custom_recv_cb cus_cb);

//rsa recv
int mde_v2x_rsa_recv_handle_register(rsa_recv_cb rsa_cb);

//psm recv
int mde_v2x_psm_recv_handle_register(psm_recv_cb psm_cb);

//pdm recv
int mde_v2x_pdm_recv_handle_register(pdm_recv_cb pdm_cb);

//bsm send
int mde_v2x_bsm_send(v2x_msg_bsm_t *usr_bsm, void *param, int aid);

//map send
int mde_v2x_map_send(v2x_msg_map_t *usr_map, void *param, int aid);

//spat send
int mde_v2x_spat_send(v2x_msg_spat_t* usr_spat, void *param, int aid);

int mde_v2x_srm_send(v2x_msg_srm_t* usr_srm, void *param, int aid);

int mde_v2x_ssm_send(v2x_msg_ssm_t* usr_ssm, void *param, int aid);

//csr send
int mde_v2x_csr_send(v2x_msg_csr_t *usr_csr, void *param, int aid);

//eva send
int mde_v2x_eva_send(v2x_msg_eva_t *usr_eva, void *param, int aid);

//ica send
int mde_v2x_ica_send(v2x_msg_ica_t *usr_ica, void *param, int aid);

//pvd send
int mde_v2x_pvd_send(v2x_msg_pvd_t *usr_pvd, void *param, int aid);

//custom send
int mde_v2x_custom_send(char* buffer, int len, int asn_check);

//rsa send
int mde_v2x_rsa_send(v2x_msg_rsa_t *usr_rsa, void *param, int aid);

//psm send
int mde_v2x_psm_send(v2x_msg_psm_t *usr_psm, void *param, int aid);

//pdm send
int mde_v2x_pdm_send(v2x_msg_pdm_t *usr_pdm, void *param, int aid);

//tim send
int mde_v2x_tim_send(v2x_msg_tim_t* usr_tim, void *param, int aid);
//rtcm send
int mde_v2x_rtcm_send(v2x_msg_rtcm_t* usr_rtcm, void *param, int aid);
//nmea send
int mde_v2x_nmea_send(v2x_msg_nmea_t* usr_nmea, void *param, int aid);

#endif
