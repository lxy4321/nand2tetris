@256
D=A
@SP
M=D
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
M=M-1
@SP
A=M
D=M
@R13
A=M
M=D
(null:LOOP_START)
@ARG
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@0
D=D+A
@R13
AM=D
D=M
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
@LCL
D=M
@0	
D=D+A
@R13
AM=D
D=M
@SP
M=M-1
@SP
A=M
D=M
@R13
A=M
M=D
@ARG
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
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
@ARG
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
M=M-1
@SP
A=M
D=M
@R13
A=M
M=D
@ARG
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@null:LOOP_START
D;JNE
@LCL
D=M
@0
D=D+A
@R13
AM=D
D=M
@SP
A=M
M=D
@SP
M=M+1
