#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <lib/holyhttp.h>
#include <errno.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define DEBUG(fmt, args...)        printf("\033[1;40;36m" fmt "\033[0m\n", ##args)

#define UPFILENAME1 "file1"
#define UPFILENAME2 "file2"

int holy_fetch_file(char *filename, void **data, unsigned *len);

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
}

static void cgi_logout(holyreq_t *req)
{
    req->set_session(req, "online", "0");
    req->redirect(req, "/login");
}

static void cgi_download(holyreq_t *req)
{
    char *txt = "This is a test file!\r\n^_^";
    req->response(req, OK, txt, strlen(txt), "text", 0, 0, NULL, NULL, "download.txt");
}

static void cgi_file1(holyreq_t *req)
{
    char *file = UPFILENAME1;
    void *data;
    unsigned len;

    if (!holy_fetch_file(file, &data, &len)) {
        req->send_status(req, INSUFFICIENT_STORAGE);
        return;
    }

    DEBUG("GO");

    req->response(req, OK, data, len, "application/octet-stream", 0, 0, NULL, NULL, NULL);

    free(data);
}

static int write_file(char *filename, void *data, unsigned len)
{
    FILE *fp = fopen(filename, "w+");
    if (!fp) {
        DEBUG("Failed to open %s, %s.", filename, strerror(errno));
        return 0;
    }

    if (fwrite(data, len, 1, fp) < 0) {
        DEBUG("Failed to write %s, %s.", filename, strerror(errno));
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

static void cgi_upload(holyreq_t *req)
{
    char *file1 = UPFILENAME1, *file2 = UPFILENAME2;
    void *data1, *data2;
    unsigned dlen1, dlen2;

    if (req->method != POST_METHOD) {
        goto exit;
    }

    if (!req->get_bin_arg(req, "file1", &data1, &dlen1)) {
        DEBUG("file1 is null");
        goto exit;
    }

    write_file(file1, data1, dlen1);

    if (!req->get_bin_arg(req, "file2", &data2, &dlen2)) {
        DEBUG("file2 is null");
        goto exit;
    }

    write_file(file2, data2, dlen2);

exit:
    req->send_frender(req, "upload.html", "last_time=%u", get_now_us());
}

static void init_cgi(void)
{
    holyhttp_set_common_render_args(",", "g.start_time=%u", get_now_us());

    holyhttp_set_route("/", cgi_index);
    holyhttp_set_route("index.html", cgi_index);
    holyhttp_set_white_route("login", cgi_login);
    holyhttp_set_route("logout", cgi_logout);
    holyhttp_set_route("test", cgi_test);
    holyhttp_set_route("render/string", cgi_render_string);
    holyhttp_set_route("render/file", cgi_render_file);
    holyhttp_set_white_route("upload", cgi_upload);
    holyhttp_set_white_route("download", cgi_download);
    holyhttp_set_white_route("file1", cgi_file1);
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
