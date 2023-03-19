#pragma once
#include <curl/curl.h>
#include <janet.h>

// jurl globals
struct jurl_handle {
	CURL* handle;
};
typedef struct jurl_handle jurl_handle;

// jurl.c
JANET_CFUN(jurl_new);
JANET_CFUN(jurl_reset);
JANET_CFUN(jurl_dup);
JANET_CFUN(jurl_perform);

#ifndef JURL_C
extern JANET_API const JanetAbstractType jurl_type;
#endif

// jurl_setopt.c
JANET_CFUN(jurl_setopt);
