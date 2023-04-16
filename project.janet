(declare-project
  :name "jurl"
  :description "Janet cUrl"
  :author "Chloe Kudryavtsev <toast@bunkerlabs.net>"
  :license "Unlicense"
  :repo "https://github.com/CosmicToast/jurl.git"
  :dependencies ["spork"])

(declare-source
  :source ["jurl"])

# --- jurl/native
(defn- execute
  [& args]
  (try
    (let [p (os/spawn args :p {:out :pipe :err :pipe})
          r (os/proc-wait p)]
      {:status r
       :out    (:read (p :out) :all)
       :err    (:read (p :err) :all)})
    ([e] {:status false
          :error  e})))
(def pkgconf (let [bin? |(if ((execute $) :status) $ false)
                   bin  (or (bin? "pkgconf") (bin? "pkg-config"))]
               (fn pkgconf
                 [defval & args]
                 (if bin
                   (string/split " " (-> bin
                                         (execute ;args)
                                         (get :out)
                                         (string/trim)))
                   defval))))
(def curl-cflags  (pkgconf []
                           "libcurl" "--cflags" "--static"))
(def curl-ldflags (pkgconf ["-lcurl"]
                           "libcurl" "--libs" "--static"))

(declare-native
  :name "jurl/native"
  :cflags  [;default-cflags ;curl-cflags]
  :ldflags [;default-lflags ;curl-ldflags]
  :headers ["src/jurl.h"]
  :source  ["src/main.c"
            "src/jurl.c"
            "src/callbacks.c"
            "src/cleanup.c"
            "src/enums.c"
            "src/errors.c"
            "src/getinfo.c"
            "src/mime.c"
            "src/setopt.c"
            "src/util.c"])
