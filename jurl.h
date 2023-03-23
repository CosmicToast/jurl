#pragma once
#include <curl/curl.h>
#include <janet.h>

// jurl globals
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

struct jurl_handle {
	CURL* handle;
	struct jurl_cleanup* cleanup;
};
typedef struct jurl_handle jurl_handle;

// jurl.c
struct jurl_cleanup *register_cleanup(jurl_handle *jurl, enum jurl_cleanup_type type);
JANET_CFUN(jurl_escape);
JANET_CFUN(jurl_unescape);
JANET_CFUN(jurl_new);
JANET_CFUN(jurl_reset);
JANET_CFUN(jurl_dup);
JANET_CFUN(jurl_perform);
JANET_CFUN(jurl_wrap_error);
jurl_handle *janet_getjurl(Janet *argv, int32_t n);

// jurl_callbacks.c
CURLcode jurl_setcallback(jurl_handle *jurl, CURLoption opt, JanetFunction *fun);

// jurl_enums.c
CURLcode jurl_setenum(jurl_handle *jurl, CURLoption opt, Janet val);

// jurl_errors.c
Janet jurl_geterror(CURLcode code);
JANET_CFUN(jurl_strerror);

// jurl_getinfo.c
JANET_CFUN(jurl_getinfo);

// jurl_mime.c
struct jurl_mime {
	CURL *curl;
	curl_mime *handle;
	int clean;
};
typedef struct jurl_mime jurl_mime;
JANET_CFUN(jurl_mime_new);
JANET_CFUN(jurl_mime_addpart);
JANET_CFUN(jurl_mime_name);
JANET_CFUN(jurl_mime_data);
JANET_CFUN(jurl_mime_data_cb);
JANET_CFUN(jurl_mime_filedata);
JANET_CFUN(jurl_mime_filename);
JANET_CFUN(jurl_mime_type);
JANET_CFUN(jurl_mime_headers);
JANET_CFUN(jurl_mime_encoder);
JANET_CFUN(jurl_mime_subparts);
jurl_mime *janet_getjurlmime(Janet *argv, int32_t n);

// jurl_setopt.c
JANET_CFUN(jurl_setopt);
