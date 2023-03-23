(declare-project
  :name "jurl"
  :description "Janet cUrl")

(declare-native
  :name "jurl/native"
  :cflags [;default-cflags]
  :lflags [;default-lflags "-lcurl"]
  :headers ["src/jurl.h"]
  :source  ["src/main.c"
            "src/jurl.c"
            "src/callbacks.c"
            "src/enums.c"
            "src/errors.c"
            "src/getinfo.c"
            "src/mime.c"
            "src/setopt.c"])

(declare-source
  :source ["jurl"])
