#include <tox/tox.h>
#include <gtk/gtk.h>
#include "dht.h"
#include "gtkwindow.h"  
#include "include/DHT.h"

typedef uint8_t ipbuf[3 * 4 + 3 + 1];

static GtkTreeIter iter[LCLIENT_LIST];
static int row_filled[LCLIENT_LIST] = {0};

Client_data *DHT_get_close_list(DHT *dht)
{
    return dht->close_clientlist;
}

static void printip(ipbuf buf, IP ip)
{
    sprintf((char *)buf, "%u.%u.%u.%u", ip.c[0], ip.c[1], ip.c[2], ip.c[3]);
}

void dht_draw(struct gtox_data *gtox)
{
    Messenger   *m = gtox->tox;
    GtkWidget   *dht_treeview = gtox->dht_treeview;
    GtkListStore *store;
    uint64_t now = unix_time();
    uint32_t i, j;
    ipbuf ipbuf;

    /* don't waste cpu if the current page is not the dht page */
    if(gtk_notebook_get_current_page(gtox->notebook) != NOTEBOOK_DHT)
        return;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(dht_treeview)));
    Client_data *close_clientlist = DHT_get_close_list(m->dht);

    for (i = 0; i < LCLIENT_LIST; i++) { /* Number of nodes in closelist */
    
        Client_data *client = close_clientlist + i;
        uint16_t port = ntohs(client->ip_port.port);
        char tmp_id[(CLIENT_ID_SIZE * 2) + 1], tmp_port[6];
        
        if (port) {
            memset(tmp_id, '\0', sizeof(tmp_id));
            memset(tmp_port, '\0', sizeof(tmp_port));
            
            if(!row_filled[i])
                gtk_list_store_append(store, &iter[i]);
                row_filled[i] = TRUE;
                        
            for (j = 0; j < CLIENT_ID_SIZE; j++)
                sprintf(tmp_id,"%s%02hhx", tmp_id, client->client_id[j]);
            gtk_list_store_set(store, &iter[i], 0, tmp_id, -1);

            printip(ipbuf, client->ip_port.ip);
            sprintf(tmp_port, "%5u", port);
            gtk_list_store_set(store, &iter[i], 1, ipbuf, 2, tmp_port, -1);        
            gtk_list_store_set(store, &iter[i], 3, now - client->timestamp, 4, now - client->last_pinged, -1);
            
            port = ntohs(client->ret_ip_port.port);
            if (port) {
                printip(ipbuf, client->ret_ip_port.ip);
                sprintf(tmp_port, "%5u", port);
                gtk_list_store_set(store, &iter[i], 5, ipbuf, 6, tmp_port, -1);
                gtk_list_store_set(store, &iter[i], 7, now - close_clientlist[i].ret_timestamp, -1);
            }
        } else if(row_filled[i]) {
            gtk_list_store_remove(store, &iter[i]);
            row_filled[i] = FALSE;
        }
    }
    
}
