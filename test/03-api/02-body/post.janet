(use ../../../jurl)
(import spork/json)
(def body "hi")
(def res ((->> "https://pie.dev/anything"
               (http :post)
               (body-plain body))))
(assert (= 200 (res :status)))
(def bres (json/decode (res :body) true))
(assert (= "POST" (bres :method)))
(assert (= body (bres :data)))
(assert (= "text/plain" (get-in bres [:headers :Content-Type])))
