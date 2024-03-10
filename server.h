#include "./router.h"
#ifndef SERVER_H
#define SERVER_H

typedef struct _server Server;
typedef struct _server {
    Router* router;
    int (*init)(Server* this);
    int (*serve)(Server* this, int port);
} Server;
Server* new_server(Router* r);
#endif