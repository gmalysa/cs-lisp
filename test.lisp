; Yes, technically this is more like Scheme. No, I don't care.

; Basic one line expression
(cons (quote a) (cons (quote b) nil))

; Test if eq? works, should return #t
(eq? (quote c) (quote c))

; Test if conditional statements work, which also requires eq?
(cond
  [(eq? (quote b) (quote c)) (quote x)]
  [#t (quote y)])


; A more complex expression, this computes the dot product of two vectors w and v
;(define dot (lambda (w v)
;	(cond
;		[(empty? w) 0]
;		[else (+ (* (car w) (car v)) (dot (cdr w) (cdr v)))])))
