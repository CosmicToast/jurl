(use ../../../jurl)
(import spork/json)

(def fone "file 1")
(def ftwo "file 2")

# POST
(def one (->> "https://pie.dev/post"
              (http :post)
              # the way to do file uploads in post is with mime/multipart
              (body [{:name "one"
                      :data fone
                      :filename "remote.file"}
                     {:name "two"
                      :data ftwo
                      :filename "other.remote"}])))
(def two (->> one
              # multipart mimes come as a bundle, so this overwrites
              (body [{:name "one"
                      :data fone
                      :filename "remote.file"}])))

(def rone (one))
(assert (= 200 (rone :status)))

(def rtwo (two))
(assert (= 200 (rtwo :status)))

(def jone ((json/decode (rone :body) true) :files))
(def jtwo ((json/decode (rtwo :body) true) :files))

(assert (= fone (jone :one)))
(assert (= ftwo (jone :two)))
(assert (= fone (jtwo :one)))
(assert (= :nothing (get jtwo :two :nothing)))

# PUT
(def pst (->> "https://pie.dev/put"
              (http :put)
              (body-plain fone)))
(def rpst (pst))
(assert (= 200 (rpst :status)))
(def jpst (json/decode (rpst :body) true))
(assert (= fone (jpst :data)))
