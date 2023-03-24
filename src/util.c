#include "jurl.h"

int janet_getslist(struct curl_slist **slist, Janet *argv, int32_t n) {
	JanetView args = janet_getindexed(argv, n);
	for (int32_t i = 0; i< args.len; i++) {
		const char *s = janet_getcstring(args.items, i);
		struct curl_slist *newlist = curl_slist_append(*slist, s);
		if (!newlist) {
			return 0;
		}
		*slist = newlist;
	}
	return 1;
}

JANET_CFUN(jurl_escape) {
	janet_fixarity(argc, 1);
	JanetByteView b = janet_getbytes(argv, 0);
	CURL* curl;
#if !CURL_AT_LEAST_VERSION(7,82,0)
	curl = curl_easy_init();
#endif

	char *s = curl_easy_escape(curl, (const char*)b.bytes, b.len);
	Janet out = janet_cstringv(s);

	curl_free(s);
#if !CURL_AT_LEAST_VERSION(7,82,0)
	curl_easy_cleanup(curl);
#endif
	return out;
}

JANET_CFUN(jurl_unescape) {
	janet_fixarity(argc, 1);
	JanetByteView b = janet_getbytes(argv, 0);
	CURL *curl;
#if !CURL_AT_LEAST_VERSION(7,82,0)
	curl = curl_easy_init();
#endif

	int len;
	char *s = curl_easy_unescape(curl, (const char*)b.bytes, b.len, &len);
	Janet out = janet_stringv((const uint8_t*)s, len);

	curl_free(s);
#if !CURL_AT_LEAST_VERSION(7,82,0)
	curl_easy_cleanup(curl);
#endif
	return out;
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
