#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>


typedef struct _response {
    int status_code;
    char* status;
    char* content_type;
    char* body;
    void (*close)(struct _response* this);
} Response;


void _response_close(Response* this) {
}

Response new_response() {
    Response r = {};
    r.close = _response_close;
    return r;
}
