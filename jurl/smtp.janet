(setdyn :doc
        ```
        Janet SMTP client based on Jurl.
        Please see `(doc new)` for details on instantiating a client.
        The rest of this documentation talks about the signature of the resulting function.

        `(send headers &opt body &named files options)`
        headers: A dictionary of headers to send. It is heavily processed.
                 :bcc, :cc, :from, :to, and :sender may all either be bytes or lists of bytes.
                 All of them can be either plain addresses "a@b.c"
                 or name references "Name <a@b.c>", with the latter being recommended.

                 The :bcc header is stripped and only used to populate the recipient list.
                 The :to header is obligatory.
                 The :from header is inferred from the client if not set.
                 If :from holds >1 address, :sender is inferred from the client if not set.

                 The :subject header is strongly recommended.
        body: Body may either be a string (in which case it is sent as text/plain),
              or a dictionary mapping from mimetypes to contents.
              Note that you can have multiple at once, which will be delivered as alternative contents.
        files: Files are a list of either strings (in which case they specify files to upload)
               or jurl/mime-compatible mime definitions.
               Note that in the string case, the default mimetype is used (text/plain).
        options: Additional options to set just for this message.
                 Note that these are set last, and thus override anything else the library does.
        ```)

(import jurl/native)
(import ./mime)
(import ./text)

(def- email ~{:addr (+ (* "<" '(to ">") ">")
                       (* (! "<") '(some 1) (! (look -1 ">"))))
              :line (* '(to " <") " " :addr -1)
              :main (+ (/ :line ,|{:name $ :address $1})
                       (/ :addr ,|{:address $}))})

(def- bytes->email (let [parser (peg/compile email)
                         parse  (partial peg/match parser)]
                     (comp first parse)))

(def- email->address  |(assert (get $ :address)))
# version that returns address surrounded by <>s
(def- email->address* |(string "<" (email->address $) ">"))
(def- email->name     |(assert (get $ :name)))
# version that doesn't throw and defaults to the address
(def- email->name*    |(get :name $ (email->address $)))

(defn- email->string
  [e]
  (try
    # name-addr
    (string (email->name e) " " (email->address* e))
    # addr-spec
    ([_] (email->address e))))

# always returns a list
(defn- process-emails
  [sender email]
  (match email
    nil              []
    :me              [sender]
    (s (bytes?   s)) [(bytes->email s)]
    (i (indexed? i)) (flatten (map (partial process-emails sender) i))))

(defn- to-table
  [ds]
  (case (type ds)
    :struct (struct/to-table ds)
    :table  ds
    (error "cannot convert argument to table")))

(def- intercommas |(string ;(interpose ", " (map email->string $))))
(def- not-empty (complement empty?))
(def- len>1 (comp (partial < 1) length))

# body is a string or dictionary
(defn- body->obj
  [newmime body]
  (let [one  (fn [[type data]] {:type type :data data})
        many |(newmime ;(map one (pairs $)))]
    {:headers {:content-disposition :inline}
     :type :multipart/alternative
     :data (case (type body)
             :string    (newmime {:type :text/plain :data body})
             :struct    (many body)
             :table     (many body)
             (error "could not encode body"))}))

(defn- send
  [handle sender headers &opt body &named files options]
  (default body "")
  (let [cl (:dup handle)
        pt (partial put cl)
        newmime (partial mime/new cl)
        me sender
        process-emails (partial process-emails me)
        process-ehdrs  |(process-emails (headers $))
        
        bcc    (process-ehdrs :bcc)
        cc     (process-ehdrs :cc)
        from   (process-ehdrs :from)
        to     (process-ehdrs :to)
        sender (process-ehdrs :sender)
        
        headers (put (to-table headers) :bcc nil)]
    # this is actually the SENDER
    (pt :mail-from (email->address* me))
    # list of <>-enclosed addresses
    (pt :mail-rcpt [;(map email->address* bcc)
                    ;(map email->address* cc)
                    ;(map email->address* to)])

    # as of currently:
    # bcc, cc, from, to, sender are potentially empty lists
    # if from is empty, it becomes me
    (def from (if (empty? from)
                [me]
                from))
    (def sender (cond
                  # empty and from is >1 it becomes me
                  (and (empty? sender) (len>1 from))
                  me
                  
                  # it's empty, from is not >1, we don't need it
                  (empty? sender)
                  nil
                  
                  # take the first
                  (first sender)))

    # handle headers
    (when (not-empty cc)   (put headers :cc (intercommas cc)))
    # from always non-empty
    (put headers :from (intercommas from))
    # to must be set
    (put headers :to (intercommas (assert to "to: must be set")))
    # sender MUST be a single value
    (when sender (put headers :sender (email->string sender)))
    # if there is no date, set it ourselves
    (when (not (headers :date)) (put headers :date (text/format-date)))

    (pt :httpheader (text/header-list headers))

    # body is always set since we default it to ""
    (def mimeparts @[(body->obj newmime body)])
    (def addmime (partial array/push mimeparts))

    # files are either strings (filename), or mimedefs
    (when files (each file files
                  (addmime (case (type file)
                              # very limited since no mimetype is specified
                              :string {:data [:file file]}
                              :struct file
                              :table  file))))
    (pt :mimepost (newmime ;mimeparts))

    (when options (eachp [k v] options
                    (pt k v)))
    (:perform cl)))

(defn new
  ```
  Generate a function that can send emails for a given configuration.
  url: A curl-compatible smtps? url.
       smtps:// is the default, so you can omit that in the common case.
       If you're using smtp:// + startls, make sure you pass {:use-ssl :usessl/all} to options.
  me: This should be your personal identifier.
      This is used as a base for many defaults, such as:
      * Your login username.
      * The From: header.
      * The Sender: header if you have multiple From: addresses.
      * Whenever you insert :me (see module documentation) in a list.
      It should look like so: "First Name <first.name@example.com>".
      However, everything will still work if it's just "first.name@example.com".
  password: This should be the password to log into the smtps? server.
            As of currently, this is the only supported login method.
  There are also the following named parameters:
  options: A dictionary of options to set on the created jurl/native handle.
           These are applied *before* everything else.
           Typical uses are to force use-ssl, set debugfunctions, and verbosity.
  username: This lets you override the username used to authenticate against the SMTP server.
            By default it'll be the address line of `me`.
  Returns a function that should be called as is detailed in the module documentation.

  Examples:
  `(new "gmail.com" "John Doe <jdoe@gmail.com>" "my application password")`
  ```
  [url me password &named options username]
  (def email (try (bytes->email me)
               ([_] (errorf "could not parse email %s" me))))
  (default options {})
  (default username (email :address))

  (def client (native/new))
  (eachp [k v] options
    (put client k v))
  (put client :default-protocol :smtps)
  (put client :url url)
  (put client :username username)
  (put client :password password)
  (partial send client email))
