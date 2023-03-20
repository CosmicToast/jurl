// jurl.c - a curl_easy minimal wrapper for janet

#define JURL_C
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

JANET_CFUN(jurl_new) {
	janet_fixarity(argc, 0);
	jurl_handle *jurl = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl));
	jurl->handle = curl_easy_init();

	// EXAMPLE
	curl_easy_setopt(jurl->handle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);

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
	jurl_handle *jurl = (jurl_handle*)janet_getabstract(argv, 0, &jurl_type);
	curl_easy_reset(jurl->handle);
	return janet_wrap_abstract(jurl);
}

JANET_CFUN(jurl_dup) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getabstract(argv, 0, &jurl_type);
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
	return janet_wrap_integer(curl_global_init(flags));
}

JANET_CFUN(jurl_global_cleanup) {
	curl_global_cleanup();
	return janet_wrap_nil();
}

size_t write_buffer_callback(void *contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	janet_buffer_push_bytes(userp, contents, realsize);
	return realsize;
}

JANET_CFUN(jurl_perform) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getabstract(argv, 0, &jurl_type);

	// receiving data slots
	JanetBuffer *buf = janet_buffer(0);
	JanetTable  *out = janet_table(3);

	curl_easy_setopt(jurl->handle, CURLOPT_WRITEFUNCTION, write_buffer_callback);
	curl_easy_setopt(jurl->handle, CURLOPT_WRITEDATA, (void*)buf);
	CURLcode res = curl_easy_perform(jurl->handle);

	// get results back
	janet_table_put(out, janet_ckeywordv("curlcode"), janet_wrap_integer(res));
	janet_table_put(out, janet_ckeywordv("body"), janet_wrap_buffer(buf));

	// also the status
	long codep = 0;
	if (res == CURLE_OK) res = curl_easy_getinfo(jurl->handle, CURLINFO_RESPONSE_CODE, &codep);
	janet_table_put(out, janet_ckeywordv("status"), janet_wrap_integer(codep));
	return janet_wrap_struct(janet_table_to_struct(out));
}
