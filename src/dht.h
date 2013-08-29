#pragma once
#include "include/Messenger.h"

struct dht_tree_data {
    Messenger *m;
    GtkWidget *gtk;
};

void dht_draw(struct dht_tree_data *);


