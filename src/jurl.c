// jurl.c - a curl_easy minimal wrapper for janet
#include "jurl.h"

static JanetMethod jurl_methods[] = {
	{"reset",   jurl_reset},
	{"dup",     jurl_dup},
	{"perform", jurl_perform},
	{"getinfo", jurl_getinfo},
	{"setopt",  jurl_setopt},
	{NULL,      NULL},
};

static int jurl_gc(void *p, size_t s) {
	(void) s;
	jurl_handle *jurl = (jurl_handle*)p;
	if (jurl->handle) curl_easy_cleanup(jurl->handle);
	jurl_do_cleanup(&jurl->cleanup);
	return 0;
}

// methods + getinfo
static int jurl_get(void *p, Janet key, Janet *out) {
	jurl_handle *jurl = (jurl_handle*)p;
	if (!janet_checktype(key, JANET_KEYWORD)) {
		return 0;
	}

	// is it a method?
	if (janet_getmethod(janet_unwrap_keyword(key), jurl_methods, out)) {
		return 1;
	}

	// try to do a getinfo
	Janet argv[2] = { janet_wrap_abstract(jurl),
					  key, };
	*out = jurl_getinfo(2, argv);
	return 1;
}

// setopt
static void jurl_put(void *data, Janet key, Janet value) {
	jurl_handle *jurl = (jurl_handle*)data;
	Janet argv[3] = { janet_wrap_abstract(jurl),
					  key, value, };
	Janet out = jurl_setopt(3, argv);
	if (!janet_keyeq(out, "ok")) {
		janet_panic("jurl_put failed");
	}
}

static const JanetAbstractType jurl_type = {
	"jurl",       // name
	jurl_gc,      // gc
	NULL,		  // gcmark
	jurl_get,     // get
	jurl_put,	  // put
	JANET_ATEND_PUT
};

JANET_CFUN(jurl_dup) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = janet_getjurl(argv, 0);
	jurl_handle *newj = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl_handle));
	newj->handle = curl_easy_duphandle(jurl->handle);
	return janet_wrap_abstract(newj);
}

jurl_handle *janet_getjurl(Janet *argv, int32_t n) {
	return (jurl_handle*)janet_getabstract(argv, n, &jurl_type);
}

JANET_CFUN(jurl_global_init) {
	long flags;
	if (argc == 0) {
		flags = CURL_GLOBAL_ALL;
		goto global_init_ret;
	}
	for (size_t i = 0; i < argc; i++) {
		Janet kw = argv[i];
		if(!janet_checktype(kw, JANET_KEYWORD)) {
			janet_panicf(
					"jurl_global_init: received a non-keyword argument of type %T: %v",
					janet_type(kw), kw);
		}
		if (janet_keyeq(kw, "all")) {
			flags |= CURL_GLOBAL_ALL;
		} else if (janet_keyeq(kw, "ssl")) {
			flags |= CURL_GLOBAL_SSL;
		} else if (janet_keyeq(kw, "win32")) {
			flags |= CURL_GLOBAL_WIN32;
		} else if (janet_keyeq(kw, "nothing")) {
			flags |= CURL_GLOBAL_NOTHING;
		} else if (janet_keyeq(kw, "ack-eintr")) {
			flags |= CURL_GLOBAL_ACK_EINTR;
		} else {
			janet_panicf("jurl_global_init: unknown keyword %v", kw);
		}
	}
global_init_ret:
	return jurl_geterror(curl_global_init(flags));
}

JANET_CFUN(jurl_global_cleanup) {
	curl_global_cleanup();
	return janet_wrap_nil();
}

JANET_CFUN(jurl_new) {
	janet_fixarity(argc, 0);
	jurl_handle *jurl = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl_handle));
	jurl->handle = curl_easy_init();
	jurl->cleanup = NULL;
	return janet_wrap_abstract(jurl);
}

JANET_CFUN(jurl_perform) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getjurl(argv, 0);
	return jurl_geterror(curl_easy_perform(jurl->handle));
}

JANET_CFUN(jurl_reset) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = janet_getjurl(argv, 0);
	curl_easy_reset(jurl->handle);
	return janet_wrap_abstract(jurl);
}

