#include "stdafx.h"
#include "patch.h"
#include <aclapi.h>
#include <TlHelp32.h>

extern "C" VOID removePebDebuggerFlag();
extern "C" VOID myCall();

BOOL Patch::removeDebuggerCheck_R3x64() {
	HMODULE kernelbase = GetModuleHandle(TEXT("kernelbase.dll"));
	FARPROC func = GetProcAddress(kernelbase, "CheckRemoteDebuggerPresent");
	CString s;
	
	QWORD obj_code = (QWORD)func + 0x50;
	s.Format(TEXT("%p"), obj_code);
	MessageBox(NULL, s, s, 0);
	char a[3] = { 0x90,0x90,0x90 };
	//memcpy(a, (PVOID64)obj_code, 3);
	DWORD pid = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_CREATE_THREAD |
		PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE,
		FALSE, pid);
	if (hProcess == NULL) {
		MessageBox(NULL, TEXT("Open process failed\n"), TEXT("err"), MB_OK);
		return FALSE;
	}
	DWORD n = WriteProcessMemory(hProcess, (PVOID64)obj_code, a, 3, NULL);
	if (n == 0) {
		MessageBox(NULL, TEXT("WriteProcessMemory failed\n"), TEXT("err"), MB_OK);
		return FALSE;
	}

	//memcpy((PVOID64)obj_code, a, 3);

	return TRUE;
}

BOOL Patch::removePebDebuggerFlag_R3x64() {
#if defined(_WIN64)
	removePebDebuggerFlag();
#endif
	return TRUE;
}


//64λHOOK
//code_size���ٴ���2+12=14
BOOL Patch::hookFunc(LPCTSTR module_name,LPCSTR func_name,DWORD code_size,DWORD_PTR func, PARG_CONTEXT &pcontext) {
	HMODULE h_module = GetModuleHandle(module_name);
	FARPROC func_base = GetProcAddress(h_module, func_name);
	/*
	1��save context
	->
	2��myProc
	->
	3��load context
	->
	4��old code
	->
	5��return code
	->
	*/
	
	DWORD save_context_size = sizeof(BYTE) * 100;
	DWORD my_call_size = sizeof(BYTE) * 50;
	DWORD load_context_size = sizeof(BYTE) * 50;
	DWORD old_code_size = code_size;
	DWORD ret_code_size = sizeof(BYTE) * 20;
	DWORD size = save_context_size + old_code_size + my_call_size + ret_code_size + load_context_size;


	PVOID code_area = VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	PVOID save_context_area = code_area;
	PVOID my_call_area = (PVOID)((DWORD_PTR)save_context_area + save_context_size);
	PVOID load_context_area = (PVOID)((DWORD_PTR)my_call_area + my_call_size);

	PVOID old_code_area = (PVOID)((DWORD_PTR)load_context_area + load_context_size);
	PVOID ret_code_area = (PVOID)((DWORD_PTR)old_code_area + old_code_size);

	FillMemory(code_area, size, 0x90);
	DWORD_PTR p = NULL;
	PCHAR machine_code = NULL;
	DWORD machine_code_size = 0;
	//1��save context
	//mov rax,[rsp]
	//mov [&pcontext->ret_address],rax
	//mov rax,rcx
	//mov [&pcontext->rcx],rax
	//mov rax,rdx
	//mov [&pcontext->rdx],rax
	//mov rax,r8
	//mov [&pcontext->r8],rax
	//mov rax,r9
	//mov [&pcontext->r9],rax

	//push rcx
	//push rdx
	//push r8
	//push r9
	//sub rsp,48h
	p = (DWORD_PTR)save_context_area;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_CRSP, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_8B_RAX, (DWORD_PTR)&(pcontext->ret_address));
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_RCX, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_8B_RAX, (DWORD_PTR)&(pcontext->rcx));
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_RDX, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_8B_RAX, (DWORD_PTR)&(pcontext->rdx));
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_R8, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_8B_RAX, (DWORD_PTR)&(pcontext->r8));
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_R9, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_8B_RAX, (DWORD_PTR)&(pcontext->r9));
	p = p + machine_code_size;
	
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, PUSH_RCX, NULL);
	p = p + machine_code_size;
	/*machine_code_size = getMachineCode(machine_code, PUSH_RCX, (DWORD_PTR)func);
	CopyMemory((PVOID)p, machine_code, machine_code_size);
	p = p + machine_code_size;
	VirtualFree(machine_code, machine_code_size, MEM_RELEASE);*/

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, PUSH_RDX, NULL);
	p = p + machine_code_size;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, PUSH_R8, NULL);
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, PUSH_R9, NULL);
	p = p + machine_code_size;

	//̧ջ��CALL��ʱ��rsp��Ҫ0x10���룬���򸡵�����������
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, SUB_RSP, (DWORD_PTR)0x48);
	p = p + machine_code_size;

	//2��my proc	�ֶ�ģ��call����Ϊcall�����Ѱַ�Ƚ�����
	//mov rax,ret_address
	//push rax
	//mov rax,func
	//push rax
	//ret
	p = (DWORD_PTR)my_call_area;
	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_RAX_8B, 0x0000000000000000)
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, PUSH_RAX, NULL);
	p = p + machine_code_size;


	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_PUSH_RET, (DWORD_PTR)func);
	p = p + machine_code_size;

	//�޸�push��ջ�ķ��ص�ַ
	CopyMemory((PVOID)(((DWORD_PTR)my_call_area) + 2), (PVOID)&p, sizeof(DWORD_PTR));
	
	//3��load context
	//add rsp,48h
	//pop r9
	//pop r8
	//pop rdx
	//pop rcx
	p = (DWORD_PTR)load_context_area;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, ADD_RSP, (DWORD_PTR)0x48);
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, POP_R9, NULL);
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, POP_R8, NULL);
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, POP_RDX, NULL);
	p = p + machine_code_size;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, POP_RCX, NULL);
	p = p + machine_code_size;

	//4��.old code
	CopyMemory((PVOID)old_code_area, (PVOID)func_base, old_code_size);

	//5��.ret code
	DWORD_PTR ret_address = (DWORD_PTR)func_base + old_code_size;
	WRITE_MACHINE_CODE(ret_code_area, machine_code_size, machine_code, MOV_PUSH_RET, ret_address);


	//4.�޸�ԭ������ַָ��
	DWORD old_protect;
	VirtualProtect(func_base, old_code_size, PAGE_EXECUTE_READWRITE, &old_protect);
	FillMemory(func_base, old_code_size, 0x90);

	p = (DWORD_PTR)func_base;

	WRITE_MACHINE_CODE(p, machine_code_size, machine_code, MOV_PUSH_RET, (DWORD_PTR)code_area);
	return TRUE;
}
//���ػ����볤�ȣ�MACHINE_CODEΪ���صĻ����룬assembly_code��һ���꣬Ϊָ���Ļ��ָ�addressΪ��ַ

DWORD Patch::getMachineCode(PCHAR &machine_code, DWORD assembly_code, DWORD_PTR param) {
	DWORD code_size;

	switch (assembly_code) {
	case(MOV_PUSH_RET): {
#if defined _WIN64
		code_size = 12;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[20] = { 0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0xC3 };
		CopyMemory(ret_c + 2, &param, sizeof(DWORD_PTR));
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
	}

	case(PUSH_RAX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x50 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
	}

	case(MOV_RAX_8B): {
#if defined _WIN64
		code_size = 10;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[100] = { 0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
		CopyMemory(ret_c + 2, &param, sizeof(DWORD_PTR));
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
	}
	case(ADD_RSP): {
		code_size = 4;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0x83,0xC4,0x00 };
		CopyMemory(ret_c + 3, &param, sizeof(BYTE));
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
	}
	case(SUB_RSP): {
		code_size = 4;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0x83,0xEC,0x00 };
		CopyMemory(ret_c + 3, &param, sizeof(BYTE));
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
	}
	case(POP_RAX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x58 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(PUSH_RCX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x51 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(PUSH_RDX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x52 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(PUSH_R8): {
#if defined _WIN64
		code_size = 2;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x41,0x50 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(PUSH_R9): {
#if defined _WIN64
		code_size = 2;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x41,0x51 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(POP_R9): {
#if defined _WIN64
		code_size = 2;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x41,0x59 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(POP_R8): {
#if defined _WIN64
		code_size = 2;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x41,0x58 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(POP_RDX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x5A };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(POP_RCX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x59 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(MOV_8B_RAX): {
#if defined _WIN64
		code_size = 10;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
		CopyMemory(ret_c + 2, &param, sizeof(DWORD_PTR));
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(MOV_RAX_RCX): {
#if defined _WIN64
		code_size = 3;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0x8B,0xC1 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;

	}
	case(MOV_RAX_RDX): {
#if defined _WIN64
		code_size = 3;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0x8B,0xC2 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(MOV_RAX_R8): {
#if defined _WIN64
		code_size = 3;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x49,0x8B,0xC0 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(MOV_RAX_R9): {
#if defined _WIN64
		code_size = 3;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x49,0x8B,0xC1 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	case(MOV_RAX_CRSP): {
#if defined _WIN64
		code_size = 4;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c[10] = { 0x48,0x8B,0x04,0x24 };
		CopyMemory(machine_code, ret_c, code_size);
		return code_size;
#else
		break;
#endif
		return code_size;
	}
	default: {
		return 0;
	}
	}

}

Patch::Patch() {

}