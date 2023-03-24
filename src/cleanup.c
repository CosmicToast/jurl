// cleanup.c: implements cleanup system that's shared across jurl abstract options
#include "jurl.h"

void jurl_do_cleanup(struct jurl_cleanup **src) {
	while (*src) {
		struct jurl_cleanup *cur = *src;
		switch (cur->type) {
		case JURL_CLEANUP_TYPE_SLIST:
			curl_slist_free_all(cur->slist);
			break;
		default:
			janet_panic("unknown type of cleanup data in do_cleanup");
		}
		*src = cur->next;
		free(cur);
	}
}

struct jurl_cleanup *register_cleanup(struct jurl_cleanup **prev, enum jurl_cleanup_type type) {
	struct jurl_cleanup *out = malloc(sizeof(struct jurl_cleanup));
	out->next = *prev;
	*prev = out;
	out->type = type;
	return out;
}
