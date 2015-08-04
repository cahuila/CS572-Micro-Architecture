	.text
	LOAD, A
	MULT, X
	MULT, X
	STO, A
	LOAD, B
	MULT, X
	STO, B
	LOAD, C
	ADD, B
	ADD, A
	STO, C
	END
	
	.data
A:	3
B:	4
C:	5
X:	5


