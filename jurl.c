// jurl.c - a curl_easy minimal wrapper for janet

#define JURL_C
#include "jurl.h"

int jurl_gc(void *p, size_t s) {
	(void) s;
	jurl_handle *jurl = (jurl_handle*)p;
	if (jurl->handle) curl_easy_cleanup(jurl->handle);
	return 0;
}

const JanetAbstractType jurl_type = {
	"jurl",       // name
	jurl_gc,      // gc
	JANET_ATEND_GC
};

Janet jurl_new(int32_t argc, Janet *argv) {
	janet_fixarity(argc, 0);
	jurl_handle *jurl = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl));
	jurl->handle = curl_easy_init();

	// EXAMPLE
	curl_easy_setopt(jurl->handle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);

	return janet_wrap_abstract(jurl);
}

Janet jurl_reset(int32_t argc, Janet *argv) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getabstract(argv, 0, &jurl_type);
	curl_easy_reset(jurl->handle);
	return janet_wrap_abstract(jurl);
}

Janet jurl_dup(int32_t argc, Janet *argv) {
	janet_fixarity(argc, 1);
	jurl_handle *jurl = (jurl_handle*)janet_getabstract(argv, 0, &jurl_type);
	jurl_handle *newj = (jurl_handle*)janet_abstract(&jurl_type, sizeof(jurl));
	newj->handle = curl_easy_duphandle(jurl->handle);
	return janet_wrap_abstract(newj);
}

size_t write_buffer_callback(void *contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	janet_buffer_push_bytes(userp, contents, realsize);
	return realsize;
}

Janet jurl_perform(int32_t argc, Janet *argv) {
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
