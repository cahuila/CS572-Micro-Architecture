
	.text

main:	
		LI $1, 0
		LI $2, 10
		Li $3, 100
		LI $4, 1000
		LI $5, 7

loop:	
		SUBI $5, $5, 1
		ADD $1, $1, $2
		ADD $1, $1, $3
		ADD $1, $1, $4
		BGE  $5, $0, 320
		NOP

		ADD $4, $0, $1
		LI $2, 1
		SYSCALL

		LI $2, 10
		SYSCALL
		END
	