
(define (mb s) 
 (call "MessageBox" (val-ext "ghwnd") (unicode-of-ascii s) (unicode-of-ascii "Message") 0))

(mb "Bonjour")
	
(define version 257)

(mb "wsadata")
(define wsadata (alloc (struct-size "WSADATA")))

(mb "sa")
(define sa (alloc (struct-size "SOCKADDR_IN")))

(mb "rbuf")
(define rbuf (make-string 1000 32))

(mb "startup")
; (define status (call "WSAStartup" version wsadata))
(define status 0)

(mb "3")

(if (not (eqv? status 0))
 (mb "WSAStartup failed")
 (begin
  (mb "WSAStartup OK")
  (define s (socket
	(val-ext "PF_INET") (val-ext "SOCK_STREAM") (val-ext "IPPROTO_TCP")))
  (mb "test socket")
  (if (eqv? s (val-ext "INVALID_SOCKET"))
   (mb "Cannot create socket")
   (begin
    (mb "Socket created")
    (define remotehost (call "gethostbyname" ;
	"www.eisti.fr"))
	; "pagesperso-orange.fr"))
    (if (eqv? remotehost (val-ext "NULL"))
     (mb "erreur gethostbyname")
     (begin
      (mb "gethostbyname correct")

	(mb "sin_family")
	(pokew (+ sa (field-offset "SOCKADDR_IN" "sin_family")) (val-ext "AF_INET"))

	(mb "sin_port")
	(pokew (+ sa (field-offset "SOCKADDR_IN" "sin_port")) (call "htons" 80))

	; (call "memcpy" (+ sa (field-offset "SOCKADDR_IN" "sin_addr"))
	;	(peekl (+ remotehost (field-offset "struct hostent" "h_addr_list")))
	;	(peekw (+ remotehost (field-offset "struct hostent" "h_length"))))

	(mb "sin_addr")	
	(define sin_addr (+ sa (field-offset "SOCKADDR_IN" "sin_addr")))

	(mb "h_addr")
	(define h_addr (peekl (peekl (+ remotehost (field-offset "struct hostent" "h_addr_list")))))

	(mb "h_length")
	(define h_length (peekw (+ remotehost (field-offset "struct hostent" "h_length"))))

	(mb "memcpy")
	(call "memcpy" sin_addr h_addr h_length)
	
	;(mb "h_addr_list")
	;(define h_addr_list (peekl (+ remotehost (field-offset "struct hostent" "h_addr_list"))))

	(mb (dump sa (struct-size "SOCKADDR_IN")))

	(define r-connect (call "connect" s sa (struct-size "SOCKADDR_IN")))
	(mb (format-int "connect->%ld" r-connect))

	(if (not (eqv? 0 r-connect))
	 
	 ;(mb (format-int "erreur connect errno=%d" (val-ext "errno")))
	 (mb "erreur connect")

	 (begin
	  (mb "connect correct")

	  ; (define buf "GET /mfturpaud/Cafe_philo.htm  ")
	  ; (string-set! buf 30 10)
	  ; (string-set! buf 31 10)

	  (define buf "GET /~bp/doc/reseaux/tp2.html HTTP/1.0  ")
	  (string-set! buf 38 10)
	  (string-set! buf 39 10)

	  (mb "send")
	  (define r-send (call "send" s buf (string-length buf) 0))
	  (mb (format-int "send->%d" r-send))

	  (define r-recv (call "recv" s rbuf 900 0))
	  (mb (format-int "recv->%d" r-recv))

	  (write "received:")	
	  (write rbuf)

       ))    
     ))
   ))
 ))


