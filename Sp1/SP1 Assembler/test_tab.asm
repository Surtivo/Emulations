load 0
store $254
#While:
load 119
sub $255
jz #Up
load 115
sub $255
jz #Down
load 97
sub $255
jz #Left
load 100
sub $255
jz #Right
jnz #While
#Up:
load 0
store $255
load $254
dec
store $254
jmp #While
#Down:
load 0
store $255
load $254
inc
store $254
jmp #While
#Left:
load 0
store $255
load $253
dec
store $253
jmp #While
#Right:
load 0
store $255
load $253
inc
store $253
jmp #While
hlt
