# wrapper around native/mime stuff
(import jurl/native)

(defn- mime-part
  [handle {:name     name
           :data     data
           :filename filename
           :type     mimetype
           :headers  headers
           :encoder  encoder}]
  (match data
    # easily differentiable api
    (b (bytes? b)) (:data handle b)
    (amime (= (type amime) :jurl-mime)) (:subparts handle amime)

    # symmetric api, the above is a shortcut
    [:bytes b] (:data handle b)

    # to differentiate against bytes, pass tuple starting with :file
    [:file fname] (:filedata fname)

    # to differentiate against bytes, pass tuple starting with :mime
    [:mime amime] (:subparts handle amime)

    # for callback you must pass [size callback] since mime wants that
    # it's the only thing that wants it, so a number in front is unambiguous
    ([sz cb] (and (number? sz) (function? cb))) (:data-cb handle sz cb)

    # allow empty body
    nil nil

    (error "mime data may only be bytes, jurl-mime, [:bytes bytes], [:file filename], [:mime jurl-mime], or [size callback]"))

  (when name     (:name     handle name))
  (when filename (:filename handle filename))
  (when mimetype (:type     handle mimetype))
  (when encoder  (:encoder  handle encoder))
  (when headers  (:headers  handle
                            (->> headers
                                 pairs
                                 (map (fn [[k v]] (string/format "%s: %s" k v)))
                                 sort
                                 freeze))))

# define a complete mime in one go
(defn new
  [handle & parts]
  (def out (native/new-mime handle))
  (each part parts
    (mime-part (:addpart out) part))
  out)

# example
(comment (new {:name "data"
               :data "a form"}
              {:name "file"
               :data [:file "local.file"]
               :filename "remote.file"
               :type "application/json"}))
