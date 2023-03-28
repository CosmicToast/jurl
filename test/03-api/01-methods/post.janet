(use ../../../jurl)
(def res ((->> "https://pie.dev/post"
               (http :post))))
(assert (= 200 (res :status)))
