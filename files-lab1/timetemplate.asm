  # timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,2
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
 hexasc:
	move	$t2, $a0

 	bge	$t2, 10, letter	# Branch if greater than or equal to 10
 	nop

 	addi	$t2, $t2, 0x30	# ASCII conversion for values less than 10

	move	$v0, $t2
	
	jr	$ra
	nop

letter:
	addi	$t2, $t2, 0x37	# ASCII conversion for values greater than or equal to 10

	move	$v0, $t2

	jr	$ra	# Jump back to call
	nop
	
delay:	
	PUSH 	$ra 	# Save registers
	PUSH 	$a0
		
	move 	$t0, $a0	# Number of miliseconds
	li	$t2, 4711	 # Delay constant
	
	beqz	$t0, exit
	nop
	
whileloop:
	subi 	$t0 ,$t0 1	# ms = ms - 1
	li 	$t3, 0	 # For loop count
forloop:
	addi 	$t3, $t3, 1	# i++
	nop	# Do nothing
	ble	$t3, $t2, forloop	# i < delay constant
	nop
	
	bgtz	$t0, whileloop	# Return to whileloop
	nop
exit:	
	POP	$a0	# Restore registers
	POP	$ra
	jr 	$ra
	nop
time2string:
    # a0, timstr, should change to mm:ss
    # a1, mytime, get time from this

    PUSH $s0 # we want to make sure s0 and s1 are restored
    PUSH $s1 # after the function has finished
    PUSH $ra # we don't want to lose this after calling hexasc

    add $s0, $a0, $zero # storing arguments in s0 and s1 so they can be used
    add $s1, $a1, $zero # when calling hexasc without relying on the stack

    # m---- / mm:ss
    andi $t0, $s1, 0xf000 # isolating
    srl  $t0, $t0, 12     # shift to far right
    add $a0, $t0, $zero   # setting argument
    jal hexasc            # ascii will returned in v0
    nop
    sb $v0, 0($s0)        # storing ascii character in timstr at index 0

    # -m--- / mm:ss
    andi $t0, $s1, 0x0f00 # isolating
    srl  $t0, $t0, 8      # shift to far right
    add $a0, $t0, $zero   # setting argument
    jal hexasc            # ascii will returned in v0
    nop
    sb $v0, 1($s0)        # storing ascii character in timstr at index 1

    # --:-- / mm:ss
    li $t0, 0x3a          # storing colon ascii into t0
    sb $t0, 2($s0)        # storing ascii character in timstr at index 2

    # ---s- / mm:ss
    andi $t0, $s1, 0x00f0 # isolating
    srl  $t0, $t0, 4      # shift to far right
    add $a0, $t0, $zero   # setting argument
    jal hexasc            # ascii will returned in v0
    nop
    sb $v0, 3($s0)        # storing ascii character in timstr at index 3

    # ----s / mm:ss
    andi $t0, $s1, 0x000f # isolating
    srl  $t0, $t0, 0      # shift to far right
    add $a0, $t0, $zero   # setting argument
    jal hexasc            # ascii will returned in v0
    nop
    sb $v0, 4($s0)        # storing ascii character in timstr at index 4

    # null, end-of-string-marker
    li $t0, 0x00          # storing null ascii into t0
    sb $t0, 5($s0)        # storing ascii character in timstr at index 5

    # popping in reverse to push
    POP $ra
    POP $s1
    POP $s0

    jr $ra
    nop
    
	
	
	
	
	
	
	
	
	
