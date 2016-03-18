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
 mov ah,1
 int 21h
 mov ah,0
 lea si,[char]
 mov [si],ax
mainloop:
 lea si,[char]
 mov ax,[si]
 mov dx,1h
 sub ax,dx
 lea si,[char]
 mov [si],ax
 mov dx,ax
 mov ah,2
 int 21h
 lea si,[char]
 mov ax,[si]
 cmp ax,0
 jge mainloop
 ret
 align 4
char:
 db 4h dup (0)
main endp
end start
