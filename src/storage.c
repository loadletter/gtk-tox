#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <tox/tox.h>
#include "misc.h"
#include "storage.h"
#include "configdir.h"

/* Connects to a random DHT server listed in the DHTservers file */
int init_connection(struct storage_data *stor_d)
{
    Tox *m = stor_d->tox;
    FILE *fp = NULL;

    if (tox_isconnected(m))
        return 0;

    fp = fopen(stor_d->srvlist_path, "r");

    if (!fp)
        return 1;

    char servers[MAXSERVERS][MAXLINE];
    char line[MAXLINE];
    int linecnt = 0;

    while (fgets(line, sizeof(line), fp) && linecnt < MAXSERVERS) {
        if (strlen(line) > MINLINE)
            strcpy(servers[linecnt++], line);
    }

    if (linecnt < 1) {
        fclose(fp);
        return 2;
    }

    fclose(fp);

    char *server = servers[rand() % linecnt];
    char *ip = strtok(server, " ");
    char *port = strtok(NULL, " ");
    char *key = strtok(NULL, " ");

    if (!ip || !port || !key)
        return 3;

    tox_IP_Port dht;
    dht.port = htons(atoi(port));
    uint32_t resolved_address = resolve_addr(ip);

    if (resolved_address == 0)
        return 0;

    dht.ip.i = resolved_address;
    uint8_t *binary_string = hex_string_to_bin(key);
    tox_bootstrap(m, dht, binary_string);
    free(binary_string);
    return 0;
}

/* mallocs the string which contains path+filename */
char *get_full_configpath(const char *filename)
{
    char *user_config_dir = get_user_config_dir();
    char *full_path;
    
    full_path = malloc(strlen(user_config_dir) + strlen(CONFIGDIR) + strlen(filename) + 1);
    strcpy(full_path, user_config_dir);
    strcat(full_path, CONFIGDIR);
    strcat(full_path, filename);
    
    return full_path;
}
