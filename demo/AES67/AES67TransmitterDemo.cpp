#include <arpa/inet.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// --- AES67 Konfiguration ---
#define SAMPLE_RATE         48000
#define NUM_STREAMS         4       // 4 Streams x 8 Kanäle = 32 Kanäle (MTU-sicher!)
#define CHANNELS_PER_STREAM 8
#define SAMPLES_PER_PKT     48      // 1 ms Paketzeit (AES67 Pflichtprofil)
#define RTP_PAYLOAD_BYTES   (CHANNELS_PER_STREAM * SAMPLES_PER_PKT * 3) // L24
#define BASE_MCAST_IP       "239.69.1.%d" // .10 bis .13
#define RTP_PORT            5004

// --- PTPv2 Konfiguration (IEEE 1588-2008) ---
#define PTP_MCAST_IP        "224.0.1.129"
#define PTP_EVENT_PORT      319
#define PTP_GENERAL_PORT    320

// Globale PTP-Synchronisationsdaten
typedef struct {
    _Atomic int64_t offset_ns;      // Differenz: PTP_Zeit - lokale_Monotonic_Zeit (ns)
    _Atomic bool locked;
    uint8_t gm_clock_id[8];         // Grandmaster Identity für SDP
    pthread_mutex_t id_lock;
} ptp_state_t;

static ptp_state_t g_ptp = {
    .offset_ns = 0,
    .locked = false,
    .id_lock = PTHREAD_MUTEX_INITIALIZER
};

// Lokale monotone Zeit in Nanosekunden
static inline uint64_t get_local_ns(void) {
    static mach_timebase_info_data_t tb = {0};
    if (tb.denom == 0) mach_timebase_info(&tb);
    return (mach_absolute_time() * tb.numer) / tb.denom;
}

// Konvertiert 24-Bit Audio-Sample (-8388608 .. 8388607) in Big Endian L24
static inline void write_l24_be(uint8_t *dst, int32_t sample) {
    dst[0] = (uint8_t)((sample >> 16) & 0xFF);
    dst[1] = (uint8_t)((sample >> 8) & 0xFF);
    dst[2] = (uint8_t)(sample & 0xFF);
}

// --- 1. PTPv2 Passive Slave Thread (Sync + Follow_Up) ---
static uint64_t parse_ptp_timestamp(const uint8_t *buf) {
    uint64_t sec = 0;
    for (int i = 0; i < 6; i++) sec = (sec << 8) | buf[i];
    uint32_t nsec = ((uint32_t)buf[6] << 24) | ((uint32_t)buf[7] << 16) |
                    ((uint32_t)buf[8] << 8)  | (uint32_t)buf[9];
    return sec * 1000000000ULL + nsec;
}

static int create_mcast_recv_sock(const char *mcast_ip, uint16_t port, const char *if_ip) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    struct ip_mreq mreq = {
        .imr_multiaddr.s_addr = inet_addr(mcast_ip),
        .imr_interface.s_addr = inet_addr(if_ip)
    };
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    return fd;
}

void *ptp_listener_thread(void *arg) {
    const char *local_ip = (const char *)arg;
    int ev_fd  = create_mcast_recv_sock(PTP_MCAST_IP, PTP_EVENT_PORT, local_ip);
    int gen_fd = create_mcast_recv_sock(PTP_MCAST_IP, PTP_GENERAL_PORT, local_ip);

    uint16_t last_sync_seq = 0;
    uint64_t last_sync_rx_ns = 0;
    uint8_t buf[256];

    fd_set rfds;
    int max_fd = (ev_fd > gen_fd ? ev_fd : gen_fd) + 1;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(ev_fd, &rfds);
        FD_SET(gen_fd, &rfds);

        if (select(max_fd, &rfds, NULL, NULL, NULL) <= 0) continue;
        uint64_t rx_ns = get_local_ns();

        if (FD_ISSET(ev_fd, &rfds)) {
            ssize_t len = recv(ev_fd, buf, sizeof(buf), 0);
            if (len >= 44) {
                uint8_t msg_type = buf[0] & 0x0F;
                uint16_t flags = ((uint16_t)buf[6]  warte auf Follow_Up
                        last_sync_seq = seq_id;
                        last_sync_rx_ns = rx_ns;
                    }
                }
            }
        }

        if (FD_ISSET(gen_fd, &rfds)) {
            ssize_t len = recv(gen_fd, buf, sizeof(buf), 0);
            if (len >= 44) {
                uint8_t msg_type = buf[0] & 0x0F;
                uint16_t seq_id = ((uint16_t)buf[30] << 8) | buf[31];
                if (msg_type == 0x8 && seq_id == last_sync_seq) { // PTP Follow_Up
                    uint64_t ptp_ns = parse_ptp_timestamp(&buf[34]);
                    // Einfacher Tiefpass-Filter gegen Netzwerk-Jitter
                    int64_t new_off = (int64_t)ptp_ns - (int64_t)last_sync_rx_ns;
                    int64_t cur_off = atomic_load(&g_ptp.offset_ns);
                    if (!atomic_load(&g_ptp.locked) || llabs(new_off - cur_off) > 500000) {
                        atomic_store(&g_ptp.offset_ns, new_off);
                    } else {
                        atomic_store(&g_ptp.offset_ns, (cur_off * 15 + new_off) / 16);
                    }
                    atomic_store(&g_ptp.locked, true);
                }
            }
        }
    }
    return NULL;
}

// --- 2. SAP / SDP Announcement (für alle 4 Streams) ---
static void send_sap_for_stream(int sock, const char *local_ip, int stream_idx) {
    char mcast_ip[32];
    snprintf(mcast_ip, sizeof(mcast_ip), BASE_MCAST_IP, 10 + stream_idx);

    uint8_t id[8];
    pthread_mutex_lock(&g_ptp.id_lock);
    memcpy(id, g_ptp.gm_clock_id, 8);
    pthread_mutex_unlock(&g_ptp.id_lock);

    char gm_str[32];
    snprintf(gm_str, sizeof(gm_str), "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
             id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

    char sdp[512];
    int ch_start = stream_idx * CHANNELS_PER_STREAM + 1;
    int ch_end   = ch_start + CHANNELS_PER_STREAM - 1;

    snprintf(sdp, sizeof(sdp),
        "v=0\r\n"
        "o=- %u 1 IN IP4 %s\r\n"
        "s=macOS_AES67_Ch%02d_%02d\r\n"
        "c=IN IP4 %s/32\r\n"
        "t=0 0\r\n"
        "m=audio %d RTP/AVP 96\r\n"
        "a=rtpmap:96 L24/%d/%d\r\n"
        "a=ptime:1\r\n"
        "a=ts-refclk:ptp=IEEE1588-2008:%s:0\r\n"
        "a=mediaclk:direct=0\r\n",
        1732740000U + stream_idx, local_ip, ch_start, ch_end,
        mcast_ip, RTP_PORT, SAMPLE_RATE, CHANNELS_PER_STREAM, gm_str);

    uint8_t pkt[1024] = {0};
    pkt[0] = 0x20; // SAP v1, IPv4, Announcement
    uint16_t hash = htons((uint16_t)(0x4200 + stream_idx));
    memcpy(&pkt[2], &hash, 2);
    inet_pton(AF_INET, local_ip, &pkt[4]);

    const char *mime = "application/sdp";
    size_t hdr_len = 8 + strlen(mime) + 1;
    strcpy((char *)&pkt[8], mime);
    size_t sdp_len = strlen(sdp);
    memcpy(&pkt[hdr_len], sdp, sdp_len);

    struct sockaddr_in sap_dst = {
        .sin_family = AF_INET,
        .sin_port = htons(9875),
        .sin_addr.s_addr = inet_addr("239.255.255.255")
    };
    sendto(sock, pkt, hdr_len + sdp_len, 0, (struct sockaddr *)&sap_dst, sizeof(sap_dst));
}

// --- 3. macOS Echtzeit-Thread Priorität setzen ---
static void set_macos_realtime_priority(void) {
    mach_timebase_info_data_t tb;
    mach_timebase_info(&tb);
    uint32_t ms_to_ticks = (uint32_t)((1000000ULL * tb.denom) / tb.numer);

    thread_time_constraint_policy_data_t policy = {
        .period      = 1 * ms_to_ticks,        // 1 ms Periode
        .computation = ms_to_ticks / 4,        // max 250 µs Rechenzeit
        .constraint  = ms_to_ticks / 2,        // max 500 µs Latenz
        .preemptible = false
    };
    thread_policy_set(mach_thread_self(), THREAD_TIME_CONSTRAINT_POLICY,
                      (thread_policy_t)&policy, THREAD_TIME_CONSTRAINT_POLICY_COUNT);
}

// --- RTP Paketstruktur ---
typedef struct __attribute__((packed)) {
    uint8_t v_p_x_cc;
    uint8_t m_pt;
    uint16_t seq;
    uint32_t timestamp;
    uint32_t ssrc;
    uint8_t payload[RTP_PAYLOAD_BYTES];
} rtp_pkt_t;


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Nutzung: %s <LOKALE_ETHERNET_IP>\n", argv[0]);
        return 1;
    }
    const char *local_ip = argv[1];

    // PTP-Listener starten
    pthread_t ptp_tid;
    pthread_create(&ptp_tid, NULL, ptp_listener_thread, (void *)local_ip);

    printf("Warte auf PTPv2 Grandmaster im Netzwerk...\n");
    while (!atomic_load(&g_ptp.locked)) usleep(100000);
    printf("PTPv2 synchronisiert! Starte 32-Kanal AES67-Sender (4x8 Kanäle)...\n");

    // Multicast Sendesocket konfigurieren
    int tx_sock = socket(AF_INET, SOCK_DGRAM, 0);
    int tos = 0xB8; // DSCP EF (Expedited Forwarding) für Audio
    setsockopt(tx_sock, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    struct in_addr if_addr = { .s_addr = inet_addr(local_ip) };
    setsockopt(tx_sock, IPPROTO_IP, IP_MULTICAST_IF, &if_addr, sizeof(if_addr));

    struct sockaddr_in dst_addrs[NUM_STREAMS];
    rtp_pkt_t pkts[NUM_STREAMS];
    uint16_t seq[NUM_STREAMS] = {0};

    for (int s = 0; s < NUM_STREAMS; s++) {
        char ip[32];
        snprintf(ip, sizeof(ip), BASE_MCAST_IP, 10 + s);
        dst_addrs[s] = (struct sockaddr_in){
            .sin_family = AF_INET,
            .sin_port = htons(RTP_PORT),
            .sin_addr.s_addr = inet_addr(ip)
        };
        pkts[s].v_p_x_cc = 0x80; // RTP v2
        pkts[s].m_pt     = 96;   // Dynamic Payload Type 96
        pkts[s].ssrc     = htonl(0xAE670001 + s);
    }

    set_macos_realtime_priority();

    mach_timebase_info_data_t tb;
    mach_timebase_info(&tb);

    uint64_t interval_ticks = (1000000ULL * tb.denom) / tb.numer; // 1 ms
    uint64_t next_tick = mach_absolute_time();
    for (uint64_t tick = 0; ; tick++) {
        // Alle 1000 ms (1 s) SAP/SDP Announcements für alle 4 Streams senden
        if (tick % 1000 == 0) {
            for (int s = 0; s < NUM_STREAMS; s++) {
                send_sap_for_stream(tx_sock, local_ip, s);
            }
        }
        // Exakten AES67 RTP-Timestamp aus der PTP-Zeit berechnen:
        // RTP_TS = (PTP_Nanosekunden * 48000 / 10^9) mod 2^32
        uint64_t ptp_now_ns = (uint64_t)((int64_t)get_local_ns() + atomic_load(&g_ptp.offset_ns));
        uint32_t rtp_ts = (uint32_t)(((__uint128_t)ptp_now_ns * SAMPLE_RATE) / 1000000000ULL);
        for (int s = 0; s < NUM_STREAMS; s++) {
            pkts[s].seq = htons(seq[s]++);
            pkts[s].timestamp = htonl(rtp_ts);
            // HIER DEINE 32-KANAL AUDIODATEN EINFÜGEN (Interleaved L24):
            // Kanal s*8 + ch, Sample i (0..47)
            for (int i = 0; i < SAMPLES_PER_PKT; i++) {
                for (int ch = 0; ch < CHANNELS_PER_STREAM; ch++) {
                    int32_t sample24 = 0; // z. B. aus deinem Lock-Free-Ringbuffer lesen
                    write_l24_be(&pkts[s].payload[(i * CHANNELS_PER_STREAM + ch) * 3], sample24);
                }
            }
            sendto(tx_sock, &pkts[s], sizeof(rtp_pkt_t), 0,
            (struct sockaddr *)&dst_addrs[s], sizeof(struct sockaddr_in));
        }
        next_tick += interval_ticks;
        mach_wait_until(next_tick);
    }
    return 0;
}