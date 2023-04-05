(use ../../../jurl)
(import spork/json)

(def body (-> "https://pie.dev/get"
              (slurp :opts {:headers {:a :b}})
              (json/decode true)))
(assert (= "b"
           (get-in body [:headers :A] :not-b)))
