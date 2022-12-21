
(in-package :cl-user)

(defpackage :c3.system
  (:use :cl :cpkg))

(in-package :c3.system)

(define-system :c3
  :depends-on (:libmd))
