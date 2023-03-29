(use ../../../jurl)
(import spork/json)

(def token "my token")

(def req (->> "https://pie.dev/bearer"
              (http :get)
              (oauth token)))

(def res (req))
(assert (= 200 (res :status)))

(def rtoken ((json/decode (res :body) true) :token))
(assert (= token rtoken))
