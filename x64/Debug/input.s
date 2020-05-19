.section sekcija:

.extern e1,       e2,        e3           
.global g1,    g2,       g3
.skip 10
.equ k,  e1 + 16
.equ g1, e1
p:     .equ    g2, 10+10+10+8+m
.word g3
g3: .byte e3
.equ  s , g2 - g3
m: .end