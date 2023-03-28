(use ../../../jurl)
(def res ((->> "https://pie.dev/anything"
               (http :options))))
(assert (= 200 (res :status)))
