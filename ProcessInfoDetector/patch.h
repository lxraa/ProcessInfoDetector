#pragma once
VOID myProc();
// mov rax,#address		
#define MOV_RAX_8B 1
// mov rax,#address		push rax	ret	用来跳转到指定address
#define MOV_PUSH_RET 2
// push rax
#define PUSH_RAX 3
//add rsp,x
#define ADD_RSP 4
//sub rsp,x
#define SUB_RSP 5
//pop rax
#define POP_RAX 6
// push rcx
#define PUSH_RCX 7
//push rdx
#define PUSH_RDX 8
//push r8
#define PUSH_R8 9
//push r9
#define PUSH_R9 10

//pop r9
#define POP_R9 11
//pop r8
#define POP_R8 12
//pop rdx
#define POP_RDX 13
//pop rcx
#define POP_RCX 14

class Patch{
public:
	Patch();
	BOOL removeDebuggerCheck_R3x64();
	BOOL removePebDebuggerFlag_R3x64();
	BOOL hookFunc(LPCTSTR module_name, LPCSTR func_name, DWORD code_size,DWORD_PTR func);
	DWORD getMachineCode(PCHAR &machine_code, DWORD asm_code, DWORD_PTR address);
};
