(declare-project
  :name "jurl"
  :description "Janet cUrl"
  :author "Chloe Kudryavtsev <toast@bunkerlabs.net>"
  :license "Unlicense"
  :repo "https://github.com/CosmicToast/jurl.git"
  :dependencies ["spork"])

(declare-native
  :name "jurl/native"
  :cflags [;default-cflags]
  :lflags [;default-lflags "-lcurl"]
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

(declare-source
  :source ["jurl"])
