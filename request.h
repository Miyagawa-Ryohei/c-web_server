#ifndef REQUEST_H
#define REQUEST_H

// http parameter 構造体
typedef struct _param Param;
typedef struct _param {
    char* key;
    char* value;
    void (*close)(Param*);
}Param;
Param* new_param(char* key, char* value);

typedef struct _params Params;
typedef struct _params {
    Param** _param;
    int len;
    char* (*get)(struct _params this, char* key);
    void (*close)(struct _params this);
}Params;
Params parse_params(char* fullpath);

typedef struct _request Request;
typedef struct _request {
    char* uri;
    char* path;
    char* method;
    char* raw_query;
    Params params;
    void (*close)(Request* this);
} Request;
Request new_request(char* raw);

#endif