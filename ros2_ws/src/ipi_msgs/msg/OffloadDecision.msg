uint8 LOCAL=0
uint8 EDGE=1
uint8 HYBRID=2

uint8 EDGE_FASTER=0
uint8 LOCAL_FASTER=1
uint8 EDGE_UNAVAILABLE=2
uint8 LINK_UNAVAILABLE=3
uint8 INSUFFICIENT_CONFIDENCE=4
uint8 DEADLINE_MISS_PREDICTED=5
uint8 INVALID_INPUT=6

builtin_interfaces/Time stamp
string request_id
uint8 execution_site
int64 predicted_local_completion_ms
int64 predicted_edge_completion_ms
bool deadline_expected_to_be_met
bool fallback_required
uint8 reason
