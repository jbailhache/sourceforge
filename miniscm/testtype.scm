
(define s1 '(struct (a (int 4)) (b (int 4)) (c (int 4))))
(define x1 (new s1))
(setvalue 1234 (getitem 'b x1))
(getvalue (getitem 'b x1))
