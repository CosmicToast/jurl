(import jurl/native)
(import ./mime)
(import ./text)

# used for 'json exclusively
(import spork/json)

(setdyn :doc
        ````Janet cURL: a convenient and complete http client for janet.

        The fastest way to get started is using the `slurp` and `spit` functions.
        `slurp` corresponds to an HTTP GET, and `spit` to an HTTP POST.
        For extra details, see their documentation.

        The medium-level API to build your own higher level one is available
        via the `*default-options*` var and the `request` function.

        For the everyday user, use all of the functions defined using `defapi`.
        All of those return a function that may either be further transformed,
        or called directly to perform the request.

        For example:
        ```
        (slurp "https://pie.dev/get") # also see: spit
        # => response body

        # define a request to pie.dev/post
        (def req (->> "https://pie.dev/post"
                      # it should be a POST
                      (http :post)
                      # pass ?a=b in the url queries
                      (query {:a :b})
                      # pass an x-www-url-encoded body c=d
                      (body {:c :d})
                      # pass a header "my-header: value"
                      (headers {:my-header "value"})))
        # you can now execute the request
        (req) # => {:body ... :status 200}
        # you can execute the request however many times you want
        (req) # => {:body ... :status 200}
        # you can also modify it further
        (def req2 (->> req
                       # pass an *additional* header "my-header2: value2"
                       (headers {:my-header2 "value2"})))
        # the old request is still valid
        (req) # => {:body ... :status 200}
        # the new one is also usable
        (req2) # => {:body ... :status 200}
        # you can quickly perform a lookup without assigning by calling the
        # value immediately
        ((http :get "https://pie.dev/get")) # => {:body ... :status 200}
        ```

        A maximally complete close-to-source native wrapper also available in `jurl/native`.
        ````)

# global init on import
(let [g (native/global-init)]
  (when (not= :ok g)
    (error (native/strerror g))))

# default options, you can put or overwrite them and future clients will utilize that
(var *default-options*
  "Default options to assign to handles automatically created by request."
  @{:autoreferer      true
    # this activates the cookie parser in curl
    :cookiefile       ""
    :default-protocol "https"
    :followlocation   true
    :postredir        :redir-post/all
    :useragent        "Jurl/1.0"})

# useful for queries and x-www-urlencoded
(defn- url-encoded
  [dict]
  (-> (->> dict
           pairs
           (map (fn [[k v]] [(native/escape k)
                             (native/escape v)]))
           (map (fn [[k v]] (string/format "%V=%V" k v))))
      (string/join "&")))

(defn- append-part
  [base add]
  (var slashes 0)
  (when (string/has-suffix? "/" base) (++ slashes))
  (when (string/has-prefix? "/" add)  (++ slashes))
  (case slashes
    0 (string base "/" add)
    1 (string base add)
    2 (string base (slice add 1))))

(defn request
  ````Performs a request as specified by the options.
  The only required parameter is url.

  The other parameters have the following meanings:
  * auth: perform http authentication. Valid forms are:
    * "user:pwd": authenticate with username "user" and password "pwd" using
      the default authentication method.
    * `["user:pwd" [:basic :digest ...]]`: as above, but specify valid
      authentication methods.
      For a list of possible values, see `CURLOPT_HTTPAUTH(3)`.
  * body: sets the body of the request.
    This is full of side-effects, so make sure you set :method if you set this.
    Valid forms are:
    * bytes: the body is set "as-is". Note that this implies setting the method
      to POST and the content-type to x-www-urlencoded.
      Both of those can be overriden using method and headers.
    * (mime/new ...): corresponds to a multipart form submission with the
      contents of passed handle.
    * (fn [bytes] ...): uses the given function as a callback to read from.
      The function must take a number of bytes to write.
      It may return either bytes (a buffer, keyword, or string) or nil to mark
      the body being done.
      If the function returns >bytes bytes, all extranious bytes will be
      truncated, so don't do that.
    * dictionary: the dictionary is urlencoded into key-value pairs
      non-recursively.
      The effective result is similar to passing bytes otherwise.
    * `[...]` equivalent to (mime/new ...) without needing to import jurl/mime.
  * client: a handle to a <jurl> abstract type, gotten from jurl/native/new.
    If this is specified, default options are not applied.
  * cookies: a dictionary of cookies to give to curl.
  * headers: a dictionary of headers to give to curl.
    Any given header value may be a list, in which case multiple headers will
    be sent, one per value in the list.
  * method: a keyword of what HTTP method to use. Valid options are
    get, post, put, head, delete, patch, options, connect and trace.
  * options: a dictionary of options to apply to the handle.
    For the list of available options, consult `curl_easy_setopt(3)`.
    For the list of options that are not implemented, see `src/setopt.c`.
  * parts: a list of parts to append to the url.
    This allows you to make clients more easily.
    As of currently, this does not extract or check for fragment or query.
    As such, using `parts` along with a url that contains either is erroneous.
  * query: a dictionary of query values to add to the url.
    You can both specify query values in the url string and in here.
    In case the same value appears in both, both will appear in the final url,
    and the dictionary value will appear last.
  * stream: if this is a function, instead of buffering the entire body of the
    response, this function will be used as a callback.
    The function must take a buffer, and it must return either the number of
    bytes handled (which must be lower than the size of the buffer given), or
    anything else (which implies having handled the entire buffer).

  The return value is a struct that represents the response.
  It holds the following keys:
  * body: either the body of the response, or :unavail if :stream was used.
  * cookies: a list of cookies as returned by curl. You can also consult the
    set-cookies header field.
  * error: the CURLcode response from curl. :ok under normal circumstances.
    You can get a fancy string out of this using `native/strerror`.
  * handle: the underlying native curl handle.
    You can use this to get additional information using getinfo, or to reuse
    it in a future request.
    Note that reusing it in a future request is very error prone,
    and thus not recommended unless you know what you're doing.
  * headers: a sorted map of received headers. If a given header appears
    multiple times, they are joined into a list.
  * status: the http status code. May not be relevant if error is not :ok.

  A few examples:
  ```
  (request {:url "https://pie.dev/get})
  ```
  Makes a get request to `pie.dev/get`.

  ```
  (request {:url "https://pie.dev/post
            :body {:a "b"
                   :c "d"}})
  ```
  Makes a post form submission to `pie.dev/post` with parameters a and c.

  ```
  (request {:url "https://pie.dev/anything"
            :stream (fn [b] (print b))})
  ```
  Performs a streaming request to "https://pie.dev/anything".
  ````
  [{:auth    auth
    :body    body
    :client  handle
    :cookies cookies
    :headers headers
    :method  method
    :options options
    :parts   parts
    :query   query
    :stream  stream
    :url     url}]

  (assert (bytes? url) "url must be present and a string")
  (def handle (or handle
                  (let [h (native/new)]
                    (eachp [k v] *default-options*
                      (put h k v))
                    h)))
  (def pt (comp freeze (partial put handle)))

  (var url url)
  (when parts (each part parts
                (set url (append-part url part))))

  (when query (set url
                   (string url
                            (if (string/find "?" url) "&" "?")
                            (url-encoded query))))

  (pt :url url)

  (when auth (match auth
               (userpwd (bytes? userpwd))
               (pt :userpwd userpwd)

               ([userpwd methods] (and (bytes? userpwd)
                                       (indexed? methods)))
               (do (pt :userpwd userpwd)
                   (pt :httpauth methods))

               (error "auth must either be a user:password string or [user:password [:valid :methods]]")))

  (var remethod true)
  (when body (match body
               (amime (= :jurl-mime (type amime) :amime))
               (do
                 (set remethod false)
                 (pt :mimepost amime))

               (bytes (bytes? bytes))
               (pt :postfields bytes)

               (cb (function? cb))
               (pt :readfunction cb)

               # here :post is fine because it's only used in post
               (dict (dictionary? dict))
               (pt :postfields (url-encoded dict))

               (indx (indexed? indx))
               (do
                 (set remethod false)
                 (pt :mimepost (mime/new handle ;indx)))

               (error "body must either be a mime to do a multipart form submission, a buffer/string, callback function, a list of mime parts, or dictionary to url-encode")))

  (when cookies
    (pt :cookie (-> (->> cookies
                         pairs
                         (map (fn [[k v]] (string/format "%V=%V;" k v))))
                    (string/join " "))))

  (when headers (cond
                  (dictionary? headers) (pt :httpheader (text/header-list headers))
                  (indexed? headers)    (pt :httpheader headers)
                  (error "headers must be a dictionary or list")))

  (when (and method remethod)
    # some methods always expect a body to be present
    (def emptybody |(when (not body)
                       (pt :readfunction (fn [bytes] ""))))
    (case (text/keyword-lower method)
     :get     (pt :httpget true)
     :post    (do
                # curl expects POST to always have a body
                (emptybody)
                (pt :post true))
     :put     (do
                # curl expects PUT to always have a body
                (emptybody)
                (pt :post true)
                (pt :customrequest "PUT"))
     :head    (pt :nobody true)
     :delete  (pt :customrequest "DELETE")
     :patch   (pt :customrequest "PATCH")
     :options (pt :customrequest "OPTIONS")
     :connect (pt :customrequest "CONNECT")
     :trace   (pt :customrequest "TRACE")
     (error "method must be one of :get :post :put :head :delete :patch :options :connect :trace")))

  (def res-body @"")
  (def res-hdr  @"")
  (pt :headerfunction |(buffer/push res-hdr  $))
  (pt :writefunction (if (function? stream)
                       stream
                       |(buffer/push res-body $)))

  (when options (eachp [k v] options
                  (pt k v)))

  (def curlcode (:perform handle))

  # TODO: cookie list to map, merge given cookies if any
  {:body    (if (function? stream) :unavail res-body)
   :cookies (handle :cookielist)
   :error   curlcode
   :handle  handle
   :headers (text/parse-headers res-hdr)
   :status  (handle :response-code)})

(defn- merge-request
  [orig new]
  (let [mdict |(merge (get orig $ {}) (get new $ {}))
        mlist |[;(get orig $ []) ;(get new $ [])]]
    # by default, we merge
    (merge (merge orig new)
           # custom merge targets are
           {:cookies (mdict :cookies)
            :headers (mdict :headers)
            :options (mdict :options)
            :parts   (mlist :parts)
            :query   (mdict :query)})))

# do as I say, not as I do
# and I say, do not do this
(defmacro- defapi
  [name docstring args & forms]
  (with-syms [$nextfn $opts]
    ~(defn ,name
       ,docstring
       [,;args &opt ,$nextfn]
       (default ,$nextfn request)
       (fn [&opt ,$opts]
         (default ,$opts {})
         (,$nextfn (merge-request
                     (do ,;forms)
                     ,$opts))))))

(defapi append-path
  ````
  Appends a path component to the url.
  This is primarily useful for building clients.

  Note that it is the caller's responsibility to ensure the base url exists
  and does not contain a query or a fragment.

  Example:
  ```
  (def base "https://example.com/api")
  (def base* "https://example.com/api/")
  # all of the below are equivalent to passing "https://example.com/api/users"
  # as the original url
  (append-path base  "users")
  (append-path base  "/users")
  (append-path base* "users")
  (append-path base* "/users")
  # this is equivalent to passing "https://example.com/api/users/me"
  (->> base
       (append-path "users")
       (append-path "me"))
  ```
  ````
  [path]
  {:parts [path]})

(defapi auth
  ``Adds authentication to the request.
  Takes a string in the format "user:password" and a list of allowed methods.

  For a list of possible methods, see `CURLOPT_HTTPAUTH(3)`.

  Example: `(auth "admin:secret" [:basic])`.
  ``
  [userpwd methods]
  {:auth [userpwd methods]})

(defapi body
  ``Adds a body to the request.
  Semantics are identical to the `body` key in `request`.

  Note that this may change the method type.

  Example: `(body "request body")`
  ``
  [b]
  {:body b})

(defapi body-type
  ``Adds a body to the request like `body`,
  while also specifying its content-type.
  ``
  [b ct]
  {:body b :headers {:content-type ct}})

(defapi body-plain
  ``Adds a body to the request like `body`,
  while specifying it to be plain text.
  ``
  [b]
  {:body b :headers {:content-type :text/plain}})

(defapi cookies
  ``Adds any amount of cookies to the request.
  This is *additive*, so any cookies you add do not erase older ones.
  Cookies with the same keys do overwrite previous ones, though.

  Example: `(cookies {:custom-session "57 id"})`
  ``
  [ck]
  {:cookies ck})

(defapi headers
  ``Adds any amount of headers to the request.
  This is *additive*, so any headers you add do not erase previous ones.
  Headers with the same keys do overwrite previous ones, though.

  Example: `(headers {:my-custom-header "my header value"})`
  ``
  [hdrs]
  {:headers hdrs})

# I expect you to use this to start the request chain
(defapi http
  ``Sets the url and method of an http request.
  This is generally expected to be the pipeline entry point.

  Example: `(http :get "https://pie.dev/get")`
  ``
  [method url]
  {:method method
   :url url})

(defapi json
  ``Sets the body of the request to the json representation of ds.
  Also sets the content-type header for you.

  Example: `(json {:json-key "json-value"})`
  ``
  [ds]
  {:body (json/encode ds) :headers {:content-type :application/json}})

(defapi oauth
  ``Sets up authentication for the request using an oauth bearer token.
  Usage of this is not required, you can set custom headers yourself using
  `header` just as well.

  Example: `(oauth "1ab9cb22ba269a7")`
  ``
  [bearer]
  {:options {:httpauth [:bearer] :xoauth2-bearer bearer}})

(defapi query
  ``Adds any amount of queries to the request.
  This is *additive*, so any query arguments you add do not erase older ones.
  Query arguments with the same keys do overwrite previous ones, though.

  Note that overwriting the url does not erase queries.

  Example: `(query {:query "spaces work"})`
  ``
  [qs]
  {:query qs})

(defn- verify-slurpit
  [f &opt opts]
  (def out (f opts))
  (assert (= :ok (out :error)) (out :error))
  (assert (and
            (>= (out :status) 200)
            (<  (out :status) 300))
          (out :status))
  (out :body))

(defn slurp
  ```Performs an HTTP GET to `url`.

  It will throw an error in case the request doesn't succeed,
  else return the request body.
  A failed request is defined as a request that returns a curl error
  or a non-2xx HTTP response code.

  Can also take the following named parameters:
  * opts: Options to intelligently merge with the resulting query.
    To see what can go into opts, see the docs for `request`.
  ```
  [url &named opts]
  (verify-slurpit
    (http :get url)
    opts))

(defn spit
  ```Performs an HTTP POST to `url` with the body `body`.
  By default, uses the `text/plain` content type.

  It will throw an error in case the request doesn't succeed,
  else return the request body.
  A failed request is defined as a request that returns a curl error
  or a non-2xx HTTP response code.

  Can also take the following named parameters:
  * content-type: The content type to set. Defaults to `text/plain`.
  * opts: Options to intelligently merge with the resulting query.
    To see what can go into opts, see the docs for `request`.
  ```
  [url body &named content-type opts]
  (default content-type "text/plain")
  (verify-slurpit
    (->> url
         (http :post)
         (body-type body content-type))
    opts))
