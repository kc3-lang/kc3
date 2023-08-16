(in-package :cl-user)

(defparameter *ops* '(+ - * /))

(defparameter *su* '(s u))

(defparameter *bits* '(8 16 32 64 128))

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
    (dolist (op *ops*)
      (do-numbers
          (lambda (a)
            (do-numbers
                (lambda (b)
                  (format expected "~A~%" (truncate (funcall op a b)))
                  (format in "~A ~A ~A~%" a op b))))))))
