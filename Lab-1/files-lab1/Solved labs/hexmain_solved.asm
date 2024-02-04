  # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
main:
	li	$a0,0		# change this to test different values

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window
	
stop:	j	stop		# stop after one run
	nop			# delay slot filler (just in case)

  # You can write your own code for hexasc here
  #

hexasc:
	move $t2, $a0

 	bge $t2, 10, letter	# Branch if greater than or equal to 10

 	addi $t2, $t2, 0x30	# ASCII conversion for values less than 10

	move $v0, $t2
	
	jr $ra	# Jump back to subroutine call

letter:
	addi $t2, $t2, 0x37	# ASCII conversion for values greater than or equal to 10

	move $v0, $t2

	jr $ra	# Jump back to subroutine call