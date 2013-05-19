; Yes, technically this is more like Scheme. No, I don't care.

; Basic one line expression
;(cons (quote a) (cons (quote b) nil))

; Test if eq? works, should return #t
;(eq? (quote c) (quote c))

; Test if conditional statements work, which also requires eq?
;(cond
;  [(eq? (quote b) (quote c)) (quote x)]
;  [#t (quote y)])

; Evaluate a lambda expression
;(define x (quote a))
;((lambda (x y) (cons x y)) x (quote b))

(define _ (cons (quote a) (cons (quote b) (cons (quote c) nil))))

((label subst (lambda (x y z)
				(cond
				  [(atom? z) (cond
							   [(eq? y z) x]
							   [#t z])]
				  [#t (cons (subst x y (car z)) (subst x y (cdr z)))]))) (quote a) (quote b) _)

; A more complex expression, this computes the dot product of two vectors w and v
;(define dot (lambda (w v)
;	(cond
;		[(empty? w) 0]
;		[else (+ (* (car w) (car v)) (dot (cdr w) (cdr v)))])))
