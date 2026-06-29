#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
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

#define IPI_RTT_MAGIC "IPI_RTT1"
#define IPI_RTT_MAX_NODE_ID 63
#define IPI_RTT_MAX_HEADER 512
#define IPI_RTT_MAX_PAYLOAD (128 * 1024)

typedef enum Role {
    ROLE_INITIATOR = 0,
    ROLE_RESPONDER = 1,
    ROLE_BOTH = 2
} Role;

typedef struct Args {
    Role role;
    char node_id[IPI_RTT_MAX_NODE_ID + 1];
    uint32_t count;
    uint32_t interval_ms;
    uint32_t timeout_ms;
    uint32_t payload_bytes;
    int asn_check;
    bool csv;
    bool quiet;
} Args;

typedef struct RttMessage {
    char kind;
    char origin_id[IPI_RTT_MAX_NODE_ID + 1];
    char responder_id[IPI_RTT_MAX_NODE_ID + 1];
    uint32_t sequence;
    uint64_t origin_send_ns;
    uint64_t responder_recv_ns;
    uint64_t responder_send_ns;
    uint32_t payload_bytes;
} RttMessage;

typedef struct ReplyState {
    bool waiting;
    bool ready;
    uint32_t expected_sequence;
    uint64_t origin_send_ns;
    uint64_t reply_recv_ns;
    uint64_t responder_recv_ns;
    uint64_t responder_send_ns;
    uint32_t reply_payload_bytes;
    char responder_id[IPI_RTT_MAX_NODE_ID + 1];
} ReplyState;

static Args g_args = {
    .role = ROLE_INITIATOR,
    .node_id = "node-a",
    .count = 100,
    .interval_ms = 100,
    .timeout_ms = 1000,
    .payload_bytes = 128,
    .asn_check = 0,
    .csv = false,
    .quiet = false,
};

static pthread_mutex_t g_reply_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_reply_cond = PTHREAD_COND_INITIALIZER;
static ReplyState g_reply = {0};
static volatile sig_atomic_t g_running = 1;

static uint64_t monotonic_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
}

static void add_ms_to_timespec(struct timespec* ts, uint32_t ms)
{
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (long)(ms % 1000) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000L;
    }
}

static void sleep_ms(uint32_t ms)
{
    struct timespec req;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (long)(ms % 1000) * 1000000L;
    while (g_running && nanosleep(&req, &req) != 0 && errno == EINTR) {
    }
}

static void handle_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

static void copy_node_id(char* dst, const char* src)
{
    size_t n = strlen(src);
    if (n > IPI_RTT_MAX_NODE_ID) {
        n = IPI_RTT_MAX_NODE_ID;
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static void usage(const char* program)
{
    printf("Usage: %s [--role initiator|responder|both] [--node-id <id>]\n", program);
    printf("       [--count <n>] [--interval-ms <ms>] [--timeout-ms <ms>]\n");
    printf("       [--payload-bytes <n>] [--asn-check <0|1>] [--csv] [--quiet]\n");
    printf("\n");
    printf("Run the same binary on both Mocar devices. Use --role responder on the\n");
    printf("echo device and --role initiator on the measuring device. RTT is computed\n");
    printf("from the initiator's local monotonic clock when the echoed payload returns.\n");
}

static Role parse_role(const char* value)
{
    if (strcasecmp(value, "initiator") == 0 || strcasecmp(value, "sender") == 0) {
        return ROLE_INITIATOR;
    }
    if (strcasecmp(value, "responder") == 0 || strcasecmp(value, "echo") == 0) {
        return ROLE_RESPONDER;
    }
    if (strcasecmp(value, "both") == 0) {
        return ROLE_BOTH;
    }
    fprintf(stderr, "invalid role: %s\n", value);
    exit(2);
}

static uint32_t parse_u32_arg(const char* name, const char* value)
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

static bool parse_u32_field(const char* value, uint32_t* out)
{
    char* end = NULL;
    errno = 0;
    unsigned long parsed = strtoul(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed > UINT32_MAX) {
        return false;
    }
    *out = (uint32_t)parsed;
    return true;
}

static bool parse_u64_field(const char* value, uint64_t* out)
{
    char* end = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0') {
        return false;
    }
    *out = (uint64_t)parsed;
    return true;
}

static void parse_args(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "--role") == 0 && i + 1 < argc) {
            g_args.role = parse_role(argv[++i]);
        } else if (strcmp(argv[i], "--node-id") == 0 && i + 1 < argc) {
            copy_node_id(g_args.node_id, argv[++i]);
        } else if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            g_args.count = parse_u32_arg("--count", argv[++i]);
        } else if (strcmp(argv[i], "--interval-ms") == 0 && i + 1 < argc) {
            g_args.interval_ms = parse_u32_arg("--interval-ms", argv[++i]);
        } else if (strcmp(argv[i], "--timeout-ms") == 0 && i + 1 < argc) {
            g_args.timeout_ms = parse_u32_arg("--timeout-ms", argv[++i]);
        } else if (strcmp(argv[i], "--payload-bytes") == 0 && i + 1 < argc) {
            g_args.payload_bytes = parse_u32_arg("--payload-bytes", argv[++i]);
            if (g_args.payload_bytes > IPI_RTT_MAX_PAYLOAD) {
                fprintf(stderr, "--payload-bytes must be <= %u\n", IPI_RTT_MAX_PAYLOAD);
                exit(2);
            }
        } else if (strcmp(argv[i], "--asn-check") == 0 && i + 1 < argc) {
            g_args.asn_check = (int)parse_u32_arg("--asn-check", argv[++i]);
            if (g_args.asn_check != 0 && g_args.asn_check != 1) {
                fprintf(stderr, "--asn-check must be 0 or 1\n");
                exit(2);
            }
        } else if (strcmp(argv[i], "--csv") == 0) {
            g_args.csv = true;
        } else if (strcmp(argv[i], "--quiet") == 0) {
            g_args.quiet = true;
        } else {
            fprintf(stderr, "unknown or incomplete argument: %s\n", argv[i]);
            usage(argv[0]);
            exit(2);
        }
    }
}

static bool parse_rtt_message(const char* buffer, int len, RttMessage* out)
{
    char header[IPI_RTT_MAX_HEADER];
    int header_len = 0;
    while (header_len < len && header_len < (IPI_RTT_MAX_HEADER - 1)) {
        header[header_len] = buffer[header_len];
        if (buffer[header_len] == '\n') {
            break;
        }
        ++header_len;
    }
    header[header_len] = '\0';

    char* fields[9] = {0};
    char* save = NULL;
    int count = 0;
    for (char* token = strtok_r(header, "|", &save);
         token != NULL && count < 9;
         token = strtok_r(NULL, "|", &save)) {
        fields[count++] = token;
    }

    if (count != 9 || strcmp(fields[0], IPI_RTT_MAGIC) != 0 || strlen(fields[1]) != 1) {
        return false;
    }

    memset(out, 0, sizeof(*out));
    out->kind = fields[1][0];
    copy_node_id(out->origin_id, fields[2]);
    copy_node_id(out->responder_id, fields[3]);
    if (!parse_u32_field(fields[4], &out->sequence) ||
        !parse_u64_field(fields[5], &out->origin_send_ns) ||
        !parse_u64_field(fields[6], &out->responder_recv_ns) ||
        !parse_u64_field(fields[7], &out->responder_send_ns) ||
        !parse_u32_field(fields[8], &out->payload_bytes)) {
        return false;
    }
    if (out->payload_bytes > IPI_RTT_MAX_PAYLOAD) {
        return false;
    }
    return out->kind == 'Q' || out->kind == 'R';
}

static int build_rtt_payload(char* buffer,
                             size_t capacity,
                             char kind,
                             const char* origin_id,
                             const char* responder_id,
                             uint32_t sequence,
                             uint64_t origin_send_ns,
                             uint64_t responder_recv_ns,
                             uint64_t responder_send_ns,
                             uint32_t requested_payload_bytes)
{
    int header_len = snprintf(buffer,
                              capacity,
                              "%s|%c|%s|%s|%" PRIu32 "|%" PRIu64 "|%" PRIu64 "|%" PRIu64 "|%" PRIu32 "\n",
                              IPI_RTT_MAGIC,
                              kind,
                              origin_id,
                              (responder_id && responder_id[0] != '\0') ? responder_id : "-",
                              sequence,
                              origin_send_ns,
                              responder_recv_ns,
                              responder_send_ns,
                              requested_payload_bytes);
    if (header_len < 0 || (size_t)header_len >= capacity) {
        return -1;
    }

    uint32_t total_len = requested_payload_bytes;
    if (total_len < (uint32_t)header_len) {
        total_len = (uint32_t)header_len;
    }
    if (total_len > capacity) {
        total_len = (uint32_t)capacity;
    }
    for (uint32_t i = (uint32_t)header_len; i < total_len; ++i) {
        buffer[i] = (char)('A' + (i % 26));
    }
    return (int)total_len;
}

static void maybe_echo_request(const RttMessage* msg)
{
    if (g_args.role == ROLE_INITIATOR || strcmp(msg->origin_id, g_args.node_id) == 0) {
        return;
    }

    char* payload = (char*)malloc(IPI_RTT_MAX_PAYLOAD);
    if (payload == NULL) {
        fprintf(stderr, "failed to allocate RTT reply payload\n");
        return;
    }
    uint64_t recv_ns = monotonic_time_ns();
    uint64_t send_ns = monotonic_time_ns();
    int len = build_rtt_payload(payload,
                                IPI_RTT_MAX_PAYLOAD,
                                'R',
                                msg->origin_id,
                                g_args.node_id,
                                msg->sequence,
                                msg->origin_send_ns,
                                recv_ns,
                                send_ns,
                                msg->payload_bytes);
    if (len < 0) {
        fprintf(stderr, "failed to build RTT reply\n");
        free(payload);
        return;
    }
    if (mde_v2x_custom_send(payload, len, g_args.asn_check) != 0) {
        fprintf(stderr, "RTT reply send failed for seq=%" PRIu32 "\n", msg->sequence);
        free(payload);
        return;
    }
    free(payload);
    if (!g_args.quiet && !g_args.csv) {
        fprintf(stderr,
                "echoed seq=%" PRIu32 " origin=%s payload_bytes=%d\n",
                msg->sequence,
                msg->origin_id,
                len);
    }
}

static void maybe_record_reply(const RttMessage* msg, int len)
{
    if (g_args.role == ROLE_RESPONDER || strcmp(msg->origin_id, g_args.node_id) != 0) {
        return;
    }

    uint64_t recv_ns = monotonic_time_ns();
    pthread_mutex_lock(&g_reply_lock);
    if (g_reply.waiting && msg->sequence == g_reply.expected_sequence) {
        g_reply.ready = true;
        g_reply.reply_recv_ns = recv_ns;
        g_reply.responder_recv_ns = msg->responder_recv_ns;
        g_reply.responder_send_ns = msg->responder_send_ns;
        g_reply.reply_payload_bytes = (uint32_t)len;
        copy_node_id(g_reply.responder_id, msg->responder_id);
        pthread_cond_signal(&g_reply_cond);
    }
    pthread_mutex_unlock(&g_reply_lock);
}

static void v2x_custom_rtt_recv_handle(char* buffer, int len)
{
    if (buffer == NULL || len <= 0) {
        return;
    }

    RttMessage msg;
    if (!parse_rtt_message(buffer, len, &msg)) {
        return;
    }

    if (msg.kind == 'Q') {
        maybe_echo_request(&msg);
    } else if (msg.kind == 'R') {
        maybe_record_reply(&msg, len);
    }
}

static void print_csv_header(void)
{
    printf("sequence,payload_bytes,packet_bytes,success,rtt_ms,origin_send_mono_ns,reply_recv_mono_ns,"
           "responder_id,responder_recv_mono_ns,responder_send_mono_ns,detail\n");
}

static void print_result(uint32_t sequence,
                         uint32_t payload_bytes,
                         uint32_t packet_bytes,
                         bool success,
                         double rtt_ms,
                         const ReplyState* reply,
                         const char* detail)
{
    if (g_args.quiet) {
        return;
    }
    if (g_args.csv) {
        printf("%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%s,%.6f,%" PRIu64 ",%" PRIu64 ",%s,%" PRIu64 ",%" PRIu64 ",%s\n",
               sequence,
               payload_bytes,
               packet_bytes,
               success ? "true" : "false",
               success ? rtt_ms : 0.0,
               reply ? reply->origin_send_ns : 0,
               reply ? reply->reply_recv_ns : 0,
               reply ? reply->responder_id : "",
               reply ? reply->responder_recv_ns : 0,
               reply ? reply->responder_send_ns : 0,
               detail);
    } else if (success) {
        printf("seq=%" PRIu32 " payload_bytes=%" PRIu32 " packet_bytes=%" PRIu32
               " rtt_ms=%.3f responder=%s\n",
               sequence,
               payload_bytes,
               packet_bytes,
               rtt_ms,
               reply ? reply->responder_id : "");
    } else {
        printf("seq=%" PRIu32 " payload_bytes=%" PRIu32 " packet_bytes=%" PRIu32
               " timeout detail=%s\n",
               sequence,
               payload_bytes,
               packet_bytes,
               detail);
    }
    fflush(stdout);
}

static int run_initiator(void)
{
    if (g_args.csv && !g_args.quiet) {
        print_csv_header();
    }

    char* payload = (char*)malloc(IPI_RTT_MAX_PAYLOAD);
    if (payload == NULL) {
        fprintf(stderr, "failed to allocate RTT request payload\n");
        return 1;
    }

    uint32_t sequence = 1;
    while (g_running && (g_args.count == 0 || sequence <= g_args.count)) {
        uint64_t send_ns = monotonic_time_ns();
        int len = build_rtt_payload(payload,
                                    IPI_RTT_MAX_PAYLOAD,
                                    'Q',
                                    g_args.node_id,
                                    "",
                                    sequence,
                                    send_ns,
                                    0,
                                    0,
                                    g_args.payload_bytes);
        if (len < 0) {
            print_result(sequence, g_args.payload_bytes, 0, false, 0.0, NULL, "payload-build-failed");
            free(payload);
            return 1;
        }

        pthread_mutex_lock(&g_reply_lock);
        memset(&g_reply, 0, sizeof(g_reply));
        g_reply.waiting = true;
        g_reply.expected_sequence = sequence;
        g_reply.origin_send_ns = send_ns;
        pthread_mutex_unlock(&g_reply_lock);

        if (mde_v2x_custom_send(payload, len, g_args.asn_check) != 0) {
            pthread_mutex_lock(&g_reply_lock);
            g_reply.waiting = false;
            pthread_mutex_unlock(&g_reply_lock);
            print_result(sequence, g_args.payload_bytes, (uint32_t)len, false, 0.0, NULL, "send-failed");
        } else {
            struct timespec deadline;
            clock_gettime(CLOCK_REALTIME, &deadline);
            add_ms_to_timespec(&deadline, g_args.timeout_ms);

            pthread_mutex_lock(&g_reply_lock);
            int wait_rc = 0;
            while (g_running && !g_reply.ready && wait_rc != ETIMEDOUT) {
                wait_rc = pthread_cond_timedwait(&g_reply_cond, &g_reply_lock, &deadline);
            }
            ReplyState result = g_reply;
            g_reply.waiting = false;
            pthread_mutex_unlock(&g_reply_lock);

            if (result.ready && result.reply_recv_ns >= result.origin_send_ns) {
                double rtt_ms = (double)(result.reply_recv_ns - result.origin_send_ns) / 1000000.0;
                print_result(sequence, g_args.payload_bytes, (uint32_t)len, true, rtt_ms, &result, "ok");
            } else {
                print_result(sequence, g_args.payload_bytes, (uint32_t)len, false, 0.0, &result, "timeout");
            }
        }

        ++sequence;
        if (g_args.interval_ms > 0 && g_running) {
            sleep_ms(g_args.interval_ms);
        }
    }

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
    if (mde_v2x_custom_recv_handle_register(v2x_custom_rtt_recv_handle) != 0) {
        fprintf(stderr, "mde_v2x_custom_recv_handle_register failed\n");
        return 1;
    }

    if (!g_args.quiet) {
        fprintf(stderr,
                "ipi_custom_rtt role=%d node_id=%s payload_bytes=%" PRIu32 " count=%" PRIu32
                " asn_check=%d\n",
                (int)g_args.role,
                g_args.node_id,
                g_args.payload_bytes,
                g_args.count,
                g_args.asn_check);
    }

    if (g_args.role == ROLE_RESPONDER) {
        while (g_running) {
            sleep(1);
        }
        return 0;
    }

    return run_initiator();
}
