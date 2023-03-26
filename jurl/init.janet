(import jurl/native)
(import ./mime)
(import ./text)

# global init on import
(let [g (native/global-init)]
  (when (not= :ok g)
    (error (native/strerror g))))

# default options, you can put or overwrite them and future clients will utilize that
(var *default-options* @{:autoreferer      true
                         :default-protocol "https"
                         :followlocation   true
                         :postredir        :redir-post/all
                         :useragent        "Jurl/1.0"})
# useful for queries and x-www-urlencoded
(defn url-encoded
  [dict]
  (-> (->> dict
           pairs
           (map (fn [[k v]] [(native/escape k)
                             (native/escape v)]))
           (map (fn [[k v]] (string/format "%s=%s" k v))))
      (string/join "&")))

(defn request
  [{:auth    auth
    :body    body
    :cookies cookies
    :headers headers
    :method  method
    :options options
    :stream  stream
    :url     url}
   &named handle]

  (assert (bytes? url) "url must be present and a string")
  (def handle (or handle
                  (let [h (native/new)]
                    (eachp [k v] *default-options*
                      (put h k v))
                    h)))
  (def pt (partial put handle))
  (pt :url url)

  (when auth (match auth
               (userpwd (bytes? userpwd))
               (pt :userpwd userpwd)

               ([userpwd methods] (and (bytes? userpwd)
                                       (indexed? methods)))
               (do (pt :userpwd userpwd)
                   (pt :httpauth methods))

               (error "auth must either be a user:password string or [user:password [:valid :methods]]")))

  (when body (match body
               (amime (= :jurl-mime (type amime) :amime))
               (pt :mimepost amime)

               (bytes (bytes? bytes))
               (pt :postfields bytes)

               (cb (function? cb))
               (pt :readfunction cb)

               # here :post is fine because it's only used in post
               (dict (dictionary? dict))
               (pt :postfields (url-encoded dict))
    
               (error "body must either be a mime to do a multipart form submission, a buffer/string, callback function, or dictionary to url-encode")))

  (when cookies
    (pt :cookie (-> (->> cookies
                         pairs
                         (map (fn [[k v]] (string/format "%s=%s;" k v))))
                    (string/join " "))))

  (when headers (cond
                  (dictionary? headers) (pt :httpheader (text/header-list headers))
                  (indexed? headers)    (pt :httpheader headers)
                  (error "headers must be a dictionary or list")))

  (when method (match (text/keyword-lower method)
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
  
  (:perform handle)

  # cookies are complicated for many reasons
  # combine :cookielist and (:headers :set-cookie) to handle yourself
  (freeze {:body (if (function? stream) :unavail res-body)
           :handle  handle
           :headers (text/parse-headers res-hdr)
           :status (handle :response-code)})) 

# request format
(comment {:auth (or
                  userpwd
                  [userpwd method])
          :body (or
                  amime
                  bytes
                  callback
                  dictionary x-www-urlencoded)
          :headers (or
                     indexed
                     dictionary)
          :method  :tag
          :options dictionary
          :stream  callback # do streaming instead of buffering the whole response body
          :url     string})

# response format
(comment {:body (or buffer
                    :unavail) # if stream = true
          :handle  handle
          :headers dictionary
          :status 200})
