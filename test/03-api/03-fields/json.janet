(use ../../../jurl)
(import spork/json)

(def body {:abc "xyz"
           :foo "bar"})

(def req (->> "https://pie.dev/post"
              (http :post)
              (json body)))
(def res (req))
(assert (= 200 (res :status)))
(def jes (freeze ((json/decode (res :body) true) :json)))

(assert (= body jes))
