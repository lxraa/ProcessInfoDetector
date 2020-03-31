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
//mov [0x1234567812345678],rax
#define MOV_8B_RAX 15
//mov rax,rcx
#define MOV_RAX_RCX 16
//mov rax,rdx
#define MOV_RAX_RDX 17
//mov rax,r8
#define MOV_RAX_R8 18
//mov rax,r9
#define MOV_RAX_R9 19
//mov rax,[rsp]
#define MOV_RAX_CRSP 20

//WRITE_MACHINE_CODE(DWORD_PTR p,DWORD machine_code_size,PACHAR machine_code,DWORD code,DWORD_PTR param)
#define WRITE_MACHINE_CODE(p,machine_code_size,machine_code,code,param) machine_code_size=getMachineCode(machine_code,code,param);\
CopyMemory((PVOID)p, machine_code, machine_code_size);\
VirtualFree(machine_code, machine_code_size, MEM_RELEASE);

typedef struct _ARG_CONTEXT {
	DWORD64 rcx;
	DWORD64 rdx;
	DWORD64 r8;
	DWORD64 r9;
	DWORD_PTR ret_address;	//调用函数的地址
}ARG_CONTEXT, *PARG_CONTEXT;

class Patch{
public:
	Patch();
	BOOL removeDebuggerCheck_R3x64();
	BOOL removePebDebuggerFlag_R3x64();
	BOOL hookFunc(LPCTSTR module_name, LPCSTR func_name, DWORD code_size, DWORD_PTR func, PARG_CONTEXT &pcontext);

	DWORD getMachineCode(PCHAR &machine_code, DWORD asm_code, DWORD_PTR address);
};

