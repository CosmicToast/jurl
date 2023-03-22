(declare-project
  :name "jurl"
  :description "Janet cUrl")

(declare-native
  :name "jurl-native"
  :cflags [;default-cflags]
  :lflags [;default-lflags "-lcurl"]
  :headers ["jurl.h"]
  :source ["main.c" "jurl.c" "jurl_callbacks.c" "jurl_enums.c" "jurl_errors.c" "jurl_getinfo.c" "jurl_setopt.c"])

(declare-source
  :source "jurl.janet")
