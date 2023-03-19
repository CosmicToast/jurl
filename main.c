#include "jurl.h"

#undef JANET_REG
#define JANET_REG JANET_REG_D

#undef JANET_FN
#define JANET_FN JANET_FN_D

// jurl.c
JANET_FN(jurl_new,
		"(jurl-native/new)",
		"Create new jurl handle");

JANET_FN(jurl_reset,
		"(jurl-native/reset handle)",
		"Reset an existing jurl handle");

JANET_FN(jurl_dup,
		"(jurl-native/dup handle)",
		"Duplicate an existing jurl handle");

JANET_FN(jurl_perform,
		"(jurl-native/perform handle)",
		"Perform the request associated with a handle");

// jurl_getinfo.c
JANET_FN(jurl_getinfo,
		"(jurl-native/getinfo handle :info)"
		"\n"
		"(jurl-native/getinfo handle 1234)",
		"Get a curl info from handle");

// jurl_setopt.c
JANET_FN(jurl_setopt,
		"(jurl-native/setopt handle :option value)"
		"\n"
		"(jurl-native/setopt handle 1234 value)",
		"Set a curl option in a handle");

static const JanetRegExt cfuns[] = {
	// jurl.c
	JANET_REG("new", jurl_new),
	JANET_REG("reset", jurl_reset),
	JANET_REG("dup", jurl_dup),
	JANET_REG("perform", jurl_perform),

	// jurl_getinfo.c
	JANET_REG("getinfo", jurl_getinfo),

	// jurl_setopt.c
	JANET_REG("setopt", jurl_setopt),
};

JANET_MODULE_ENTRY(JanetTable *env) {
	janet_cfuns_ext(env, "jurl-native", cfuns);
}
