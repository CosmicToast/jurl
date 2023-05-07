#include "jurl.h"

#ifdef POLYFILL_CBYTES
const char* janet_getcbytes(const Janet *argv, int32_t n) {
    JanetByteView view = janet_getbytes(argv, n);
    const char *cstr = (const char *)view.bytes;
    if (strlen(cstr) != (size_t) view.len) {
        janet_panic("bytes contain embedded 0s");
    }
    return cstr;
}

const char *janet_optcbytes(const Janet *argv, int32_t argc, int32_t n, const char *dflt) {
    if (n >= argc || janet_checktype(argv[n], JANET_NIL)) {
        return dflt;
    }
    return janet_getcbytes(argv, n);
}
#endif
