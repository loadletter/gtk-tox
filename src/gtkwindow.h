#pragma once

struct gtox_data {
    Tox     *tox;
    char    *srvlist_path;
    char    *datafile_path;
    GtkWidget *statusbar;
    GtkNotebook *notebook;
    GtkWidget *dht_treeview;
    guint   statusbar_context_id;
};

void tw_add_to_list(GtkWidget *list, int list_item, const gchar *str);
void tw_clear_list(GtkWidget *list);
