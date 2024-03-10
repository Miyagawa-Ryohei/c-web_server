#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

static const int PORT = 8080; 
static const int QUERY_KEY = 0;
static const int QUERY_VALUE = 1;

typedef struct _param Param;
typedef struct _param {
    char* key;
    char* value;
    void (*close)(Param*);
}Param;

void _param_close(Param* this){
    free(this->key);
    free(this->value);
    free(this);
}

Param* new_param(char* key, char* value) {
    Param* p = (Param*)malloc(sizeof(Param*));
    p->key = (char*)malloc(strlen(key));
    p->value = (char*)malloc(strlen(value));
    strcpy(p->key,key);
    strcpy(p->value,value);
    p->close = _param_close;
    return p;
}

typedef struct _params Params;
typedef struct _params {
    Param** _param;
    int len;
    char* (*get)(struct _params this, char* key);
    void (*close)(struct _params this);
}Params;
Params parse_params(char* fullpath);

char* _params_get(Params this, char* key) {
    const int len = this.len;
    for(int i = 0; i < len; i ++) {
        if(strcmp(this._param[i]->key, key) == 0) {
            return this._param[i]->value;
        }
    }
    return NULL;
}

void _params_close(Params this) {
    const int len = this.len;
    for(int i = 0; i < len; i ++) {
        this._param[i]->close(this._param[i]);
    }
}

// param_length
// @param fullpath 
// @return length of query parameter
// ex.
//    param : http://foo.com/bar?hoge=fuga&piyo=some
//    return : 2
int _param_length(char* raw_query){
    int len = strlen(raw_query);
    int cnt = 1;
    if(len == 0) {
        return 0;
    }
    for(int i = 0; i < len; i ++) {
        if(raw_query[i] == '&') {
            cnt ++;
        }
    }
    return cnt;
}

// to_query_string
// @param fullpath 
// @return pointer of a part of query string
// ex.
//    param : http://foo.com/bar?hoge=fuga&piyo=some
//    return : "hoge=fuga&piyo=some"
char* _to_query_string(char* fullpath){
    return NULL;
}

// to_query_tuple
// @param query_str 
// @return pointer of array like [[key1,value1],[key2,value2]]. 
// ex.
//    param : hoge=fuga&piyo=some
//    return : [["hoge","fuga"],["piyo","some"]]
char*** _to_query_tuple(char* query_str, int len){
    char whole[512], *query, query_buf[512], *key, *value;
    char*** table = (char***)malloc(sizeof(char***)*len);
    char* ret;
    int i = 0;

    ret = strcpy(whole, query_str);
    char* query_list[512];

    query = strtok(whole, "&");
    while(query != NULL) {
        if (query == NULL) {
            break;
        } 
        query_list[i] = query;
        query = strtok(NULL,"&");
        i++;
    }

    for( int j=0; j < len; j++) {
        key = strtok(query_list[j], "=");
        value = strtok(NULL, "=");

        table[j] = (char**)malloc(sizeof(char**)*2);
        table[j][QUERY_KEY] = (char*)malloc(sizeof(char)*strlen(key));
        ret = strcpy(table[j][QUERY_KEY],key);

        table[j][QUERY_VALUE] = (char*)malloc(sizeof(char)*strlen(value));
        ret = strcpy(table[j][QUERY_VALUE], value);
    }
    return table;
}

Params parse_params(char* raw_query) {

    Params p = {};
    p.close = &_params_close;
    p.get = _params_get;
    const int len = _param_length(raw_query);
    p.len = len;
    if(len == 0) {
        return p;
    }

    char*** pair = _to_query_tuple(raw_query, p.len);

    p._param = (Param**)malloc(len * sizeof(Param*));

    for(int i = 0; i < len; i++) {
        p._param[i] = new_param(pair[i][QUERY_KEY],pair[i][QUERY_VALUE]);
        free(pair[i][QUERY_KEY]);
        free(pair[i][QUERY_VALUE]);
        free(pair[i]);
    }
    free(pair);
    
    return p;
}



typedef struct _request Request;
typedef struct _request {
    char* uri;
    char* path;
    char* method;
    char* raw_query;
    Params params;
    void (*close)(Request* this);
} Request;
Request new_request(char* header);

void _request_close(Request* this) {
    this->params.close;
    free(this->path);
    free(this->uri);
    free(this->method);
    free(this->raw_query);
}

Request new_request(char* header) {
    Request req = {};
    char buf[2000];
    char *first_line, *method, *uri, *raw, *path, *ret;
    //非破壊にしたいためコピー
    strcpy(buf,header);

    first_line = strtok(buf, "\r\n");
    
    method = strtok(first_line, " ");
    req.method = (char*)malloc(sizeof(strlen(method)));
    uri = strcpy(req.method, method);

    uri = strtok(NULL, " ");
    req.uri = (char*)malloc(sizeof(strlen(uri)));
    ret = strcpy(req.uri, uri);

    path = strtok(uri,"?");
    req.path = (char*)malloc(sizeof(strlen(path)));
    ret = strcpy(req.path, path);

    raw = strtok(NULL, "?");
    req.close = _request_close;
    if(raw != NULL) {
        req.raw_query = (char*)malloc(sizeof(strlen(raw)));
        ret = strcpy(req.raw_query, raw);
        req.params = parse_params(req.raw_query);
    }


    return req;
}

