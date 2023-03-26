// enums.c: implements
// * curl_easy_setopt options that take bitmasks or a specific value
// * curl_easy_getinfo options that return bitmasks or a specific value
#include "jurl.h"

// setopt section
struct jurl_enum {
	CURLoption opt;
	long value;
	const char *keyword;
};

static const struct jurl_enum jurl_enums[] = {
	{CURLOPT_PROXYTYPE, CURLPROXY_HTTP,            "proxy/http"},
	{CURLOPT_PROXYTYPE, CURLPROXY_HTTPS,           "proxy/https"},
	{CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0,        "proxy/http-1.0"},
	{CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4,          "proxy/socks4"},
	{CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4A,         "proxy/socks4a"},
	{CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5,          "proxy/socks5"},
	{CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME, "proxy/socks5-hostname"},

	{CURLOPT_SOCKS5_AUTH, CURLAUTH_BASIC,  "auth/basic"},
	{CURLOPT_SOCKS5_AUTH, CURLAUTH_GSSAPI, "auth/gssapi"},
	{CURLOPT_SOCKS5_AUTH, CURLAUTH_NONE,   "auth/none"},

	{CURLOPT_NETRC, CURL_NETRC_IGNORED,  "netrc/ignored"},
	{CURLOPT_NETRC, CURL_NETRC_OPTIONAL, "netrc/optional"},
	{CURLOPT_NETRC, CURL_NETRC_REQUIRED, "netrc/required"},

	{CURLOPT_HTTPAUTH, CURLAUTH_BASIC,     "basic"},
	{CURLOPT_HTTPAUTH, CURLAUTH_DIGEST,    "digest"},
	{CURLOPT_HTTPAUTH, CURLAUTH_DIGEST_IE, "digest-ie"},
	{CURLOPT_HTTPAUTH, CURLAUTH_BEARER,    "bearer"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE, "negotiate"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NTLM,      "ntlm"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NTLM_WB,   "ntlm-wb"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ANY,       "any"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ANYSAFE,   "anysafe"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ONLY,      "only"},
	{CURLOPT_HTTPAUTH, CURLAUTH_AWS_SIGV4, "aws-sigv4"},

	{CURLOPT_PROXYAUTH, CURLAUTH_BASIC,     "basic"},
	{CURLOPT_PROXYAUTH, CURLAUTH_DIGEST,    "digest"},
	{CURLOPT_PROXYAUTH, CURLAUTH_DIGEST_IE, "digest-ie"},
	{CURLOPT_PROXYAUTH, CURLAUTH_BEARER,    "bearer"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NEGOTIATE, "negotiate"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NTLM,      "ntlm"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NTLM_WB,   "ntlm-wb"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ANY,       "any"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE,   "anysafe"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ONLY,      "only"},
	{CURLOPT_PROXYAUTH, CURLAUTH_AWS_SIGV4, "aws-sigv4"},

	{CURLOPT_POSTREDIR, CURL_REDIR_POST_301, "redir-post/301"},
	{CURLOPT_POSTREDIR, CURL_REDIR_POST_302, "redir-post/302"},
	{CURLOPT_POSTREDIR, CURL_REDIR_POST_303, "redir-post/303"},
	{CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL, "redir-post/all"},

	{CURLOPT_HEADEROPT, CURLHEADER_UNIFIED,  "header/unified"},
	{CURLOPT_HEADEROPT, CURLHEADER_SEPARATE, "header/separate"},

	{CURLOPT_ALTSVC_CTRL, CURLALTSVC_READONLYFILE, "altsvc/readonlyfile"},
	{CURLOPT_ALTSVC_CTRL, CURLALTSVC_H1,           "altsvc/h1"},
	{CURLOPT_ALTSVC_CTRL, CURLALTSVC_H2,           "altsvc/h2"},
	{CURLOPT_ALTSVC_CTRL, CURLALTSVC_H3,           "altsvc/h3"},

	{CURLOPT_HSTS_CTRL, CURLHSTS_ENABLE,       "hsts/enable"},
	{CURLOPT_HSTS_CTRL, CURLHSTS_READONLYFILE, "hsts/readonlyfile"},

	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_NONE,              "http-version/none"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0,               "http-version/1.0"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1,               "http-version/1.1"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0,               "http-version/2.0"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS,              "http-version/2tls"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE, "http-version/2-prior-knowledge"},
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3,                 "http-version/3"},
#if CURL_AT_LEAST_VERSION(7,88,0)
	{CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3ONLY,             "http-version/3only"},
#endif

	{CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_NONE,  "ftp-create-dir/none"},
	{CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR,       "ftp-create-dir/dir"},
	{CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY, "ftp-create-dir/dir-entry"},

	{CURLOPT_FTPSSLAUTH, CURLFTPAUTH_DEFAULT, "ftpauth/default"},
	{CURLOPT_FTPSSLAUTH, CURLFTPAUTH_SSL,     "ftpauth/ssl"},
	{CURLOPT_FTPSSLAUTH, CURLFTPAUTH_TLS,     "ftpauth/tls"},

	{CURLOPT_FTP_SSL_CCC, CURLFTPSSL_CCC_NONE,    "ftpssl-ccc/none"},
	{CURLOPT_FTP_SSL_CCC, CURLFTPSSL_CCC_PASSIVE, "ftpssl-ccc/passive"},
	{CURLOPT_FTP_SSL_CCC, CURLFTPSSL_CCC_ACTIVE,  "ftpssl-ccc/active"},

	{CURLOPT_FTP_FILEMETHOD, CURLFTPMETHOD_MULTICWD,  "ftpmethod/multicwd"},
	{CURLOPT_FTP_FILEMETHOD, CURLFTPMETHOD_NOCWD,     "ftpmethod/nocwd"},
	{CURLOPT_FTP_FILEMETHOD, CURLFTPMETHOD_SINGLECWD, "ftpmethod/singlecwd"},

	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_OPTIONS,       "rtspreq/options"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_DESCRIBE,      "rtspreq/describe"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_ANNOUNCE,      "rtspreq/announce"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SETUP,         "rtspreq/setup"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_PLAY,          "rtspreq/play"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_PAUSE,         "rtspreq/pause"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_TEARDOWN,      "rtspreq/teardown"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_GET_PARAMETER, "rtspreq/get-parameter"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_SET_PARAMETER, "rtspreq/set-parameter"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_RECORD,        "rtspreq/record"},
	{CURLOPT_RTSP_REQUEST, CURL_RTSPREQ_RECEIVE,       "rtspreq/receive"},

	{CURLOPT_MIME_OPTIONS, CURLMIMEOPT_FORMESCAPE, "mimeopt/formescape"},

	{CURLOPT_TIMECONDITION, CURL_TIMECOND_IFMODSINCE,   "timecond/ifmodsince"},
	{CURLOPT_TIMECONDITION, CURL_TIMECOND_IFUNMODSINCE, "timecond/ifunmodsince"},

	{CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER, "ipresolve/whatever"},
	{CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4,       "ipresolve/v4"},
	{CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6,       "ipresolve/v6"},

	{CURLOPT_USE_SSL, CURLUSESSL_NONE,    "usessl/none"},
	{CURLOPT_USE_SSL, CURLUSESSL_TRY,     "usessl/try"},
	{CURLOPT_USE_SSL, CURLUSESSL_CONTROL, "usessl/control"},
	{CURLOPT_USE_SSL, CURLUSESSL_ALL,     "usessl/all"},

	{CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT,     "sslversion/default"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1,       "sslversion/tlsv1"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv2,       "sslversion/sslv2"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3,       "sslversion/sslv3"},
#if CURL_AT_LEAST_VERSION(7,34,0)
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_0,     "sslversion/tlsv1.0"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_1,     "sslversion/tlsv1.1"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2,     "sslversion/tlsv1.2"},
#endif
#if CURL_AT_LEAST_VERSION(7,52,0)
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3,     "sslversion/tlsv1.3"},
#endif
#if CURL_AT_LEAST_VERSION(7,54,0)
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_DEFAULT, "sslversion/max-default"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_0, "sslversion/max-tlsv1.0"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_1, "sslversion/max-tlsv1.1"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_2, "sslversion/max-tlsv1.2"},
	{CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_3, "sslversion/max-tlsv1.3"},
#endif

	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_ALLOW_BEAST,        "sslopt/allow-beast"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_ALLOW_BEAST,        "sslopt/allow-beast"},
#if CURL_AT_LEAST_VERSION(7,44,0)
	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_NO_REVOKE,          "sslopt/no-revoke"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE,          "sslopt/no-revoke"},
#endif
#if CURL_AT_LEAST_VERSION(7,68,0)
	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_NO_PARTIALCHAIN,    "sslopt/no-partialchain"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_NO_PARTIALCHAIN,    "sslopt/no-partialchain"},
#endif
#if CURL_AT_LEAST_VERSION(7,70,0)
	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_REVOKE_BEST_EFFORT, "sslopt/revoke-best-effort"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_REVOKE_BEST_EFFORT, "sslopt/revoke-best-effort"},
#endif
#if CURL_AT_LEAST_VERSION(7,71,0)
	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_NATIVE_CA,          "sslopt/native-ca"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA,          "sslopt/native-ca"},
#endif
#if CURL_AT_LEAST_VERSION(7,77,0)
	{CURLOPT_SSL_OPTIONS,       CURLSSLOPT_AUTO_CLIENT_CERT,   "sslopt/auto-client-cert"},
	{CURLOPT_PROXY_SSL_OPTIONS, CURLSSLOPT_AUTO_CLIENT_CERT,   "sslopt/auto-client-cert"},
#endif

	{CURLOPT_GSSAPI_DELEGATION, CURLGSSAPI_DELEGATION_NONE,        "gssapi-delegation/none"},
	{CURLOPT_GSSAPI_DELEGATION, CURLGSSAPI_DELEGATION_FLAG,        "gssapi-delegation/flag"},
	{CURLOPT_GSSAPI_DELEGATION, CURLGSSAPI_DELEGATION_POLICY_FLAG, "gssapi-delegation/policy-flag"},

	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY, "ssh-auth/publickey"},
	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD,  "ssh-auth/password"},
	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_HOST,      "ssh-auth/host"},
	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_KEYBOARD,  "ssh-auth/keyboard"},
	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_AGENT,     "ssh-auth/agent"},
	{CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_ANY,       "ssh-auth/any"},
};
#define jurl_enum_size (sizeof(jurl_enums) / sizeof(struct jurl_enum))

static long get_eq(CURLoption opt, JanetKeyword kw) {
	for (size_t i = 0; i < jurl_enum_size; i++) {
		if (jurl_enums[i].opt == opt && !strcmp(jurl_enums[i].keyword, (const char*)kw)) {
			return jurl_enums[i].value;
		}
	}
	janet_panicf("jurl_setenum: unrecognized keyword for option %d: %s", opt, kw);
}

CURLcode jurl_setenum(jurl_handle *jurl, CURLoption opt, Janet val) {
	long set = 0;
	if (janet_checktype(val, JANET_KEYWORD)) {
		set = get_eq(opt, janet_unwrap_keyword(val));
	} else {
		const Janet *args;
		int32_t len;
		if (!janet_indexed_view(val, &args, &len)) {
			janet_panicf("jurl_setenum: expected keyword or indexed type, got %T", janet_type(val));
		}
		for (int32_t i = 0; i < len; i++) {
			set |= get_eq(opt, janet_getkeyword(args, i));
		}
	}
	return curl_easy_setopt(jurl->handle, opt, set);
}

// getinfo section
struct jurl_info_enum {
	CURLINFO info;
	long code;
	const char *keyword;
};

static struct jurl_info_enum info_enums[] = {
	{CURLINFO_HTTP_VERSION, 0,                     "http-version/cannot-determine"},
	{CURLINFO_HTTP_VERSION, CURL_HTTP_VERSION_1_0, "http-version/1.0"},
	{CURLINFO_HTTP_VERSION, CURL_HTTP_VERSION_1_1, "http-version/1.1"},
	{CURLINFO_HTTP_VERSION, CURL_HTTP_VERSION_2_0, "http-version/2.0"},
	{CURLINFO_HTTP_VERSION, CURL_HTTP_VERSION_3,   "http-version/3"},

	{CURLINFO_PROXY_ERROR, CURLPX_OK,                               "proxy-error/ok"},
	{CURLINFO_PROXY_ERROR, CURLPX_BAD_ADDRESS_TYPE,                 "proxy-error/bad-address-type"},
	{CURLINFO_PROXY_ERROR, CURLPX_BAD_VERSION,                      "proxy-error/bad-version"},
	{CURLINFO_PROXY_ERROR, CURLPX_CLOSED,                           "proxy-error/closed"},
	{CURLINFO_PROXY_ERROR, CURLPX_GSSAPI,                           "proxy-error/gssapi"},
	{CURLINFO_PROXY_ERROR, CURLPX_GSSAPI_PERMSG,                    "proxy-error/gssapi-permsg"},
	{CURLINFO_PROXY_ERROR, CURLPX_GSSAPI_PROTECTION,                "proxy-error/gssapi-protection"},
	{CURLINFO_PROXY_ERROR, CURLPX_IDENTD,                           "proxy-error/identd"},
	{CURLINFO_PROXY_ERROR, CURLPX_IDENTD_DIFFER,                    "proxy-error/identd-differ"},
	{CURLINFO_PROXY_ERROR, CURLPX_LONG_HOSTNAME,                    "proxy-error/long-hostname"},
	{CURLINFO_PROXY_ERROR, CURLPX_LONG_PASSWD,                      "proxy-error/long-passwd"},
	{CURLINFO_PROXY_ERROR, CURLPX_LONG_USER,                        "proxy-error/long-user"},
	{CURLINFO_PROXY_ERROR, CURLPX_NO_AUTH,                          "proxy-error/no-auth"},
	{CURLINFO_PROXY_ERROR, CURLPX_RECV_ADDRESS,                     "proxy-error/recv-address"},
	{CURLINFO_PROXY_ERROR, CURLPX_RECV_AUTH,                        "proxy-error/recv-auth"},
	{CURLINFO_PROXY_ERROR, CURLPX_RECV_CONNECT,                     "proxy-error/connect"},
	{CURLINFO_PROXY_ERROR, CURLPX_RECV_REQACK,                      "proxy-error/reqack"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_ADDRESS_TYPE_NOT_SUPPORTED, "proxy-error/reply-address-type-not-supported"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_COMMAND_NOT_SUPPORTED,      "proxy-error/reply-command-not-supported"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_CONNECTION_REFUSED,         "proxy-error/reply-connection-refused"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_GENERAL_SERVER_FAILURE,     "proxy-error/reply-general-server-failure"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_HOST_UNREACHABLE,           "proxy-error/reply-host-unreachable"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_NETWORK_UNREACHABLE,        "proxy-error/reply-network-unreachable"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_NOT_ALLOWED,                "proxy-error/reply-not-allowed"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_TTL_EXPIRED,                "proxy-error/reply-ttl-expired"},
	{CURLINFO_PROXY_ERROR, CURLPX_REPLY_UNASSIGNED,                 "proxy-error/reply-unassigned"},
	{CURLINFO_PROXY_ERROR, CURLPX_REQUEST_FAILED,                   "proxy-error/request-failed"},
	{CURLINFO_PROXY_ERROR, CURLPX_RESOLVE_HOST,                     "proxy-error/resolve-host"},
	{CURLINFO_PROXY_ERROR, CURLPX_SEND_AUTH,                        "proxy-error/send-auth"},
	{CURLINFO_PROXY_ERROR, CURLPX_SEND_CONNECT,                     "proxy-error/send-connect"},
	{CURLINFO_PROXY_ERROR, CURLPX_SEND_REQUEST,                     "proxy-error/send-request"},
	{CURLINFO_PROXY_ERROR, CURLPX_UNKNOWN_FAIL,                     "proxy-error/unknown-fail"},
	{CURLINFO_PROXY_ERROR, CURLPX_UNKNOWN_MODE,                     "proxy-error/unknown-mode"},
	{CURLINFO_PROXY_ERROR, CURLPX_USER_REJECTED,                    "proxy-error/user-rejected"},

	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_BASIC,     "basic"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_DIGEST,    "digest"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_DIGEST_IE, "digest-ie"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_BEARER,    "bearer"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_NEGOTIATE, "negotiate"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_NTLM,      "ntlm"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_NTLM_WB,   "ntlm-wb"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_ANY,       "any"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_ANYSAFE,   "anysafe"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_ONLY,      "only"},
	{CURLINFO_HTTPAUTH_AVAIL, CURLAUTH_AWS_SIGV4, "aws-sigv4"},

	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_BASIC,     "basic"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_DIGEST,    "digest"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_DIGEST_IE, "digest-ie"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_BEARER,    "bearer"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_NEGOTIATE, "negotiate"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_NTLM,      "ntlm"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_NTLM_WB,   "ntlm-wb"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_ANY,       "any"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_ANYSAFE,   "anysafe"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_ONLY,      "only"},
	{CURLINFO_PROXYAUTH_AVAIL, CURLAUTH_AWS_SIGV4, "aws-sigv4"},
};
#define jurl_info_size (sizeof(info_enums) / sizeof(struct jurl_info_enum))

// these must be separate because the semantics differ greatly
Janet jurl_getinfoenum(CURLINFO info, long code) {
	for (size_t i = 0; i < jurl_info_size; i++) {
		if (info_enums[i].info == info && info_enums[i].code == code) {
			return janet_ckeywordv(info_enums[i].keyword);
		}
	}
	janet_panicf("jurl_getinfo: unrecognized code for option %d: %d", info, code);
}

Janet jurl_getinfomask(CURLINFO info, long code) {
	JanetArray *arr = janet_array(0);

	for (size_t i = 0; i < jurl_info_size; i++) {
		if (info_enums[i].info == info && (code & info_enums[i].code)) {
			janet_array_ensure(arr, arr->count + 1, 1);
			janet_array_push(arr, janet_ckeywordv(info_enums[i].keyword));
		}
	}

	return janet_wrap_array(arr);
}
