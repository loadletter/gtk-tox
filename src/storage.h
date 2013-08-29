#pragma once

struct storage_data {
    Tox     *tox;
    char    *srvlist_path;
    char    *datafile_path;
    GtkWidget *statusbar;
    guint   statusbar_context_id;
};

/* DHTservers stuff */
#define MAXLINE 90    /* Approx max number of chars in a sever line (IP + port + key) */
#define MINLINE 70
#define MAXSERVERS 50

int init_connection(struct storage_data *stor_d);
char *get_full_configpath(const char *filename);

/* Userdata storage */
/* TODO */
