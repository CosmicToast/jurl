#include "jurl.h"

#ifdef POLYFILL_GETCBYTES
const char *poly_getcbytes(const Janet *argv, int32_t n) {
    /* Ensure buffer 0-padded */
    if (janet_checktype(argv[n], JANET_BUFFER)) {
        JanetBuffer *b = janet_unwrap_buffer(argv[n]);
        if ((b->gc.flags & JANET_BUFFER_FLAG_NO_REALLOC) && b->count == b->capacity) {
            /* Make a copy with janet_smalloc in the rare case we have a buffer that
             * cannot be realloced and pushing a 0 byte would panic. */

			/* FIXME: This will fail under jurl, since jurl pre-supposes that the
			 * lifetime of a getcbytes call string is equivalent to the lifetime
			 * of the given Janet value. Maybe manually garbage collect? */
            char *new_string = janet_smalloc(b->count + 1);
            memcpy(new_string, b->data, b->count);
            new_string[b->count] = 0;
            if (strlen(new_string) != (size_t) b->count) goto badzeros;
            return new_string;
        } else {
            /* Ensure trailing 0 */
            janet_buffer_push_u8(b, 0);
            b->count--;
            if (strlen((char *)b->data) != (size_t) b->count) goto badzeros;
            return (const char *) b->data;
        }
    }
    JanetByteView view = janet_getbytes(argv, n);
    const char *cstr = (const char *)view.bytes;
    if (strlen(cstr) != (size_t) view.len) goto badzeros;
    return cstr;

badzeros:
    janet_panic("bytes contain embedded 0s");
}
#endif

#ifdef POLYFILL_OPTCBYTES
const char *poly_optcbytes(const Janet *argv, int32_t argc, int32_t n, const char *dflt) {
    if (n >= argc || janet_checktype(argv[n], JANET_NIL)) {
        return dflt;
    }
    return janet_getcbytes(argv, n);
}
#endif
