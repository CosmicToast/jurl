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
(defn- compiles?
  [& flags]
  (let [[inr inw] (os/pipe)
        [_   out] (os/pipe)
        [_   err] (os/pipe)]
    (:write inw "int main(){}")
    (:close inw)
    (zero? (os/execute ["cc" "-xc" "-" "-o/dev/null" ;flags]
                      :p
                      {:err err :in inr :out out}))))
(def pkgconf (let [bin? |(if ((execute $) :status) $ false)
                   bin  (or (bin? "pkgconf") (bin? "pkg-config"))]
               (fn pkgconf
                 [defval & args]
                 (if bin
                   (or (-?>> (-?> bin
                                 (execute ;args)
                                 (get :out)
                                 string/trim)
                            (string/split " ")
                            (filter |(< 0 (length $))))
                       defval)
                   defval))))
(def {:cflags  curl-cflags
      :ldflags curl-ldflags}
  (let [cflags  (partial pkgconf []         "libcurl" "--cflags")
        ldflags (partial pkgconf ["-lcurl"] "libcurl" "--libs")
        curl-c  (cflags)
        curl-l  (ldflags)
        scurl-c (cflags  "--static")
        scurl-l (ldflags "--static")]
    (if (compiles? ;scurl-c ;scurl-l)
      {:cflags  scurl-c
       :ldflags scurl-l}
      {:cflags  curl-c
       :ldflags curl-l})))

(declare-native
  :name "jurl/native"
  :cflags  [;default-cflags  ;curl-cflags]
  :ldflags [;default-ldflags ;curl-ldflags]
  :headers ["src/jurl.h"]
  :source  ["src/main.c"
            "src/jurl.c"
            "src/callbacks.c"
            "src/cleanup.c"
            "src/enums.c"
            "src/errors.c"
            "src/getinfo.c"
            "src/mime.c"
            "src/polyfill.c"
            "src/setopt.c"
            "src/util.c"])
