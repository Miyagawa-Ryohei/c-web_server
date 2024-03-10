#include <stdio.h>
#include "./request.h"
#include "./server.h"
#include "./router.h"

int test(Request* req, Response* res) {
    char* a = req->params.get(req->params, "a");
    printf("param a = %s", a);
    res->status_code=200;
    res->status="OKOK!!!";
    return 0;
}

int index(Request* req, Response* res) {
    char* html = "\
<html xmlns='http://www.w3.org/1999/xhtml' lang='ja'>\
 <head>\
  <title>Sample Index Page</title>\
 </head>\
 <body>\
  <h1>Kakkaの不思議</h1>\
  <p>意味のまとまりのある段落を書こう</p>\
  <ul>\
   <li>内容をリストとして</li>\
   <li>列挙してもよい</li>\
   <li><a href='http://www.kanzaki.com/'>役立つ情報</a>にリンクもできる</li>\
  </ul>\
  <hr />\
  <footer>\
   <address>\
    何某作;  \
    <a href='mailto:yourmail@domain.com'>作者へのメッセージ</a>\
   </address>\
   20YY-MM-DD更新\
  </footer>\
 </body>\
</html>"; 
    res->status_code=200;
    res->status="OK";
    res->body=html;
    res->content_type="text/html";
    return 0;
}

int main(void) {
    Param* p = new_param("staff", "sample");
    printf("hello world %s:%s\n", p->key, p->value);
    Router r = new_router();
    Server* s = new_server(&r);
    s->router->set(s->router,"/","GET",index);
    s->router->set(s->router,"/test","GET",test);
    s->serve(s, 8080);
}