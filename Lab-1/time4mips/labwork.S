  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
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
.globl  delay   # Makes it refrenceable from other files
delay:	
	PUSH 	$ra 	# Save registers
	PUSH 	$a0
		
	move 	$t0, $a0	# Number of miliseconds
	li	$t2, 5211	 # Delay constant
	
	blez	$t0, exit
	nop
	
whileloop:
	addi 	$t0 ,$t0, -1	# ms = ms - 1
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
.globl  time2string  # Makes it refrenceable from other files
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
                          # number is already in the right position, therefore a shift isn't needed
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
