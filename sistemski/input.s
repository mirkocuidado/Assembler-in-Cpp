.section data:

	.extern c
	.global m, n
	m: .equ s, m-n+p
	p: .skip 15
	n: .equ t, c
	

	.end