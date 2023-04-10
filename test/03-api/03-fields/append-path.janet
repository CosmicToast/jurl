(use ../../../jurl)

(def base (http :get "https://pie.dev/status"))
(defn- check-code
  [code]
  (def out ((append-path (string code) base)))
  (assert (out :status) code) code)
(loop [code :range [200 210]]
  (check-code code))
