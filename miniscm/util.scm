
(define false '#f)
(define true '#t)

(define (mp x)
 (if (not (pair? x)) x
  (if (eq? (car x) ':)
   (list (mp (cdr x)))
   (cons (mp (car x)) (mp (cdr x))))))

(eval (mp '(begin

(define (contient l f) :
 and (pair? l) : 
 or (f : car l) :
 contient (cdr l) f)

(define (trans l x f) :
 if (not : pair? l) x :
 f (car l) : trans (cdr l) x f)



)))
