#include "jurl.h"

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

	{CURLOPT_HTTPAUTH, CURLAUTH_BASIC,     "auth/basic"},
	{CURLOPT_HTTPAUTH, CURLAUTH_DIGEST,    "auth/digest"},
	{CURLOPT_HTTPAUTH, CURLAUTH_DIGEST_IE, "auth/digest-ie"},
	{CURLOPT_HTTPAUTH, CURLAUTH_BEARER,    "auth/bearer"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE, "auth/negotiate"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NTLM,      "auth/ntlm"},
	{CURLOPT_HTTPAUTH, CURLAUTH_NTLM_WB,   "auth/ntlm-wb"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ANY,       "auth/any"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ANYSAFE,   "auth/anysafe"},
	{CURLOPT_HTTPAUTH, CURLAUTH_ONLY,      "auth/only"},
	{CURLOPT_HTTPAUTH, CURLAUTH_AWS_SIGV4, "auth/aws-sigv4"},

	{CURLOPT_PROXYAUTH, CURLAUTH_BASIC,     "auth/basic"},
	{CURLOPT_PROXYAUTH, CURLAUTH_DIGEST,    "auth/digest"},
	{CURLOPT_PROXYAUTH, CURLAUTH_DIGEST_IE, "auth/digest-ie"},
	{CURLOPT_PROXYAUTH, CURLAUTH_BEARER,    "auth/bearer"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NEGOTIATE, "auth/negotiate"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NTLM,      "auth/ntlm"},
	{CURLOPT_PROXYAUTH, CURLAUTH_NTLM_WB,   "auth/ntlm-wb"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ANY,       "auth/any"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE,   "auth/anysafe"},
	{CURLOPT_PROXYAUTH, CURLAUTH_ONLY,      "auth/only"},
	{CURLOPT_PROXYAUTH, CURLAUTH_AWS_SIGV4, "auth/aws-sigv4"},

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
