(import jurl-native)

(jurl-native/global-init)

# TODO: POST_REDIR
(var *default-options* {:default-protocol "https"
                        :followlocation   true
                        :useragent        "Jurl/1.0"})

# this is a bit janky but also quite efficient
(def-  bodybuf @"")
(defn- write-callback [b]
  (buffer/push bodybuf b))

(def- headerbuf @[])
(defn- header-callback [h]
  (array/push headerbuf h))

(defn- array-move [a]
  (defer (array/clear a)
    (array/slice a)))

(defn- buffer-move [b]
  (defer (buffer/clear b)
    (buffer/slice b)))

# Low level client
(def Low @{:type "JurlLowClient"
           :handle nil # make sure to set this

           # native forwards
           # we want to use reset so this is native reset
           :nreset  |(jurl-native/reset   ($ :handle))
           :dup     |(jurl-native/dup     ($ :handle))
           :perform |(jurl-native/perform ($ :handle))
           :getinfo |(jurl-native/getinfo ($ :handle) $1)
           # we want to use setopt so this is native setopt
           :nsetopt |(jurl-native/setopt  ($ :handle) $1 $2)

           :options @{}
           :setmethod (fn [self method]
                        (case method
                          :get      (:nsetopt self :httpget true)
                          :post     (:nsetopt self :post true)
                          :head (do (:setmethod self :get)
                                    (:nsetopt self :nobody true))
                          :put      (:nsetopt self :upload true)
                          :delete   (:nsetopt self :customrequest "DELETE")
                          :connect  (:nsetopt self :customrequest "CONNECT")
                          :options  (:nsetopt self :customrequest "OPTIONS")
                          :trace    (:nsetopt self :customrequest "TRACE")
                          :patch    (:nsetopt self :customrequest "PATCH")))
           :applyopts (fn [self opts]
                        (->> opts
                             (pairs)
                             (map (fn [[k v]] (:nsetopt self k v)))))
           :setopt (fn [self k v]
                     (put (self :options) k v)
                     (:nsetopt self k v))
           :reset (fn [self]
                    (:nreset self)
                    (:applyopts self (merge *default-options* (self :options)))
                    self)

           # actual work being done for once
           # you don't have to use this one, you can handle this stuff yourself too
           :request (fn [self url &opt opts]
                      (defer (:reset self)
                        (when opts (:applyopts self opts))
                        (:nsetopt self :url url)
                        (:nsetopt self :writefunction write-callback)
                        (:nsetopt self :headerfunction header-callback)
                        (when (:perform self)
                          {:status (:getinfo self :response-code)
                           :body (buffer-move bodybuf)
                           :headers (array-move headerbuf)})))})

(defn make-client
  []
  (-> @{:handle (jurl-native/new)}
      (table/setproto Low)
      (:reset)))

(var *default-client* (make-client))
(defn request [url &opt opts] (:request *default-client* url opts))
