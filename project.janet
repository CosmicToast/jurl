(declare-project
  :name "jurl"
  :description "Janet cUrl")

(declare-native
  :name "jurl-native"
  :cflags [;default-cflags]
  :lflags [;default-lflags "-lcurl" "-undefined" "error"]
  :headers ["jurl.h"]
  :source ["main.c" "jurl.c" "jurl_callbacks.c" "jurl_getinfo.c" "jurl_setopt.c"])

(declare-source
  :source "jurl.janet")
