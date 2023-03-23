#include "jurl.h"

#undef JANET_REG
#define JANET_REG JANET_REG_D

#undef JANET_FN
#define JANET_FN JANET_FN_D

// jurl.c
JANET_FN(jurl_escape,
		"(escape string)",
		"Returns URL-encoded version of string");

JANET_FN(jurl_unescape,
		"(unescape string)",
		"Return URL-decoded version of string");

JANET_FN(jurl_new,
		"(new)",
		"Create new jurl handle");

JANET_FN(jurl_reset,
		"(reset handle)",
		"Reset an existing jurl handle");

JANET_FN(jurl_dup,
		"(dup handle)",
		"Duplicate an existing jurl handle");

JANET_FN(jurl_global_init,
		"(global-init)"
		"\n"
		"(global-init :ssl)",
		"Run curl global init");

JANET_FN(jurl_global_cleanup,
		"(global-cleanup)",
		"Run curl global cleanup");

JANET_FN(jurl_perform,
		"(perform handle)",
		"Perform the request associated with a handle");

JANET_FN(jurl_wrap_error,
		"(wrap-error :ok body)"
		"\n"
		"(wrap-error :err _)",
		"Boxes an :ok signal with a body, else the error with its explanation in string form");

// jurl_errors.c
JANET_FN(jurl_strerror,
		"(strerror :error/write-error)"
		"\n"
		"(strerror 1234)",
		"Get an explanation string about a jurl error");

// jurl_getinfo.c
JANET_FN(jurl_getinfo,
		"(getinfo handle :info)"
		"\n"
		"(getinfo handle 1234)",
		"Get a curl info from handle");

// jurl_mime.c
JANET_FN(jurl_mime_new,
		"(mime-new)",
		"Create a new mime form");

JANET_FN(jurl_mime_addpart,
		"(mime-addpart mime)"
		"\n"
		"(:addpart mime)",
		"Add a part to a mime form");

JANET_FN(jurl_mime_name,
		"(mime-name part string)",
		"Set a name for the mimepart");

JANET_FN(jurl_mime_data,
		"(mime-data part string)"
		"\n"
		"(mime-data part buffer)",
		"Set a string or buffer as the data source for the mimepart");

JANET_FN(jurl_mime_data_cb,
		"(mime-data-cb part datasize (fn [mode size|offset &opt position]))",
		"Set a callback as the data source for the mimepart");

JANET_FN(jurl_mime_filedata,
		"(mime-filedata part \"local.file\")",
		"Set a file as the data source for the mimepart");

JANET_FN(jurl_mime_filename,
		"(mime-filename part \"remote.file\")",
		"Set a custom remote filename for the upload mimepart");

JANET_FN(jurl_mime_type,
		"(mime-type part \"image/png\")",
		"Set a custom mimetype for the mimepart");

JANET_FN(jurl_mime_headers,
		"(mime-headers part [headers...])",
		"Set custom headers for the mimepart");

JANET_FN(jurl_mime_encoder,
		"(mime-encoder part :encoder)",
		"Set a part to be encoded using :encoder.\n"
		"Available encoders are:\n"
		":binary :8bit :7bit :base64 :quoted-printable");

JANET_FN(jurl_mime_subparts,
		"(mime-attach mime part)",
		"Attach mime to part as a subpart mime");

// jurl_setopt.c
JANET_FN(jurl_setopt,
		"(setopt handle :option value)"
		"\n"
		"(setopt handle 1234 value)",
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
	JANET_REG_END,
};

JANET_MODULE_ENTRY(JanetTable *env) {
	janet_cfuns_ext(env, "jurl/native", cfuns);
}
