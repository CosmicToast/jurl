(use ../../../jurl)
(import spork/json)

(def hone "header 1")
(def htwo "header 2")

(def one (->> "https://pie.dev/get"
              (http :get)
              (headers {:one hone})))
(def two (->> one
              # headers are additive
              (headers {:two htwo})))

(def rone (one))
(assert (= 200 (rone :status)))

(def rtwo (two))
(assert (= 200 (rtwo :status)))

(def jone ((json/decode (rone :body) true) :headers))
(def jtwo ((json/decode (rtwo :body) true) :headers))

# note: pie.dev Capitalizes-Headers-Like-This on its own
# we actually always lowercase them on send, but receive as-is
(assert (= hone (jone :One)))
(assert (= :nothing (get jone :Two :nothing)))
(assert (= hone (jtwo :One)))
(assert (= htwo (jtwo :Two)))
