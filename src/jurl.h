#pragma once
#include <curl/curl.h>
#include <janet.h>

// = polyfills
#if JANET_VERSION_MAJOR < 2 && JANET_VERSION_MINOR < 28
#define POLYFILL_GETCBYTES
#define POLYFILL_OPTCBYTES
#elif JANET_VERSION_MAJOR < 2 && JANET_VERSION_MINOR < 30 && JANET_VERSION_PATCH < 2
// TODO: consider making this unconditional, see polyfill.c for details
#define POLYFILL_GETCBYTES
#endif

#ifdef POLYFILL_GETCBYTES
const char *poly_getcbytes(const Janet *argv, int32_t n);
#define janet_getcbytes poly_getcbytes
#endif

#ifdef POLYFILL_OPTCBYTES
const char *poly_optcbytes(const Janet *argv, int32_t argc, int32_t n, const char *dflt);
#define janet_optcbytes poly_optcbytes
#endif

// = structures

// cleanup.c
enum jurl_cleanup_type {
	JURL_CLEANUP_TYPE_SLIST,
};

struct jurl_cleanup {
	struct jurl_cleanup *next;
	enum jurl_cleanup_type type;
	union {
		struct curl_slist *slist; // when JURL_CLEANUP_TYPE_SLIST
	};
};

// jurl.c
struct jurl_handle {
	CURL* handle;
	struct jurl_cleanup* cleanup;
};
typedef struct jurl_handle jurl_handle;

// mime.c
struct jurl_mime {
	curl_mime *mime;
	int clean;
};
typedef struct jurl_mime jurl_mime;

struct jurl_mimepart {
	curl_mimepart *mimepart;
	jurl_mime     *parent;
	struct jurl_cleanup *cleanup;
};
typedef struct jurl_mimepart jurl_mimepart;

// = functions

// callbacks.c
CURLcode jurl_setcallback(jurl_handle *jurl, CURLoption opt, JanetFunction *fun);

// cleanup.c
void jurl_do_cleanup(struct jurl_cleanup **src);
struct jurl_cleanup *register_cleanup(struct jurl_cleanup **prev, enum jurl_cleanup_type type);

// enums.c
CURLcode jurl_setenum(jurl_handle *jurl, CURLoption opt, Janet val);
Janet jurl_getinfoenum(CURLINFO info, long code);
Janet jurl_getinfomask(CURLINFO info, long code);

// errors.c
Janet jurl_geterror(CURLcode code);
JANET_CFUN(jurl_strerror);

// getinfo.c
JANET_CFUN(jurl_getinfo);

// jurl.c
JANET_CFUN(jurl_dup);
jurl_handle *janet_getjurl(Janet *argv, int32_t n);
JANET_CFUN(jurl_global_init);
JANET_CFUN(jurl_global_cleanup);
JANET_CFUN(jurl_new);
JANET_CFUN(jurl_perform);
JANET_CFUN(jurl_reset);

// mime.c - mime
JANET_CFUN(jurl_mime_addpart);
JANET_CFUN(jurl_mime_new);
jurl_mime *janet_getjurlmime(Janet *argv, int32_t n);

// mime.c - mimepart
jurl_mimepart *janet_getjurlmimepart(Janet *argv, int32_t n);
JANET_CFUN(jurl_mime_data);
JANET_CFUN(jurl_mime_data_cb);
JANET_CFUN(jurl_mime_encoder);
JANET_CFUN(jurl_mime_filedata);
JANET_CFUN(jurl_mime_filename);
JANET_CFUN(jurl_mime_headers);
JANET_CFUN(jurl_mime_name);
JANET_CFUN(jurl_mime_subparts);
JANET_CFUN(jurl_mime_type);

// setopt.c
JANET_CFUN(jurl_setopt);

// util.c
int janet_getslist(struct curl_slist **slist, Janet *argv, int32_t n);
JANET_CFUN(jurl_escape);
JANET_CFUN(jurl_unescape);
JANET_CFUN(jurl_wrap_error);
