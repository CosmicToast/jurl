# Jurl
Janet Curl (secretly Jean Curl)

A libcurl easy API wrapper for Janet.
It's divided into two components: `jurl/native`, which holds the low level C interfacing, and the high level `jurl` janet wrapper.

The library is mostly finished.
There are a few minor features missing (e.g ioctl overrides).
There's also a few niceties to think about (for example, custom cookie parsing and mapping).
However, for 90+% of use-cases, it should be more than sufficient,
and is far more complete and convenient than most http clients I've worked with so far, including in other languages.

For examples of the various APIs, see the tests!

## Installation
`jpm install https://github.com/cosmictoast/jurl.git`

You can add it to your `project.janet` dependency list like so:
```janet
(declare-project
  :name "my-project"
  :dependencies
  [{:url "https://github.com/cosmictoast/jurl.git"
    :tag "v1.0.0"}]
```

This wraps around libcurl, and as such needs libcurl (library and headers for building) installed.
Tested on linux and macos.

## Caveats
Not all features are gated behind version flags, patches welcome.
A few features are not implemented, grep for "not implemented" and "SKIP".
I'm open to implementing what's left, but it's generally very niche things, such as ioctl callbacks.

## Jurl
`jurl` is a requests-style API, but more complete.
You can use the `jurl/request` function directly (see its documentation for more details),
or the pipeline high level API.

Here are a few basic examples.
```janet
# prepare and execute a GET to pie.dev/get
((http :get "https://pie.dev/get"))
# => {:body "..." :cookies [] :error :ok :handle <jurl> :status 200}
```

```janet
# prepare a POST to pie.dev/post
(def post (http :post "https://pie.dev/post"))
# add a body
(def post-body (body "my body" post))
# set content-type to text/plain
(def post-body-ct (headers {:content-type "text/plain"} post-body))
# launch the request
(post-body-ct)
# => {:body "..." :cookies [] :error :ok :handle <jurl> :status 200}
```

```janet
# equivalent to the previous example
((->> "https://pie.dev/post"
      (http :post)
      (body "my body")
      (headers {:content-type "text/plain"})))
```

```janet
# example of preparing and modifying a request
(def req (->> "https://pie.dev/post"
              (http :post)
              (body "my body")
              (headers {:content-type "application/octet-stream"})))
# oops I want it to be application/json
(def req2 (->> req
               (headers {:content-type "application/json"
                         :custom-header "custom-value"})))
# you can still use the old prepared request
(req) # => {:body ...}
# actually, in the end, I want text/plain
# note that custom-header is still set
(req2 {:headers {:content-type "text/plain"}})
# => {:body ...}
```

For more details, have a read through `jurl/init.janet`.
I promise it's not very complex.
For instance, you'll see how to extend the pipeline yourself with custom functions (`defapi`).

## Jurl/Native
`jurl/native` attempts to implement as much of libcurl's easy API in as direct a manner as possible.

In cases of bitmaps and enums (including magic numbers), they are translated as directly as possible from their C forms into keywords.
For example, `CURLOPT_TCP_FASTOPEN` becomes `:tcp-fastopen`.

Enums (and bitmap members) are translated using a prefix notation.
For example, `CURLPROXY_HTTP` becomes `:proxy/http`.
Ones that are extremely common are not prefixed, such as various `:auth/` keys.

Bitmaps are translated back and forth using indexables (arrays and tuples).
For example, `CURLAUTH_BASIC | CURLAUTH_DIGEST` becomes `[basic digest]`.

Here's an example of a basic POST request using `jurl/native`.
```janet
(import jurl/native)
(def output-body @"")
(def client (native/new))
(put client :post true)
(put client :postfields "mybody")
(put client :httpheader ["my-header: header value"])
(put client :url "https://pie.dev/post")
(put client :writefunction |(buffer/push output-body $))
(:perform client)
(print output-body)
(print (client :response-code))
```
