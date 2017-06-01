#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/**
 * dostava informaci o adrese ktera je uvedena v argumentech
 */
struct addrinfo *get_adr_info();

/**
 * hlavni cyklus pro traceroute IPV4
 */
int trace_v4(int ttl, struct addrinfo *adr_info);

/**
 * hlavni cyklus pro traceroute IPV6
 */
int trace_v6(int ttl, struct addrinfo *adr_info);

#endif /* TRACEROUTE_H */