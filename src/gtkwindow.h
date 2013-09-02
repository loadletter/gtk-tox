#pragma once
#include <tox/tox.h>

struct gtox_data {
    Tox     *tox;
    char    *srvlist_path;
    char    *datafile_path;
    GtkWidget *window;
    GtkWidget *statusbar;
    GtkNotebook *notebook;
    GtkWidget *dht_treeview;
    GtkWidget *friends_treeview;
    GtkWidget *friendreq_treeview;
    guint   statusbar_context_id;
};

enum
{
  NOTEBOOK_FRIENDS = 0,
  NOTEBOOK_FRIENDREQ = 1,
  NOTEBOOK_DHT = 2
};

#define ICON_FRIEND_ADD "icons/user_add.png"

void tw_add_to_list(GtkWidget *list, int list_item, const gchar *str);
void tw_clear_list(GtkWidget *list);

void note_hide_page(GtkNotebook *notebook, int pagenumber);
void note_show_page(GtkNotebook *notebook, int pagenumber);

void dialog_show_error(const gchar *message);

void dialog_friendrequest_show(gpointer window, gchar *text_id, gchar *text_msg);
gint dialog_friendrequest_accept(gpointer window, gchar *text_id, gchar *text_msg);
