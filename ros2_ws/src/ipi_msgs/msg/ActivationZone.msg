uint8 CIRCLE=0
uint8 POLYGON=1

uint8 MODEL_PULL=0
uint8 TELEMETRY_PUSH=1
uint8 ENABLE_EDGE_PERCEPTION=2
uint8 ENABLE_EDGE_PLANNING=3
uint8 SERVICE_MODE_HINT=4

builtin_interfaces/Time stamp
string zone_id
uint64 revision
uint8 geometry_type
float64[] latitudes
float64[] longitudes
float64 radius_m
builtin_interfaces/Time valid_from
builtin_interfaces/Time expires_at
float64 enter_hysteresis_m
float64 exit_hysteresis_m
uint32 minimum_dwell_ms
uint8[] actions
