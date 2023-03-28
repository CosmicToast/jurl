# Reading Guide
A general discussion on the way the project is organized (how you should read it) and the design decisions at play.

## Jurl/Native
The bindings start on the low level, inside of the `src` directory, where you can find all of the native code.
The primary entrypoint is `main.c`, which defines the functions exposed to janet.
However, the actual interesting part is `jurl.h`, which defines all of the data types and exported functions (visible to janet or not).
Functions visible to janet are easy to identify, since they're defined using `JANET_CFUN`.

Of particular interest is `jurl_handle`, `jurl_mime` and `jurl_mimepart`, which represent abstract types.

Importantly, I do not implement anything of `curl_multi`.
It's a tradeoff, but I think it's justified.
It's fairly complex (wrapping it would more than double the size of the code), and isn't that useful since janet is not massively threaded.

### jurl_handle (jurl.c)
`jurl_handle` represents a `CURL*` object and contains cleanup information.
Said cleanup information is primarily required to free instances of `struct curl_slist*` once they are no longer needed.
It's an abstract type, which means we can plug freeing both it and other cleanup into its garbage collection routine, `jurl_gc`.
We also make some methods available via the self-like messaging system, such as `:getinfo` and `:setopt`, and importantly `:perform`,
all of which map more or less directly to `curl_easy_*` functions.

For additional points, we also implement `get` and `put` forwarders, which panic in case something goes wrong.
Coincidentally, `get`/`getinfo` and `put`/`setopt` are some of the most complex parts of the native code.
As such both of them have their own files.
Callbacks are also used across multiple components, and so have their own file.

### jurl_mime (mime.c)
`jurl_mime` represents a `struct curl_mime*`.
We only have to garbage collect it itself, and only if there isn't a subpart.

However, we do get some nice methods.

`jurl_mimepart` which represents a `struct curl_mimepart*` though, does have associated cleanup,
but doesn't need to be freed itself.
It has a lot of methods though, all of which are thin wrappers.

None of these are too complicated, and all of them are just inside `mime.c`.

### jurl_getinfo (getinfo.c)
`curl_easy_getinfo` has a lot of options, and so it gets its own file.
In it, we define a compile-time-sized array of CURLINFO codes and associated keywords to the type of parameter they return.
The parameter is the only thing that determines the actual `getinfo` behavior!

`jurl_getinfo` essentially finds the correct entry in the array, then does things depending on the parameter type.
The main complex ones are bitmasks and enums, which are done separately, in `enums.c`'s `jurl_getinfoenum` and `jurl_getinfomask`.

### jurl_setopt (setopt.c)
`curl_easy_setopt` has a lot of options, and so it gets its own file.
It works almost exactly like `getinfo`, with an exception:
We treat enums and bitmasks identically, because the janet representation of a bitmask is an array of enums.
As such, we can switch on array vs value to determine at runtime.
This does mean that the user must provide correct data at runtime too though!

Furthermore, callbacks all require individual handling.
This is because every callback is different and has its own semantics!
Therefore, callbacks are implemented in `callbacks.c`.

### jurl_setcallback (callbacks.c)
Here we define the callbacks and how they pass data through to the janet functions.
Ultimately, we can't just use a JANET_CFUN as a function.
So we lie.

Instead, when the janet consumer specifies a `:*function` option, we actually plug it into `:*data`.
Then we set the `:*function` to a prewritten custom callback that does the conversion between semantics.

### jurl_getinfoenum, jurl_getinfomask, jurl_setenum (enums.c)
Enums are all differently typed and need to be represented individually in `getinfo` and `setopt`.
This file maps various codes to keywords, and does the translation as needed by the above two functions.

### CURLcode (errors.c)
Curl's easy API universally returns a CURLcode to signal errors, with `CURLE_OK` being the "normal" return.
This file maps codes to various error keywords, and lets you query an explanatory string on what the keyword means via `strerror`.

## Janet API
The API is divided into three parts: `jurl/mime`, `jurl/text`, and `jurl`.

### jurl/mime
`jurl/mime` is a helper around `mime.c` to make it (significantly) easier to generate valid `jurl_mime` objects.
It also just uses a struct with pattern matching, something that'll come in useful later...

### jurl/text
`jurl/text` is a helper around general text-based operations.
For example, `keyword-lower` takes any bytes-like, makes sure it's lowercase, and turns it into a keyword.
This is extremely useful for merging headers, which are (by the spec) case-insensitive... *unlike* janet's keywords.

It also defines a parser for headers, so we can take curl's output (a big string) and turn it into a dictionary via `parse-headers`!
The converse is also present, transforming a dictionary of headers into a curl-compatible list.

### jurl
`jurl` is the workhorse of the high level API.
First, it defines `request`, which does all the actual work of wrapping around the native bindings.
Looking at the documentation and the code for it should make that much clear.

Then, to make it more lispy and convenient, we have `defapi` and everything defined using it.
