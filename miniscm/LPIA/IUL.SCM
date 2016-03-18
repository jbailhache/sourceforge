

(define consp pair?)
(define eq eq?)
; (define progn begin)
(define print write)

; (define extdp (lambda (x)
;       (if (consp x)
;               (if (eq (car x) ':) (list (extdp (cdr x)))
;                       (cons (extdp (car x)) (extdp (cdr x))) )
;               x)))

; (define mp (lambda (x)
;       (extdp x) ))

(define mp (lambda (x)
	(if (consp x)
		(if (eq (car x) ':) (list (mp (cdr x)))
			(cons (mp (car x)) (mp (cdr x))) )
		(if (eq x '^) 'lambda
		 (if (eq x 'def) 'define
		  (if (eq x 'progn) 'begin
		   x ) ) ) ) ))

(define mapkar1 (lambda (l f)
	(if (not (consp l)) l
		(cons (f (car l))
			(mapkar1 (cdr l) f) ) ) ))

(eval (mp '(begin

	(define mapkar : lambda (l f) :
		if (not : consp l) l :
		cons (f : car l) :
		mapkar (cdr l) f)

	(def mapkan : ^ (l f) :
		if (not : consp l) l :
		append (f : car l) :
		mapkan (cdr l) f)

	(def mapk : ^ (l f) :
		if (not : consp l) '() :
		begin (f : car l) :
		mapk (cdr l) f)

	(def == : ^ (x f) : f x)

	(def contient : ^ (l f) :
		if (not : consp l) '#f :
		or (f : car l) :
		contient (cdr l) f)

	(def extraire : ^ (l p) :
		if (not : consp l) l :
		if (p : car l) (cons (car l) : extraire (cdr l) p) :
		extraire (cdr l) p)

	(def trans : ^ (l x f) :
		if (not : consp l) x :
		f (car l) : trans (cdr l) x f)

	(def parcours : ^ (c l f) :
		and (consp l) :
		c (f : car l) :
		parcours c (cdr l) f)

	(def ensemble : ^ (egal l) :
		union egal l '())

	(def union : ^ (egal l e) :
		trans l e : ^ (x r) :
		if (contien r : ^ (y) : egal y x) r :
		cons x r)

	(def vide : ^ (x) : not : consp x)

	(def trier : ^ (ordre l) :
		interclasser ordre l '())

	(def interclasser : ^ (ordre lnt lt) :
		trans lnt lt : ^ (x l) :
		inserer ordre x l)

	(def inserer : ^ (ordre x l) :
		if (vide l) (list x) :
		if (ordre x : car l) (cons x l) :
		cons (car l) : inserer ordre x : cdr l)

	(def fusionner : ^ (ordre lt1 lt2) :
		if (vide lt1) lt2 :
		if (vide lt2) lt1 :
		if (ordre (car lt1) (car lt2))
			(cons (car lt1) : fusionner ordre (cdr lt1) lt2)
			(cons (car lt2) : fusionner ordre lt1 (cdr lt2)) )

	(def atom : ^(x) : not : consp x)

	(def equal : ^(x y) : 
		if (atom x) (eq? x y) :
		and (equal (car x) (car y))
		    (equal (cdr x) (cdr y)))
)))

(define newtop (lambda ()
	(print '>)
;       (print (eval (mp (read))))
	(define topread (mp (read)))
	(if (eq topread 'exit)
		(begin
			(print 'bye)
			(newline) )
		(begin
			(define result (eval topread))
			(print '=)
			(print result)
			(newline)
			(newtop) ) )
))

; (newtop)


(define LOAD '(READFILE EXEC))

(define CADR '(CDR CAR))

(define def '(C QUOTE (C QUOTE (ECH DECLSYM SETDEF))))

(define ALT2 '(B ALT1 K))

(define SEND '(GETCTX-ENLINSTR (INSTR-SEND SETCTXS)))
(define RECEIVE '(GETCTX-ENLINSTR (INSTR-RECEIVE SETCTXS)))
(define NEWCNL '(Q () Q () CONS Q () CONS Q () CONS Q CANAL CONS))

(define LOOP 'Y)
(define EXIT 'KI)

(define PREMINSTR '(CDR CAR CAR))

(define AJINSTR '(
	DES DES 
	GETH0 CDR CDR 
	GETH0 CDR CAR GETH1 CONS CONS
	GETH0 CAR CONS
	MONDEP MONDEP))

(define ENLINSTR '(
	DES 
	GETH0 CDR CDR
	GETH0 CDR CAR CDR CONS
	GETH0 CAR CONS
	MONDEP))

(define EMPIL '(
	DES DES
	GETH0 CDR CDR CDR
	GETH0 CDR CDR CAR GETH1 CONS CONS
	GETH0 CDR CAR CONS
	GETH0 CAR CONS
	MONDEP MONDEP))

(define THROW '(
	ECH EMPIL SETCTX))

(eval (mp '(begin

(define VALUEINCTX '(
	DES DES
	(GETCTX-ENLINSTR :
	 GETH0
		Q THROW AJINSTR
		ECH AJINSTR
		Q QUOTE AJINSTR
		GETH1 AJINSTR
	 SETCTX)
	MONDEP MONDEP))

(define freeze '(
	GETCTX-ENLINSTR KI))

(define UNFREEZE '(
	REP ENLINSTR ECH PREMINSTR
	VALUEINCTX))

(define TEST-STOP '(Q STOP1 EQ))

(define STOP '(STOP1 STOP1))

(define EVOL 'REVOL1)
 
(define EVOL-LCTXS 
	'(GETLLCTXS GETLCTXS CONS SETLLCTXS
	  REP CDR SETLCTXS CAR EVOL
          REP NCONSPTHEN (DEP GETLCTXS) (GETLCTXS ECH CONS)
	  GETLLCTXS REP CDR SETLLCTXS CAR SETLCTXS))

(define INTERP 
	'(LOOP : 
	  REP NCONSPTHEN EXIT : 
	  REP CAR PREMINSTR TEST-STOP THEN EXIT EVOL-LCTXS))

(LPIA '(Q () SETLCTXS Q () SETLLCTXS Q () ELPIA))

)))



(eval (mp '(begin
 
(define LIST1 '(Q () ECH CONS))

(define /REPEVOL '(
	INTERP 
	REP NCONSPTHEN I :
	REP CDR ECH CAR ENLINSTR CONS))

(define /APPLIQ '(
	Q () Q STOP CONS ECH CONS AJINSTR LIST1 /REPEVOL))

(define /APPREMINSTR '(
	REP ENLINSTR ECH PREMINSTR /APPLIQ))

(define *REPEVOL '(
	/REPEVOL REP NCONSPTHEN I :
	REP CDR *REPEVOL ECH CAR CONS))

(define *APPLIQ '(
	Q () Q STOP CONS ECH CONS AJINSTR LIST1 *REPEVOL))

(define *APPREMINSTR '(
	REP ENLINSTR ECH PREMINSTR *APPLIQ))

(define MAPKAR '(
	C Q (DES DES DES 
		(GETH0 NCONSPTHEN GETH0
		    (GETH0 CAR GETH1 GETH2 EXEC 
			(GETH0 CDR GETH1 Q () GETH2 CONS Q MAPKAR CONS EXEC
				(ECH CONS) 
			)
		    )
	      )
		MONDEP MONDEP MONDEP)))

(define values '(
	GETCTX-ENLINSTR : (REP *APPREMINSTR Q () MAPKAR (DEP SOMPIL) 
				 ECH ENLINSTR ECH EMPIL) 
				 SETCTX))
	
(define SOMPIL '(CDR CDR CAR CAR))

(define cut '(GETCTX-ENLINSTR : /APPREMINSTR CAR SETCTX))

(define bloc '(GETCTX-ENLINSTR : *APPREMINSTR SETCTXS))

(define hyp '(
	GETCTX-ENLINSTR : REP /APPREMINSTR NCONSPTHEN END :
	ENLINSTR SETCTX))

(define prolognot '(
	GETCTX-ENLINSTR : REP /APPREMINSTR NCONSPTHEN (ENLINSTR SETCTX)
	END))

(define prologif '(
	GETCTX-ENLINSTR : DES 
		GETH0 ENLINSTR ENLINSTR ENLINSTR 
		GETH0 PREMINSTR
		*APPLIQ 
		REP NCONSPTHEN
			(GETH0 ENLINSTR ENLINSTR SETCTX) :
		GETH0 CDR CAR CDR CAR MAPKAR AJINSTR SETCTXS))

)))






	 

(eval (mp '(begin

(LPIA '(Q EMPTY_ENV SETENV Q () ELPIA))

(define CALT2 '(C ALT2))

(define UNIFPROLOG '(GETENV UNIF REP THEN SETENV END))

(define CLAUSE '(ARG args : ARG corps :
	Q (RENAME REP DES CAR UNIFPROLOG
           DES GETH1 CDR EXEC :
           GETH1 CAR MON UNIFPROLOG
           GETENV REP CONSTR SETENV
           MON VARIABLES GETENV UNBINDVENV SETENV)
	GETVQ corps GETVQ args CONS CONS Q Q CONS Q REP CONS LIST1
	Q APPLYTO CONS))

(define defclause '(C Q : C Q : C Q :
		    ARG corps : ARG args : ARG pred :
	GETVQ corps GETVQ args CLAUSE ARG clause :
	GETVQ pred DECLSYM GETDEF NOT
		THEN (GETVQ clause GETVQ pred SETDEF) :
	Q () GETVQ clause CONS GETVQ pred GETDEF CONS Q ALT CONS 
		GETVQ pred SETDEF))


(LPIA '(

	(defclause plappend (() (VAR l) (VAR l)) I)

	(defclause plappend (((VAR x) . (VAR a)) (VAR b) ((VAR x) . (VAR c))) :
		plappend (Q ((VAR a) (VAR b) (VAR c))))

Q () ELPIA))

)))

(LPIA '(

Q "util.lpi" LOAD 

TOP

Q exit ELPIA))


