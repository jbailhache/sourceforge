
(eval (mp '(begin

(define (var? x) : 
 and (pair? x) : eq? 'var : car x)

(define (varname x) : cadr x)

(define (makevar name) : list 'var name)

(define (van? x) : 
 and (var? x) :
 eq? '() : varname x)

(define (setof x) :
 if (not (pair? x)) x :
 if (member (car x) : cdr x)
  (setof : cdr x) :
 cons (car x) : setof : cdr x)

(define (varlist x)  :
 if (not (pair? x)) '() :
 if (var? x) (list (varname x)) :
 append (varlist : car x) (varlist : cdr x))

(define (varset x) :
  setof : varlist x)

(define (subst x y z) :
 if (eq? y z) x :
 if (not (pair? z)) z :
 cons (subst x y (car z)) (subst x y (cdr z)))

(define (rename1 x vars) :
 if (not (pair? vars)) x :
 rename1 (subst (gensym) (car vars) x)
  (cdr vars))

(define (rename x) :
 rename1 x (varset x))

(define (link env var val) :
 addvenv env var val)

(define (getrecvenv env var) :
 if (not (boundvenv env var))
  (makevar var) :
 let ((val : getvenv env var)) :
 if (not (var? val)) val :
 getrecvenv env : varname val)

(define (build env x) :
 if (var? x) 
  (getrecvenv env (varname x)) :
 if (not (pair? x)) x :
 cons (build env (car x)) (build env (cdr x)))

(define occurs-check '#t)

(define (unify1 env x y) :
 if (or (van? x) (van? y) (equal? x y)) env :
 if (and (var? x) : or (not occurs-check) (not : memq (varname x) (varset y))) 
  (link env (varname x) y) :
 if (and (var? y) : or (not occurs-check) (not : memq (varname y) (varset x)))
  (link env (varname y) x) :
 if (or (not : pair? x) (not : pair? y)) '#f :
 unify (unify1 env (car x) (car y)) (cdr x) (cdr y))

(define (unify env x y) :
 if (not env) env :
 unify1 env (build env x) (build env y)) 

(define clause : flambda (args body) :
 lambda (env x) :
 let ((ab : rename : cons args body)) :
 let ((env1 : unify env (car ab) x)) :
 if (not env1) (end) :
 let ((env2 : (eval (cdr ab)) env1)) :
 let ((env3 : unify env2 x (car ab))) :
 if (not env3) (end) :
 let ((env4 : build env3 env3)) :
 unbindvenv env4 : varset ab)

(define plequal : clause ((var x) (var x)) : lambda (env) env)

(define plappend : lambda (env args) :
 amb ((clause (() (var a) (var a)) 
          (lambda (env) env) ) env args)
     ((clause (((var x) . (var a)) (var b) ((var x) . (var c)))
       (lambda (env) : plappend env '((var a) (var b) (var c))) ) env args ) )

(define parent : lambda (env args) :
 amb ((clause (Allan Bob) (lambda (e) e)) env args)  :
 amb ((clause (Bob Cindy) (lambda (e) e)) env args)
     ((clause (Bob Charles) (lambda (e) e)) env args))

(define grandparent : 
 clause ((var x) (var y)) : 
 lambda (e) :
  let ((e1 : parent e '((var x) (var z)))) :
  parent e1 '((var z) (var y)))


)))




