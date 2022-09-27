// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
    @prev
    M=1
(LISTEN)
    @KBD
    D=M
    @prev
    D=D-M
    M=D+M
    @BLACKEN
    D;JGT
    @CLEAR
    D;JLT
    @LISTEN
    D;JMP

(BLACKEN)
    @SCREEN
    D=A
    @addr
    M=D
    @i
    M=1
    @8192
    D=A
    @n
    M=D
(BLOOP)
    @n
    D=M
    @i
    D=M-D
    @LISTEN
    D;JGT
    @addr
    A=M
    M=-1
    @addr
    M=M+1
    @i
    M=M+1
    @BLOOP
    0;JMP

(CLEAR)
    @SCREEN
    D=A
    @addr
    M=D
    @i
    M=1
    @8192
    D=A
    @n
    M=D
(CLOOP)
    @n
    D=M
    @i
    D=M-D
    @LISTEN
    D;JGT
    @addr
    A=M
    M=0
    @addr
    M=M+1
    @i
    M=M+1
    @CLOOP
    0;JMP

