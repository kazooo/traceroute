#ifndef ARGS_H
#define ARGS_H

/**
 * struktura predstavuje argumenty
 * prikazove radky pro traceroute
 */
struct args {
    int first_ttl;
    int max_ttl;
    int protocol;
    const char *adres;
};

/**
 * globalni promena ktera uchovava argumenty
 */
extern struct args global_args;

/**
 * nacte argmenty prikazove radky
 * do struktury global_args
 * vola se na zacatku programu
 */
void set_args(int argc, char **argv);

#endif /* ARGS_H */