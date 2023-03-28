(use ../../../jurl)
(import spork/json)
(def one (->> "https://pie.dev/cookies"
              (http :get)
              (cookies {:a :b})))
(def two (->> one
              (cookies {:a :c
                        :b :d})))

(def rone (one))
(assert (= 200 (rone :status)))

(def rtwo (two))
(assert (= 200 (rtwo :status)))

(def jone ((json/decode (rone :body) true) :cookies))
(def jtwo ((json/decode (rtwo :body) true) :cookies))

(assert (= (freeze jone) {:a "b"}))
(assert (= (freeze jtwo) {:a "c"
                          :b "d"}))
