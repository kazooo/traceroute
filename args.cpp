#include "args.h"
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>

struct args global_args;

void set_args(int argc, char **argv) {

    global_args.first_ttl = 1;
    global_args.max_ttl = 30;

    int adr_count = 0;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-f") {
            if (i + 1 == argc) {
                fprintf(stderr, "chyba: argument -f byl zadan bez hodnoty\n");
                exit(1);
            }

            try {
                global_args.first_ttl = std::stoi(argv[i + 1]);
            }
            catch (...) {
                fprintf(stderr, "chyba: spatny argument -f\n");
                exit(1);
            }

            i++;
        }
        else if (std::string(argv[i]) == "-m") {
            if (i + 1 == argc) {
                fprintf(stderr, "chyba: argument -m byl zadan bez hodnoty\n");
                exit(1);
            }

            try {
                global_args.max_ttl = std::stoi(argv[i + 1]);
            }
            catch (...) {
                fprintf(stderr, "chyba: spatny argument -f\n");
                exit(1);
            }

            i++;
        }
        else {
            if (adr_count < 1) {
                global_args.adres = argv[i];
                adr_count++;
            }
            else {
                fprintf(stderr, "chyba: musi byt uvedena pouze 1 adresa\n");
                exit(1);
            }
        }
    }

    if (global_args.adres == NULL) {
        fprintf(stderr, "chyba: musi byt uvedena adresa serveru\n");
        exit(1);
    }

    struct sockaddr_in6 sa6;
    if (inet_pton(AF_INET6, global_args.adres, &sa6.sin6_addr)) {
        global_args.protocol = AF_INET6;
        return; /* adresa neni ipv6 */
    }

    if (inet_pton(AF_INET, global_args.adres, &sa6.sin6_addr)) {
        global_args.protocol = AF_INET;
        return; /* adresa neni ipv4 */
    }

    /* volame chybu protoze adresa nema spravny format */
    fprintf(stderr, "chyba: spatna adresa\n");
    exit(1);
}