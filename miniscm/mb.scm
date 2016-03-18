
(define (test)
(call "MessageBox"
 (val-ext "ghwnd")
 (unicode-of-ascii "Ceci est un test")
 (unicode-of-ascii "Test")
 (val-ext "MB_OK"))
)
