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

void note_hide_page(GtkNotebook *notebook, int pagenumber);
void note_show_page(GtkNotebook *notebook, int pagenumber);

void dialog_show_error(const gchar *message);
gint dialog_show_question(gpointer window, gchar *title, gchar *message);

void dialog_friendrequest_show(gpointer window, gchar *text_id, gchar *text_msg);
gint dialog_friendrequest_accept(gpointer window, gchar *text_id, gchar *text_msg);

void update_friendrequest_tab(GtkNotebook *notebook, GtkTreeView *treeview);

GtkWidget *do_chat_window(GtkWidget *window);
