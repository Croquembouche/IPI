#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <v2x_api.h>

#define MAX_PAYLOAD_BYTES 2048
#define PREVIEW_BYTES 64
#define MOCAR_CUSTOM_MSG_ID 0x1b

/* The deployed SDK's mde_v2x_custom_send() symbol returns success without
 * sending, so use the SDK packet sender with the custom message id directly.
 * Keep experiment payload sweeps capped at 2 KB; larger custom frames were not
 * reliable on the OBU/RSU link. */
extern int v2x_packet_data_send(char* buffer, int len, int msg_id);

typedef enum Mode {
    MODE_RX = 0,
    MODE_TX = 1,
    MODE_BOTH = 2
} Mode;

typedef struct Args {
    Mode mode;
    uint32_t count;
    uint32_t interval_ms;
    uint32_t payload_bytes;
    uint32_t wait_after_ms;
    int asn_check;
    char node_id[64];
} Args;

static volatile sig_atomic_t g_running = 1;
static uint64_t g_rx_count = 0;

static Args g_args = {
    .mode = MODE_RX,
    .count = 20,
    .interval_ms = 100,
    .payload_bytes = 32,
    .wait_after_ms = 1000,
    .asn_check = 0,
    .node_id = "node"
};

static uint64_t realtime_ns(void)
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

static void sleep_ms(uint32_t ms)
{
    struct timespec req;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (long)(ms % 1000) * 1000000L;
    while (g_running && nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
}

static void usage(const char* program)
{
    fprintf(stderr,
            "Usage: %s [--mode rx|tx|both] [--node-id <id>] [--count <n>]\n"
            "       [--interval-ms <ms>] [--payload-bytes <n>] [--asn-check <0|1>]\n"
            "       [--wait-after-ms <ms>]\n",
            program);
}

static uint32_t parse_u32(const char* name, const char* value)
{
    char* end = NULL;
    errno = 0;
    unsigned long parsed = strtoul(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed > UINT32_MAX) {
        fprintf(stderr, "invalid %s: %s\n", name, value);
        exit(2);
    }
    return (uint32_t)parsed;
}

static void parse_args(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            const char* mode = argv[++i];
            if (strcasecmp(mode, "rx") == 0) {
                g_args.mode = MODE_RX;
            } else if (strcasecmp(mode, "tx") == 0) {
                g_args.mode = MODE_TX;
            } else if (strcasecmp(mode, "both") == 0) {
                g_args.mode = MODE_BOTH;
            } else {
                fprintf(stderr, "invalid --mode: %s\n", mode);
                exit(2);
            }
        } else if (strcmp(argv[i], "--node-id") == 0 && i + 1 < argc) {
            snprintf(g_args.node_id, sizeof(g_args.node_id), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            g_args.count = parse_u32("--count", argv[++i]);
        } else if (strcmp(argv[i], "--interval-ms") == 0 && i + 1 < argc) {
            g_args.interval_ms = parse_u32("--interval-ms", argv[++i]);
        } else if (strcmp(argv[i], "--payload-bytes") == 0 && i + 1 < argc) {
            g_args.payload_bytes = parse_u32("--payload-bytes", argv[++i]);
            if (g_args.payload_bytes == 0 || g_args.payload_bytes > MAX_PAYLOAD_BYTES) {
                fprintf(stderr, "--payload-bytes must be in 1..%d\n", MAX_PAYLOAD_BYTES);
                exit(2);
            }
        } else if (strcmp(argv[i], "--asn-check") == 0 && i + 1 < argc) {
            g_args.asn_check = (int)parse_u32("--asn-check", argv[++i]);
            if (g_args.asn_check != 0 && g_args.asn_check != 1) {
                fprintf(stderr, "--asn-check must be 0 or 1\n");
                exit(2);
            }
        } else if (strcmp(argv[i], "--wait-after-ms") == 0 && i + 1 < argc) {
            g_args.wait_after_ms = parse_u32("--wait-after-ms", argv[++i]);
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[i]);
            usage(argv[0]);
            exit(2);
        }
    }
}

static void print_hex_preview(const char* buffer, int len)
{
    int preview = len < PREVIEW_BYTES ? len : PREVIEW_BYTES;
    for (int i = 0; i < preview; ++i) {
        fprintf(stderr, "%02x", (unsigned int)(unsigned char)buffer[i]);
    }
}

static void custom_rx_cb(char* buffer, int len)
{
    ++g_rx_count;
    fprintf(stderr,
            "rxmsg-CUS,count=%" PRIu64 ",time_ns=%" PRIu64 ",len=%d,ascii=\"%.*s\",hex=",
            g_rx_count,
            realtime_ns(),
            len,
            len > PREVIEW_BYTES ? PREVIEW_BYTES : len,
            buffer == NULL ? "" : buffer);
    if (buffer != NULL && len > 0) {
        print_hex_preview(buffer, len);
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

static void fill_payload(char* payload, uint32_t len, uint32_t sequence)
{
    int written = snprintf(payload,
                           len + 1,
                           "IPI_CUSTOM_PROBE node=%s seq=%" PRIu32 " time_ns=%" PRIu64 " ",
                           g_args.node_id,
                           sequence,
                           realtime_ns());
    if (written < 0) {
        written = 0;
    }
    for (uint32_t i = (uint32_t)written; i < len; ++i) {
        payload[i] = (char)('A' + (i % 26));
    }
    payload[len] = '\0';
}

static int send_probe(uint32_t sequence)
{
    char* payload = calloc((size_t)g_args.payload_bytes + 1U, 1U);
    if (payload == NULL) {
        fprintf(stderr, "calloc failed\n");
        return -1;
    }
    fill_payload(payload, g_args.payload_bytes, sequence);

    int ret = v2x_packet_data_send(payload, (int)g_args.payload_bytes, MOCAR_CUSTOM_MSG_ID);
    if (ret != 0) {
        fprintf(stderr,
                "txmsg-CUS-fail,count=%" PRIu32 ",time_ns=%" PRIu64 ",ret=%d,len=%" PRIu32 "\n",
                sequence,
                realtime_ns(),
                ret,
                g_args.payload_bytes);
        fflush(stderr);
        free(payload);
        return ret;
    }

    fprintf(stderr,
            "txmsg-CUS,count=%" PRIu32 ",time_ns=%" PRIu64 ",len=%" PRIu32 ",asn_check=%d\n",
            sequence,
            realtime_ns(),
            g_args.payload_bytes,
            g_args.asn_check);
    fflush(stderr);
    free(payload);
    return 0;
}

int main(int argc, char** argv)
{
    parse_args(argc, argv);
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (mde_v2x_init(0) != 0) {
        fprintf(stderr, "mde_v2x_init failed\n");
        return 1;
    }
    if (mde_v2x_custom_recv_handle_register(custom_rx_cb) != 0) {
        fprintf(stderr, "mde_v2x_custom_recv_handle_register failed\n");
        return 1;
    }

    fprintf(stderr,
            "custom_probe_ready,mode=%d,node_id=%s,count=%" PRIu32 ",interval_ms=%" PRIu32
            ",payload_bytes=%" PRIu32 ",asn_check=%d\n",
            g_args.mode,
            g_args.node_id,
            g_args.count,
            g_args.interval_ms,
            g_args.payload_bytes,
            g_args.asn_check);
    fflush(stderr);

    if (g_args.mode == MODE_TX || g_args.mode == MODE_BOTH) {
        for (uint32_t seq = 1; g_running && seq <= g_args.count; ++seq) {
            send_probe(seq);
            sleep_ms(g_args.interval_ms);
        }
        sleep_ms(g_args.wait_after_ms);
        return 0;
    }

    while (g_running) {
        sleep_ms(1000);
    }
    return 0;
}
