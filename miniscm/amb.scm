
(define lconts '())

(define (eamb a b)
 (call/cc (lambda (ka)
  (call/cc (lambda (kb)
   (set! lconts (cons kb lconts))
   (ka a) )) 
  b
 )) )

(define famb (flambda (a b)
 (call/cc (lambda (ka)
  (call/cc (lambda (kb)
   (set! lconts (cons kb lconts))
   (ka (eval a)) )) 
  (eval b)
 )) ))

(define mlamb (mlambda (m a b)
 (call/cc (lambda (ka)
  (call/cc (lambda (kb)
   (set! lconts (cons kb lconts))
   (ka (eval a)) )) 
  (eval b)
 )) ))

(macro mamb (lambda ((m a b))
 (call/cc (lambda (ka)
  (call/cc (lambda (kb)
   (set! lconts (cons kb lconts))
   (ka a) )) 
  b
 )) ))

(macro m_amb (lambda ((m a b))
 (list 'call/cc (list 'lambda '(ka)
  (list 'call/cc (list 'lambda '(kb)
   '(set! lconts (cons kb lconts))
   (list 'ka a) ))
  b
 )) )) 

(define amb mamb)

(define (end)
 (if (not (pair? lconts))
  (begin
   (display "end: no more continuation
")
   '#f)
  (let ((kb (car lconts)))
   (set! lconts (cdr lconts))
   (kb) ) ) )


