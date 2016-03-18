; ft.scm - Functional tensors

(define (length l)
 (if (not (pair? l)) 0
  (+ 1 (length (cdr l))) ))

(define (nth l i)
 (if (not (pair? l)) l
  (if (<= i 0) (car l)
   (nth (cdr l) (- i 1)) )))

(define (iter-inter a b) 
 (lambda (x f)
  (if (>= a b) x
   (f a ((iter-inter (+ 1 a) b) x f)) )))

(define (scalar? a)
 (not (procedure? a)))

(define (ft d f)
 (lambda x 
  (if (eq? x '()) d
   (f (car x)) )))

(define (ftexpr x)
 (if (not (pair? x)) x
  (lambda arg
   (if (eq? arg '()) 
    (iter-inter 0 (length x))
    (ftexpr (nth x (car arg))) ))))

(define (exprft x)
 (if (scalar? x) x
  ((x) '() (lambda (i r) (cons (exprft (x i)) r))) ))

(define v1 (ftexpr '(6 4 9)))
(define v2 (ftexpr '(1 0 2 4)))
(define v3 (ftexpr '(3 4 1 9 8 5 7 2 6 8)))

(define a1 (ftexpr '((1 2 3) (4 5 6) (7 8 9))))

(define (diagof a)
 (lambda arg
  (if (eq? arg '())
   ((a 0))
   ((a (car arg)) (car arg)) )))

(define (sum a)
 ((a) 0 (lambda (i r) (iw + (a i) r))))

(define (test a)
 ((a) '() cons))

(define (ftid n) (lambda arg
 (if (eq? arg '())
  (iter-inter 0 n)
  (lambda (i) ) )))

(define (iw op a b)
 (if (scalar? a)
  (if (scalar? b)
   (op a b)
   (lambda arg
    (if (eq? arg '())
     (b)
     (iw op a (b (car arg))) )))
  (if (scalar? b)
   (lambda arg
    (if (eq? arg '())
     (a)
     (iw op (a (car arg)) b)))
   (lambda arg
    (if (eq? arg '())
     (a)
     (iw op (a (car arg)) (b (car arg))) )))))
      
(define (prodex1 op a b)
 (lambda arg
  (if (eq? arg '())
   (a)
   (lambda arg2 
    (if (eq? arg2 '())
     (b)
     (op (a (car arg)) (b (car arg2)))
    )))))
  
(define (prodex op a b)
 (if (scalar? a)
  (if (scalar? b)
   (op a b)
   (lambda arg
    (if (eq? arg '())
     (b)
     (prodex op a (b (car arg))) )))
  (if (scalar? b)
    (lambda arg
     (if (eq? arg '())
      (a)
      (prodex op (a (car arg)) b) ))
    (lambda arg1
     (if (eq? arg1 '())
      (a)
      (lambda arg2
       (if (eq? arg2 '())
        (b)
        (prodex op (a (car arg1)) (b (car arg2))) )))))))

(define (transpose a)
 (if (scalar? a) a
  (if (scalar? (a 0))
   (lambda arg1
    (if (eq? arg1 '())
     (iter-inter 0 1)
     (lambda arg2
      (if (eq? arg2 '())
       (a)
       (a (car arg2)) ))))
   (lambda arg1
    (if (eq? arg1 '())
     ((a 0))
     (lambda arg2
      (if (eq? arg2 '())
       (a)
       ((a (car arg2)) (car arg1)) )))) )))
     
(define (trace a)
 (sum (diagof a)))

(define (prod a b)
 (trace (prodex * a b)))

(define (prodmat a b)
 (prod (transpose a) b))
