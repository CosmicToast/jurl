# wrapper around native/mime stuff
(import jurl/native)

(setdyn :doc
        "Wrapper around native jurl functions to handle multipart mime form data.")

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
    [:file fname] (:filedata handle (freeze fname))

    # to differentiate against bytes, pass tuple starting with :mime
    [:mime amime] (:subparts handle amime)

    # for callback you must pass [size callback] since mime wants that
    # it's the only thing that wants it, so a number in front is unambiguous
    ([sz cb] (and (number? sz) (function? cb))) (:data-cb handle sz cb)

    # allow empty body
    nil nil

    (error "mime data may only be bytes, jurl-mime, [:bytes bytes], [:file filename], [:mime jurl-mime], or [size callback]"))

  (when name     (:name     handle (freeze name)))
  (when filename (:filename handle (freeze filename)))
  (when mimetype (:type     handle (freeze mimetype)))
  (when encoder  (:encoder  handle encoder))
  (when headers  (:headers  handle
                            (->> headers
                                 pairs
                                 (map (fn [[k v]] (string/format "%V: %V" k v)))
                                 sort
                                 freeze))))

# define a complete mime in one go
(defn new
  ````Generates a new multipart mime handle bound to `handle`.
  Each part denotes a field using a struct that takes the following keys:
  * name: the name of the field
  * data: the contents of the field. Possible options are:
    * `buffer|string`: raw data to use
    * `mime`: the putput of a previous `new` call to use as subparts
    * `[:bytes buffer|string]`: see buffer|string
    * `[:file fname]`: the name of a file whose contents to use as a body.
      Note that this automatically sets filename. You can set it back to null manually.
    * `[:mime mime]`: see mime
  * filename: the remote filename to use.
    This is the only option that determines whether the form specifies a file upload or not.
  * type: the content-type to use for the part.
  * headers: any custom headers to include for this mimepart.
  * encoder: the encoder to use.
    Valid options can be seen in `curl_mime_encoder(3)`.

  For example:
  ```
  (new {:name "data"
        :data "a form"}
       {:name "file"
        :data [:file "local.json"]
        :filename "remote.json"
        :type "application/json"})
  ```
  Specifies a multipart form submission with two fields.
  The "data" field submits "a form" in plaintext.
  The "file" field submits the contents of the "local.json" file to the server
  as if it was called "remote.json",
  and specifies that it's the "application/json" mimetype.
  ````
  [handle & parts]
  (def out (native/new-mime handle))
  (each part parts
    (mime-part (:addpart out) part))
  out)
