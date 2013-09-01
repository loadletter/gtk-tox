#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <tox/tox.h>
#include <netdb.h>
#include <stdlib.h>

/*
  resolve_addr():
    address should represent IPv4 or a hostname with A record

    returns a data in network byte order that can be used to set IP.i or IP_Port.ip.i
    returns 0 on failure

    TODO: Fix ipv6 support
*/
uint32_t resolve_addr(const char *address)
{
    struct addrinfo *server = NULL;
    struct addrinfo  hints;
    int              rc;
    uint32_t         addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;    // IPv4 only right now.
    hints.ai_socktype = SOCK_DGRAM; // type of socket Tox uses.

    rc = getaddrinfo(address, "echo", &hints, &server);

    // Lookup failed.
    if (rc != 0) {
        return 0;
    }

    // IPv4 records only..
    if (server->ai_family != AF_INET) {
        freeaddrinfo(server);
        return 0;
    }


    addr = ((struct sockaddr_in *)server->ai_addr)->sin_addr.s_addr;

    freeaddrinfo(server);
    return addr;
}

// XXX: FIX
unsigned char *hex_string_to_bin(char hex_string[])
{
    size_t len = strlen(hex_string);
    unsigned char *val = malloc(len);
    char *pos = hex_string;
    int i;

    for (i = 0; i < len; ++i, pos += 2)
        sscanf(pos, "%2hhx", &val[i]);

    return val;
}

char *human_readable_id(uint8_t address[TOX_FRIEND_ADDRESS_SIZE])
{
    char id[TOX_FRIEND_ADDRESS_SIZE * 2 + 1] = {0};
    int i;

    for (i = 0; i < TOX_FRIEND_ADDRESS_SIZE; i++)
        sprintf(id,"%s%02hhx", id, address[i]);
    return strdup(id);
}

char *own_id(Tox *m)
{
    uint8_t address[TOX_FRIEND_ADDRESS_SIZE];
    tox_getaddress(m, address);
    return human_readable_id(address);
}
