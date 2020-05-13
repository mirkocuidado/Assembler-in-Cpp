.section a:
MOV a,b
.equ c, 5+10+a-b
d: .equ b,5+10+15
e: .equ f,-d+e-c
m: .equ n,-a+b-c+d-e+10-f
.equ t, m-n+a-b-c-f+d+e-5
.end