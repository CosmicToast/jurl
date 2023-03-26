# text helpers
# parsing of text and generation of text
(def keyword-lower (comp keyword string/ascii-lower))

(def header-pegs
  ~{:field-line (* ':field-name ":" :ows ':field-value :ows)
    :field-name :token
    :ws (set " \t")
    :ows (any :ws)
    :field-value (any :field-content)
    :field-content (* :field-vchar
                      (? (some ( * (+ :ws :field-vchar)
                                   :field-vchar))))
    :field-vchar (+ :vchar :obs-text)
    :obs-text (range "\x80\xFF")
    :token (some :tchar)
    :tchar (+ :w #any VCHAR, except delimiters
              (set "!#$%&'*+-.^_`|~"))
    :vchar (+ :tchar
              (set "\"(),/:;<=>?@[\\]{}"))
    :crlf "\r\n"

    # start-line CRLF *( field-line CRLF ) CRLF [ message-body ]
    # curl removes [ message-body ] for us
    # we still have to parse start-line
    # start-line is either a request or a response
    # we will be nice and parse response (what it is most of the time)
    # however, requests are complicated, so we will skip until \r\n if it's not a response
    # technically not 100% compliant since the spec uses "/" to imply that request-line should be tried first, but... I don't care
    :start-line (+ :status-line :request-line)
    :status-line (* :http-version " " :status-code " " (? :reason-phrase) :crlf)
    :http-version (+ :http-version-1)
    :http-version-1 (* :http-name "/" :d "." :d)
    # I couldn't find the spec for this
    :http-version-2 (* :http-name "/2")
    :http-name "HTTP"
    :status-code (repeat 3 :d)
    :reason-phrase (some (+ :ws :vchar :obs-text))
    :request-line (thru :crlf) 

    :field-line-crlf (/ (* :field-line :crlf) ,tuple)
    :main (* :start-line (any :field-line-crlf) :crlf)})
(def header-peg (peg/compile header-pegs))

(defn from-pairs-merge
  [ps]
  (def out @{})
  (each [k v] ps
    (put out k (let [o (out k)]
                 (cond
                    (bytes? o)   (sorted [o v])
                    (indexed? o) (sorted [;o v])
                    v))))
  (freeze out))

# I do not parse content, including the "," joining
# the reason is that I don't want to handle every single header in the ledger
# since they can all change the semantics
# however, if I receive multiple headers of a given type, I join them in a list
(defn parse-headers
  [s]
  (or (-?>> s
            (peg/match header-peg)
            (map (fn [[k v]] [(keyword-lower k) v]))
            (sort-by first)
            from-pairs-merge)
      {}))

(defn- format-header
  [k v]
  (string/format "%s: %s" k v))

# if you give me a list, I will split it into multiple instances of that header
(defn header-list
  [& maps]
  (->> maps
       (mapcat pairs)
       (map (fn [[k v]] (if (indexed? v)
                          (map |(format-header k $) v)
                          (format-header k v))))
       flatten
       sort
       freeze))
