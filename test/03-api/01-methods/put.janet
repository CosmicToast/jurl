(use ../../../jurl)
(def res ((->> "https://pie.dev/put"
               (http :put))))
(assert (= 200 (res :status)))
