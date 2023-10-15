(in-package :cl-user)

(require :positional)

(defparameter *ops* '(+ - * / < == >))

(defparameter *su* '(s u))

(defparameter *bits* '(8 16 32 64 128))

(defun == (a b)
  (= a b))

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
            (output-base "0x" 16))))
    (dolist (op *ops*)
      (do-numbers
          (lambda (a)
            (do-numbers
                (lambda (b)
                  (format expected "~A~%" (translate (funcall op a b)))
                  (format in "~A ~A ~A~%" a op b))))))))
