#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct _response Response;
typedef struct _response {
    int status_code;
    char* status;
    char* content_type;
    char* body;
    void (*close)(struct _response* this);
} Response;
Response new_response();

#endif