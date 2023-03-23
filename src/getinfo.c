#include "jurl.h"

enum jurl_paramtype {
	JURL_PARAMTYPE_STRING,
	JURL_PARAMTYPE_LONG,
	JURL_PARAMTYPE_ENUM,
	JURL_PARAMTYPE_OFF_T,
	JURL_PARAMTYPE_SLIST,
	JURL_PARAMTYPE_BITMASK,
	JURL_PARAMTYPE_BOOLEAN,
};

struct jurl_opt {
	CURLINFO info;
	const char *keyword;
	enum jurl_paramtype type;
};

static const struct jurl_opt jurl_opts[] = {
	{CURLINFO_EFFECTIVE_METHOD,          "effective-method",        JURL_PARAMTYPE_STRING},
	{CURLINFO_EFFECTIVE_URL,             "effective-url",           JURL_PARAMTYPE_STRING},
	{CURLINFO_RESPONSE_CODE,             "response-code",           JURL_PARAMTYPE_LONG},
	{CURLINFO_REFERER,                   "referer",                 JURL_PARAMTYPE_STRING},
	{CURLINFO_HTTP_CONNECTCODE,          "http-connectcode",        JURL_PARAMTYPE_LONG},
	{CURLINFO_HTTP_VERSION,              "http-version",            JURL_PARAMTYPE_ENUM},
	{CURLINFO_FILETIME,                  "filetime",                JURL_PARAMTYPE_LONG},
	{CURLINFO_FILETIME_T,                "filetime-t",              JURL_PARAMTYPE_OFF_T},
	{CURLINFO_TOTAL_TIME,                "total-time",              JURL_PARAMTYPE_LONG},
	{CURLINFO_TOTAL_TIME_T,              "total-time-t",            JURL_PARAMTYPE_OFF_T},
	{CURLINFO_NAMELOOKUP_TIME,           "namelookup-time",         JURL_PARAMTYPE_LONG},
	{CURLINFO_NAMELOOKUP_TIME_T,         "namelookup-time-t",       JURL_PARAMTYPE_OFF_T},
	{CURLINFO_CONNECT_TIME,              "connect-time",            JURL_PARAMTYPE_LONG},
	{CURLINFO_CONNECT_TIME_T,            "connect-time-t",          JURL_PARAMTYPE_OFF_T},
	{CURLINFO_APPCONNECT_TIME,           "appconnect-time",         JURL_PARAMTYPE_LONG},
	{CURLINFO_APPCONNECT_TIME_T,         "appconnect-time-t",       JURL_PARAMTYPE_OFF_T},
	{CURLINFO_PRETRANSFER_TIME,          "pretransfer-time",        JURL_PARAMTYPE_LONG},
	{CURLINFO_PRETRANSFER_TIME_T,        "pretransfer-time-t",      JURL_PARAMTYPE_OFF_T},
	{CURLINFO_STARTTRANSFER_TIME,        "starttransfer-time",      JURL_PARAMTYPE_LONG},
	{CURLINFO_STARTTRANSFER_TIME_T,      "starttransfer-time-t",    JURL_PARAMTYPE_OFF_T},
	{CURLINFO_REDIRECT_TIME,             "redirect-time",           JURL_PARAMTYPE_LONG},
	{CURLINFO_REDIRECT_TIME_T,           "redirect-time-t",         JURL_PARAMTYPE_OFF_T},
	{CURLINFO_REDIRECT_COUNT,            "redirect-count",          JURL_PARAMTYPE_LONG},
	{CURLINFO_REDIRECT_URL,              "redirect-url",            JURL_PARAMTYPE_STRING},
	// SKIP: size-upload: deprecated for size-upload-t
	{CURLINFO_SIZE_UPLOAD_T,             "size-upload",             JURL_PARAMTYPE_OFF_T},
	// SKIP: size-download: deprecated for size-download-t
	{CURLINFO_SIZE_DOWNLOAD_T,           "size-download",           JURL_PARAMTYPE_OFF_T},
	// SKIP: speed-download: deprecated for speed-download-t
	{CURLINFO_SPEED_DOWNLOAD_T,          "speed-download",          JURL_PARAMTYPE_OFF_T},
	// SKIP: speed-upload: deprecated for speed-upload-t
	{CURLINFO_SPEED_UPLOAD_T,            "speed-upload",            JURL_PARAMTYPE_OFF_T},
	{CURLINFO_HEADER_SIZE,               "header-size",             JURL_PARAMTYPE_LONG},
	{CURLINFO_REQUEST_SIZE,              "request-size",            JURL_PARAMTYPE_LONG},
	{CURLINFO_SSL_VERIFYRESULT,          "ssl-verifyresult",        JURL_PARAMTYPE_BOOLEAN},
	{CURLINFO_PROXY_ERROR,               "proxy-error",             JURL_PARAMTYPE_ENUM},
	{CURLINFO_PROXY_SSL_VERIFYRESULT,    "proxy-ssl-verifyresult",  JURL_PARAMTYPE_BOOLEAN},
	{CURLINFO_SSL_ENGINES,               "ssl-engines",             JURL_PARAMTYPE_SLIST},
	// SKIP: content-length-download: deprecated for content-length-download-t
	{CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, "content-length-download", JURL_PARAMTYPE_OFF_T},
	// SKIP: content-length-upload: deprecated for content-length-upload-t
	{CURLINFO_CONTENT_LENGTH_UPLOAD_T,   "content-length-upload",   JURL_PARAMTYPE_OFF_T},
	{CURLINFO_CONTENT_TYPE,              "content-type",            JURL_PARAMTYPE_STRING},
	{CURLINFO_RETRY_AFTER,               "retry-after",             JURL_PARAMTYPE_OFF_T},
	// SKIP: private: complex representation
	{CURLINFO_HTTPAUTH_AVAIL,            "httpauth-avail",          JURL_PARAMTYPE_BITMASK},
	{CURLINFO_PROXYAUTH_AVAIL,           "proxyauth-avail",         JURL_PARAMTYPE_BITMASK},
	{CURLINFO_OS_ERRNO,                  "os-errno",                JURL_PARAMTYPE_LONG},
	{CURLINFO_NUM_CONNECTS,              "num-connects",            JURL_PARAMTYPE_LONG},
	{CURLINFO_PRIMARY_IP,                "primary-ip",              JURL_PARAMTYPE_STRING},
	{CURLINFO_PRIMARY_PORT,              "primary-port",            JURL_PARAMTYPE_LONG},
	{CURLINFO_LOCAL_IP,                  "local-ip",                JURL_PARAMTYPE_STRING},
	{CURLINFO_LOCAL_PORT,                "local-port",              JURL_PARAMTYPE_LONG},
	{CURLINFO_COOKIELIST,                "cookielist",              JURL_PARAMTYPE_SLIST},
	// SKIP: lastsocket: deprecated for activesocket
	// SKIP: activesocket: complex representation
	{CURLINFO_FTP_ENTRY_PATH,            "ftp-entry-path",          JURL_PARAMTYPE_STRING},
	{CURLINFO_CAPATH,                    "capath",                  JURL_PARAMTYPE_STRING},
	{CURLINFO_CAINFO,                    "cainfo",                  JURL_PARAMTYPE_STRING},
	// SKIP: certinfo: complex representation
	// SKIP: tls-ssl-ptr: complex representation
	// SKIP: tls-session: deprecated for tls-ssl-ptr
	{CURLINFO_CONDITION_UNMET,           "condition-unmet",         JURL_PARAMTYPE_BOOLEAN},
	{CURLINFO_RTSP_SESSION_ID,           "rtsp-session-id",         JURL_PARAMTYPE_STRING},
	{CURLINFO_RTSP_CLIENT_CSEQ,          "rtsp-client-cseq",        JURL_PARAMTYPE_LONG},
	{CURLINFO_RTSP_SERVER_CSEQ,          "rtsp-server-cseq",        JURL_PARAMTYPE_LONG},
	{CURLINFO_RTSP_CSEQ_RECV,            "rtsp-cseq-recv",          JURL_PARAMTYPE_LONG},
	// SKIP: protocol: deprecated for scheme
	{CURLINFO_SCHEME,                    "scheme",                  JURL_PARAMTYPE_STRING},
};

JANET_CFUN(jurl_getinfo) {
	janet_fixarity(argc, 2);
	jurl_handle *jurl = janet_getjurl(argv, 0);
	CURL* curl = jurl->handle;

	const struct jurl_opt *opt;
	Janet jopt = argv[1];
	for (size_t i = 0; i < sizeof(jurl_opts) / sizeof(struct jurl_opt); i++) {
		if(janet_keyeq(jopt, jurl_opts[i].keyword)) {
			opt = &jurl_opts[i];
			break;
		}
	}
	if (!opt) {
		int num = janet_getinteger(argv, 1);
		for (size_t i = 0; i < sizeof(jurl_opts) / sizeof(struct jurl_opt); i++) {
			if (num == jurl_opts[i].info) {
				opt = &jurl_opts[i];
				break;
			}
		}
	}
	if (!opt) {
		janet_panic("could not find option to set in jurl_getinfo");
	}

	switch(opt->type) {
		case JURL_PARAMTYPE_STRING: {
			char *out;
			CURLcode res = curl_easy_getinfo(curl, opt->info, &out);
			if (res != CURLE_OK) janet_panic("getinfo returned != CURLE_OK");
			return janet_cstringv(out);
		}
		case JURL_PARAMTYPE_LONG: {
			long out;
			CURLcode res = curl_easy_getinfo(curl, opt->info, &out);
			if (res != CURLE_OK) janet_panic("getinfo returned != CURLE_OK");
			return janet_wrap_integer(out);
		}
		case JURL_PARAMTYPE_ENUM:
			janet_panic("jurl_getinfo: enums not implemented");
		case JURL_PARAMTYPE_OFF_T: {
			curl_off_t out;
			CURLcode res = curl_easy_getinfo(curl, opt->info, &out);
			if (res != CURLE_OK) janet_panic("getinfo returned != CURLE_OK");
			return janet_wrap_integer(out);
		}
		case JURL_PARAMTYPE_SLIST:
			janet_panic("jurl_getinfo: slists not implemented");
		case JURL_PARAMTYPE_BITMASK:
			janet_panic("jurl_getinfo: bitmasks not implemented");
		case JURL_PARAMTYPE_BOOLEAN: {
			long out;
			CURLcode res = curl_easy_getinfo(curl, opt->info, &out);
			if (res != CURLE_OK) janet_panic("getinfo returned != CURLE_OK");
			return janet_wrap_boolean(out);
		}
		default:
			janet_panic("jurl_getinfo: unrecognized output type");
	}

	return janet_wrap_nil(); // unreachable
}
