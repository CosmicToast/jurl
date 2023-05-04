# Jurl
Janet Curl (secretly Jean Curl)

A libcurl easy API wrapper for Janet.
It contains the following (public) components:
* `jurl/native`: a low level C interface to curl's `easy` suite.
* `jurl`: an HTTP client based on `jurl/native`, including quick and dirty `slurp` and `spit` functions, an API builder, and a `request` function.
* `jurl/smtp`: an SMTP client based on `jurl/native`.

The goals between the components vary.
`jurl/native` seeks to be unopinionated and as complete as possible, minus the `multi` interface.
The other components seek to be convenient native-feeling clients.

Outside of `jurl/native`, `(doc module)` for any given component were written to be complete and useful.
Of additional use can be the examples.

## Installation
`jpm install https://github.com/cosmictoast/jurl.git`

You can add it to your `project.janet` dependency list like so:
```janet
(declare-project
  :name "my-project"
  :dependencies
  [{:repo "https://github.com/cosmictoast/jurl.git"
    :tag "v1.4.0"}]
```

`jurl` is also available in the `jpm` package list.
This means you can also install it with `jpm install jurl`, and use
`:repo "jurl"` in your dependencies list.

Jurl is a `libcurl` wrapper, and as such needs libcurl (the library at runtime (unless linking statically), and also the headers at build time) installed.
It is tested on the latest release of Fedora Linux and MacOS.

## Caveats
* Not all features are gated behind version flags.
  * You are presumed to have at least curl 7.81.0.
  * Features and flags that are deprecated in the latest release of curl tend to be removed from the bindings.
* Some new features are not yet implemented or skipped. Grep for "SKIP" and "not implemented" to see details.

If you run into issues with building, please see the CONTRIBUTING document - chances are I can help fix it.

## Jurl
`jurl` is an HTTP client.
The following is a quick high-level introduction.
For additional details, see `(doc jurl)`, `(doc jurl/request)` and `jurl/init.janet`.

`slurp` and `spit` are easy to use functions to make basic http requests.
```janet
(slurp "https://pie.dev/get")
# => @`{"args": {} ...}`

(slurp "https://example.com/404")
# => (error 404)

(slurp "https://example.com/nxdomain")
# => (error :error/couldnt-resolve-host)

(spit "https://pie.dev/post" "post body")
# => @`{... "data": "post body" ...}`

(spit "https://pie.dev/post" `{"key": "value"}` :content-type :application/json)
# => @`{... "json": {"key": "value"} ...}`

(spit "https://pie.dev/post" (slurp "https://pie.dev/get") :content-type :application/json)
# => ...
```

`http`, `body`, `headers` and similar functions (all `jurl/` functions besides `request`, `slurp` and `spit`) are the pipeline API.
```janet
# prepare a GET request to pie.dev/get
(def myreq (http :get "https://pie.dev/get"))
# execute the request
(myreq)
# => {:body "..." :cookies [] :error :ok :handle <jurl> :status 200}

# the request can be executed again
(myreq)
# => {:body "..." :cookies [] :error :ok :handle <jurl> :status 200}

# it can also be extended further
(def newreq (headers {:my-header :my-value} myreq)
(newreq)
# => ...

# the name pipeline API comes from idiomatic usage
# this is equivalent to "newreq"
(def newreq2 (->> "https://pie.dev/get"
                  (http :get)
                  (headers {:my-header :my-value})))
```

Finally, there is the `request` function.
It's not strictly intended to be used directly, but rather serves as the backbone to the above.
However, using it directly is valid.
```janet
(request {:url "https://pie.dev/get" :headers {:my-header :my-value}})
# => {:body @`{...}` ...}
```

For implementation details, feel free to read through `jurl/init.janet`.

## Jurl/SMTP
`jurl/smtp` is an SMTP client.
The following is a quick high-level introduction.
For additional details, see `(doc jurl/smtp)`, `(doc smtp/new)` and `jurl/smtp.janet`.

To send emails, you must be authenticated.
As such, the first step is to instantiate a client.
You do this with `smtp/new`.
```janet
(def myclient (smtp/new "smtps://smtp.provider.com"
                        "John Doe <jdoe@provider.com>"
                        "my application password"))
```
The arguments are, in order:
1. The `smtp` or `smtps` URL for the SMTP server that will be delivering your emails on your behalf.
2. Your default identity, as you would specify it in the `from` header.
   * Your full identity line, like `Name Family_Name <email@address>`.
   * Your email address on its own.
3. Your password. `jurl/smtp` does not support oauth2 client flows, and as such you will need to generate application passwords to use it with oauth2-enabled providers.
   This is fairly common, however, and should not cause any problems.
4. The keyword parameters:
   * options: additional options to pass to the `jurl/native` handle at the time of creation.
   * username: if your login username is *not* your email address from 2, you must specify this to log in.

Once you have your client created, you can call it like a function to send emails.
The function signature is `(send headers &opt body &named files options)`.
Of note is that body can be a string if it's of type text/plain, else it should be a dictionary from mimetype to contents.
Details on files and what can go into body values can be found in `(doc jurl/mime)`.
For exact details on what to put into those, see `(doc jurl/smtp)`, but here is a short but complete example.
```janet
(myclient {:to "Some Body <once@told.me>"
           :cc ["First CC <reci@pie.nt>" "second@recipie.nt"]
           :subject "Test Email"}
          {:text/plain "plaintext version of the email body"
           :text/html "<p>html version of the email body</p>"}
          :files ["local.file" {:filename "remote.txt" :data "contents of file"}])
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

Here's an example of a basic HTTP POST request using `jurl/native`.
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
