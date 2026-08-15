uint8 LANE_KEEPING_AID=0
uint8 UNPROTECTED_LEFT_AVAILABILITY=1
uint8 PERCEPTION_AID=2
uint8 PLANNING_AID=3
uint8 CONTROL_AID=4
uint8 COMPUTATION_AID=5
uint8 HD_MAP_UPDATE=6

uint8 SAFETY_CRITICAL=0
uint8 REAL_TIME=1
uint8 INTERACTIVE=2
uint8 BEST_EFFORT=3

builtin_interfaces/Time stamp
string request_id
string session_id
uint8 service_type
uint8 tier
int64 deadline_ms
int64 maximum_rtt_ms
float64 maximum_loss_fraction
float64 minimum_rate_hz
bool local_fallback_available
bool currently_active
