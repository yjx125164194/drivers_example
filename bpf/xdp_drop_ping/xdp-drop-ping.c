#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h> // for htons
#include <bpf/bpf_helpers.h>

SEC("maps")
struct bpf_map_def ping_cnt = {
    .type = BPF_MAP_TYPE_PERCPU_ARRAY,
    .key_size = sizeof(__u32),
    .value_size = sizeof(__u64),
    .max_entries = 1,
};

SEC("xdp")
int xdp_drop_ping(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    struct ethhdr *eth = (struct ethhdr *)(long)ctx->data;
    struct iphdr *ip;
    struct icmphdr *icmp;

    if (eth + 1 > (struct ethhdr *)data_end)
        return XDP_PASS;

    if (eth->h_proto != htons(ETH_P_IP))
        return XDP_PASS;

    ip = (struct iphdr *)(eth + 1);
    if (ip + 1 > (struct iphdr *)data_end)
        return XDP_PASS;

    if (ip->protocol != IPPROTO_ICMP)
        return XDP_PASS;

    icmp = (struct icmphdr *)(ip + 1);
    if (icmp + 1 > (struct icmphdr *)data_end)
        return XDP_PASS;

    if (icmp->type != ICMP_ECHO && icmp->type != ICMP_ECHOREPLY)
        return XDP_PASS;

    // Update stats
    __u32 key = 0;
    __u64 *value = bpf_map_lookup_elem(&ping_cnt, &key);
    if (value) {
        *value += 1;
    }

    return XDP_DROP;
}

char _license[] SEC("license") = "GPL";

