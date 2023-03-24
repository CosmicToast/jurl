# writer.janet: utility for consistent :readfunction and mime :data-cb use
(defn- read-cb 
  [self bytes]
  (def start (self :pos))
  (def end (min (length (self :buf)) (+ (self :pos) bytes)))
  (put self :pos end)
  (string/slice (self :buf) start end))

# no safety checks, just for fun
(defn- seek-cb
  [self offset &opt pos]
  (default pos :set)
  (put self :pos (case pos
                   :set offset
                   :cur (+ (self :pos) offset)
                   :end (- (length (self :buf)) offset))))

(defn- make-buf
  [b]
  (assert (bytes? b))
  (def buf (freeze b))
  @{:pos 0
    :buf buf
    :read read-cb
    :seek seek-cb})

# returns a function you can put into :readfunction
(defn make-writer
  [b]
  (def buf (make-buf b))
  |(:read buf $))

# returns a function you can put into mime_data_cb
(defn make-mime-writer
  [b]
  (def buf (make-buf b))
  (fn [mode & args] 
    (assert (keyword? mode))
    (mode buf ;args)))
