option casemap:none

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


end