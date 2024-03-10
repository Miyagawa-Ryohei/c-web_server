#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./router.h"
#include "./request.h"
static const int PORT = 8080; 

typedef struct _server Server;
typedef struct _server {
    Router* router;
    int (*init)(Server* this);
    int (*serve)(Server* this, int port);
} Server;

int _server_init(Server* this) {
    return 0;
}

int _server_serve(Server* this, int port) {
    if(port = -1) {
        port = PORT;
    }
    int rsock, wsock, len, read_result, err;
    char buf[1000]; 
    char resp[1000]; 
    struct sockaddr_in addr, client;

    // ソケット作成
    printf("socket creating...\n");
    rsock = socket(AF_INET, SOCK_STREAM, 0);
    printf("done\n");

    // 待受用のソケットアドレス設定
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // ソケットにバインド
    printf("socket is binding ...\n");
    err = bind(rsock, (struct sockaddr *)&addr, sizeof(addr));
    printf("done\n");
    if (err != 0) {
        printf("cannot bind socket \n");
        return -1;
    }

    // リッスン
    printf("listen....\n");
    err = listen(rsock, 10);
    if (err != 0) {
        printf("cannot listen \n");
        return -1;
    }

    while (1) {
        // 接続要求を待ち受ける
        wsock = accept(rsock, (struct sockaddr *)&client, &len);
        read_result = read(wsock, buf, sizeof(buf));
        // 要求が来たら、なんか色々処理する
        if(read_result == -1) {
            return -1;
        }
        Request req = new_request(buf);
        Response res = new_response();
        HandlerFunc handler = this->router->detect_handler(this->router, req.path, req.method);
        if(handler == NULL) {
            sprintf(resp,"HTTP1.1 %d %s", 404, "NotFound");
            write(wsock,resp, strlen(resp));            
        } else {
            handler(&req, &res);
            sprintf(resp,"HTTP1.1 %d %s\r\n", res.status_code, res.status);
            write(wsock,resp, strlen(resp));
            if(res.content_type != NULL) {
                sprintf(resp,"Content-Type: %s; charset=UTF-8\r\n", res.content_type);
                write(wsock,resp, strlen(resp));
            }
            if(res.body != NULL) {
                sprintf(resp,"\r\n %s", res.body);
                write(wsock,resp, strlen(resp));
            }
        }
        memset(buf,0,sizeof(buf));
        memset(resp,0,sizeof(buf));
        close(wsock);
    }
    return 0;
}

Server* new_server(Router* r) {
    Server* s = (Server*)malloc(sizeof(Server));
    s->router = r;
    s->init = _server_init;
    s->serve = _server_serve;
    return s;
}


