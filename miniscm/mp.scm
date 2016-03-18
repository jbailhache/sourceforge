
(define (mp x)
 (if (pair? x)
  (if (eq? ': (car x))
   (list (mp (cdr x)))
   (cons (mp (car x)) (mp (cdr x))) )
  x ) )

(define mpq (flambda (x)
 (mp x) ))

(define empq (flambda x 
 (eval (mp (cons 'begin x))) ))

(define defmp1 (flambda (name value)
  (eval (list 'define name (mp value))) ))

(define defmpq (flambda (name value)
   (list 'define name (mp value)) ))

(macro defmp (lambda ((name value))
 (list 'define name (mp value)) ))

(eval (defmpq snd1 (lambda (x) : car : cdr x)))

(eval (mpq (define (snd x) : car : cdr x)))

