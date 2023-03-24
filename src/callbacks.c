// callbacks.c: implements curl_easy_setopt options that take callbacks
#include "jurl.h"

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	JanetFunction *fun = (JanetFunction*)userdata;
	size_t realsize = size * nmemb;

	JanetBuffer *buf = janet_buffer(realsize);
	janet_buffer_push_bytes(buf, (const uint8_t*)ptr, realsize);

	Janet argv[1] = { janet_wrap_buffer(buf), };
	janet_call(fun, 1, argv);

	return realsize;
}

static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
	JanetFunction *fun = (JanetFunction*)userdata;
	size_t realsize = size * nitems;

	Janet argv[1] = { janet_wrap_integer(realsize), };
	Janet res = janet_call(fun, 1, argv);
	if (janet_checktype(res, JANET_NIL)) return 0;
	JanetByteView bytes;
	if (!janet_bytes_view(res, &bytes.bytes, &bytes.len)) {
		janet_panic("could not open bytesview in read_callback");
	}

	size_t actualsize = realsize > bytes.len ? bytes.len : realsize;
	memcpy(buffer, bytes.bytes, actualsize);

	return actualsize;
}

static int progress_callback(void *clientp,
                      curl_off_t dltotal,
                      curl_off_t dlnow,
                      curl_off_t ultotal,
                      curl_off_t ulnow) {
	JanetFunction *fun = (JanetFunction*)clientp;

	Janet argv[4] = { janet_wrap_integer(dltotal),
					  janet_wrap_integer(dlnow),
					  janet_wrap_integer(ultotal),
					  janet_wrap_integer(ulnow), };
	Janet res = janet_call(fun, 4, argv);

	return janet_truthy(res) ? CURL_PROGRESSFUNC_CONTINUE : 1;
}

static size_t header_callback(char *buffer,
                              size_t size,
                              size_t nitems,
                              void *userdata) {
	JanetFunction *fun = (JanetFunction*)userdata;
	size_t realsize = size * nitems;

	Janet argv[1] = { janet_stringv((const uint8_t*)buffer, realsize), };
	janet_call(fun, 1, argv);

	return realsize;
}

static int debug_callback(CURL *handle,
                   curl_infotype type,
                   char *data,
                   size_t size,
                   void *clientp) {
	JanetFunction *fun = (JanetFunction*)clientp;
	char *kw;
	switch (type) {
	case CURLINFO_TEXT:
		kw = "text";
		break;
	case CURLINFO_HEADER_IN:
		kw = "header-in";
		break;
	case CURLINFO_HEADER_OUT:
		kw = "header-out";
		break;
	case CURLINFO_DATA_IN:
		kw = "data-in";
		break;
	case CURLINFO_DATA_OUT:
		kw = "data-out";
		break;
	case CURLINFO_SSL_DATA_IN:
		kw = "ssl-data-in";
		break;
	case CURLINFO_SSL_DATA_OUT:
		kw = "ssl-data-out";
		break;
	case CURLINFO_END:
		kw = "end";
		break;
	default:
		janet_panic("unrecognized curl_infotype in debug_callback");
	}

	Janet argv[3] = { janet_wrap_pointer(handle),
					  janet_ckeywordv(kw),
					  janet_stringv((const uint8_t*)data, size), };
	janet_call(fun, 3, argv);

	return 0;
}

static int fnmatch_callback(void *ptr, const char *pattern, const char *string) {
	JanetFunction *fun = (JanetFunction*)ptr;

	Janet argv[2] = { janet_cstringv(pattern),
					  janet_cstringv(string), };
	Janet res = janet_call(fun, 2, argv);

	if        (janet_keyeq(res, "match")) {
		return CURL_FNMATCHFUNC_MATCH;
	} else if (janet_keyeq(res, "nomatch")) {
		return CURL_FNMATCHFUNC_NOMATCH;
	} else if (janet_keyeq(res, "fail")) {
		return CURL_FNMATCHFUNC_FAIL;
	}

	return CURL_FNMATCHFUNC_FAIL;
}

static int prereq_callback(void *clientp,
                           char *conn_primary_ip,
                           char *conn_local_ip,
                           int conn_primary_port,
                           int conn_local_port) {
	JanetFunction *fun = (JanetFunction*)clientp;

	Janet argv[4] = { janet_cstringv(conn_primary_ip),
					  janet_cstringv(conn_local_ip),
					  janet_wrap_integer(conn_primary_port),
					  janet_wrap_integer(conn_local_port), };
	Janet res = janet_call(fun, 4, argv);

	return janet_truthy(res);
}

CURLcode jurl_setcallback(jurl_handle *jurl, CURLoption opt, JanetFunction *fun) {
	CURLcode res = 0; // we |= it so just check for != CURLE_OK
	switch (opt) {
		case CURLOPT_WRITEFUNCTION: // buffer -> void
			res |= curl_easy_setopt(jurl->handle, CURLOPT_WRITEDATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_WRITEFUNCTION, write_callback);
			break;
		case CURLOPT_READFUNCTION: // bytes -> buffer | string | nil
			res |= curl_easy_setopt(jurl->handle, CURLOPT_READDATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_READFUNCTION, read_callback);
			break;
		case CURLOPT_XFERINFOFUNCTION: // dltotal, dlnow, ultotal, ulnow -> boolean
			res |= curl_easy_setopt(jurl->handle, CURLOPT_XFERINFODATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
			break;
		case CURLOPT_HEADERFUNCTION: // string -> void
			res |= curl_easy_setopt(jurl->handle, CURLOPT_HEADERDATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_HEADERFUNCTION, header_callback);
			break;
		case CURLOPT_DEBUGFUNCTION:
			// curl*,
			// :text | :header-in | :header-out | :data-in | :data-out |
			// :ssl-data-in | :ssl-data-out | :end,
			// string (non-cstring) -> void (0)
			res |= curl_easy_setopt(jurl->handle, CURLOPT_DEBUGDATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_DEBUGFUNCTION, debug_callback);
			break;
		case CURLOPT_CHUNK_DATA:
			janet_panic("chunk callbacks not implemented");
			break;
		case CURLOPT_FNMATCH_FUNCTION:
			// input, pattern -> :match | :nomatch | :fail
			res |= curl_easy_setopt(jurl->handle, CURLOPT_FNMATCH_DATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_FNMATCH_FUNCTION, fnmatch_callback);
			break;
		case CURLOPT_PREREQFUNCTION:
			// remote ip, local ip, remote port, local port -> boolean
			res |= curl_easy_setopt(jurl->handle, CURLOPT_PREREQDATA, fun);
			res |= curl_easy_setopt(jurl->handle, CURLOPT_PREREQFUNCTION, prereq_callback);
			break;
		default:
			janet_panic("unknown callback option in jurl_setcallback");
	}
	return res;
}
