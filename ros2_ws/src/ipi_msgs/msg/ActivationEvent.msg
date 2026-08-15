uint8 OUTSIDE=0
uint8 ENTERING=1
uint8 ACTIVE=2
uint8 EXITING=3
uint8 FALLBACK=4

uint8 NONE=0
uint8 ZONE_INSTALLED=1
uint8 ENTRY_CANDIDATE=2
uint8 ENTRY_CONFIRMED=3
uint8 ENTRY_CANCELLED=4
uint8 EXIT_CANDIDATE=5
uint8 EXIT_CONFIRMED=6
uint8 EXIT_CANCELLED=7
uint8 INFRASTRUCTURE_UNAVAILABLE=8
uint8 ZONE_NOT_VALID=9
uint8 RECOVERED=10

builtin_interfaces/Time stamp
string zone_id
uint64 revision
uint8 previous_state
uint8 state
uint64 sequence
uint8 reason
uint8[] enabled_actions
