#include "jurl.h"

#undef JANET_REG
#define JANET_REG JANET_REG_D

#undef JANET_FN
#define JANET_FN JANET_FN_D

// jurl.c
JANET_FN(jurl_escape,
		"(jurl-native/escape string)",
		"Returns URL-encoded version of string");

JANET_FN(jurl_unescape,
		"(jurl-native/unescape string)",
		"Return URL-decoded version of string");

JANET_FN(jurl_new,
		"(jurl-native/new)",
		"Create new jurl handle");

JANET_FN(jurl_reset,
		"(jurl-native/reset handle)",
		"Reset an existing jurl handle");

JANET_FN(jurl_dup,
		"(jurl-native/dup handle)",
		"Duplicate an existing jurl handle");

JANET_FN(jurl_global_init,
		"(jurl-native/global-init)"
		"\n"
		"(jurl-native/global-init :ssl)",
		"Run curl global init");

JANET_FN(jurl_global_cleanup,
		"(jurl-native/global-cleanup)",
		"Run curl global cleanup");

JANET_FN(jurl_perform,
		"(jurl-native/perform handle)",
		"Perform the request associated with a handle");

JANET_FN(jurl_wrap_error,
		"(jurl-native/wrap-error :ok body)"
		"\n"
		"(jurl-native/wrap-error :err _)",
		"Boxes an :ok signal with a body, else the error with its explanation in string form");

// jurl_errors.c
JANET_FN(jurl_strerror,
		"(jurl-native/strerror :error/write-error)"
		"\n"
		"(jurl-native/strerror 1234)",
		"Get an explanation string about a jurl error");

// jurl_getinfo.c
JANET_FN(jurl_getinfo,
		"(jurl-native/getinfo handle :info)"
		"\n"
		"(jurl-native/getinfo handle 1234)",
		"Get a curl info from handle");

// jurl_mime.c
JANET_FN(jurl_mime_new,
		"(jurl-native/mime-new)",
		"Create a new mime form");

JANET_FN(jurl_mime_addpart,
		"(jurl-native/mime-addpart mime)"
		"\n"
		"(:addpart mime)",
		"Add a part to a mime form");

JANET_FN(jurl_mime_name,
		"(jurl-native/mime-name part string)",
		"Set a name for the mimepart");

JANET_FN(jurl_mime_data,
		"(jurl-native/mime-data part string)"
		"\n"
		"(jurl-native/mime-data part buffer)",
		"Set a string or buffer as the data source for the mimepart");

JANET_FN(jurl_mime_data_cb,
		"(jurl-native/mime-data-cb part (fn ...))",
		"Set a callback as the data source for the mimepart");

JANET_FN(jurl_mime_filedata,
		"(jurl-native/mime-filedata part \"local.file\")",
		"Set a file as the data source for the mimepart");

JANET_FN(jurl_mime_filename,
		"(jurl-native/mime-filename part \"remote.file\")",
		"Set a custom remote filename for the upload mimepart");

JANET_FN(jurl_mime_type,
		"(jurl-native/mime-type part \"image/png\")",
		"Set a custom mimetype for the mimepart");

JANET_FN(jurl_mime_headers,
		"(jurl-native/mime-headers part [headers...])",
		"Set custom headers for the mimepart");

JANET_FN(jurl_mime_encoder,
		"(jurl-native/mime-encoder part :encoder)",
		"Set a part to be encoded using :encoder.\n"
		"Available encoders are:\n"
		":binary :8bit :7bit :base64 :quoted-printable");

JANET_FN(jurl_mime_subparts,
		"(jurl-native/mime-attach mime part)",
		"Attach mime to part as a subpart mime");

// jurl_setopt.c
JANET_FN(jurl_setopt,
		"(jurl-native/setopt handle :option value)"
		"\n"
		"(jurl-native/setopt handle 1234 value)",
		"Set a curl option in a handle");

static const JanetRegExt cfuns[] = {
	// jurl.c
	JANET_REG("escape", jurl_escape),
	JANET_REG("unescape", jurl_unescape),
	JANET_REG("new", jurl_new),
	JANET_REG("reset", jurl_reset),
	JANET_REG("dup", jurl_dup),
	JANET_REG("global-init", jurl_global_init),
	JANET_REG("global-cleanup", jurl_global_cleanup),
	JANET_REG("perform", jurl_perform),
	JANET_REG("wrap-error", jurl_wrap_error),

	// jurl_errors.c
	JANET_REG("strerror", jurl_strerror),

	// jurl_getinfo.c
	JANET_REG("getinfo", jurl_getinfo),

	// jurl_mime.c
	JANET_REG("mime-new", jurl_mime_new),
	JANET_REG("mime-addpart", jurl_mime_addpart),
	JANET_REG("mime-name", jurl_mime_name),
	JANET_REG("mime-data", jurl_mime_data),
	JANET_REG("mime-data-cb", jurl_mime_data_cb),
	JANET_REG("mime-filedata", jurl_mime_filedata),
	JANET_REG("mime-filename", jurl_mime_filename),
	JANET_REG("mime-type", jurl_mime_type),
	JANET_REG("mime-headers", jurl_mime_headers),
	JANET_REG("mime-encoder", jurl_mime_encoder),
	JANET_REG("mime-attach", jurl_mime_subparts),

	// jurl_setopt.c
	JANET_REG("setopt", jurl_setopt),
};

JANET_MODULE_ENTRY(JanetTable *env) {
	janet_cfuns_ext(env, "jurl-native", cfuns);
}
