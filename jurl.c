// jurl.c - a curl_easy minimal wrapper for janet
#include "jurl.h"

int jurl_gc(void *p, size_t s) {
	(void) s;
	jurl_handle *jurl = (jurl_handle*)p;
	if (jurl->handle) curl_easy_cleanup(jurl->handle);
	while (jurl->cleanup) {
		struct jurl_cleanup *cur = jurl->cleanup;
		switch (cur->type) {
		case JURL_CLEANUP_TYPE_SLIST:
			curl_slist_free_all(cur->slist);
			break;
		default:
			janet_panic("unknown type of cleanup data in jurl_gc");
		}
		jurl->cleanup = cur->next;
		free(cur);
	}
	return 0;
}

const JanetAbstractType jurl_type = {
	"jurl",       // name
	jurl_gc,      // gc
	JANET_ATEND_GC
};

jurl_handle *janet_getjurl(Janet *argv, int32_t n) {
	return (jurl_handle*)janet_getabstract(argv, n, &jurl_type);
}

// this function plays fast and loose and I hope the gc doesn't kill me for it
// anyway, it goes like so:
// (wrap-error :ok  something) -> [:ok something]
// (wrap-error :err something) -> [:err "explanation of :err"]
JANET_CFUN(jurl_wrap_error) {
	janet_fixarity(argc, 2);
	CURLcode code;
	if (janet_checktype(argv[0], JANET_NUMBER)) {
		argv[0] = jurl_geterror(janet_getinteger(argv, 0));
	}
	if (!janet_checktype(argv[0], JANET_KEYWORD)) {
		janet_panicf("jurl_wrap_error: expected number or keyword, got %T", janet_type(argv[0]));
	}
	if (!janet_keyeq(argv[0], "ok")) {
		// will this break one day?
		// find out next time!
		argv[1] = jurl_strerror(1, argv);
	}
	return janet_wrap_tuple(janet_tuple_n(argv, 2));
}

JANET_CFUN(jurl_new) {
	janet_fixarity(argc, 0);
	jurl_handle *jurl = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl));
	jurl->handle = curl_easy_init();
	return janet_wrap_abstract(jurl);
}

struct jurl_cleanup *register_cleanup(jurl_handle *jurl, enum jurl_cleanup_type type) {
	struct jurl_cleanup *out = malloc(sizeof(struct jurl_cleanup));
	out->next = jurl->cleanup;
	jurl->cleanup = out;
	out->type = type;
	return out;
}

JANET_CFUN(jurl_reset) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = janet_getjurl(argv, 0);
	curl_easy_reset(jurl->handle);
	return janet_wrap_abstract(jurl);
}

JANET_CFUN(jurl_dup) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = janet_getjurl(argv, 0);
	jurl_handle *newj = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl));
	newj->handle = curl_easy_duphandle(jurl->handle);
	return janet_wrap_abstract(newj);
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

JANET_CFUN(jurl_perform) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getjurl(argv, 0);
	return jurl_geterror(curl_easy_perform(jurl->handle));
}
