// main.c: registers functions for use in janet
#include "jurl.h"

#undef JANET_REG
#define JANET_REG JANET_REG_D

#undef JANET_FN
#define JANET_FN JANET_FN_D

// errors.c
JANET_FN(jurl_strerror,
		"(strerror :error/write-error)"
		"\n"
		"(strerror 1234)",
		"Get an explanation string about a jurl error");

// getinfo.c
JANET_FN(jurl_getinfo,
		"(getinfo handle :info)"
		"\n"
		"(getinfo handle 1234)",
		"Get a curl info from handle");

// jurl.c
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

JANET_FN(jurl_new,
		"(new)",
		"Create new jurl handle");

JANET_FN(jurl_perform,
		"(perform handle)",
		"Perform the request associated with a handle");

JANET_FN(jurl_reset,
		"(reset handle)",
		"Reset an existing jurl handle");

// mime.c - mime
JANET_FN(jurl_mime_addpart,
		"(mime-addpart mime)"
		"\n"
		"(:addpart mime)",
		"Add a part to a mime form");

JANET_FN(jurl_mime_new,
		"(mime-new)",
		"Create a new mime form");

// mime.c - mimepart
JANET_FN(jurl_mime_data,
		"(mime-data part string)"
		"\n"
		"(mime-data part buffer)",
		"Set a string or buffer as the data source for the mimepart");

JANET_FN(jurl_mime_data_cb,
		"(mime-data-cb part datasize (fn [mode size|offset &opt position]))",
		"Set a callback as the data source for the mimepart");

JANET_FN(jurl_mime_encoder,
		"(mime-encoder part :encoder)",
		"Set a part to be encoded using :encoder.\n"
		"Available encoders are:\n"
		":binary :8bit :7bit :base64 :quoted-printable");

JANET_FN(jurl_mime_filedata,
		"(mime-filedata part \"local.file\")",
		"Set a file as the data source for the mimepart");

JANET_FN(jurl_mime_filename,
		"(mime-filename part \"remote.file\")",
		"Set a custom remote filename for the upload mimepart");

JANET_FN(jurl_mime_headers,
		"(mime-headers part [headers...])",
		"Set custom headers for the mimepart");

JANET_FN(jurl_mime_name,
		"(mime-name part string)",
		"Set a name for the mimepart");

JANET_FN(jurl_mime_subparts,
		"(mime-attach mime part)",
		"Attach mime to part as a subpart mime");

JANET_FN(jurl_mime_type,
		"(mime-type part \"image/png\")",
		"Set a custom mimetype for the mimepart");

// setopt.c
JANET_FN(jurl_setopt,
		"(setopt handle :option value)"
		"\n"
		"(setopt handle 1234 value)",
		"Set a curl option in a handle");

// util.c
JANET_FN(jurl_escape,
		"(escape string)",
		"Returns URL-encoded version of string");

JANET_FN(jurl_unescape,
		"(unescape string)",
		"Return URL-decoded version of string");

JANET_FN(jurl_wrap_error,
		"(wrap-error :ok body)"
		"\n"
		"(wrap-error :err _)",
		"Boxes an :ok signal with a body, else the error with its explanation in string form");

static const JanetRegExt cfuns[] = {
	// errors.c
	JANET_REG("strerror", jurl_strerror), // (strerror :ok)
	
	// getinfo.c
	JANET_REG("getinfo", jurl_getinfo), // (getinfo handle :tag), (get handle :tag), (:getinfo handle :tag)
	
	// jurl.c
	JANET_REG("dup",            jurl_dup),            // (:dup handle), (dup handle)
	JANET_REG("global-init",    jurl_global_init),    // (global-init :tag?)
	JANET_REG("global-cleanup", jurl_global_cleanup), // (global-cleanup)
	JANET_REG("new",            jurl_new),            // (new)
	JANET_REG("perform",        jurl_perform),        // (:perform handle), (perform handle)
	JANET_REG("reset",          jurl_reset),          // (:reset handle), (reset handle)

	// mime.c - mime
	JANET_REG("addpart",  jurl_mime_addpart), // (:addpart mime), (addpart mime)
	JANET_REG("new-mime", jurl_mime_new),     // (new-mime)

	// mime.c - mimepart
	JANET_REG("mime-data",     jurl_mime_data),     // (mime-data part data)
	JANET_REG("mime-data-cb",  jurl_mime_data_cb),  // (mime-data-cb part cb)
	JANET_REG("mime-encoder",  jurl_mime_encoder),  // (mime-encoder part encoder)
	JANET_REG("mime-filedata", jurl_mime_filedata), // (mime-filedata part fdata)
	JANET_REG("mime-filename", jurl_mime_filename), // (mime-filename part fname)
	JANET_REG("mime-name",     jurl_mime_name),     // (mime-name part name)
	JANET_REG("mime-subparts", jurl_mime_subparts), // (mime-subparts part mime)
	JANET_REG("mime-type",     jurl_mime_type),     // (mime-type part type)

	// setopt.c
	JANET_REG("setopt", jurl_setopt), // (put handle :tag val), (:setopt handle :tag val), (setopt handle :tag val)

	// util.c
	JANET_REG("escape",     jurl_escape),     // (escape string)
	JANET_REG("unescape",   jurl_unescape),   // (unescape string)
	JANET_REG("wrap-error", jurl_wrap_error), // (wrap-error :tag)

	JANET_REG_END,
};

JANET_MODULE_ENTRY(JanetTable *env) {
	janet_cfuns_ext(env, "jurl/native", cfuns);
}
