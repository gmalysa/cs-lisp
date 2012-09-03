; Yes, technically this is more like Scheme. No, I don't care.

; Basic one line expression
(cons (quote a) (cons (quote b) nil))

; Multi-line expression
(cons (quote b)
	(cons (quote c)
		nil))

; A more complex expression, this computes the dot product of two vectors w and v
(define dot (lambda (w v)
	(cond
		[(empty? w) 0]
		[else (+ (* (car w) (car v)) (dot (cdr w) (cdr v)))])))
