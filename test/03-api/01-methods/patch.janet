(use ../../../jurl)
(def res ((->> "https://pie.dev/patch"
               (http :patch))))
(assert (= 200 (res :status)))
