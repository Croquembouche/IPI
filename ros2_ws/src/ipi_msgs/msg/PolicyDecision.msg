uint8 MAINTAIN=0
uint8 START=1
uint8 THROTTLE=2
uint8 PAUSE=3
uint8 USE_LOCAL_FALLBACK=4
uint8 REJECT=5

uint8 NONE=0
uint8 ZONE_INACTIVE=1
uint8 SERVICE_UNAVAILABLE=2
uint8 LINK_UNAVAILABLE=3
uint8 LATENCY_BUDGET_EXCEEDED=4
uint8 LOSS_BUDGET_EXCEEDED=5
uint8 CONGESTED=6
uint8 LOCAL_FALLBACK_REQUIRED=7
uint8 INVALID_INTENT=8

uint8 NO_TRANSPORT=255

builtin_interfaces/Time stamp
string request_id
uint8 action
uint8 selected_transport
float64 maximum_rate_hz
int64 validity_ms
uint8 reason
