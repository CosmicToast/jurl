// jurl_mime implements curl_mime_*
#include "jurl.h"

static int jurl_mime_gc(void *p, size_t s) {
	(void) s;
	jurl_mime *mime = (jurl_mime*)p;
	if (mime->clean) curl_mime_free(mime->handle);
	curl_easy_cleanup(mime->curl);
	return 0;
}

static JanetMethod jurl_mime_methods[] = {
	{"addpart", jurl_mime_addpart},
	{"attach",  jurl_mime_subparts},
	{NULL, NULL},
};

static int jurl_mime_get(void *p, Janet key, Janet *out) {
	(void) p;
	if (!janet_checktype(key, JANET_KEYWORD)) {
		return 0;
	}
	return janet_getmethod(janet_unwrap_keyword(key), jurl_mime_methods, out);
}

static const JanetAbstractType jurl_mimetype = {
	"jurl-mime",  // name
	jurl_mime_gc, // gc
	NULL,         // gcmark
	jurl_mime_get,     //get
	JANET_ATEND_GET
};

jurl_mime *janet_getjurlmime(Janet *argv, int32_t n) {
	return (jurl_mime*)janet_getabstract(argv, n, &jurl_mimetype);
}

// we generate a separate handle for the generation, it's used a lot
JANET_CFUN(jurl_mime_new) {
	janet_fixarity(argc, 0);
	jurl_mime *mime = (jurl_mime*)janet_abstract(&jurl_mimetype, sizeof(jurl_mime));
	mime->clean  = 1; // clean by default
	mime->curl   = curl_easy_init();
	mime->handle = curl_mime_init(mime->curl);
	return janet_wrap_abstract(mime);
}

JANET_CFUN(jurl_mime_addpart) {
	janet_fixarity(argc, 1);
	jurl_mime *mime = janet_getjurlmime(argv, 0);
	return janet_wrap_pointer(curl_mime_addpart(mime->handle));
}

JANET_CFUN(jurl_mime_name) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_name(part, NULL);
	} else {
		const char *s = (const char*)janet_getcstring(argv, 1);
		ret = curl_mime_name(part, s);
	}
	return jurl_geterror(ret);
}

JANET_CFUN(jurl_mime_data) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_data(part, NULL, 0);
	} else {
		JanetByteView bytes = janet_getbytes(argv, 1);
		ret = curl_mime_data(part, (const char*)bytes.bytes, bytes.len);
	}
	return jurl_geterror(ret);
}

// TODO
JANET_CFUN(jurl_mime_data_cb) {
	janet_panic("not implemented");
	return janet_wrap_nil(); // unreachable
}

JANET_CFUN(jurl_mime_filedata) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_filedata(part, NULL);
	} else {
		const char *s = (const char*)janet_getcstring(argv, 1);
		ret = curl_mime_filedata(part, s);
	}
	return jurl_geterror(ret);
}

JANET_CFUN(jurl_mime_filename) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_filename(part, NULL);
	} else {
		const char *s = (const char*)janet_getcstring(argv, 1);
		ret = curl_mime_filename(part, s);
	}
	return jurl_geterror(ret);
}

JANET_CFUN(jurl_mime_type) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_type(part, NULL);
	} else {
		const char *s = (const char*)janet_getcstring(argv, 1);
		ret = curl_mime_type(part, s);
	}
	return jurl_geterror(ret);
}

// TODO: really? I have to do cleanup *again*? ;-;
JANET_CFUN(jurl_mime_headers) {
	janet_panic("not implemented");
	return janet_wrap_nil(); // unreachable
}

JANET_CFUN(jurl_mime_encoder) {
	janet_fixarity(argc, 2);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 0);
	CURLcode ret;
	if (janet_checktype(argv[1], JANET_NIL)) {
		ret = curl_mime_encoder(part, NULL);
	} else {
		if        (janet_keyeq(argv[1], "binary")) {
			ret = curl_mime_encoder(part, "binary");
		} else if (janet_keyeq(argv[1], "8bit")) {
			ret = curl_mime_encoder(part, "8bit");
		} else if (janet_keyeq(argv[1], "7bit")) {
			ret = curl_mime_encoder(part, "7bit");
		} else if (janet_keyeq(argv[1], "base64")) {
			ret = curl_mime_encoder(part, "base64");
		} else if (janet_keyeq(argv[1], "quoted-printable")) {
			ret = curl_mime_encoder(part, "quoted-printable");
		} else {
			janet_panic("jurl_mime_encoder: invalid encoding type");
		}
	}
	return jurl_geterror(ret);
}

// this is exposed as (:attach mime part)
JANET_CFUN(jurl_mime_subparts) {
	janet_fixarity(argc, 2);
	jurl_mime *mime = janet_getjurlmime(argv, 0);
	curl_mimepart *part = (curl_mimepart*)janet_getpointer(argv, 1);
	CURLcode ret = curl_mime_subparts(part, mime->handle);
	if (ret == CURLE_OK) mime->clean = 0; // no longer needs cleaning
	return jurl_geterror(ret);
}
