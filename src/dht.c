#include <tox/tox.h>
#include <gtk/gtk.h>
#include "dht.h"  
#include "gtkwindow.h"
#include "include/DHT.h"


typedef uint8_t ipbuf[3 * 4 + 3 + 1];

Client_data *DHT_get_close_list(DHT *dht)
{
    return dht->close_clientlist;
}

static void printip(ipbuf buf, IP ip)
{
    sprintf((char *)buf, "%u.%u.%u.%u", ip.c[0], ip.c[1], ip.c[2], ip.c[3]);
}

void dht_draw(struct dht_tree_data *dht_d)
{
    Messenger *m = dht_d->m;
    GtkWidget *dht_treeview = dht_d->gtk;
    
    Client_data *close_clientlist = DHT_get_close_list(m->dht);
    
    uint64_t now = unix_time();
    uint32_t i, j;
    ipbuf ipbuf;
    printf("\n%llu  ______________________ CLOSE LIST ________________________  ___ IP ADDR ___ _PRT_   LST   PNG    ____ SELF ____ _PRT_  LST\n\n", now);

    for (i = 0; i < LCLIENT_LIST; i++) { /*Number of nodes in closelist*/
    
        Client_data *client = close_clientlist + i;

        uint16_t port = ntohs(client->ip_port.port);

        if (port) {
            for (j = 0; j < CLIENT_ID_SIZE; j++)
                printf("%02hhx", client->client_id[j]);

            printip(ipbuf, client->ip_port.ip);
            printf("  %15s %5u ", ipbuf, port);
            printf("  %3llu ", now - client->timestamp);
            printf("  %3llu ", now - client->last_pinged);

            port = ntohs(client->ret_ip_port.port);

            if (port) {
                printip(ipbuf, client->ret_ip_port.ip);
                printf("  %15s %5u  %3llu", ipbuf, port, now - close_clientlist[i].ret_timestamp);
            }
        }

        printf("\n");
    }
    
}
