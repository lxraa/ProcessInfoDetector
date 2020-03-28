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


//64位HOOK
//code_size至少大于2+12=14
BOOL Patch::hookFunc(LPCTSTR module_name,LPCSTR func_name,DWORD code_size,DWORD_PTR func) {
	HMODULE h_module = GetModuleHandle(module_name);
	FARPROC func_base = GetProcAddress(h_module, func_name);
	/*
	->save context


	->
	1、myProc

	->load context

	->
	2、old code
	->
	3、return code

	
	*/
	
	DWORD save_context_size = sizeof(BYTE) * 50;

	DWORD my_call_size = sizeof(BYTE) * 50;

	DWORD load_context_size = sizeof(BYTE) * 50;

	DWORD old_code_size = code_size;
	DWORD ret_code_size = sizeof(BYTE) * 20;
	DWORD size = save_context_size + old_code_size + my_call_size + ret_code_size + load_context_size;


	PVOID code_area = VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	//PVOID my_call_area = (PVOID)((DWORD_PTR)code_area + old_code_size);
	PVOID save_context_area = code_area;
	PVOID my_call_area = (PVOID)((DWORD_PTR)save_context_area + save_context_size);
	PVOID load_context_area = (PVOID)((DWORD_PTR)my_call_area + my_call_size);

	PVOID old_code_area = (PVOID)((DWORD_PTR)load_context_area + load_context_size);
	PVOID ret_code_area = (PVOID)((DWORD_PTR)old_code_area + old_code_size);

	FillMemory(code_area, size, 0x90);

	//***save context
	//push rcx
	//push rdx
	//push r8
	//push r9
	//sub rsp,48h


	//1.my proc
	//mov rax,ret_address
	//push rax
	//mov rax,func
	//push rax
	//ret
	DWORD_PTR p = (DWORD_PTR)my_call_area;
	PCHAR machine_code = NULL;
	DWORD machine_code_size = 0;
	//抬栈，CALL的时候rsp需要0x10对齐，否则浮点数运算会出错

	machine_code_size = getMachineCode(machine_code, MOV_RAX_8B, (DWORD_PTR)func);
	CopyMemory((PVOID)p, machine_code, machine_code_size);
	p = p + machine_code_size;
	VirtualFree(machine_code, machine_code_size,MEM_RELEASE);


	machine_code_size = getMachineCode(machine_code, PUSH_RAX, 0);
	CopyMemory((PVOID)p, machine_code, machine_code_size);
	p = p + machine_code_size;
	VirtualFree(machine_code, machine_code_size, MEM_RELEASE);


	machine_code_size = getMachineCode(machine_code, MOV_PUSH_RET, (DWORD_PTR)func);
	CopyMemory((PVOID)p, machine_code, machine_code_size);
	p = p + machine_code_size;
	VirtualFree(machine_code, machine_code_size, MEM_RELEASE);

	//修复push进栈的返回地址
	CopyMemory((PVOID)(((DWORD_PTR)my_call_area) + 2), (PVOID)&p, sizeof(DWORD_PTR));
	
	//***load context
	//add rsp,48h
	//pop r9
	//pop r8
	//pop rdx
	//pop rcx


	//2.old code
	CopyMemory((PVOID)old_code_area, (PVOID)func_base, old_code_size);
	/*
	//mov rax,offset mycall ;call rax 12Byte
	//由于myCall外部导入，因此myCall指向的是一个jmp 
	//由于jmp是相对跳，这里计算myCall的绝对地址
	LONG offset = NULL;
	DWORD_PTR offset_address = (DWORD_PTR)myCall + 1;
	CopyMemory((PVOID)&offset, (PVOID)offset_address, 4);
	DWORD_PTR real_address = (DWORD_PTR)myCall + 5 + offset;
	*/
	//3.ret code
	DWORD_PTR ret_address = (DWORD_PTR)func_base + old_code_size;
	machine_code_size = getMachineCode(machine_code, MOV_PUSH_RET, ret_address);
	CopyMemory(ret_code_area, machine_code, machine_code_size);
	VirtualFree(machine_code, machine_code_size, MEM_RELEASE);

	//4.修改原函数地址指令
	DWORD old_protect;
	VirtualProtect(func_base, old_code_size, PAGE_EXECUTE_READWRITE, &old_protect);
	
	FillMemory(func_base, old_code_size, 0x90);

	p = (DWORD_PTR)func_base;


	machine_code_size = getMachineCode(machine_code, MOV_PUSH_RET, (DWORD_PTR)code_area);
	CopyMemory((PVOID)p, machine_code, machine_code_size);
	VirtualFree(machine_code, machine_code_size, MEM_RELEASE);
	
	return TRUE;
}
//返回机器码长度，MACHINE_CODE为返回的机器码，assembly_code是一个宏，为指定的汇编指令，address为地址

DWORD Patch::getMachineCode(PCHAR &machine_code,DWORD assembly_code,DWORD_PTR param) {
	DWORD code_size;

	switch (assembly_code) {
	case(MOV_PUSH_RET): {
#if defined _WIN64
		code_size = 12;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c1[100] = { 0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0xC3 };
		CopyMemory(ret_c1 + 2, &param, sizeof(DWORD_PTR));
		CopyMemory(machine_code, ret_c1, code_size);
		return code_size;
#else
		break;
#endif
	}

	case(PUSH_RAX): {
#if defined _WIN64
		code_size = 1;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c2[10] = { 0x50 };
		CopyMemory(machine_code, ret_c2, code_size);
		return code_size;
#else
		break;
#endif
	}

	case(MOV_RAX_8B): {
#if defined _WIN64
		code_size = 10;
		machine_code = (PCHAR)VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		CHAR ret_c3[100] = { 0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
		CopyMemory(ret_c3 + 2, &param, sizeof(DWORD_PTR));
		CopyMemory(machine_code, ret_c3, code_size);
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
	default: {
		return 0;
	}
	}
}

Patch::Patch() {

}