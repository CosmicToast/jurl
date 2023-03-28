(import jurl/native)
(import ./mime)
(import ./text)

(setdyn :doc
        ``Janet cURL: a convenient and complete http client for janet.

        Important syms are:
        * `*default-options*`
        * request

        A maximally complete close-to-source native wrapper also available in `jurl/native`.
        ``)

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
    :query   query
    :stream  stream
    :url     url}]

  (assert (bytes? url) "url must be present and a string")
  (def handle (or handle
                  (let [h (native/new)]
                    (eachp [k v] *default-options*
                      (put h k v))
                    h)))
  (def pt (partial put handle))

  (if query
    (pt :url (string url
                     (if (string/find "?" url) "&" "?")
                     (url-encoded query)))
    (pt :url url))

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
    (case (text/keyword-lower method)
     :get     (pt :httpget true)
     :post    (pt :post true)
     :put     (pt :upload true)
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
  (let [mdict |(merge (get orig $ {}) (get new $ {}))]
    # by default, we merge
    (merge (merge orig new)
           # custom merge targets are
           {:cookies (mdict :cookies)
            :headers (mdict :headers)
            :options (mdict :options)
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

# TODO: defapi json; can I do this without depending on spork?
(comment (defapi json
           ``Sets the body of the request to the json representation of ds.
           Also sets the content-type header for you.

           Example: `(json {:json-key "json-value"})`
           ``
           [ds]
           {:body (json/encode ds)
            :headers {:content-type "application/json"}}))

(defapi oauth
  ``Sets up authentication for the request using an oauth bearer token.
  Usage of this is not required, you can set custom headers yourself using
  `header` just as well.

  Example: `(oauth "1ab9cb22ba269a7")`
  ``
  [bearer]
  {:options {:xoauth2-bearer bearer}})

(defapi query
  ``Adds any amount of queries to the request.
  This is *additive*, so any query arguments you add do not erase older ones.
  Query arguments with the same keys do overwrite previous ones, though.

  Note that overwriting the url does not erase queries.

  Example: `(query {:query "spaces work"})`
  ``
  [qs]
  {:query qs})
