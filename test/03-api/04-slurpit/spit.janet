(use ../../../jurl)
(import spork/json)

(def data "data")
(def body (-> "https://pie.dev/post"
              (spit data)
              (json/decode true)))
(assert (= data (body :data)))

(def data @{:a "b"})
(def body (-> "https://pie.dev/post"
              (spit (json/encode data) :content-type :application/json)
              (json/decode true)))
(assert (deep= data (body :json)))
