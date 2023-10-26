(in-package #:cl-user)

(defpackage #:c3.test.ic3.integer
  (:use #:cl)
  (:shadow #:mod))

(in-package #:c3.test.ic3.integer)

(require :positional)

(setf *print-case* :downcase)

(defparameter *ops* '(+ - * / mod < == > ^ & bor))

(defparameter *su* '(s u))

(defparameter *bits* '(8 16 32 64 128))

(defgeneric == (a b))

(defmethod == (a b)
  (= a b))

(defgeneric ^ (a b))

(defmethod ^ ((a integer) (b integer))
  (logxor a b))

(defgeneric & (a b))

(defmethod & ((a integer) (b integer))
  (logand a b))

(defgeneric bor (a b))

(defmethod bor ((a integer) (b integer))
  (logior a b))

(defgeneric mod (a b))

(defmethod mod (a b)
  (let ((c (* (floor (/ a b)) b)))
    (if (zerop c)
        a
        (- a c))))

(defgeneric translate (x))

(defmethod translate ((x integer))
  x)

(defmethod translate ((x number))
  (truncate x))

(defmethod translate ((x (eql 't)))
  "true")

(defmethod translate ((x null))
  "false")

(defun do-numbers (fn)
  (dolist (su *su*)
    (let ((sign (if (eq su 's) -1 1)))
      (dolist (bits (if (eq su 's) (reverse *bits*) *bits*))
        (let ((su-bits (if (eq su 's) (1- bits) bits)))
          (when (< sign 0)
            (funcall fn (* sign (expt 2 su-bits))))
          (funcall fn (* sign (1- (expt 2 su-bits))))
          (when (> sign 0)
            (funcall fn (* sign (expt 2 su-bits)))))))))

(with-open-file (ret #P"integer.ret.expected"
                     :direction :output
                     :element-type 'character
                     :if-exists :supersede)
  (format ret "0~%"))

(with-open-file (expected #P"integer.out.expected"
                          :direction :output
                          :element-type 'character
                          :if-exists :supersede)
  (with-open-file (in #P"integer.in"
                      :direction :output
                      :element-type 'character
                      :if-exists :supersede)
    (do-numbers
        (lambda (a)
          (flet ((output-base (prefix base)
                   (format expected "~A~A~%"
                           (if (< a 0) "-" "")
                           (positional:positional (abs a) 10))
                   (format in "~A~A~A~%"
                           (if (< a 0) "-" "")
                           prefix
                           (positional:positional (abs a) base))))
            (output-base "0b" 2)
            (output-base "0o" 8)
            (output-base "" 10)
            (output-base "0x" 16))))))

(defgeneric op-to-str (op))

(defmethod op-to-str ((op (eql '+)))
  "add")

(defmethod op-to-str ((op (eql '-)))
  "sub")

(defmethod op-to-str ((op (eql '*)))
  "mul")

(defmethod op-to-str ((op (eql '/)))
  "div")

(defmethod op-to-str ((op (eql '%)))
  "mod")

(defmethod op-to-str ((op (eql '&)))
  "band")

(defmethod op-to-str ((op (eql '^)))
  "bxor")

(defmethod op-to-str ((op (eql '<)))
  "lt")

(defmethod op-to-str ((op (eql '<=)))
  "lte")

(defmethod op-to-str ((op (eql '==)))
  "eq")

(defmethod op-to-str ((op (eql '>)))
  "gt")

(defmethod op-to-str ((op (eql '>=)))
  "gte")

(defmethod op-to-str (op)
  (string-downcase (symbol-name op)))

(dolist (op *ops*)
  (with-open-file (ret (pathname
                        (concatenate
                         'string "integer_" (op-to-str op) ".ret.expected"))
                       :direction :output
                       :element-type 'character
                       :if-exists :supersede)
    (format ret "0~%"))
  (with-open-file (expected (pathname
                             (concatenate
                              'string "integer_" (op-to-str op) ".out.expected"))
                          :direction :output
                          :element-type 'character
                          :if-exists :supersede)
  (with-open-file (in (pathname
                       (concatenate
                        'string "integer_" (op-to-str op) ".in"))
                      :direction :output
                      :element-type 'character
                      :if-exists :supersede)
      (do-numbers
          (lambda (a)
            (do-numbers
                (lambda (b)
                  (format expected "~A ~A ~A~%" a op b)
                  (format expected "~A~%" (translate (funcall op a b)))
                  (format in "quote ~A ~A ~A~%" a op b)
                  (format in "~A ~A ~A~%" a op b))))))))
