#include "./request.h"
#include "./response.h"
#ifndef ROUTER_H
#define ROUTER_H

typedef int (*HandlerFunc)(Request* req, Response* res);
typedef struct _route Route;
typedef struct _route {
    char* path;
    char* method;
    Route* next;
    HandlerFunc handler;
}Route;


typedef struct _router Router;
typedef struct _router {
    Route* first;
    Route* last;
    HandlerFunc (*detect_handler)(Router* this, char* path, char* method);
    int (*set)(Router* this, char* path, char* method, HandlerFunc handler);
}Router;
Router new_router();
#endif