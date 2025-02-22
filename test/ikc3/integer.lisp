;; c3
;; Copyright 2022,2023 kmx.io <contact@kmx.io>
;;
;; Permission is hereby granted to use this software granted the above
;; copyright notice and this permission paragraph are included in all
;; copies and substantial portions of this software.
;;
;; THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
;; PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
;; AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
;; THIS SOFTWARE.

(in-package #:cl-user)

(defpackage #:c3.test.ic3.integer
  (:use #:cl)
  (:shadow #:mod))

(in-package #:c3.test.ic3.integer)

(require :positional)

(setf *print-case* :downcase)

(defparameter *ops1* '(~ -))
(defparameter *ops2* '(+ - * / mod < == > ^ & bor))

;;(defparameter *ops1* '(~))
;;(defparameter *ops2* '())

(defparameter *su* '(s u))

(defparameter *bits*  '(8 16 32 64 128))
(defparameter *~bits* '(8 16 32 64 128 256))

;;(defparameter a 256)

(defgeneric ~ (a))

(defmethod ~ ((a integer))
  (if (< a (expt 2 64))
      (cl:mod (lognot a) (expt 2 (find (integer-length a)
                                       *~bits*
                                       :test (lambda (a b)
                                               (<= a b)))))
      (lognot a)))

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
  (let* ((c (* (floor a b) b))
         (d (- a c)))
    (if (< b 0)
        (if (< 0 d)
            (+ d b)
            d)
        (if (< d 0)
            (+ d b)
            d))))

(mod 3 4)
(mod -3 4)
(mod 3 -4)
(mod -3 -4)

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
  (with-open-file (in #P"integer.kc3"
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

(defgeneric op-to-str (op arity))

(defmethod op-to-str ((op (eql '~)) (arity (eql 1)))
  "bnot")

(defmethod op-to-str ((op (eql '-)) (arity (eql 1)))
  "neg")

(defmethod op-to-str ((op (eql '+)) (arity (eql 2)))
  "add")

(defmethod op-to-str ((op (eql '-)) (arity (eql 2)))
  "sub")

(defmethod op-to-str ((op (eql '*)) (arity (eql 2)))
  "mul")

(defmethod op-to-str ((op (eql '/)) (arity (eql 2)))
  "div")

(defmethod op-to-str ((op (eql '%)) (arity (eql 2)))
  "mod")

(defmethod op-to-str ((op (eql '&)) (arity (eql 2)))
  "band")

(defmethod op-to-str ((op (eql '^)) (arity (eql 2)))
  "bxor")

(defmethod op-to-str ((op (eql '<)) (arity (eql 2)))
  "lt")

(defmethod op-to-str ((op (eql '<=)) (arity (eql 2)))
  "lte")

(defmethod op-to-str ((op (eql '==)) (arity (eql 2)))
  "eq")

(defmethod op-to-str ((op (eql '>)) (arity (eql 2)))
  "gt")

(defmethod op-to-str ((op (eql '>=)) (arity (eql 2)))
  "gte")

(defmethod op-to-str (op arity)
  (format nil "~A-~A" (string-downcase (symbol-name op)) arity))

(dolist (op *ops1*)
  (with-open-file (ret (pathname
                        (concatenate
                         'string "integer_" (op-to-str op 1) ".ret.expected"))
                       :direction :output
                       :element-type 'character
                       :if-exists :supersede)
    (format ret "0~%"))
  (with-open-file (expected (pathname
                             (concatenate
                              'string "integer_" (op-to-str op 1) ".out.expected"))
                          :direction :output
                          :element-type 'character
                          :if-exists :supersede)
  (with-open-file (in (pathname
                       (concatenate
                        'string "integer_" (op-to-str op 1) ".kc3"))
                      :direction :output
                      :element-type 'character
                      :if-exists :supersede)
      (do-numbers
          (lambda (a)
            (format expected "~A ~A~%" op a)
            (format expected "~A~%" (translate (funcall op a)))
            (format in "quote ~A ~A~%" op a)
            (format in "~A ~A~%" op a))))))

(dolist (op *ops2*)
  (with-open-file (ret (pathname
                        (concatenate
                         'string "integer_" (op-to-str op 2) ".ret.expected"))
                       :direction :output
                       :element-type 'character
                       :if-exists :supersede)
    (format ret "0~%"))
  (with-open-file (expected (pathname
                             (concatenate
                              'string "integer_" (op-to-str op 2) ".out.expected"))
                          :direction :output
                          :element-type 'character
                          :if-exists :supersede)
  (with-open-file (in (pathname
                       (concatenate
                        'string "integer_" (op-to-str op 2) ".kc3"))
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
