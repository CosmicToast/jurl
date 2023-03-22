# Jurl
Janet Curl (secretly Jean Curl)

A libcurl easy API wrapper for Janet.
It's divided into two components: `jurl-native`, which holds the low level C interfacing, and the high level `jurl` janet wrapper.

`jurl-native` is relatively feature-complete and is now mostly in maintenance mode.
Potential improvements are:
* gate additional features behind version checks
* implement the (very) few curl_easy things still left out (mostly things that I can't think of a sane way to represent)

`jurl` is under active construction.

## Jurl
A higher level API that's currently in construction.

## Jurl-Native
`jurl-native` attempts to implement as much of libcurl's easy API in as direct a manner as possible.

In cases of bitmaps and enums (including magic numbers), they are translated as directly as possible from their C forms into keywords.
For example, `CURLOPT_TCP_FASTOPEN` becomes `:tcp-fastopen`.

Enums (and bitmap members) are translated using a prefix notation.
For example, `CURLPROXY_HTTP` becomes `:proxy/http`.

Bitmaps are translates back and forth using indexables (arrays and tuples).
For example, `CURLAUTH_BASIC | CURLAUTH_DIGEST` becomes `[:auth/basic :auth/digest]`.
