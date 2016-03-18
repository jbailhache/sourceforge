
(eval (mp '(begin

(define (mk-typed-ptr ptr type) (cons ptr type))
(define (ptr x) (car x))
(define (type x) (cdr x))

(define (typesize t) :
 ; begin (display (list 'typesize t)) :
 if (eq? (car t) 'int) (cadr t) :
 if (eq? (car t) 'struct)
  (trans (cdr t) 0 : lambda ((fldname typ) n) : + (typesize typ) n) :
 if (eq? (car t) 'ext-struct)
  (struct-size (cadr t)) :
 if (eq? (car t) 'array)
  (* (cadr t) : typesize : caddr t)
 false)

(define (fldofs1 r l fld) :
 ; begin (display (list 'fldofs1 r l fld)) :
 if (not (pair? l)) false :
 if (eqv? fld (caar l)) r :
 fldofs1 (+ r (typesize (cadar l))) (cdr l) fld)

(define (fldofs t fld) :
 if (eq? 'struct (car t)) 
  (fldofs1 0 (cdr t) fld) :
 if (eq? 'ext-struct (car t))
  (field-offset (cadr t) fld)
 false)
 
; contient (cdr t) : lambda ((fld1 ofs1 typ1)) :
; and (eqv? fld1 fld) ofs1)

(define (fldtype t fld) :
 if (not (eq? 'struct (car t))) false :
 contient (cdr t) : lambda ((fld1 typ1)) :
 and (eqv? fld1 fld) typ1)

(define (fldsize t fld)
 (typesize (fldtype t fld)))

(define (getitem item x) :
 if (eq? (car (type x)) 'struct)
  (mk-typed-ptr (+ (ptr x) (fldofs (type x) item)) 
                (fldtype (type x) item)) :
 if (eq? (car (type x)) 'array)
  (mk-typed-ptr (+ (ptr x) (* item : typesize : caddr : type x))
    (caddr : type x)) 
 false)

(define (getvalue x) :
 if (eq? (car (type x)) 'int)
  (if (eqv? (cadr (type x)) 1) 
    (peekb (ptr x)) :
   if (eqv? (cadr (type x)) 2)
    (peekw (ptr x)) :
   if (eqv? (cadr (type x)) 4)
    (peekl (ptr x)) 
   false) 
 false)

(define (setvalue y x) :
 if (eq? (car (type x)) 'int)
  (if (eqv? (cadr (type x)) 1)
    (pokeb (ptr x) y) :
   if (eqv? (cadr (type x)) 2)
    (pokew (ptr x) y) :
   if (eqv? (cadr (type x)) 4)
    (pokel (ptr x) y)
   false)
  false)

(define (new t)
 (mk-typed-ptr (alloc (typesize t)) t))

(define (array n t)
 (list 'array n t))

  
)))


