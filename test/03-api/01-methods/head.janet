(use ../../../jurl)
(def res ((->> "https://pie.dev/anything"
               (http :head))))
(assert (= 200 (res :status)))
