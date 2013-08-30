#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <tox/tox.h>
#include <gtk/gtk.h>
#include "misc.h"
#include "storage.h"
#include "configdir.h"

/* Connects to a random DHT server listed in the DHTservers file */
int init_connection(struct gtox_data *gtox)
{
    Tox *m = gtox->tox;
    FILE *fp = NULL;

    if (tox_isconnected(m))
        return 0;

    fp = fopen(gtox->srvlist_path, "r");

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
    
    free(user_config_dir);
    
    return full_path;
}


/*
 * Store Messenger to given location
 * Return 0 stored successfully
 * Return 1 malloc failed
 * Return 2 opening path failed
 * Return 3 fwrite failed
 */
int store_data(struct gtox_data *gtox)
{
    FILE *fd;
    size_t len;
    uint8_t *buf;

    len = tox_size(gtox->tox);
    buf = malloc(len);

    if (buf == NULL) {
        return 1;
    }

    tox_save(gtox->tox, buf);

    fd = fopen(gtox->datafile_path, "w");

    if (fd == NULL) {
        free(buf);
        return 2;
    }

    if (fwrite(buf, len, 1, fd) != 1) {
        free(buf);
        fclose(fd);
        return 3;
    }

    free(buf);
    fclose(fd);
    return 0;
}

int load_data(struct gtox_data *gtox)
{
    FILE *fd;
    size_t len;
    uint8_t *buf;

    if ((fd = fopen(gtox->datafile_path, "r")) != NULL) {
        fseek(fd, 0, SEEK_END);
        len = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        buf = malloc(len);

        if (buf == NULL) {
            fprintf(stderr, "malloc() failed.\n");
            fclose(fd);
            return(1);
        }

        if (fread(buf, len, 1, fd) != 1) {
            fprintf(stderr, "fread() failed.\n");
            free(buf);
            fclose(fd);
            return(4);
        }

        tox_load(gtox->tox, buf, len);

        uint32_t i = 0;

        char name[TOX_MAX_NAME_LENGTH];
        while (tox_getname(gtox->tox, i, (uint8_t *)name) != -1) {
            /*on_friendadded(gtox->tox, i); TODO: implement*/
            i++;
        }

        free(buf);
        fclose(fd);
    } else {
        int st;

        if ((st = store_data(gtox)) != 0) {
            fprintf(stderr, "Store messenger failed with return code: %d\n", st);
            exit(1);
        }
    }
    return 0;
}
