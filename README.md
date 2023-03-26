# Jurl
Janet Curl (secretly Jean Curl)

A libcurl easy API wrapper for Janet.
It's divided into two components: `jurl/native`, which holds the low level C interfacing, and the high level `jurl` janet wrapper.

`jurl/native` is mostly feature complete.
If something you depend on is missing, and you have a good idea on how to represent it in Janet, let me know in the issues.

`jurl`'s core functionality is complete.
There are still some utility functions for convenience to be written, but it's perfectly usable as it is.

For examples of the various APIs, see the tests!
P.S. There are no tests yet, soon though.

## Installation
`jpm install https://github.com/cosmictoast/jurl.git`

This wraps around libcurl, and as such needs libcurl (library and headers for building) installed.
Tested on linux and macos.

## Caveats
Not all features are gated behind version flags, patches welcome.
A few features are not implemented, grep for "not implemented" and "SKIP".
I'm open to implementing what's left, but it's generally very niche things, such as ioctl callbacks.

## Jurl
`jurl` is a requests-style API, but more complete.
For the main chunk of docs, see `jurl/init.janet`'s `request` symbol documentation.

A few basic examples.
Get with a query:
```janet
(request {:url "https://pie.dev/get"
          :query {:a "b"
                  :c "d"}})
# -> {:body @"..." :error :ok :handle <jurl> :status 200}
```

A post with multipart mime (including file uploads):
```janet
(request {:url "https://pie.dev/post"
          :body [{:name "field name"
                  :filename "remote.file" # this is what makes it a file upload
                  :data [:file "local.file"]}]})
```

A post with json:
```janet
(request {:url "https://pie.dev/post"
          :body (json/encode {:a "b"
                              :c "d"})
          :headers {:content-type "application/json"})
# -> {:body @"..." :error :ok :handle <jurl> :status 200}
```

## Jurl/Native
`jurl/native` attempts to implement as much of libcurl's easy API in as direct a manner as possible.

In cases of bitmaps and enums (including magic numbers), they are translated as directly as possible from their C forms into keywords.
For example, `CURLOPT_TCP_FASTOPEN` becomes `:tcp-fastopen`.

Enums (and bitmap members) are translated using a prefix notation.
For example, `CURLPROXY_HTTP` becomes `:proxy/http`.
Ones that are extremely common are not prefixed, such as various `:auth/` keys.

Bitmaps are translated back and forth using indexables (arrays and tuples).
For example, `CURLAUTH_BASIC | CURLAUTH_DIGEST` becomes `[basic digest]`.
