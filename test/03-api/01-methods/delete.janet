(use ../../../jurl)
(def res ((->> "https://pie.dev/delete"
               (http :delete))))
(assert (= 200 (res :status)))
