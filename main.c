#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <holyhttp.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define DEBUG(fmt, args...)        printf("\033[1;40;36m" fmt "\033[0m\n", ##args)

static unsigned get_now_us()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned)(tv.tv_sec) * 1000 * 1000 + (unsigned)(tv.tv_usec);
}

static void cgi_test(holyreq_t *req)
{
    req->send_html(req, "baby is ok!");
}

static void cgi_render_string(holyreq_t *req)
{
    req->send_srender(req,
                      "Hello, @name! Are you @age?",
                      "name=%s,age=%d",
                      "baby", get_now_us() % 100);
}

static void cgi_render_file(holyreq_t *req)
{
    req->send_frender(req,
                      "test.html",
                      "name=%s,age=%d,a.1=%s,a.2=%s,a.b.1=A,a.b.2=B,a.b.3=C",
                      "baby", get_now_us() % 100, "Zeng", "Fanfan");
}

static void cgi_index(holyreq_t *req)
{
    req->send_frender(req, "index.html",
                      "title=%s,name=%s,page.name=%s",
                      "Lucky Day!", "Zeng Fanfan", "holyhttp-demo");
}

static void cgi_login(holyreq_t *req)
{
    char *name, *pwd;
    
    if (req->method != POST_METHOD) {
        req->send_frender(req, "login.html", "");
        return;
    }
    
    name = req->get_arg(req, "username");
    pwd = req->get_arg(req, "password");
    if (!name || !pwd) {
        DEBUG("username and password are both needed");
        return;
    }
    
    if (strcmp(name, "holy") != 0 || strcmp(pwd, "http") != 0) {
        req->redirect(req, "/login");
        return;
    }

    req->set_session(req, "online", "1");
    req->redirect(req, "/");
    return;
}

static void cgi_logout(holyreq_t *req)
{
    req->set_session(req, "online", "0");
    req->redirect(req, "/login");
    return;
}

static void init_cgi(void)
{
    holyhttp_set_route("/", cgi_index);
    holyhttp_set_route("index.html", cgi_index);
    holyhttp_set_white_route("login", cgi_login);
    holyhttp_set_route("logout", cgi_logout);
    holyhttp_set_route("test", cgi_test);
    holyhttp_set_route("render/string", cgi_render_string);
    holyhttp_set_route("render/file", cgi_render_file);
}

static int check_authorized(holyreq_t *req)
{
    char *online = req->get_session(req, "online");
    if (online && atoi(online)) {
        return 1;
    } else {
        req->redirect(req, "/login");
        return 0;
    }
}

int main(int argc, char *argv[])
{
    // holycfg_t cfg;
    holyhttp_init(NULL);
    holyhttp_set_debug_level(HOLY_DBG_DETAIL);
    holyhttp_set_prerouting(check_authorized);
    init_cgi();
    holyhttp_run();
    return 0;
}
