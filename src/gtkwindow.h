#pragma once

struct gtox_data {
    Tox     *tox;
    char    *srvlist_path;
    char    *datafile_path;
    GtkWidget *statusbar;
    GtkNotebook *notebook;
    GtkWidget *dht_treeview;
    GtkWidget *friends_treeview;
    GtkWidget *friendreq_treeview;
    GtkWidget *friendreq_dialog;
    GtkLabel *friendreq_dialog_id;
    GtkLabel *friendreq_dialog_msg;
    guint   statusbar_context_id;
};

enum
{
  NOTEBOOK_FRIENDS = 0,
  NOTEBOOK_FRIENDREQ = 1,
  NOTEBOOK_DHT = 2
};


void tw_add_to_list(GtkWidget *list, int list_item, const gchar *str);
void tw_clear_list(GtkWidget *list);

void note_hide_page(GtkNotebook *notebook, int pagenumber);
void note_show_page(GtkNotebook *notebook, int pagenumber);

void error_message (const gchar *message);
