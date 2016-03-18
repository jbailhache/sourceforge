
(define (mb s) 
 (call "MessageBox" (val-ext "ghwnd") (unicode-of-ascii s) (unicode-of-ascii "Message") 0))

(mb "Bonjour")

(define null (val-ext "NULL"))
(define uc unicode-of-ascii)
(define vx val-ext)

(mb "wc1")
(define wc1 (alloc (struct-size "WNDCLASS")))

;(mb "lpszClassName")
(pokel (+ wc1 (field-offset "WNDCLASS" "lpszClassName")) (string-adr (uc "class1")))

;(mb "lpfnWndProc")
(pokel (+ wc1 (field-offset "WNDCLASS" "lpfnWndProc")) (vx "WndProc"))

;(mb "style")
(pokel (+ wc1 (field-offset "WNDCLASS" "style"))(logor (vx "CS_VREDRAW") (vx "CS_HREDRAW")))

;(mb "hInstance")
(pokel (+ wc1 (field-offset "WNDCLASS" "hInstance")) (vx "ghInstance"))

;(mb "hbrBackground")
(pokel (+ wc1 (field-offset "WNDCLASS" "hbrBackground")) (call "GetStockObject" (vx "WHITE_BRUSH")))

;(mb (dump wc1 (struct-size "WNDCLASS")))

(mb "RegisterClass")
(if (eqv? 0 (call "RegisterClass" wc1))
	(mb (error-message))
	; (mb (format-int "RegisterClass: Erreur %d" (call "GetLastError"))))
	(mb "Classe enregistree"))

(mb "CreateWindowEx")
(define hwnd1 (call "CreateWindowEx" 
	0
	(uc "class1")
	(uc "Ma fenetre")
	(logor (vx "WS_VISIBLE") (vx "WS_SYSMENU"))
	30 40 250 150
	null 
	null
	(vx "ghInstance")
	null
))

(mb "hedit11")
(define hedit11 (call "CreateWindow1"
	(uc "EDIT")
	(uc "")
	(logor (vx "WS_CHILD") (vx "WS_VISIBLE") (vx "ES_LEFT") (vx "WS_BORDER"))
	15 15 100 20
	hwnd1 null (vx "ghInstance") null))


(define status-settext (call "SendMessage" hedit11 (vx "WM_SETTEXT") 3 (uc "Test")))
(if (eqv? (vx "TRUE") status-settext)
    (mb "Text set")
    (mb (format-int "Erreur SETTEXT %d" status-settext)))

(define hbutton11 (call "CreateWindow1"
	(uc "BUTTON")
	(uc "Hello")
	(logor (vx "WS_CHILD") (vx "WS_VISIBLE") (vx "ES_LEFT") (vx "WS_BORDER"))
	15 40 40 20
	hwnd1 null (vx "ghInstance") null))

(define hbutton12 (call "CreateWindow1"
	(uc "BUTTON")
	(uc "Bye")
	(logor (vx "WS_CHILD") (vx "WS_VISIBLE") (vx "ES_LEFT") (vx "WS_BORDER"))
	60 40 40 20
	hwnd1 null (vx "ghInstance") null))

(call "ShowWindow" hwnd1 (vx "gnCmdShow"))
(call "UpdateWindow" hwnd1)

(define msg (alloc (struct-size "MSG")))

(define (traiter-message)
     (let ((message (peekl (+ (vx "&msgparam") (field-offset "struct msgparam" "message"))))
	   (lparam (peekl (+ (vx "&msgparam") (field-offset "struct msgparam" "lParam"))))
	   (hwnd (peekl (+ (vx "&msgparam") (field-offset "struct msgparam" "hwnd"))))
	  )
       (if (eqv? message (vx "WM_COMMAND"))
	   (begin 
	   (if (eqv? lparam hbutton11)
	       (let ((buf (uc "                              "))) 
		 ; (mb "hbutton11")
		 (call "SendMessage" hedit11 (vx "WM_GETTEXT") 29 buf)
		 ; (mb (ascii-of-unicode buf))
		 (call "SendMessage" hedit11 (vx "WM_SETTEXT") 0 (uc "Bonjour")))
	       (if (eqv? lparam hbutton12)
		   (begin 
		     (call "DestroyWindow" hwnd1)
		     (set! cont #f))
		   ; autre commande
	       )))
	   (if (eqv? message (vx "WM_PAINT"))
	       (if (eqv? hwnd hwnd1)
		     (let ((ps (alloc (struct-size "PAINTSTRUCT")))
			   (rc (alloc (struct-size "RECT")))
			   (brush1 (call "CreateSolidBrush" (scan-int "%X" "B0A070")))
			  )
		    
		       ; (if (eqv? (vx "NULL") brush1) 
		       ;   (mb "Erreur CreateSolidBrush")
		       ;   (mb "CreateSolidBrush correct"))
		       (let ((status-beginpaint (call "BeginPaint" hwnd1 ps)))
		       ;  (if (eqv? status-beginpaint (vx "NULL"))
		       ;     (mb "Erreur BeginPaint")
		       ;     (mb "BeginPaint correct"))
			 (let ((hdc (peekl (+ ps (field-offset "PAINTSTRUCT" "hdc")))))
			;   (mb (format-int "hdc=%d" hdc))
			   (call "GetClientRect" hwnd1 rc)
			   (call "SelectObject" hdc brush1)
			   (call "Rectangle" hdc 20 110 60 130)
			   (call "EndPaint" hwnd1 ps)
		         ))
		      ))
	   ))))
		  
(define cont #f)

(define (message-loop)
 (let ((status (call "GetMessage" msg)))
  (if (not (eqv? 0 status))
   (begin
     (call "TranslateMessage" msg)
     (call "DispatchMessage" msg)
     (traiter-message)
     (if cont (message-loop))))))

(set! cont #t)
;(mb "Message loop")
(message-loop)
(mb "Fin")



   
	


