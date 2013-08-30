#pragma once
#include "gtkwindow.h"
#include "include/Messenger.h"

struct dht_tree_data {
    Messenger *m;
    GtkWidget *gtk;
};

void dht_draw(struct gtox_data *gtox);


