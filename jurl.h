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
JANET_CFUN(jurl_getopt);

// jurl_setopt.c
JANET_CFUN(jurl_setopt);
