(setdyn :doc
        "Helper module to help deal with various text-based tasks.")

# text helpers
# parsing of text and generation of text
(def keyword-lower
  ``Takes arbitrary bytes (buffer, keyword, or string), lowercases them,
  then returns them as a keyword.
  This is particularly useful for dealing with headers, which are
  case-insensitive as per the spec.
  ``
  (comp keyword string/ascii-lower))

(def- header-pegs
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
(def- header-peg (peg/compile header-pegs))

(defn- from-pairs-merge
  [ps]
  (def out @{})
  (each [k v] ps
    (put out k (let [o (out k)]
                 (cond
                    (bytes? o)   [o v]
                    (indexed? o) [;o v]
                    v))))
  (freeze out))

# I do not parse content, including the "," joining
# the reason is that I don't want to handle every single header in the ledger
# since they can all change the semantics
# however, if I receive multiple headers of a given type, I join them in a list
(defn parse-headers
  ``Parses a header string (as written by curl's :headerfunction) into a header map.

  Content is not parsed, including to split on ","s.
  However, if multiple headers with the same key are present, they are joined in a list
  Tuples are non-problematic since they do not modify content, unlike the "," approach.

  Output is sorted by key. Multivalue lists are in the order they appeared in.
  ``
  [s]
  (or (-?>> s
            (peg/match header-peg)
            (map (fn [[k v]] [(keyword-lower k) v]))
            (sort-by first)
            from-pairs-merge)
      {}))

(defn- format-header
  [k v]
  (string/format "%V: %V" k v))

# if you give me a list, I will split it into multiple instances of that header
(defn header-list
  ``Transforms a header map into a list compatible with :httpheader.

  Values must either be bytes or a list of bytes.
  In case of list, each entry in the list will be turned into a separate header
  with the same key.
  ``
  [& maps]
  (->> maps
       (mapcat pairs)
       (map (fn [[k v]] (if (indexed? v)
                          (map |(format-header k $) v)
                          (format-header k v))))
       flatten
       freeze))

(def- day-name
  # unfortunately, 0 = Sun under os/date
  ["Sun" "Mon" "Tue" "Wed" "Thu" "Fri" "Sat"])
(def- month-name
  ["Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec"])

(defn format-date
  ``Formats a date for use in Date: headers.
  ``
  []
  (let [d  (os/date)
        dn (day-name   (d :week-day))
        mn (month-name (d :month))
        md (inc (d :month-day))]
    # day-name day month-name year H:M:S
    (string/format "%s, %02d %s %04d %02d:%02d:%02d GMT"
                   dn md mn (d :year)
                   (d :hours) (d :minutes) (d :seconds))))
