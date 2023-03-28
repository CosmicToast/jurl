(use ../../../jurl)
(def res ((->> "https://pie.dev/get"
               (http :get))))
(assert (= 200 (res :status)))
