@256
D=A
@SP
M=D
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue0
D;JEQ
@SP
A=M-1
M=0
@exit0
0; JMP
(isTrue0)
@SP
A=M-1
M=-1
(exit0)
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue1
D;JEQ
@SP
A=M-1
M=0
@exit1
0; JMP
(isTrue1)
@SP
A=M-1
M=-1
(exit1)
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue2
D;JEQ
@SP
A=M-1
M=0
@exit2
0; JMP
(isTrue2)
@SP
A=M-1
M=-1
(exit2)
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue3
D;JLT
@SP
A=M-1
M=0
@exit3
0; JMP
(isTrue3)
@SP
A=M-1
M=-1
(exit3)
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue4
D;JLT
@SP
A=M-1
M=0
@exit4
0; JMP
(isTrue4)
@SP
A=M-1
M=-1
(exit4)
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue5
D;JLT
@SP
A=M-1
M=0
@exit5
0; JMP
(isTrue5)
@SP
A=M-1
M=-1
(exit5)
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue6
D;JGT
@SP
A=M-1
M=0
@exit6
0; JMP
(isTrue6)
@SP
A=M-1
M=-1
(exit6)
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue7
D;JGT
@SP
A=M-1
M=0
@exit7
0; JMP
(isTrue7)
@SP
A=M-1
M=-1
(exit7)
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@isTrue8
D;JGT
@SP
A=M-1
M=0
@exit8
0; JMP
(isTrue8)
@SP
A=M-1
M=-1
(exit8)
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
@53
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M+D
@SP
A=M-1
M=D
@112
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@SP
A=M-1
M=D
@0
D=A
@SP
A=M
A=A-1
M=D-M
@SP
AM=M-1
D=M
A=A-1
D=M&D
@SP
A=M-1
M=D
@82
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M|D
@SP
A=M-1
M=D
@SP
A=M
A=A-1
M=!M
