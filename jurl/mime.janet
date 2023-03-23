# wrapper around native/mime stuff
(import jurl/native)

(defn- some? [x] (not (nil? x)))
(defmacro- some-do
  [handle sym act]
  ~(when (some? ,sym) (,act ,handle ,sym)))

(defn- mime-part
  [handle {:name     name
           :attach   amime
           :data     data
           :data-cb  data-cb
           :filedata filedata
           :filename filename
           :type     mimetype
           :headers  headers
           :encoder  encoder}]
  (some-do handle name     native/mime-name)
  (some-do handle data     native/mime-data)
  (some-do handle filedata native/mime-filedata)
  (some-do handle filename native/mime-filename)
  (some-do handle mimetype native/mime-type)
  (some-do handle encoder  native/mime-encoder)
  (when (some? data-cb)
    (native/mime-data-cb handle ;data-cb))
  (when (some? amime)
    (:attach amime handle))
  (when (some? headers)
    (native/mime-headers handle (->> headers
                                     (map pairs)
                                     (map (fn [[k v]] (string/format "%s: %s" k v)))
                                     sort
                                     freeze))))

# define a complete mime in one go
(defn new
  [& parts]
  (def out (native/mime-new))
  (each part parts
    (mime-part (:addpart out) part))
  out)

# example
(comment (new {:name data
               :data "a form"}
              {:name file
               :data "pretending to be a file"
               :filename "remote.file"
               :type "application/json"}))
