(define hello (rnt-id "hello"))
(define (adder) 
 (let 
  ((x (rnt-pop)) (y (rnt-pop))) 
  (begin 
   (display "Adding ")
   (display x)
   (display " and ")
   (display y)
   (newline)
   (rnt-push (+ x y)))))

(display "hello from scheme!")
(newline)
(rnt-ex hello)
(display (rnt-pop))
(newline)
(rnt-push 456)
