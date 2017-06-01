#include "traceroute.h"
#include "args.h"
#include <iostream>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>
#include <linux/errqueue.h>


#define SIZE_BUF 2017
#define SIZE_ADR 42

/**
* Vypise chybove hlaseni a ukonci program s kodem 1
*/
void throw_error(const char* message) {
    printf("%s\n", message);
    exit(1);
}

/**
 * vrati BSD schranku s nastavenem TTL
 */
static int create_ttl_socket(int ttl) {
    int ttl_socket; /* vytvarime UDP schranku */
    ttl_socket = socket(global_args.protocol, SOCK_DGRAM, IPPROTO_UDP);

    if (ttl_socket < 0)
        throw_error("chyba: socket\n");

    int result;
    int jedn = 1;
    if (global_args.protocol == AF_INET) {
        /* nastavime ttl pro ipv4 */
        result = setsockopt(ttl_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
        if (result) 
            throw_error("chyba: setsockopt\n");

        result = setsockopt(ttl_socket, IPPROTO_IP, IP_RECVERR, &jedn, sizeof(jedn));
        if (result)
            throw_error("chyba: setsockopt\n");
    }
    else {
        /* nastavime ttl pro ipv6 */
        result = setsockopt(ttl_socket, SOL_IPV6, IPV6_UNICAST_HOPS, &ttl, sizeof (ttl));
        if (result)
            throw_error("chyba: setsockopt\n");

        result = setsockopt(ttl_socket, SOL_IPV6, IPV6_RECVERR, &jedn, sizeof (jedn));
        if (result)
            throw_error("chyba: setsockopt\n");
    }
    return ttl_socket;
}

struct addrinfo *get_adr_info()
{
    struct addrinfo info_adr;
    struct addrinfo *info_adr_uk;

    memset(&info_adr, 0, sizeof info_adr);
    info_adr.ai_family = global_args.protocol;

    int result = getaddrinfo(global_args.adres, "33437", &info_adr, &info_adr_uk);

    if (result) {
        fprintf(stderr, "chyba addrinfo: %s\n", gai_strerror(result));
        exit(1);
    }

    return info_adr_uk;
}

int trace_v4(int ttl, struct addrinfo *info_adr) {

    int socket = create_ttl_socket(ttl);
    struct timeval cas;
    /* posilame zpravu na server */
    if (sendto(socket, "", 0, 0, info_adr->ai_addr, info_adr->ai_addrlen) < 0)
        throw_error("chyba: sendto\n");

    /**
     * pro spocitani doby icmp zpravy pouzijeme
     * strukturu timeval
     */
    gettimeofday(&cas, NULL);
    /* uchovame cas kdy byla zprava odeslana */
    double cas_odesilani_ms = 1000.0*cas.tv_sec;
    cas_odesilani_ms += cas.tv_usec/1000.0;

    struct pollfd multplx;
    multplx.fd = socket;
    multplx.events = 0;
    int result;
    /**
     * timeout pro zpravu 2 vteriny podle zadani
     */
    if ((result = poll(&multplx, 1, 2000)) < 0)
        throw_error("chyba: poll\n");

    if (result == 0) {
        printf("%d    *\n", ttl);
        return 1;
    }

    struct icmphdr icmph;
    struct iovec iov;
    iov.iov_base = &icmph;
    iov.iov_len = sizeof(icmph);

    char buf[SIZE_BUF];
    struct msghdr zprava;
    memset(&zprava, 0, sizeof(zprava));
    zprava.msg_iov = &iov;

    /**
     * initializace struktury pro zpravu
     */
    struct sockaddr_in adresa;
    zprava.msg_iovlen = 1;
    zprava.msg_flags = 0;
    zprava.msg_name = &adresa;
    zprava.msg_namelen = sizeof(struct sockaddr_in);
    zprava.msg_control = buf;
    zprava.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &zprava, MSG_ERRQUEUE) < 0)
        throw_error("chyba: recvmsg\n");

    gettimeofday(&cas, NULL);
    /* uchovame cas kdy byla zprava prijata */
    double cas_prijimani_ms = 1000.0*cas.tv_sec;
    cas_prijimani_ms += cas.tv_usec/1000.0;

    for (struct cmsghdr *uk_zprava = CMSG_FIRSTHDR(&zprava);
        uk_zprava;
        uk_zprava = CMSG_NXTHDR(&zprava, uk_zprava)) {

        struct sock_extended_err *zp_info;
        zp_info = (struct sock_extended_err *)CMSG_DATA(uk_zprava);
        adresa = *(struct sockaddr_in *)SO_EE_OFFENDER(zp_info);

        char adr_retez[SIZE_ADR];
        if (!inet_ntop(global_args.protocol, &adresa.sin_addr, (char *)&adr_retez, SIZE_ADR))
            throw_error("chyba: inet_ntop\n");

        double rozdil_cas_ms = cas_prijimani_ms - cas_odesilani_ms;
        printf("%d    %s    %.3f ms ", ttl, adr_retez, rozdil_cas_ms);

        if (uk_zprava->cmsg_level == SOL_IP && uk_zprava->cmsg_type == IP_RECVERR) {
            int icmp_chyba = zp_info->ee_type;
            /**
             * icmp chyby ipv4
             */
            switch (icmp_chyba) {
            case ICMP_HOST_UNREACH:
                printf(" H!");
                break;
            case ICMP_NET_UNREACH:
                printf(" N!");
                break;
            case ICMP_PROT_UNREACH:
                printf(" P!");
                break;
            case ICMP_PKT_FILTERED:
                printf(" X!");
                break;
            case ICMP_DEST_UNREACH:
                putchar('\n');
                return 0;
                break;
            default:
                break;
            }
        }
    } // for

    putchar('\n');
    return 1;
}

int trace_v6(int ttl, struct addrinfo *info_adr)
{
    /**
     * trace_v6 je stejna jako ipv4 ale s tim rozdilem ze
     * pouzivame struktury pro ipv6 verze jako struct sockaddr_in6,
     * icmp6_hdr atd.
     */
    int socket = create_ttl_socket(ttl);
    struct timeval cas;
    if (sendto(socket, "", 0, 0, info_adr->ai_addr, info_adr->ai_addrlen) < 0)
        throw_error("chyba: sendto\n");

    gettimeofday(&cas, NULL);
    double cas_odesilani_ms = 1000.0*cas.tv_sec;
    cas_odesilani_ms += cas.tv_usec/1000.0;

    struct pollfd multplx;
    multplx.fd = socket;
    multplx.events = 0;
    int result;
    if ((result = poll(&multplx, 1, 2000)) < 0)
        throw_error("chyba: poll\n");
  
    if (result == 0) {
        printf("%d    *\n", ttl);
        return 1;
    }

    struct icmp6_hdr icmp_hlavicka_6;
    struct iovec iov;
    iov.iov_base = &icmp_hlavicka_6;
    iov.iov_len = sizeof(icmp_hlavicka_6);

    char buf[SIZE_BUF];
    struct msghdr zprava;
    memset(&zprava, 0, sizeof(zprava));
    zprava.msg_iov = &iov;

    struct sockaddr_in6 adresa_6;
    zprava.msg_iovlen = 1;
    zprava.msg_flags = 0;
    zprava.msg_name = &adresa_6;
    zprava.msg_namelen = sizeof(struct sockaddr_in6);
    zprava.msg_control = buf;
    zprava.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &zprava, MSG_ERRQUEUE) < 0)
        throw_error("chyba: recvmsg\n");

    gettimeofday(&cas, NULL);
    double cas_prijimani_ms = 1000.0*cas.tv_sec;
    cas_prijimani_ms += cas.tv_usec/1000.0;

    for (struct cmsghdr *uk_zprava = CMSG_FIRSTHDR(&zprava);
        uk_zprava;
        uk_zprava = CMSG_NXTHDR(&zprava, uk_zprava)) {

        struct sock_extended_err *zp_info;
        zp_info = (struct sock_extended_err *)CMSG_DATA(uk_zprava);
        adresa_6 = *(struct sockaddr_in6 *)SO_EE_OFFENDER(zp_info);

        char adr_retez_6[SIZE_ADR];
        if (!inet_ntop(global_args.protocol, &adresa_6.sin6_addr, (char *)&adr_retez_6, SIZE_ADR))
            throw_error("chyba: inet_ntop\n");

        double rozdil_cas_ms = cas_prijimani_ms - cas_odesilani_ms;
        printf("%d    %s    %.3f ms", ttl, adr_retez_6, rozdil_cas_ms);

        if (uk_zprava->cmsg_level == SOL_IPV6 && uk_zprava->cmsg_type == IPV6_RECVERR) {
            int icmp_chyba = zp_info->ee_type;
            /**
             * icmp chyby ipv6
             */
            switch (icmp_chyba) {
            case ICMP6_DST_UNREACH_BEYONDSCOPE:
                printf(" H!");
                break;
            case ICMP6_DST_UNREACH_NOROUTE:
                printf(" N!");
                break;
            case ICMP6_DST_UNREACH_ADDR:
                printf(" P!");
                break;
            case ICMP6_DST_UNREACH_ADMIN:
                printf(" X!");
                break;
            default:
                break;
            }

            if (icmp_chyba == ICMP6_DST_UNREACH) {
                putchar('\n');
                return 0;
            }
        }
    }

    putchar('\n');
    return 1;
}