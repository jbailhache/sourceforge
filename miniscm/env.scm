
(eval (mp '(begin

(define empty-env '#t)

(define (boundvenv env var) :
 if (not (pair? env)) '#f :
 if (eq? var : caar env) '#t :
 boundvenv (cdr env) var)

(define (getvenv env var) :
 if (not (pair? env)) '() :
 if (eq? var : caar env) (cadar env) :
 getvenv (cdr env) var)

(define (setvenv env var val) :
 if (not (pair? env)) (cons (list var val) env) :
 if (eq? var : caar env) 
  (cons (cons var : cons val : cddar env) : cdr env) :
 cons (car env) : setvenv (cdr env) var val)

(define (addvenv env var val) :
 if (not (pair? env)) (cons (list var val) env) :
 if (eq? var (caar env)) 
  (cons (cons var : cons val : cdar env) : cdr env) :
 cons (car env) : addvenv (cdr env) var val)

(define keepvars '#f)

(define (remvenv env var) :
 if (not (pair? env)) env :
 if (eq? var : caar env)
  (let ((x : cddar env)) :
   if (and (not keepvars) (not (pair? x))) (cdr env) :
   cons (cons var x) : cdr env) :
 cons (car env) : remvenv (cdr env) var)

(define (getvsenv env vars) :
 if (not (pair? vars)) (getvenv env vars) :
 if (and (eq? (car vars) 'quote) (pair? (cdr vars))) 
  (cadr vars) :
 cons (getvsenv env (car vars)) (getvsenv env (cdr vars)))

(define (bindvenv env vars vals) :
 if (not vars) env :
 if (not (pair? vars)) (addvenv env vars vals) :
 let ((a : if (pair? vals) (car vals) vals)
      (b : if (pair? vals) (cdr vals) vals)) :
 bindvenv (bindvenv env (car vars) a) (cdr vars) b)

(define (unbindvenv env vars) :
 if (not vars) env :
 if (not (pair? vars)) (remvenv env vars) :
 unbindvenv (unbindvenv env (car vars)) (cdr vars))

)))


