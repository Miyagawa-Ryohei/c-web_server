#include <string.h>
#include <stdlib.h>
#include "./request.h"
#include "./response.h"

typedef int (*HandlerFunc)(Request* req, Response* res);
typedef struct _route Route;
typedef struct _route {
    char* path;
    char* method;
    Route* next;
    HandlerFunc handler;
}Route;

Route* new_route(char* path, char* method, HandlerFunc handler) {
    Route* r = (Route*)malloc(sizeof(Route));
    r->path = path;
    r->method = method;
    r->handler = handler;
    r->next = NULL;
    return r;
}

typedef struct _router Router;
typedef struct _router {
    Route* first;
    Route* last;
    HandlerFunc (*detect_handler)(Router* this, char* path, char* method);
    int (*set)(Router* this, char* path, char* method, HandlerFunc handler);
}Router;

int _router_set(Router* this, char* path, char* method, HandlerFunc handler) {
    // 引数チェック
    if (path == NULL || method == NULL || handler == NULL) {
        return 1;
    }
    Route* yet = this->first;
    // 重複チェック
    while(yet != NULL) {
        if (strcmp(yet->method, method) && strcmp(yet->path, path)){
            return 2;
        }
        yet = yet->next;
    }
    Route* r = new_route(path, method, handler);

    if(this->first == NULL) {
        this->first = r;
    }
    if(this->last != NULL) {
        this->last->next = r;
    }
    this->last = r;

    return 0;
}

HandlerFunc _router_detect_handler(Router* this, char* path, char* method) {
    // 引数チェック
    if (path == NULL || method == NULL) {
        return NULL;
    }

    Route* yet = this->first;
    // 重複チェック
    while(yet != NULL) {
        if (!strcmp(yet->method, method) && !strcmp(yet->path, path)){
            return yet->handler;
        }
        yet = yet->next;
    }
    return NULL;
}

Router new_router(){
    Router r = {
        first: NULL,
        last: NULL,
    };
    r.set = _router_set;
    r.detect_handler = _router_detect_handler;
    return r;
}