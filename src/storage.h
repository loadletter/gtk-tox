#pragma once

/* DHTservers stuff */
#define MAXLINE 90    /* Approx max number of chars in a sever line (IP + port + key) */
#define MINLINE 70
#define MAXSERVERS 50

extern char* SRVLIST_FILE;

int init_connection(Tox *m);

/* Userdata storage */
/* TODO */
