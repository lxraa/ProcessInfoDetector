option casemap:none
includelib kernel32.lib
includelib user32.lib
MessageBoxA PROTO :DWORD, :DWORD, :DWORD, :DWORD
MessageBox equ <MessageBoxA>
.data
	tt db "123",0
.code

removePebDebuggerFlag PROC
	push rax
	push rdi
	mov rax,0
	mov rdi,qword ptr gs:[60h]
	mov [rdi+2h],rax
	pop rdi
	pop rax
	ret
removePebDebuggerFlag ENDP

myProc PROC
	
	ret
myProc ENDP

myCall PROC
	mov rax,offset myProc
	call rax
myCall ENDP
end