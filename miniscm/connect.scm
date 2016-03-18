
(define (mb s) 
 (call "MessageBox" (val-ext "ghwnd") (unicode-of-ascii s) (unicode-of-ascii "Message") 0))

(define PF_INET (val-ext "PF_INET"))
(define SOCK_STREAM (val-ext "SOCK_STREAM"))
(define IPPROTO_TCP (val-ext "IPPROTO_TCP")) 

(define (connect s hostname port)
  (if (eqv? s (val-ext "INVALID_SOCKET"))
   (mb "Invalid socket")
   (begin  
    (define remotehost (call "gethostbyname" hostname))
    (if (eqv? remotehost (val-ext "NULL"))
     (mb "erreur gethostbyname")
     (let ((sa (alloc (struct-size "SOCKADDR_IN"))))	
	(pokew (+ sa (field-offset "SOCKADDR_IN" "sin_family")) (val-ext "AF_INET"))
	(pokew (+ sa (field-offset "SOCKADDR_IN" "sin_port")) (call "htons" 80))	
	(define sin_addr (+ sa (field-offset "SOCKADDR_IN" "sin_addr")))
	(define h_addr (peekl (peekl (+ remotehost (field-offset "struct hostent" "h_addr_list")))))
	(define h_length (peekw (+ remotehost (field-offset "struct hostent" "h_length"))))
	(call "memcpy" sin_addr h_addr h_length)	
	(define r-connect (call "connect" s sa (struct-size "SOCKADDR_IN")))
              r-connect)))))
