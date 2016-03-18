.model tiny
.stack 07000h
extrn _scanhex
extrn _printhex
extrn _sscandec
extrn _printdec
extrn _getword
extrn _f_fopen
extrn _f_fclose
extrn _f_fgetc
extrn _f_fputc
extrn _fillmem
extrn _dmpcode
extrn _initcvm
extrn _getsva
.code
start proc
 mov ax,cs
 mov ds,ax
 mov ss,ax
 mov es,ax
 mov ax,07000h
 mov sp,ax
 call main
 mov ah,0
 int 21h
start endp
sax: dw 0
sdx: dw 0
scx: dw 0
ssi: dw 0
sbx: dw 0
sdi: dw 0
main proc
 lea ax,[stk]
 mov bx,ax
 lea ax,[cod]
 mov di,ax
 lea ax,[lw]
 mov [word ptr sax],ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getsva
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[word ptr sax]
 mov [si+4h],ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_initcvm
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 lea si,[mode]
 mov ax,01h
 mov [si],ax
mainloop:
 lea si,[mode]
 mov ax,[si]
 cmp ax,0
 je lmla
 call beginim
lmla:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 call execword
 lea si,[mode]
 mov ax,[si]
 cmp ax,0
 je lmlb
 call endim
lmlb:
 mov ax,00h
 cmp ax,0
 je mainloop
execword:
 mov ax,[si+04h]
 mov cx,ax
 mov ax,[si+08h]
 mov dx,si
 mov si,cx
 call si
 ret
 align 4
mode:
 db 04h dup (0)
align 4
w1: dw 0
 dw 0
 dw c1
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n1
 dw 0
n1: db 'NOP'
 db 0
 align 4
c1:
 ret
align 4
w2: dw w1
 dw 0
 dw c2
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n2
 dw 0
n2: db 'EXECCODE'
 db 0
 align 4
c2:
 lea si,[cod]
 call si
 ret
align 4
w3: dw w2
 dw 0
 dw c3
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n3
 dw 0
n3: db 'EXECWORD'
 db 0
 align 4
c3:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[si+04h]
 mov si,ax
 call si
 ret
align 4
w4: dw w3
 dw 0
 dw c4
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n4
 dw 0
n4: db 'PUTCODE'
 db 0
 align 4
c4:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_scanhex
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov [di],ax
 inc di
 ret
align 4
w5: dw w4
 dw 0
 dw c5
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n5
 dw 0
n5: db 'HERE'
 db 0
 align 4
c5:
 mov ax,di
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b70-a70
a70:
 sub bx,2
 mov [bx],ax
b70:
 ret
align 4
w6: dw w5
 dw 0
 dw c6
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n6
 dw 0
n6: db 'SP@'
 db 0
 align 4
c6:
 lea bp,_fillmem
 call bp
 db b71-a71
a71:
 mov ax,bx
 sub bx,2
 mov [bx],ax
b71:
 ret
align 4
w7: dw w6
 dw 0
 dw c7
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n7
 dw 0
n7: db 'SP!'
 db 0
 align 4
c7:
 lea bp,_fillmem
 call bp
 db b72-a72
a72:
 mov ax,[bx]
 add bx,2
 mov bx,ax
b72:
 ret
align 4
w8: dw w7
 dw 0
 dw c8
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n8
 dw 0
n8: db '@'
 db 0
 align 4
c8:
 lea bp,_fillmem
 call bp
 db b73-a73
a73:
 mov si,[bx]
 add bx,2
 mov ax,[si]
 sub bx,2
 mov [bx],ax
b73:
 ret
align 4
w9: dw w8
 dw 0
 dw c9
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n9
 dw 0
n9: db 'C@'
 db 0
 align 4
c9:
 lea bp,_fillmem
 call bp
 db b74-a74
a74:
 mov si,[bx]
 add bx,2
 mov al,[byte ptr si]
mov ah,0
 sub bx,2
 mov [bx],ax
b74:
 ret
align 4
w10: dw w9
 dw 0
 dw c10
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n10
 dw 0
n10: db '!'
 db 0
 align 4
c10:
 lea bp,_fillmem
 call bp
 db b75-a75
a75:
 mov si,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 mov [si],ax
b75:
 ret
align 4
w11: dw w10
 dw 0
 dw c11
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n11
 dw 0
n11: db 'C!'
 db 0
 align 4
c11:
 lea bp,_fillmem
 call bp
 db b76-a76
a76:
 mov si,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 mov [byte ptr si],al
b76:
 ret
align 4
w12: dw w11
 dw 0
 dw c12
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n12
 dw 0
n12: db 'KEY'
 db 0
 align 4
c12:
 lea bp,_fillmem
 call bp
 db b77-a77
a77:
 mov ah,1
 int 21h
 mov ah,0
 sub bx,2
 mov [bx],ax
b77:
 ret
align 4
w13: dw w12
 dw 0
 dw c13
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n13
 dw 0
n13: db 'EMIT'
 db 0
 align 4
c13:
 lea bp,_fillmem
 call bp
 db b78-a78
a78:
 mov ax,[bx]
 add bx,2
 mov dx,ax
 mov ah,2
 int 21h
b78:
 ret
align 4
w14: dw w13
 dw 0
 dw c14
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n14
 dw 0
n14: db 'HEX.'
 db 0
 align 4
c14:
 lea bp,_fillmem
 call bp
 db b79-a79
a79:
 mov ax,[bx]
 add bx,2
 push ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_printhex
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 inc sp
 inc sp
b79:
 ret
align 4
w15: dw w14
 dw 0
 dw c15
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n15
 dw 0
n15: db '.'
 db 0
 align 4
c15:
 lea bp,_fillmem
 call bp
 db b80-a80
a80:
 mov ax,[bx]
 add bx,2
 push ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_printdec
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 inc sp
 inc sp
b80:
 ret
align 4
w16: dw w15
 dw 0
 dw c16
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n16
 dw 0
n16: db 'HEXLIT'
 db 0
 align 4
c16:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_scanhex
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b81-a81
a81:
 sub bx,2
 mov [bx],ax
b81:
 ret
align 4
w17: dw w16
 dw 0
 dw c17
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n17
 dw 0
n17: db 'DROP'
 db 0
 align 4
c17:
 lea bp,_fillmem
 call bp
 db b82-a82
a82:
 mov ax,[bx]
 add bx,2
b82:
 ret
align 4
w18: dw w17
 dw 0
 dw c18
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n18
 dw 0
n18: db 'DUP'
 db 0
 align 4
c18:
 lea bp,_fillmem
 call bp
 db b83-a83
a83:
 mov ax,[bx]
 add bx,2
 sub bx,2
 mov [bx],ax
 sub bx,2
 mov [bx],ax
b83:
 ret
align 4
w19: dw w18
 dw 0
 dw c19
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n19
 dw 0
n19: db 'SWAP'
 db 0
 align 4
c19:
 lea bp,_fillmem
 call bp
 db b84-a84
a84:
 mov ax,[bx]
 add bx,2
 mov dx,[bx]
 add bx,2
 sub bx,2
 mov [bx],ax
 sub bx,2
 mov [bx],dx
b84:
 ret
align 4
w20: dw w19
 dw 0
 dw c20
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n20
 dw 0
n20: db 'ROT'
 db 0
 align 4
c20:
 lea bp,_fillmem
 call bp
 db b85-a85
a85:
 mov ax,[bx]
 add bx,2
 mov cx,ax
 mov dx,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 sub bx,2
 mov [bx],dx
 mov dx,cx
 sub bx,2
 mov [bx],dx
 sub bx,2
 mov [bx],ax
b85:
 ret
align 4
w21: dw w20
 dw 0
 dw c21
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n21
 dw 0
n21: db 'OVER'
 db 0
 align 4
c21:
 lea bp,_fillmem
 call bp
 db b86-a86
a86:
 mov ax,[bx]
 add bx,2
 mov dx,[bx]
 add bx,2
 sub bx,2
 mov [bx],dx
 sub bx,2
 mov [bx],ax
 sub bx,2
 mov [bx],dx
b86:
 ret
align 4
w22: dw w21
 dw 0
 dw c22
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n22
 dw 0
n22: db 'TUCK'
 db 0
 align 4
c22:
 lea bp,_fillmem
 call bp
 db b87-a87
a87:
 mov ax,[bx]
 add bx,2
 mov dx,[bx]
 add bx,2
 sub bx,2
 mov [bx],ax
 sub bx,2
 mov [bx],dx
 sub bx,2
 mov [bx],ax
b87:
 ret
align 4
w23: dw w22
 dw 0
 dw c23
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n23
 dw 0
n23: db '+'
 db 0
 align 4
c23:
 lea bp,_fillmem
 call bp
 db b88-a88
a88:
 mov ax,[bx]
 add bx,2
 mov dx,[bx]
 add bx,2
 add ax,dx
 sub bx,2
 mov [bx],ax
b88:
 ret
align 4
w24: dw w23
 dw 0
 dw c24
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n24
 dw 0
n24: db '-'
 db 0
 align 4
c24:
 lea bp,_fillmem
 call bp
 db b89-a89
a89:
 mov dx,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 sub ax,dx
 sub bx,2
 mov [bx],ax
b89:
 ret
align 4
w25: dw w24
 dw 0
 dw c25
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n25
 dw 0
n25: db 'AND'
 db 0
 align 4
c25:
 lea bp,_fillmem
 call bp
 db b90-a90
a90:
 mov dx,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 and ax,dx
 sub bx,2
 mov [bx],ax
b90:
 ret
align 4
w26: dw w25
 dw 0
 dw c26
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n26
 dw 0
n26: db 'XOR'
 db 0
 align 4
c26:
 lea bp,_fillmem
 call bp
 db b91-a91
a91:
 mov dx,[bx]
 add bx,2
 mov ax,[bx]
 add bx,2
 xor ax,dx
 sub bx,2
 mov [bx],ax
b91:
 ret
align 4
w27: dw w26
 dw 0
 dw c27
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n27
 dw 0
n27: db 'ASR'
 db 0
 align 4
c27:
 lea bp,_fillmem
 call bp
 db b92-a92
a92:
 mov ax,[bx]
 add bx,2
 sar ax,1
 sub bx,2
 mov [bx],ax
b92:
 ret
align 4
w28: dw w27
 dw 0
 dw c28
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n28
 dw 0
n28: db 'RUN'
 db 0
 align 4
c28:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[si+04h]
 mov si,ax
 call si
 ret
align 4
w29: dw w28
 dw 0
 dw c29
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n29
 dw 0
n29: db 'INIT'
 db 0
 align 4
c29:
align 4
w30: dw w29
 dw 0
 dw c30
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n30
 dw 0
n30: db 'EXIT'
 db 0
 align 4
c30:
caldef:
 push cx
 mov cl,0BEh
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b93-a93
a93:
 call si
b93:
 ret
calldef:
 push cx
 mov cl,0BEh
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b94-a94
a94:
 call si
b94:
 ret
defw:
 db 04h dup (0)
align 4
w31: dw w30
 dw 0
 dw c31
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n31
 dw 0
n31: db ':'
 db 0
 align 4
c31:
 lea si,[svrc]
 mov ax,[si]
 mov di,ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,si
 lea si,[defw]
 mov [si],ax
 mov si,ax
 lea ax,[calldef]
 mov [si+4h],ax
 mov ax,di
 mov [si+8h],ax
 lea bp,_fillmem
 call bp
 db b95-a95
a95:
b95:
 lea si,[mode]
 mov ax,00h
 mov [si],ax
 ret
align 4
w32: dw w31
 dw 0
 dw c32
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n32
 dw 0
n32: db 'MACRO:'
 db 0
 align 4
c32:
 lea si,[svrc]
 mov ax,[si]
 mov di,ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,di
 mov [si+4h],ax
 mov ax,00h
 mov [si+8h],ax
 lea bp,_fillmem
 call bp
 db b96-a96
a96:
b96:
 lea si,[mode]
 mov ax,00h
 mov [si],ax
 ret
align 4
w33: dw w32
 dw 0
 dw c33
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n33
 dw 0
n33: db ';'
 db 0
 align 4
c33:
 lea bp,_fillmem
 call bp
 db b97-a97
a97:
 ret
b97:
 lea si,[mode]
 mov ax,01h
 mov [si],ax
 call beginim
 ret
 align 4
adrim:
 db 04h dup (0)
align 4
w34: dw w33
 dw 0
 dw c34
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n34
 dw 0
n34: db '[['
 db 0
 align 4
c34:
 lea si,[adrim]
 mov ax,di
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b98-a98
a98:
b98:
 ret
align 4
w35: dw w34
 dw 0
 dw c35
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n35
 dw 0
n35: db ']]'
 db 0
 align 4
c35:
 lea bp,_fillmem
 call bp
 db b99-a99
a99:
 ret
b99:
 lea si,[adrim]
 mov si,[si]
 call si
 lea si,[adrim]
 mov si,[si]
 mov di,si
 ret
 align 4
svrc:
 db 04h dup (0)
beginim:
 lea si,[svrc]
 mov ax,di
 mov [si],ax
 lea ax,[imcod]
 mov di,ax
 lea bp,_fillmem
 call bp
 db b100-a100
a100:
b100:
 ret
endim:
 lea bp,_fillmem
 call bp
 db b101-a101
a101:
 ret
b101:
 lea si,[svrc]
 mov ax,[si]
 mov di,ax
 call imcod
 ret
align 4
w36: dw w35
 dw 0
 dw c36
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n36
 dw 0
n36: db '[*'
 db 0
 align 4
c36:
 call beginim
 ret
align 4
w37: dw w36
 dw 0
 dw c37
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n37
 dw 0
n37: db '*]'
 db 0
 align 4
c37:
 call endim
 ret
align 4
w38: dw w37
 dw 0
 dw c38
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n38
 dw 0
n38: db '['
 db 0
 align 4
c38:
 call beginim
 lea si,[mode]
 mov ax,01h
 mov [si],ax
 ret
align 4
w39: dw w38
 dw 0
 dw c39
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n39
 dw 0
n39: db ']'
 db 0
 align 4
c39:
 call endim
 lea si,[mode]
 mov ax,00h
 mov [si],ax
 ret
 align 4
imcod:
 db 0100h dup (0)
align 4
w40: dw w39
 dw 0
 dw c40
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n40
 dw 0
n40: db 'SVA'
 db 0
 align 4
c40:
 lea bp,_fillmem
 call bp
 db b102-a102
a102:
 mov [word ptr sax],ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getsva
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[word ptr sax]
 mov ax,si
 sub bx,2
 mov [bx],ax
b102:
 ret
align 4
w41: dw w40
 dw 0
 dw c41
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n41
 dw 0
n41: db 'IMMEDIATE'
 db 0
 align 4
c41:
 lea si,[defw]
 mov si,[si]
 mov ax,[si+08h]
 mov [si+4h],ax
 ret
getparam:
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b103-a103
a103:
 sub bx,2
 mov [bx],ax
b103:
 ret
align 4
w42: dw w41
 dw 0
 dw c42
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n42
 dw 0
n42: db '[CREATE]'
 db 0
 align 4
c42:
code_icreate:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 lea ax,[getparam]
 mov [si+4h],ax
 mov ax,di
 mov [si+8h],ax
 ret
align 4
w43: dw w42
 dw 0
 dw c43
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n43
 dw 0
n43: db 'CREATE'
 db 0
 align 4
c43:
 lea ax,[code_icreate]
 push cx
 mov cl,0BEh
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b104-a104
a104:
 call si
b104:
 ret
getrz:
 lea bp,_fillmem
 call bp
 db b105-a105
a105:
 sub bx,2
 mov [bx],ax
b105:
 ret
align 4
w44: dw w43
 dw 0
 dw c44
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n44
 dw 0
n44: db 'DOES>'
 db 0
 align 4
c44:
 lea bp,_fillmem
 call bp
 db b106-a106
a106:
 mov [word ptr sax],ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getsva
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[word ptr sax]
 mov ax,[si+04h]
 mov si,ax
b106:
 mov ax,00h
 mov dx,di
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b107-a107
a107:
 mov [si+4h],ax
 ret
b107:
 mov ax,di
 mov di,dx
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b108-a108
a108:
 mov [si+4h],ax
 ret
 lea si,[getparam]
 call si
b108:
 mov ax,00h
 mov dx,di
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b109-a109
a109:
 lea si,[calldef]
 call si
 ret
b109:
 mov ax,di
 mov di,dx
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b110-a110
a110:
 lea si,[calldef]
 call si
 ret
b110:
 ret
align 4
w45: dw w44
 dw 0
 dw c45
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n45
 dw 0
n45: db 'ALLOT'
 db 0
 align 4
c45:
 lea bp,_fillmem
 call bp
 db b111-a111
a111:
 mov ax,[bx]
 add bx,2
 mov dx,di
 add ax,dx
 mov di,ax
b111:
 ret
align 4
w46: dw w45
 dw 0
 dw c46
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n46
 dw 0
n46: db 'ALLOTWLIT'
 db 0
 align 4
c46:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_scanhex
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov dx,ax
 mov ax,di
 add ax,dx
 mov di,ax
 ret
align 4
w47: dw w46
 dw 0
 dw c47
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n47
 dw 0
n47: db ','
 db 0
 align 4
c47:
 lea bp,_fillmem
 call bp
 db b112-a112
a112:
 mov ax,[bx]
 add bx,2
 mov si,di
 mov [si],ax
 mov ax,04h
 mov dx,di
 add ax,dx
 mov di,ax
b112:
 ret
align 4
w48: dw w47
 dw 0
 dw c48
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n48
 dw 0
n48: db 'C,'
 db 0
 align 4
c48:
 lea bp,_fillmem
 call bp
 db b113-a113
a113:
 mov ax,[bx]
 add bx,2
 mov si,di
 mov [byte ptr si],al
 mov ax,01h
 mov dx,di
 add ax,dx
 mov di,ax
b113:
 ret
align 4
w49: dw w48
 dw 0
 dw c49
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n49
 dw 0
n49: db '''
 db 0
 align 4
c49:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,si
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b114-a114
a114:
 sub bx,2
 mov [bx],ax
b114:
 ret
ewrz:
 mov si,ax
 call execword
 ret
align 4
w50: dw w49
 dw 0
 dw c50
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n50
 dw 0
n50: db 'EXECUTE1'
 db 0
 align 4
c50:
 lea bp,_fillmem
 call bp
 db b115-a115
a115:
 mov ax,[bx]
 add bx,2
 lea si,[ewrz]
 call si
b115:
 ret
align 4
w51: dw w50
 dw 0
 dw c51
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n51
 dw 0
n51: db 'EXECUTE'
 db 0
 align 4
c51:
 lea bp,_fillmem
 call bp
 db b116-a116
a116:
 mov si,[bx]
 add bx,2
 mov ax,[si+08h]
 mov si,ax
 call si
b116:
 ret
align 4
w52: dw w51
 dw 0
 dw c52
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n52
 dw 0
n52: db 'CALL'
 db 0
 align 4
c52:
 lea bp,_fillmem
 call bp
 db b117-a117
a117:
 mov si,[bx]
 add bx,2
 call si
b117:
 ret
align 4
w53: dw w52
 dw 0
 dw c53
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n53
 dw 0
n53: db 'COMPILE'
 db 0
 align 4
c53:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov ax,[si+08h]
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 mov ax,si
 push cx
 mov cl,0BAh
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 mov ax,[si+04h]
 push cx
 mov cl,0BEh
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b118-a118
a118:
 call si
b118:
 ret
align 4
w54: dw w53
 dw 0
 dw c54
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n54
 dw 0
n54: db '[COMPILE]'
 db 0
 align 4
c54:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 call execword
 ret
align 4
w55: dw w54
 dw 0
 dw c55
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n55
 dw 0
n55: db 'CALLADR'
 db 0
 align 4
c55:
 lea bp,_fillmem
 call bp
 db b119-a119
a119:
 mov ax,[bx]
 add bx,2
 mov si,ax
 call si
b119:
 ret
 align 4
abifn:
 db 04h dup (0)
align 4
w56: dw w55
 dw 0
 dw c56
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n56
 dw 0
n56: db 'IFN'
 db 0
 align 4
c56:
 lea bp,_fillmem
 call bp
 db b120-a120
a120:
 mov ax,[bx]
 add bx,2
b120:
 mov ax,di
 lea si,[abifn]
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b121-a121
a121:
 cmp ax,0
 jge lifp
lifp:
b121:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 call execword
 lea si,[abifn]
 mov si,[si]
 mov ax,di
 mov cx,ax
 sub cx,si
 sub cx,5
 mov [byte ptr si+4],cl
 ret
 align 4
wthen:
align 4
w57: dw w56
 dw 0
 dw c57
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n57
 dw 0
n57: db 'THEN'
 db 0
 align 4
c57:
 ret
 align 4
abif:
 db 04h dup (0)
align 4
w58: dw w57
 dw 0
 dw c58
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n58
 dw 0
n58: db 'IF'
 db 0
 align 4
c58:
 lea bp,_fillmem
 call bp
 db b122-a122
a122:
 mov ax,[bx]
 add bx,2
b122:
 mov ax,di
 lea si,[abif]
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b123-a123
a123:
 cmp ax,0
 je lif
lif:
b123:
loopif:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_getword
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 mov si,ax
 mov dx,si
 lea ax,[wthen]
 sub ax,dx
 mov dx,7FFFh
 and ax,dx
 cmp ax,0
 je lthen
 call execword
 mov ax,00h
 cmp ax,0
 je loopif
lthen:
 lea si,[abif]
 mov si,[si]
 mov ax,di
 mov cx,ax
 sub cx,si
 sub cx,5
 mov [byte ptr si+4],cl
 ret
 align 4
labels:
 db 0800h dup (0)
align 4
w59: dw w58
 dw 0
 dw c59
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n59
 dw 0
n59: db '>=0BRANCH'
 db 0
 align 4
c59:
 mov ah,1
 int 21h
 mov ah,0
 lea bp,_fillmem
 call bp
 db b124-a124
a124:
 mov ax,[bx]
 add bx,2
b124:
 mov dx,ax
 add ax,dx
 mov dx,ax
 add ax,dx
 mov dx,ax
 add ax,dx
 mov dx,ax
 lea ax,[labels]
 add ax,dx
 mov si,ax
 mov ax,di
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b125-a125
a125:
 cmp ax,0
 jge ba
ba:
b125:
 mov ax,si
 mov dx,4h
 add ax,dx
 mov si,ax
 mov ax,[si]
 cmp ax,0
 je za
 mov cx,ax
 mov ax,si
 mov dx,4h
 sub ax,dx
 mov si,ax
 mov ax,[si]
 mov dx,cx
 mov cx,ax
 sub cx,si
 sub cx,5
 mov [byte ptr si+4],cl
za:
 ret
 align 4
levellambda:
 db 04h dup (0)
adrblambda:
 db 0400h dup (0)
align 4
w60: dw w59
 dw 0
 dw c60
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n60
 dw 0
n60: db 'LAMBDA'
 db 0
 align 4
c60:
 mov cx,di
 mov ax,00h
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b126-a126
a126:
 sub bx,2
 mov [bx],ax
 mov ax,00h
b126:
 lea si,[adrblambda]
 mov ax,di
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b127-a127
a127:
 cmp ax,0
 je bla
bla:
b127:
 mov ax,di
 mov di,cx
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b128-a128
a128:
 sub bx,2
 mov [bx],ax
 mov ax,00h
 cmp ax,0
 je blb
blb:
b128:
 ret
align 4
w61: dw w60
 dw 0
 dw c61
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n61
 dw 0
n61: db ';LAMBDA'
 db 0
 align 4
c61:
 lea bp,_fillmem
 call bp
 db b129-a129
a129:
 ret
b129:
 lea si,[adrblambda]
 mov si,[si]
 mov ax,di
 mov cx,ax
 sub cx,si
 sub cx,5
 mov [byte ptr si+4],cl
 ret
align 4
w62: dw w61
 dw 0
 dw c62
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n62
 dw 0
n62: db 'LLAMBDA'
 db 0
 align 4
c62:
 lea si,[levellambda]
 mov ax,[si]
 mov dx,1h
 add ax,dx
 mov [si],ax
 mov cx,ax
 mov dx,di
 lea bp,_fillmem
 call bp
 db b130-a130
a130:
 mov ax,00h
 sub bx,2
 mov [bx],ax
b130:
 mov ax,cx
 mov dx,ax
 add ax,dx
 mov dx,ax
 add ax,dx
 mov dx,ax
 lea ax,[adrblambda]
 add ax,dx
 mov si,ax
 mov ax,di
 mov [si],ax
 lea bp,_fillmem
 call bp
 db b131-a131
a131:
 mov ax,00h
 cmp ax,0
 jge bb
bb:
b131:
 mov ax,di
 mov di,dx
 mov dx,ax
 lea bp,_fillmem
 call bp
 db b132-a132
a132:
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
b132:
 mov di,dx
 lea bp,_fillmem
 call bp
 db b133-a133
a133:
b133:
 ret
align 4
w63: dw w62
 dw 0
 dw c63
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n63
 dw 0
n63: db 'ENDLLAMBDA'
 db 0
 align 4
c63:
 lea bp,_fillmem
 call bp
 db b134-a134
a134:
 ret
b134:
 lea si,[levellambda]
 mov ax,[si]
 mov dx,ax
 add ax,dx
 mov dx,ax
 add ax,dx
 mov dx,ax
 lea ax,[adrblambda]
 add ax,dx
 mov si,ax
 mov si,[si]
 mov ax,di
 mov cx,ax
 sub cx,si
 sub cx,5
 mov [byte ptr si+4],cl
 lea si,[levellambda]
 mov ax,[si]
 mov dx,1h
 sub ax,dx
 mov [si],ax
 ret
align 4
w64: dw w63
 dw 0
 dw c64
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n64
 dw 0
n64: db 'DUMPCODE'
 db 0
 align 4
c64:
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_dmpcode
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 ret
align 4
w65: dw w64
 dw 0
 dw c65
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n65
 dw 0
n65: db 'BIGNOP'
 db 0
 align 4
c65:
 lea bp,_fillmem
 call bp
 db b135-a135
a135:
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
 mov ax,ax
b135:
 ret
align 4
w66: dw w65
 dw 0
 dw c66
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n66
 dw 0
n66: db 'CALLDUMMY'
 db 0
 align 4
c66:
 lea bp,_fillmem
 call bp
 db b136-a136
a136:
b136:
 ret
align 4
w67: dw w66
 dw 0
 dw c67
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n67
 dw 0
n67: db 'UNDEFINED'
 db 0
 align 4
c67:
 mov si,dx
 mov ax,[si+010h]
 push ax
 mov [word ptr sdx],dx
 mov [word ptr scx],cx
 mov [word ptr ssi],si
 mov [word ptr sbx],bx
 mov [word ptr sdi],di
 lea bp,_sscandec
 call bp
 mov dx,[word ptr sdx]
 mov cx,[word ptr scx]
 mov si,[word ptr ssi]
 mov bx,[word ptr sbx]
 mov di,[word ptr sdi]
 pop cx
 push cx
 mov cl,0B8h
 mov [di],cl
 inc di
 mov [di],al
 inc di
 mov[di],ah
 inc di
 pop cx
 lea bp,_fillmem
 call bp
 db b137-a137
a137:
 sub bx,2
 mov [bx],ax
b137:
 ret
 align 4
lw:
align 4
w68: dw w67
 dw 0
 dw c68
 dw 0
 dw 0
 dw 0
 dw 0
 dw 0
 dw n68
 dw 0
n68: db 'BYE'
 db 0
 align 4
c68:
 lea bp,_fillmem
 call bp
 db b138-a138
a138:
 mov ax,0
 int 21h
b138:
 ret
cod:
 db 02000h dup (0)
stk:
 db 01000h dup (0)
main endp
end start
