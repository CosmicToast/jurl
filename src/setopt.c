#include "jurl.h"

enum jurl_paramtype {
	JURL_PARAMTYPE_BOOLEAN,
	JURL_PARAMTYPE_STRING,
	JURL_PARAMTYPE_SLIST,
	JURL_PARAMTYPE_FILE,
	JURL_PARAMTYPE_LONG,
	JURL_PARAMTYPE_ENUM,
	JURL_PARAMTYPE_OFF_T,
	JURL_PARAMTYPE_CALLBACK,
};
struct jurl_opt {
	CURLoption opt;
	const char *keyword;
	enum jurl_paramtype type;
};

static const struct jurl_opt jurl_opts[] = {
	// * behavior options
	{CURLOPT_VERBOSE,       "verbose",       JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_HEADER,        "header",        JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_NOPROGRESS,    "noprogress",    JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_NOSIGNAL,      "nosignal",      JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_WILDCARDMATCH, "wildcardmatch", JURL_PARAMTYPE_BOOLEAN},

	// * callback options
	// we actually pass the function as the data, but expose "writefunction"
	{CURLOPT_WRITEFUNCTION,            "writefunction",            JURL_PARAMTYPE_CALLBACK},
	// SKIP: writedata: see above
	{CURLOPT_READFUNCTION,             "readfunction",             JURL_PARAMTYPE_CALLBACK},
	// SKIP: readdata: see above
	// SKIP: ioctlfunction and ioctldata: janet ioctl capabilities are strictly lesser than libcurl; also deprecated
	// SKIP: seekfunction and seekdata: seeking makes no sense here
	// SKIP: sockoptfunction and sockoptdata: janet doesn't really interact with socket options directly
	// SKIP: opensocketfunction and opensocketdata: I'm not sure janet sockets can be passed through nor much of a point given TLS
	// SKIP: closesocketfunction and closesocketdata: see previous
	// SKIP: progressfunction and progressdata: obsoleted by xferinfofunction
	{CURLOPT_XFERINFOFUNCTION,         "xferinfofunction",         JURL_PARAMTYPE_CALLBACK},
	// SKIP: xferinfodata: see above
	{CURLOPT_HEADERFUNCTION,           "headerfunction",           JURL_PARAMTYPE_CALLBACK},
	// SKIP: headerdata: see above
	{CURLOPT_DEBUGFUNCTION,            "debugfunction",            JURL_PARAMTYPE_CALLBACK},
	// SKIP: debugdata: see above
	// SKIP: ssl-ctx-function and ssl-ctx-data: I'm not sure how this could possibly be used
	// SKIP: conv-to-network-function; conv-from-network-function and conv-from-utf8-function: we can't pass a janet function to these; also obsolete
	// SKIP: interleavefunction and interleave-data: I don't understand what this is tbh
	// this one is a little bit complex; we only set BGN because END seemingly serves no purpose and we can only set one function for both
	// this may change later if there's demand; though I doubt so
	{CURLOPT_CHUNK_DATA,               "chunk-function",           JURL_PARAMTYPE_CALLBACK},
	// SKIP: chunk-bgn-function and chunk-end-function: see previous
	{CURLOPT_FNMATCH_FUNCTION,         "fnmatch-function",         JURL_PARAMTYPE_CALLBACK},
	// SKIP: fnmatch-data: see above
	{CURLOPT_SUPPRESS_CONNECT_HEADERS, "suppress-connect-headers", JURL_PARAMTYPE_BOOLEAN},
	// SKIP: resolver-start-function and resolver-start-data: not useful
	{CURLOPT_PREREQFUNCTION,           "prereqfunction",           JURL_PARAMTYPE_CALLBACK},
	// SKIP: prereqdata: see above

	// * error options
	// SKIP: errorbuffer: complex
	{CURLOPT_STDERR,                "stderr",                JURL_PARAMTYPE_FILE},
	{CURLOPT_FAILONERROR,           "failonerror",           JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_KEEP_SENDING_ON_ERROR, "keep-sending-on-error", JURL_PARAMTYPE_BOOLEAN},

	// * network options
	{CURLOPT_URL,                  "url",                  JURL_PARAMTYPE_STRING},
	{CURLOPT_PATH_AS_IS,           "path-as-is",           JURL_PARAMTYPE_STRING},
	// SKIP: protocols: deprecated for protocols-str
	{CURLOPT_PROTOCOLS_STR,        "protocols",            JURL_PARAMTYPE_STRING},
	// SKIP: redir-protocols: deprecated for redir-protocols-str
	{CURLOPT_REDIR_PROTOCOLS_STR,  "redir-protocols",      JURL_PARAMTYPE_STRING},
	{CURLOPT_DEFAULT_PROTOCOL,     "default-protocol",     JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXY,                "proxy",                JURL_PARAMTYPE_STRING},
	{CURLOPT_PRE_PROXY,            "pre-proxy",            JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXYPORT,            "proxyport",            JURL_PARAMTYPE_LONG},
	{CURLOPT_PROXYTYPE,            "proxytype",            JURL_PARAMTYPE_ENUM},
	{CURLOPT_NOPROXY,              "noproxy",              JURL_PARAMTYPE_STRING},
	{CURLOPT_HTTPPROXYTUNNEL,      "httpproxytunnel",      JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_CONNECT_TO,           "connect-to",           JURL_PARAMTYPE_SLIST},
	{CURLOPT_SOCKS5_AUTH,          "socks5-auth",          JURL_PARAMTYPE_ENUM},
	// SKIP: socks5-gssapi-service: deprecated for service-name
	{CURLOPT_SOCKS5_GSSAPI_NEC,    "socsk5-gssapi-nec",    JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_PROXY_SERVICE_NAME,   "proxy-service-name",   JURL_PARAMTYPE_STRING},
	{CURLOPT_HAPROXYPROTOCOL,      "haproxyprotocol",      JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_SERVICE_NAME,         "service-name",         JURL_PARAMTYPE_STRING},
	{CURLOPT_INTERFACE,            "interface",            JURL_PARAMTYPE_STRING},
	{CURLOPT_LOCALPORT,            "localport",            JURL_PARAMTYPE_LONG},
	{CURLOPT_LOCALPORTRANGE,       "localportrange",       JURL_PARAMTYPE_LONG},
	{CURLOPT_DNS_CACHE_TIMEOUT,    "dns-cache-timeout",    JURL_PARAMTYPE_LONG},
	// SKIP: dns-use-global-cache: deprecated in favor of using a share interface
	{CURLOPT_DOH_URL,              "doh-url",              JURL_PARAMTYPE_STRING},
	{CURLOPT_BUFFERSIZE,           "buffersize",           JURL_PARAMTYPE_LONG},
	{CURLOPT_PORT,                 "port",                 JURL_PARAMTYPE_LONG},
	{CURLOPT_TCP_FASTOPEN,         "tcp-fastopen",         JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_TCP_NODELAY,          "tcp-nodelay",          JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_ADDRESS_SCOPE,        "address-scope",        JURL_PARAMTYPE_LONG},
	{CURLOPT_TCP_KEEPALIVE,        "tcp-keepalive",        JURL_PARAMTYPE_LONG},
	{CURLOPT_TCP_KEEPIDLE,         "tcp-keepidle",         JURL_PARAMTYPE_LONG},
	{CURLOPT_TCP_KEEPINTVL,        "tcp-keepintvl",        JURL_PARAMTYPE_LONG},
	{CURLOPT_UNIX_SOCKET_PATH,     "unix-socket-path",     JURL_PARAMTYPE_STRING},
	{CURLOPT_ABSTRACT_UNIX_SOCKET, "abstract-unix-socket", JURL_PARAMTYPE_STRING},

	// * names and passwords options
	{CURLOPT_NETRC,                    "netrc",                    JURL_PARAMTYPE_ENUM},
	{CURLOPT_NETRC_FILE,               "netrc-file",               JURL_PARAMTYPE_STRING},
	{CURLOPT_USERPWD,                  "userpwd",                  JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXYUSERPWD,             "proxyuserpwd",             JURL_PARAMTYPE_STRING},
	{CURLOPT_USERNAME,                 "username",                 JURL_PARAMTYPE_STRING},
	{CURLOPT_PASSWORD,                 "password",                 JURL_PARAMTYPE_STRING},
	{CURLOPT_LOGIN_OPTIONS,            "login-options",            JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXYUSERNAME,            "proxyusername",            JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXYPASSWORD,            "proxypassword",            JURL_PARAMTYPE_STRING},
	{CURLOPT_HTTPAUTH,                 "httpauth",                 JURL_PARAMTYPE_ENUM},
	{CURLOPT_TLSAUTH_USERNAME,         "tlsauth-username",         JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXY_TLSAUTH_USERNAME,   "proxy-tlsauth-username",   JURL_PARAMTYPE_STRING},
	{CURLOPT_TLSAUTH_PASSWORD,         "tlsauth-password",         JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXY_TLSAUTH_PASSWORD,   "proxy-tlsauth-password",   JURL_PARAMTYPE_STRING},
	{CURLOPT_TLSAUTH_TYPE,             "tlsauth-type",             JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXY_TLSAUTH_TYPE,       "proxy-tlsauth-type",       JURL_PARAMTYPE_STRING},
	{CURLOPT_PROXYAUTH,                "proxyauth",                JURL_PARAMTYPE_ENUM},
	{CURLOPT_SASL_AUTHZID,             "sasl-authzid",             JURL_PARAMTYPE_STRING},
	{CURLOPT_SASL_IR,                  "sasl-ir",                  JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_XOAUTH2_BEARER,           "xoauth2-bearer",           JURL_PARAMTYPE_STRING},
	{CURLOPT_DISALLOW_USERNAME_IN_URL, "disallow-username-in-url", JURL_PARAMTYPE_BOOLEAN},

	// * http options
	{CURLOPT_AUTOREFERER,            "autoreferer",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_ACCEPT_ENCODING,        "accept-encoding",        JURL_PARAMTYPE_STRING},
	{CURLOPT_TRANSFER_ENCODING,      "transfer-encoding",      JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FOLLOWLOCATION,         "followlocation",         JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_UNRESTRICTED_AUTH,      "unrestricted-auth",      JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_MAXREDIRS,              "maxredirs",              JURL_PARAMTYPE_LONG},
	{CURLOPT_POSTREDIR,              "postredir",              JURL_PARAMTYPE_ENUM},
	// SKIP: put: deprecated for upload
	{CURLOPT_POST,                   "post",                   JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_POSTFIELDS,             "postfields",             JURL_PARAMTYPE_STRING},
	{CURLOPT_POSTFIELDSIZE,          "postfieldsize",          JURL_PARAMTYPE_LONG},
	{CURLOPT_POSTFIELDSIZE_LARGE,    "postfieldsize-large",    JURL_PARAMTYPE_OFF_T},
	{CURLOPT_COPYPOSTFIELDS,         "copypostfields",         JURL_PARAMTYPE_STRING},
	// SKIP: httppost: deprecated for mimepost
	{CURLOPT_REFERER,                "referer",                JURL_PARAMTYPE_STRING},
	{CURLOPT_USERAGENT,              "useragent",              JURL_PARAMTYPE_STRING},
	{CURLOPT_HTTPHEADER,             "httpheader",             JURL_PARAMTYPE_SLIST},
	{CURLOPT_HEADEROPT,              "headeropt",              JURL_PARAMTYPE_ENUM},
	{CURLOPT_PROXYHEADER,            "proxyheader",            JURL_PARAMTYPE_SLIST},
	{CURLOPT_HTTP200ALIASES,         "http200aliases",         JURL_PARAMTYPE_SLIST},
	{CURLOPT_COOKIE,                 "cookie",                 JURL_PARAMTYPE_STRING},
	{CURLOPT_COOKIEFILE,             "cookiefile",             JURL_PARAMTYPE_FILE},
	{CURLOPT_COOKIEJAR,              "cookiejar",              JURL_PARAMTYPE_FILE},
	{CURLOPT_COOKIESESSION,          "cookiesession",          JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_COOKIELIST,             "cookielist",             JURL_PARAMTYPE_STRING},
	{CURLOPT_ALTSVC,                 "altsvc",                 JURL_PARAMTYPE_STRING},
	{CURLOPT_ALTSVC_CTRL,            "altsvc-ctrl",            JURL_PARAMTYPE_ENUM},
	{CURLOPT_HSTS,                   "hsts",                   JURL_PARAMTYPE_STRING},
	{CURLOPT_HSTS_CTRL,              "hsts-ctrl",              JURL_PARAMTYPE_ENUM},
	// SKIP: hstsreadfunction: callback
	// SKIP: hstsreaddata: callback
	// SKIP: hstswritefunction: callback
	// SKIP: hstswritedata: callback
	{CURLOPT_HTTPGET,                "httpget",                JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_REQUEST_TARGET,         "request-target",         JURL_PARAMTYPE_STRING},
	{CURLOPT_HTTP_VERSION,           "http-version",           JURL_PARAMTYPE_ENUM},
	{CURLOPT_HTTP09_ALLOWED,         "http09-allowed",         JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_IGNORE_CONTENT_LENGTH,  "ignore-content-length",  JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_HTTP_CONTENT_DECODING,  "http-content-decoding",  JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_HTTP_TRANSFER_DECODING, "http-transfer-decoding", JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_EXPECT_100_TIMEOUT_MS,  "expect-100-timeout-ms",  JURL_PARAMTYPE_LONG},
	// SKIP: trailerfunction: callback
	// SKIP: trailerdata: callback
	{CURLOPT_PIPEWAIT,               "pipewait",               JURL_PARAMTYPE_BOOLEAN},
	// SKIP: stream-depends: not implementing this weird thing
	// SKIP: stream-depends-e: ditto
	{CURLOPT_STREAM_WEIGHT,          "stream-weight",          JURL_PARAMTYPE_LONG},

	// * smtp options
	{CURLOPT_MAIL_FROM,             "mail-from",            JURL_PARAMTYPE_STRING},
	{CURLOPT_MAIL_RCPT,             "mail-rcpt",            JURL_PARAMTYPE_SLIST},
	{CURLOPT_MAIL_AUTH,             "mail-auth",            JURL_PARAMTYPE_STRING},
	// curl has a typo here. I remove the typo from the keyword
	{CURLOPT_MAIL_RCPT_ALLLOWFAILS, "mail-rcpt-allowfails", JURL_PARAMTYPE_BOOLEAN},

	// * tftp options
	{CURLOPT_TFTP_BLKSIZE,    "tftp-blksize",    JURL_PARAMTYPE_LONG},
	{CURLOPT_TFTP_NO_OPTIONS, "tftp-no-options", JURL_PARAMTYPE_BOOLEAN},

	// * ftp options
	{CURLOPT_FTPPORT,                 "ftpport",                 JURL_PARAMTYPE_STRING},
	{CURLOPT_QUOTE,                   "quote",                   JURL_PARAMTYPE_SLIST},
	{CURLOPT_POSTQUOTE,               "postquote",               JURL_PARAMTYPE_SLIST},
	{CURLOPT_PREQUOTE,                "prequote",                JURL_PARAMTYPE_SLIST},
	{CURLOPT_APPEND,                  "append",                  JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FTP_USE_EPRT,            "ftp-use-eprt",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FTP_USE_EPSV,            "ftp-use-epsv",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FTP_USE_PRET,            "ftp-use-pret",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FTP_CREATE_MISSING_DIRS, "ftp-create-missing-dirs", JURL_PARAMTYPE_ENUM},
	{CURLOPT_SERVER_RESPONSE_TIMEOUT, "server-response-timeout", JURL_PARAMTYPE_LONG},
	{CURLOPT_FTP_ALTERNATIVE_TO_USER, "ftp-alternative-to-user", JURL_PARAMTYPE_STRING},
	{CURLOPT_FTP_SKIP_PASV_IP,        "ftp-skip-pasv-ip",        JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FTPSSLAUTH,              "ftpsslauth",              JURL_PARAMTYPE_ENUM},
	{CURLOPT_FTP_SSL_CCC,             "ftp-ssl-ccc",             JURL_PARAMTYPE_ENUM},
	{CURLOPT_FTP_ACCOUNT,             "ftp-account",             JURL_PARAMTYPE_STRING},
	{CURLOPT_FTP_FILEMETHOD,          "ftp-filemethod",          JURL_PARAMTYPE_ENUM},

	// * rtsp options
	{CURLOPT_RTSP_REQUEST,     "rtsp-request",     JURL_PARAMTYPE_ENUM},
	{CURLOPT_RTSP_SESSION_ID,  "rtsp-session-id",  JURL_PARAMTYPE_STRING},
	{CURLOPT_RTSP_STREAM_URI,  "rtsp-stream-uri",  JURL_PARAMTYPE_STRING},
	{CURLOPT_RTSP_TRANSPORT,   "rtsp-transport",   JURL_PARAMTYPE_STRING},
	{CURLOPT_RTSP_CLIENT_CSEQ, "rtsp-client-cseq", JURL_PARAMTYPE_LONG},
	{CURLOPT_RTSP_SERVER_CSEQ, "rtsp-server-cseq", JURL_PARAMTYPE_LONG},
	{CURLOPT_AWS_SIGV4,        "aws-sigv4",        JURL_PARAMTYPE_STRING},

	// * protocol options
	{CURLOPT_TRANSFERTEXT,        "transfertext",        JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_PROXY_TRANSFER_MODE, "proxy-transfer-mode", JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_CRLF,                "crlf",                JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_RANGE,               "range",               JURL_PARAMTYPE_STRING},
	{CURLOPT_RESUME_FROM,         "resume-from",         JURL_PARAMTYPE_LONG},
	{CURLOPT_RESUME_FROM_LARGE,   "resume-from-large",   JURL_PARAMTYPE_OFF_T},
	// SKIP: curlu: not implementing CURLU*
	{CURLOPT_CUSTOMREQUEST,       "customrequest",       JURL_PARAMTYPE_STRING},
	{CURLOPT_FILETIME,            "filetime",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_DIRLISTONLY,         "dirlistonly",         JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_NOBODY,              "nobody",              JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_INFILESIZE,          "infilesize",          JURL_PARAMTYPE_LONG},
	{CURLOPT_INFILESIZE_LARGE,    "infilesize-large",    JURL_PARAMTYPE_OFF_T},
	{CURLOPT_UPLOAD,              "upload",              JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_UPLOAD_BUFFERSIZE,   "upload-buffersize",   JURL_PARAMTYPE_LONG},
	// TODO: mimepost
	{CURLOPT_MIME_OPTIONS,        "mime-options",        JURL_PARAMTYPE_ENUM},
	{CURLOPT_MAXFILESIZE,         "maxfilesize",         JURL_PARAMTYPE_LONG},
	{CURLOPT_MAXFILESIZE_LARGE,   "maxfilesize-large",   JURL_PARAMTYPE_OFF_T},
	{CURLOPT_TIMECONDITION,       "timecondition",       JURL_PARAMTYPE_ENUM},
	{CURLOPT_TIMEVALUE,           "timevalue",           JURL_PARAMTYPE_LONG},
	{CURLOPT_TIMEVALUE_LARGE,     "timevalue-large",     JURL_PARAMTYPE_OFF_T},

	// * connection options
	{CURLOPT_TIMEOUT,                   "timeout",                   JURL_PARAMTYPE_LONG},
	{CURLOPT_TIMEOUT_MS,                "timeout-ms",                JURL_PARAMTYPE_LONG},
	{CURLOPT_LOW_SPEED_LIMIT,           "low-speed-limit",           JURL_PARAMTYPE_LONG},
	{CURLOPT_LOW_SPEED_TIME,            "low-speed-time",            JURL_PARAMTYPE_LONG},
	{CURLOPT_MAX_SEND_SPEED_LARGE,      "max-send-speed-large",      JURL_PARAMTYPE_OFF_T},
	{CURLOPT_MAX_RECV_SPEED_LARGE,      "max-recv-speed-large",      JURL_PARAMTYPE_OFF_T},
	{CURLOPT_MAXCONNECTS,               "maxconnects",               JURL_PARAMTYPE_LONG},
	{CURLOPT_FRESH_CONNECT,             "fresh-connect",             JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_FORBID_REUSE,              "forbid-reuse",              JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_MAXAGE_CONN,               "maxage-conn",               JURL_PARAMTYPE_LONG},
	{CURLOPT_MAXLIFETIME_CONN,          "maxlifetime-conn",          JURL_PARAMTYPE_LONG},
	{CURLOPT_CONNECTTIMEOUT,            "connecttimeout",            JURL_PARAMTYPE_LONG},
	{CURLOPT_CONNECTTIMEOUT_MS,         "connecttimeout-ms",         JURL_PARAMTYPE_LONG},
	{CURLOPT_IPRESOLVE,                 "ipresolve",                 JURL_PARAMTYPE_ENUM},
	{CURLOPT_CONNECT_ONLY,              "connect-only",              JURL_PARAMTYPE_LONG},
	{CURLOPT_USE_SSL,                   "use-ssl",                   JURL_PARAMTYPE_ENUM},
	{CURLOPT_RESOLVE,                   "resolve",                   JURL_PARAMTYPE_SLIST},
	{CURLOPT_DNS_INTERFACE,             "dns-interface",             JURL_PARAMTYPE_STRING},
	{CURLOPT_DNS_LOCAL_IP4,             "dns-local-ip4",             JURL_PARAMTYPE_STRING},
	{CURLOPT_DNS_LOCAL_IP6,             "dns-local-ip6",             JURL_PARAMTYPE_STRING},
	{CURLOPT_DNS_SERVERS,               "dns-servers",               JURL_PARAMTYPE_STRING},
	{CURLOPT_DNS_SHUFFLE_ADDRESSES,     "dns-shuffle-addresses",     JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_ACCEPTTIMEOUT_MS,          "accepttimeout-ms",          JURL_PARAMTYPE_LONG},
	{CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS, "happy-eyeballs-timeout-ms", JURL_PARAMTYPE_LONG},
	{CURLOPT_UPKEEP_INTERVAL_MS,        "upkeep-interval-ms",        JURL_PARAMTYPE_LONG},

	// * ssl and security options
	// TODO: holy shit it's long and I'm bored

	// * ssh options
	{CURLOPT_SSH_AUTH_TYPES,             "ssh-auth-types",             JURL_PARAMTYPE_ENUM},
	{CURLOPT_SSH_COMPRESSION,            "ssh-compression",            JURL_PARAMTYPE_BOOLEAN},
	{CURLOPT_SSH_HOST_PUBLIC_KEY_MD5,    "ssh-host-public-key-md5",    JURL_PARAMTYPE_STRING},
	{CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256, "ssh-host-public-key-sha256", JURL_PARAMTYPE_STRING},
	{CURLOPT_SSH_PUBLIC_KEYFILE,         "ssh-public-keyfile",         JURL_PARAMTYPE_STRING},
	{CURLOPT_SSH_PRIVATE_KEYFILE,        "ssh-private-keyfile",        JURL_PARAMTYPE_STRING},
	{CURLOPT_SSH_KNOWNHOSTS,             "ssh-knownhosts",             JURL_PARAMTYPE_STRING},
	// SKIP: ssh-keyfunction: callback
	// SKIP: ssh-keydata: callback
	// SKIP: ssh-hostkeyfunction: callback
	// SKIP: ssh-hostkeydata: callback

	// * websocket options
#if CURL_AT_LEAST_VERSION(7,86,0)
	{CURLOPT_WS_OPTIONS, "ws-options", JURL_PARAMTYPE_ENUM},
#endif

	// * other options
	// SKIP: private: complex representation
	// SKIP: share: complex representation
	{CURLOPT_NEW_FILE_PERMS,      "new-file-perms",      JURL_PARAMTYPE_LONG},
	{CURLOPT_NEW_DIRECTORY_PERMS, "new-directory-perms", JURL_PARAMTYPE_LONG},
#if CURL_AT_LEAST_VERSION(7,87,0)
	{CURLOPT_QUICK_EXIT,          "quick-exit",          JURL_PARAMTYPE_BOOLEAN},
#endif

	// * telnet options
	{CURLOPT_TELNETOPTIONS, "telnetoptions", JURL_PARAMTYPE_SLIST},
};

JANET_CFUN(jurl_setopt) {
	janet_fixarity(argc, 3);
	jurl_handle *jurl = janet_getjurl(argv, 0);

	const struct jurl_opt *opt;
	Janet jopt = argv[1];
	for (size_t i = 0; i < sizeof(jurl_opts) / sizeof(struct jurl_opt); i++) {
		if (janet_keyeq(jopt, jurl_opts[i].keyword)) {
			opt = &jurl_opts[i];
			break;
		}
	}
	if (!opt) {
		int num = janet_getinteger(argv, 1);
		for (size_t i = 0; i < sizeof(jurl_opts) / sizeof(struct jurl_opt); i++) {
			if (num == jurl_opts[i].opt) {
				opt = &jurl_opts[i];
				break;
			}
		}
	}
	if (!opt) {
		janet_panic("could not find option to set in jurl_setopt");
	}

	switch (opt->type) {
		case JURL_PARAMTYPE_BOOLEAN:
			if (janet_checktype(argv[2], JANET_NUMBER)) {
				return jurl_geterror(
					curl_easy_setopt(
						jurl->handle, opt->opt, janet_getinteger(argv, 2) == 0 ? 0 : 1
					));
			} else {
				return jurl_geterror(
					curl_easy_setopt(
						jurl->handle, opt->opt, janet_truthy(argv[2]) ? 1 : 0
					));
			}
			break;
		case JURL_PARAMTYPE_LONG:
		case JURL_PARAMTYPE_OFF_T:
			return jurl_geterror(
				curl_easy_setopt(jurl->handle, opt->opt, janet_getinteger64(argv, 2)
				));
			break;
		case JURL_PARAMTYPE_STRING:
			// strings may be set to null
			if (janet_checktype(argv[2], JANET_NIL)) {
				return jurl_geterror(
					curl_easy_setopt(jurl->handle, opt->opt, NULL
					));
			} else {
				return jurl_geterror(
					curl_easy_setopt(jurl->handle, opt->opt, janet_getcstring(argv, 2)
					));
			}
			break;
		case JURL_PARAMTYPE_FILE:
			return jurl_geterror(
				curl_easy_setopt(jurl->handle, opt->opt, janet_getjfile(argv, 2)
				));
			break;
		case JURL_PARAMTYPE_SLIST: {
			// we register the cleanup ahead of time, because...
			struct jurl_cleanup *clean = register_cleanup(jurl, JURL_CLEANUP_TYPE_SLIST);

			JanetView args = janet_getindexed(argv, 2);
			for (int32_t i = 0; i < args.len; i++) {
				const char *s = janet_getcstring(args.items, i);
				// we use the cleanup pointer directly, this way...
				struct curl_slist *newlist = curl_slist_append(clean->slist, s);
				if (!newlist) {
					// if we ever panic in the *middle* of handling,
					// the slist will get cleaned up during garbage collection anyway
					janet_panic("failed to append to slist in jurl_setopt");
				}
				clean->slist = newlist;
			}
			return jurl_geterror(
				curl_easy_setopt(jurl->handle, opt->opt, clean->slist
				));
			break;
		}
		case JURL_PARAMTYPE_ENUM:
			return jurl_geterror(
				jurl_setenum(jurl, opt->opt, argv[2]
				));
			break;
		case JURL_PARAMTYPE_CALLBACK:
			// callbacks are complex and need individual handling
			return jurl_geterror(
				jurl_setcallback(jurl, opt->opt, janet_getfunction(argv, 2)
				));
			break;
		default:
			janet_panic("jurl_setopt: unrecognized param type");
	}

	return janet_wrap_nil(); // unreachable
}
