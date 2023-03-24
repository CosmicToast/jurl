// errors.c: libcurl-errors(3)
#include "jurl.h"

struct jurl_error {
	CURLcode code;
	char *keyword;
};
static const struct jurl_error jurl_errors[] = {
	{CURLE_OK,                       "ok"}, // only unprefixed keyword in the collection
	{CURLE_UNSUPPORTED_PROTOCOL,     "error/unsupported-protocol"},
	{CURLE_FAILED_INIT,              "error/failed-init"},
	{CURLE_URL_MALFORMAT,            "error/url-malformat"},
	{CURLE_NOT_BUILT_IN,             "error/not-built-in"},
	{CURLE_COULDNT_RESOLVE_PROXY,    "error/couldnt-resolve-proxy"},
	{CURLE_COULDNT_RESOLVE_HOST,     "error/couldnt-resolve-host"},
	{CURLE_COULDNT_CONNECT,          "error/couldnt-connect"},
	{CURLE_WEIRD_SERVER_REPLY,       "error/weird-server-reply"},
	{CURLE_REMOTE_ACCESS_DENIED,     "error/remote-access-denied"},
	{CURLE_FTP_ACCEPT_FAILED,        "error/ftp-access-failed"},
	{CURLE_FTP_WEIRD_PASS_REPLY,     "error/ftp-weird-pass-reply"},
	{CURLE_FTP_ACCEPT_TIMEOUT,       "error/ftp-accept-timeout"},
	{CURLE_FTP_WEIRD_PASV_REPLY,     "error/ftp-weird-pasv-reply"},
	{CURLE_FTP_WEIRD_227_FORMAT,     "error/ftp-weird-227-format"},
	{CURLE_FTP_CANT_GET_HOST,        "error/ftp-cant-get-host"},
	{CURLE_HTTP2,                    "error/http2"},
	{CURLE_FTP_COULDNT_SET_TYPE,     "error/ftp-couldnt-set-type"},
	{CURLE_PARTIAL_FILE,             "error/partial-file"},
	{CURLE_FTP_COULDNT_RETR_FILE,    "error/ftp-couldnt-retr-file"},
	// SKIP: 20: obsolete
	{CURLE_QUOTE_ERROR,              "error/quote-error"},
	{CURLE_HTTP_RETURNED_ERROR,      "error/http-returned-error"},
	{CURLE_WRITE_ERROR,              "error/write-error"},
	// SKIP: 24: obsolete
	{CURLE_UPLOAD_FAILED,            "error/upload-failed"},
	{CURLE_READ_ERROR,               "error/read-error"},
	{CURLE_OUT_OF_MEMORY,            "error/out-of-memory"},
	{CURLE_OPERATION_TIMEDOUT,       "error/operation-timeout"},
	// SKIP: 29: obsolete
	{CURLE_FTP_PORT_FAILED,          "error/ftp-port-failed"},
	{CURLE_FTP_COULDNT_USE_REST,     "error/ftp-couldnt-use-rest"},
	// SKIP: 32: obsolete
	{CURLE_RANGE_ERROR,              "error/range-error"},
	{CURLE_HTTP_POST_ERROR,          "error/http-post-error"},
	{CURLE_SSL_CONNECT_ERROR,        "error/ssl-connect-error"},
	{CURLE_BAD_DOWNLOAD_RESUME,      "error/bad-download-resume"},
	{CURLE_FILE_COULDNT_READ_FILE,   "error/file-couldnt-read-file"},
	{CURLE_LDAP_CANNOT_BIND,         "error/ldap-cannot-bind"},
	{CURLE_LDAP_SEARCH_FAILED,       "error/ldap-search-failed"},
	// SKIP: 40: obsolete
	{CURLE_FUNCTION_NOT_FOUND,       "error/function-not-found"},
	{CURLE_ABORTED_BY_CALLBACK,      "error/aborted-by-callback"},
	{CURLE_BAD_FUNCTION_ARGUMENT,    "error/bad-function-argument"},
	// SKIP: 44: obsolete
	{CURLE_INTERFACE_FAILED,         "error/interface-failed"},
	// SKIP: 46: obsolete
	{CURLE_TOO_MANY_REDIRECTS,       "error/too-many-redirects"},
	{CURLE_UNKNOWN_OPTION,           "error/unknown-option"},
	{CURLE_SETOPT_OPTION_SYNTAX,     "error/setopt-option-syntax"},
	// SKIP: 50: obsolete
	// SKIP: 51: obsolete
	{CURLE_GOT_NOTHING,              "error/got-nothing"},
	{CURLE_SSL_ENGINE_NOTFOUND,      "error/ssl-engine-notfound"},
	{CURLE_SSL_ENGINE_SETFAILED,     "error/ssl-engine-setfailed"},
	{CURLE_SEND_ERROR,               "error/send-error"},
	{CURLE_RECV_ERROR,               "error/recv-error"},
	// SKIP: 57: obsolete
	{CURLE_SSL_CERTPROBLEM,          "error/ssl-certproblem"},
	{CURLE_SSL_CIPHER,               "error/ssl-cipher"},
	{CURLE_PEER_FAILED_VERIFICATION, "error/peer-failed-verification"},
	{CURLE_BAD_CONTENT_ENCODING,     "error/bad-content-encoding"},
	// SKIP: 62: obsolete
	{CURLE_FILESIZE_EXCEEDED,        "error/filesize-exceeded"},
	{CURLE_USE_SSL_FAILED,           "error/use-ssl-failed"},
	{CURLE_SEND_FAIL_REWIND,         "error/send-fail-rewind"},
	{CURLE_SSL_ENGINE_INITFAILED,    "error/ssl-engine-initfailed"},
	{CURLE_LOGIN_DENIED,             "error/login-denied"},
	{CURLE_TFTP_NOTFOUND,            "error/tftp-notfound"},
	{CURLE_TFTP_PERM,                "error/tftp-perm"},
	{CURLE_REMOTE_DISK_FULL,         "error/remote-disk-full"},
	{CURLE_TFTP_ILLEGAL,             "error/tftp-illegal"},
	{CURLE_TFTP_UNKNOWNID,           "error/tftp-unknownid"},
	{CURLE_REMOTE_FILE_EXISTS,       "error/remote-file-exists"},
	{CURLE_TFTP_NOSUCHUSER,          "error/tftp-nosuchuser"},
	// SKIP: 75: obsolete
	// SKIP: 76: obsolete
	{CURLE_SSL_CACERT_BADFILE,       "error/ssl-cacert-badfile"},
	{CURLE_REMOTE_FILE_NOT_FOUND,    "error/remote-file-not-found"},
	{CURLE_SSH,                      "error/ssh"},
	{CURLE_SSL_SHUTDOWN_FAILED,      "error/ssl-shutdown-failed"},
	{CURLE_AGAIN,                    "error/again"},
	{CURLE_SSL_CRL_BADFILE,          "error/ssl-crl-badfile"},
	{CURLE_SSL_ISSUER_ERROR,         "error/ssl-issuer-error"},
	{CURLE_FTP_PRET_FAILED,          "error/ftp-pret-failed"},
	{CURLE_RTSP_CSEQ_ERROR,          "error/rtsp-cseq-error"},
	{CURLE_RTSP_SESSION_ERROR,       "error/rtsp-session-error"},
	{CURLE_FTP_BAD_FILE_LIST,        "error/ftp-bad-file-list"},
	{CURLE_CHUNK_FAILED,             "error/chunk-failed"},
	{CURLE_NO_CONNECTION_AVAILABLE,  "error/no-connection-available"},
	{CURLE_SSL_PINNEDPUBKEYNOTMATCH, "error/ssl-pinnedpubkeynotmatch"},
	{CURLE_SSL_INVALIDCERTSTATUS,    "error/ssl-invalidcertstatus"},
	{CURLE_HTTP2_STREAM,             "error/http2-stream"},
	{CURLE_RECURSIVE_API_CALL,       "error/recursive-api-call"},
	{CURLE_AUTH_ERROR,               "error/auth-error"},
	{CURLE_HTTP3,                    "error/http3"},
	{CURLE_QUIC_CONNECT_ERROR,       "error/quic-connect-error"},
	{CURLE_PROXY,                    "error/proxy"},
	{CURLE_SSL_CLIENTCERT,           "error/ssl-clientcert"},
	{CURLE_UNRECOVERABLE_POLL,       "error/unrecoverable-poll"},
};
#define jurl_error_size (sizeof(jurl_errors) / sizeof(struct jurl_error))

// translate a CURLcode return code into a keyword
Janet jurl_geterror(CURLcode code) {
	for (size_t i = 0; i < jurl_error_size; i++) {
		if (jurl_errors[i].code == code) {
			return janet_ckeywordv(jurl_errors[i].keyword);
		}
	}
	janet_panicf("jurl_geterror: could not find error for code %d", code);
	return janet_wrap_nil(); // unreachable
}

JANET_CFUN(jurl_strerror) {
	janet_fixarity(argc, 1);
	CURLcode code;
	Janet arg = argv[0];
	switch (janet_checktypes(arg, JANET_TFLAG_KEYWORD | JANET_TFLAG_NUMBER)) {
		case JANET_TFLAG_KEYWORD:
			for (size_t i = 0; i < jurl_error_size; i++) {
				if (janet_keyeq(arg, jurl_errors[i].keyword)) {
					code = jurl_errors[i].code;
					break;
				}
			}
			break;
		case JANET_TFLAG_NUMBER:
			code = janet_unwrap_integer(arg);
			break;
		default:
			janet_panicf("expected keyword or number in jurl_strerror, got %T", janet_type(arg));
	}

	return janet_cstringv(curl_easy_strerror(code));
}
