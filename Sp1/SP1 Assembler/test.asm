load 0
store $1 ; $1 = 0
load 1
store $3 ; $3 = 1
load 10 ; acc = 10
#While:
store $2 ; $2 = acc
load $1 ; acc = $1
add $2 ; acc = $2 + $1
store $1 ; $1 = acc
load $2 ; acc = $2
sub $3 ; acc = acc- 1
jnz #While
load $1
hlt
