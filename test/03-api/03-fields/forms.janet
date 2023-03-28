(use ../../../jurl)
(import spork/json)

(def fone "form 1")
(def ftwo "form 2")

# mime/multipart
(def one (->> "https://pie.dev/post"
              (http :post)
              (body [{:name "one"
                      :data fone}
                     {:name "two"
                      :data ftwo}])))
(def two (->> one
              (body [{:name "one"
                      :data fone}])))

(def rone (one))
(assert (= 200 (rone :status)))

(def rtwo (two))
(assert (= 200 (rtwo :status)))

(def jone ((json/decode (rone :body) true) :form))
(def jtwo ((json/decode (rtwo :body) true) :form))

(assert (= fone (jone :one)))
(assert (= ftwo (jone :two)))
(assert (= fone (jtwo :one)))
(assert (= :nothing (get jtwo :two :nothing)))

# x-www-urlencoded
(def one (->> "https://pie.dev/post"
              (http :post)
              (body {:one fone
                     :two ftwo})))
(def two (->> one
              # body similarly gets overwritten
              (body {:one fone})))

(def rone (one))
(assert (= 200 (rone :status)))

(def rtwo (two))
(assert (= 200 (rtwo :status)))

(def jone ((json/decode (rone :body) true) :form))
(def jtwo ((json/decode (rtwo :body) true) :form))

(assert (= fone (jone :one)))
(assert (= ftwo (jone :two)))
(assert (= fone (jtwo :one)))
(assert (= :nothing (get jtwo :two :nothing)))
