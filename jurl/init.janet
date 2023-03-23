(import jurl/native)

# = global init

# global init on import
(let [g (native/global-init)]
  (when (not= :ok g)
    (error (native/strerror g))))

# = simple client

(defn- apply-opts
  [handle opts]
  (->> opts
       (pairs)
       (map (fn [[k v]] (:setopt handle k v)))))

# default-options get applied to the simple client on every reset
# they are indeed meant as "defaults"
(var *default-options* @{:default-protocol "https"
                         :followlocation   true
                         :postredir        :redir-post/all
                         :useragent        "Jurl/1.0"})

# the simple client is a tiny iterative layer on top of the native handle
# the latter being preferrable for anyone building their own wrapper
# it's intended for relatively low level use, but still usable directly
# the primary purpose is to allow options to be persisted
(def Simple @{:type "JurlSimpleClient"
              # make sure to set:
              # :handle; output of native/new
              # :options; optional, same as *default-options* but client-specific
              
              :getinfo |(:getinfo ($ :handle) $1)
              :perform (fn [$ &opt opts]
                         (when opts (apply-opts ($ :handle) opts))
                         (:perform ($ :handle))
                         $)
              :reset (fn [$]
                       (:reset ($ :handle))
                       (apply-opts ($ :handle) (merge *default-options* ($ :options)))
                       $)
              :setopt (fn [$ k v]
                        (put ($ :options) k v)
                        (:setopt ($ :handle) k v)
                        $)})

(defn new-simple
  [&opt opts &named handle]
  (default handle (native/new))
  (default opts @{})
  (-> @{:handle handle
        :options opts}
      (table/setproto Simple)
      (:reset)))

# no forward declarations that feel nice but this is a small function so it's fine
(put Simple :dup |(new-simple ($ :options) (:dup ($ :handle))))
