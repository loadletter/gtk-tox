/*
Compile with:
  gcc -Wall -g -o tutorial configdir.c main.c `pkg-config --cflags --libs gtk+-3.0 libtoxcore` -export-dynamic

TODO: CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP
*/
#include <gtk/gtk.h>
#include <tox/tox.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "configdir.h"

char* SRVLIST_FILE = NULL;

static Tox *init_tox()
{
    /* Init core */
    Tox *m = tox_new();

    /* Callbacks */
/*    tox_callback_friendrequest(m, on_request, NULL);                  //TODO
    tox_callback_friendmessage(m, on_message, NULL);
    tox_callback_namechange(m, on_nickchange, NULL);
    tox_callback_statusmessage(m, on_statuschange, NULL);
    tox_callback_action(m, on_action, NULL);*/
#ifdef __linux__
    tox_setname(m, (uint8_t *) "Cool guy", sizeof("Cool guy"));
#elif defined(WIN32)
    tox_setname(m, (uint8_t *) "I should install GNU/Linux", sizeof("I should install GNU/Linux"));
#elif defined(__APPLE__)
    tox_setname(m, (uint8_t *) "Hipster", sizeof("Hipster")); //This used to users of other Unixes are hipsters
#else
    tox_setname(m, (uint8_t *) "Registered Minix user #4", sizeof("Registered Minix user #4"));
#endif
    return m;
}

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

/* TODO: move things to another file*/ 
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

#define MAXLINE 90    /* Approx max number of chars in a sever line (IP + port + key) */
#define MINLINE 70
#define MAXSERVERS 50

/* Connects to a random DHT server listed in the DHTservers file */
int init_connection(Tox *m)
{
    FILE *fp = NULL;

    if (tox_isconnected(m))
        return 0;

    fp = fopen(SRVLIST_FILE, "r");

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

static void do_tox(Tox *m)
{
    static int conn_try = 0;
    static int conn_err = 0;
    static int dht_on = FALSE;

    if (!dht_on && !tox_isconnected(m) && !(conn_try++ % 100)) {
        if (!conn_err) {
            conn_err = init_connection(m);
            printf("Establishing connection...\n");
            if (conn_err)
                printf("Auto-connect failed with error code %d\n", conn_err);
        }
    } else if (!dht_on && tox_isconnected(m)) {
        dht_on = TRUE;
        printf("DHT connected.\n");
    } else if (dht_on && !tox_isconnected(m)) {
        dht_on = FALSE;
        printf("DHT disconnected. Attempting to reconnect.\n");
    }

    tox_do(m);
}

/* exits from gtk_main after closing */

void on_window_destroy (GtkWidget *object, gpointer user_data)
{
    gtk_main_quit ();
}

/* treeview */

enum
{
  LIST_ITEM = 0,
  N_COLUMNS
};

static void add_to_list(GtkWidget *list, const gchar *str)
{
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
}

/* timer */

static gboolean timer_handler(Tox *m) //change GtkWidget with Tox *
{
    do_tox(m);
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *treeview;

    gtk_init (&argc, &argv);

    Tox *m = init_tox();
    
    char *user_config_dir = get_user_config_dir();
    SRVLIST_FILE = malloc(strlen(user_config_dir) + strlen(CONFIGDIR) + strlen("DHTservers") + 1);
    strcpy(SRVLIST_FILE, user_config_dir);
    strcat(SRVLIST_FILE, CONFIGDIR);
    strcat(SRVLIST_FILE, "DHTservers");
    
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gtktox.ui", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    
    gtk_builder_connect_signals (builder, NULL);


    add_to_list(treeview, "vomit");
    
    
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    g_object_unref (G_OBJECT (builder));
    
    g_timeout_add(50, (GSourceFunc) timer_handler, m);
    gtk_widget_show (window);                
    gtk_main ();
    
    free(SRVLIST_FILE); /* TODO: make a cleanup function */
    return 0;
}