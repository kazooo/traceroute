#include <iostream>
#include "args.h"
#include "traceroute.h"

int main(int argc, char **argv) {

    set_args(argc, argv);
    struct addrinfo *info_adr = get_adr_info();
    if (info_adr == NULL) {
        fprintf(stderr, "chyba: addrinfo\n");
        exit(1);
    }
    /* trace it */
    if (global_args.protocol == AF_INET) {
        int ttl = global_args.first_ttl;
        for ( ; ttl <= global_args.max_ttl; ++ttl) {
            if (!trace_v4(ttl, info_adr))
                break;
        }
    }
    else {
        int ttl = global_args.first_ttl;
        for ( ; ttl <= global_args.max_ttl; ++ttl) {
            if (!trace_v6(ttl, info_adr))
                break;
        }
    }

    freeaddrinfo(info_adr);
    return 0;
}