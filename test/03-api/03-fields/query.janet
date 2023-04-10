(use ../../../jurl)
(import spork/json)

(defn test
  [url queries expect]
  (def r ((->> url
               (http :get)
               (query queries))))
  (assert (= 200 (r :status)))
  (def j (json/decode (r :body) true))
  (def c (freeze (j :args)))
  (assert (= c expect)))

(test "https://pie.dev/get" {:a :b} {:a "b"})
(test "https://pie.dev/get?c=d" {:a :b} {:a "b" :c "d"})
(test "https://pie.dev/get?a=b" {:a :c} {:a ["b" "c"]})
