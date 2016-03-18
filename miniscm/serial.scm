
(define (mb s) 
 (call "MessageBox" (val-ext "ghwnd") (unicode-of-ascii s) (unicode-of-ascii "Message") 0))

(mb "com-status")
(define com-status (alloc 4))

(mb "n")
(define n (alloc 4))

(mb "bufread")
(define bufread (make-string 1000 32))

(mb "port-serie")
(define port-serie 
 (call "CreateFile" 
  (unicode-of-ascii "COM5:")
  (logor (val-ext "GENERIC_READ") 
	 (val-ext "GENERIC_WRITE"))
  0
  (val-ext "NULL")
  (val-ext "OPEN_EXISTING")
  0
  (val-ext "NULL")
 ))

(mb "etat-port")
(define etat-port (alloc (struct-size "DCB")))

(mb "GetCommState")
(call "GetCommState" port-serie etat-port)

(mb "DCBlength")
(pokel (+ etat-port (field-offset "DCB" "DCBlength")) (struct-size "DCB"))

(mb "BaudRate")
(pokel (+ etat-port (field-offset "DCB" "BaudRate")) 4800)

(mb "flags")
(pokel (+ etat-port (field-offset "DCB" "BaudRate") 4) 0)

(mb "ByteSize")
(pokeb (+ etat-port (field-offset "DCB" "ByteSize")) 8)

(mb "StopBits")
(pokeb (+ etat-port (field-offset "DCB" "StopBits")) 1)

(if (eqv? 0 (call "SetCommState" port-serie etat-port))
 (mb "Erreur configuration port")
 (begin
  (mb "Port configure")

  (call "SetCommMask" port-serie (val-ext "EV_RXCHAR"))

  (mb "WaitCommEvent")
  (call "WaitCommEvent" port-serie com-status 0)

  (mb "ReadFile")
  (call "ReadFile" port-serie bufread 999 n 0)

  (mb "string-set!")
  (string-set! bufread (pokel n) 0)

  (mb bufread)

  (mb "display")
  (write bufread)

  (mb "CloseHandle")
  (call "CloseHandle" port-serie)

 ))

(mb "fin")

