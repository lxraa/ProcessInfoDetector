// HookDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "HookDialog.h"
#include "afxdialogex.h"
#include "patch.h"
#include <TlHelp32.h>
#include <vector>
typedef NTSTATUS(WINAPI *ZWQUERYVIRTUALMEMORY)(HANDLE ProcessHandle, PVOID BaseAddress, WIN32_MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, ULONG MemoryInformationLength, PULONG ReturnLength);

HookDialog *hookdialog = NULL;
PARG_CONTEXT args_CreateToolhelp32SnapShot = NULL;
PARG_CONTEXT args_NtTerminateProcess = NULL;
PARG_CONTEXT args_CreateFileW = NULL;
PARG_CONTEXT args_CreateFileA = NULL;
PARG_CONTEXT args_DispatchMessageW = NULL;
PARG_CONTEXT args_VirtualQuery = NULL;
PARG_CONTEXT args_VirtualQueryEx = NULL;
PARG_CONTEXT args_ZwQueryVirtualMemory = NULL;
PARG_CONTEXT args_ReadProcessMemory = NULL;
PARG_CONTEXT args_ZwReadVirtualMemory = NULL;


BOOL hooked_CreateToolhelp32SnapShot = FALSE;
BOOL hooked_NtTerminateProcess = FALSE;
BOOL hooked_CreateFileW = FALSE;
BOOL hooked_CreateFileA = FALSE;
BOOL hooked_DispatchMessageW = FALSE;
BOOL hooked_VirtualQuery = FALSE;
BOOL hooked_VirtualQueryEx = FALSE;
BOOL hooked_ZwQueryVirtualMemory = FALSE;
BOOL hooked_ReadProcessMemory = NULL;
BOOL hooked_ZwReadVirtualMemory = NULL;

// HookDialog 对话框

IMPLEMENT_DYNAMIC(HookDialog, CDialogEx)

HookDialog::HookDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{
	
	hookdialog = this;
	CString s;
	#define INSERT_KV(k,v) s=(k);kv.insert(std::pair<CString, INT>(s, v));
	INSERT_KV(TEXT("CreateToolhelp32SnapShot"), 0);
	INSERT_KV(TEXT("NtTerminateProcess"), 1);
	INSERT_KV(TEXT("CreateFileW"), 2);
	INSERT_KV(TEXT("CreateFileA"), 3);
	//INSERT_KV(TEXT("GetMessageW"), 4);
	//INSERT_KV(TEXT("GetMessageA"), 5);
	INSERT_KV(TEXT("DispatchMessageW"), 4);
	INSERT_KV(TEXT("VirtualQuery"), 5);
	INSERT_KV(TEXT("ZwQueryVirtualMemory"), 6);
	INSERT_KV(TEXT("VirtualQueryEx"), 7);
	INSERT_KV(TEXT("ReadProcessMemory"), 8);
	INSERT_KV(TEXT("ZwReadVirtualMemory"), 9);
}

HookDialog::~HookDialog()
{
}

void HookDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
}


BEGIN_MESSAGE_MAP(HookDialog, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST2, &HookDialog::OnLbnSelchangeList2)
	ON_CBN_DROPDOWN(IDC_COMBO1, &HookDialog::OnCbnDropdownCombo1)
	ON_BN_CLICKED(IDC_BUTTON5, &HookDialog::OnBnClickedButton5)
END_MESSAGE_MAP()


// HookDialog 消息处理程序
void proc() {

	CString s;
	s.Format(TEXT("Invoke CreateToolhelp32SnapShot ret_address: %p param:%p %p %p %p"), args_CreateToolhelp32SnapShot->ret_address, args_CreateToolhelp32SnapShot->rcx, args_CreateToolhelp32SnapShot->rdx, args_CreateToolhelp32SnapShot->r8, args_CreateToolhelp32SnapShot->r9);
	hookdialog->m_list.AddString(s);
	return;
}



VOID proc_NtTerminateProcess() {
	CString s(TEXT("Invoke NtTerminateProcess"));
	hookdialog->m_list.AddString(s);

	MessageBox(NULL, TEXT("PAUSE"), TEXT("0"), MB_OK);
	return;
}

void HookDialog::OnLbnSelchangeList2()
{
	// TODO: 在此添加控件通知处理程序代码
}



VOID proc_CreateFileW() {
	CString s;
	s.Format(TEXT("Invoke CreateFileW:%s %p %p %p"), *(PWCHAR)args_CreateFileW->rcx, args_CreateFileW->rdx, args_CreateFileW->r8, args_CreateFileW->r9);

	hookdialog->m_list.AddString(s);
	return;
}



VOID proc_CreateFileA() {
	CString s;
	s.Format(TEXT("Invoke CreateFileW:%p %s %p %p %p"),args_CreateFileA->ret_address, *(PCHAR)args_CreateFileA->rcx, args_CreateFileA->rdx, args_CreateFileA->r8, args_CreateFileA->r9);

	hookdialog->m_list.AddString(s);
	return;
}



void HookDialog::OnCbnDropdownCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_combox.ResetContent();
	for (auto iter = kv.begin(); iter != kv.end(); iter++) {
		m_combox.AddString(iter->first);
	}
}

VOID proc_DispatchMessageW() {
	//args_DispatchMessageW->rcx = &msg;
	MSG msg = *((PMSG)args_DispatchMessageW->rcx);
	if (msg.message == WM_CHAR) {
		CString s;
		PVOID wndproc = (PVOID)GetWindowLongPtr(msg.hwnd, GWLP_WNDPROC);
		s.Format(TEXT("WM_CHAR: %x address:%p wndproc:%p"), msg.wParam, args_DispatchMessageW->ret_address, wndproc);
		hookdialog->m_list.AddString(s);
	}
	
	return;
}

VOID proc_VirtualQuery() {
	
	CString s;
	s.Format(TEXT("Invoke VirtualQuery : addr: %p len: %u"), args_VirtualQuery->rcx, args_VirtualQuery->r8);
	hookdialog->m_list.AddString(s);
	return;
}
VOID proc_ZwQueryVirtualMemory() {
	CString s;
	s.Format(TEXT("Invoke ZwQueryVirtualMemory addr:%p caller: %p"), args_ZwQueryVirtualMemory->rdx, args_ZwQueryVirtualMemory->ret_address);
	hookdialog->m_list.AddString(s);
	hookdialog->m_list.SetCurSel(hookdialog->m_list.GetCount() - 1);

	return;
}

VOID proc_VirtualQueryEx(){
	CString s;
	s.Format(TEXT("Invoke VirtualQueryEx : process : %u addr: %p len: %u"), args_VirtualQueryEx->rcx, args_VirtualQueryEx->rdx, args_VirtualQuery->r9);
	hookdialog->m_list.AddString(s);
	return;
}

VOID proc_ReadProcessMemory() {
	CString s;
	s.Format(TEXT("Invoke ReadProcessMemory : baseaddress: %p len: %u"), args_ReadProcessMemory->rdx, args_ReadProcessMemory->r9);
	hookdialog->m_list.AddString(s);
	return;
}

VOID proc_ZwReadVirtualMemory() {
	CString s;
	s.Format(TEXT("Invoke ZwReadVirtualMemory : baseaddress:%p len %u "), args_ZwReadVirtualMemory->rdx, args_ZwReadVirtualMemory->r9);
	hookdialog->m_list.AddString(s);
	return;
}
void HookDialog::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CString k;
	m_combox.GetLBText(m_combox.GetCurSel(), k);
	switch (kv[k]) {
	case(0): {
		if (hooked_CreateToolhelp32SnapShot) {
			return;
		}
		if (args_CreateToolhelp32SnapShot == NULL) {
			args_CreateToolhelp32SnapShot = new ARG_CONTEXT;
		}
		
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("kernel32.dll"), "CreateToolhelp32Snapshot", 15, (DWORD_PTR)proc, args_CreateToolhelp32SnapShot)) {
			m_list.AddString(TEXT("hook CreateToolhelp32Snapshot success"));
			hooked_CreateToolhelp32SnapShot = TRUE;
		}
		
		delete p;
		
		return;
	}
	case(1): {
		if (hooked_NtTerminateProcess) {
			return;
		}
		if (args_NtTerminateProcess == NULL) {
			args_NtTerminateProcess = new ARG_CONTEXT;
		}
		Patch *p = new Patch();

		if (p->hookFunc(TEXT("ntdll.dll"), "NtTerminateProcess", 16, (DWORD_PTR)proc_NtTerminateProcess, args_NtTerminateProcess)) {
			m_list.AddString(TEXT("hook NtTerminateProcess success"));
			hooked_NtTerminateProcess = TRUE;
		}
		
		delete p;
		
		return;
	}
	case(2): {
		if (hooked_CreateFileW) {
			return;
		}
		if (args_CreateFileW == NULL) {
			args_CreateFileW = new ARG_CONTEXT;
		}
		
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("KERNELBASE.dll"), "CreateFileW", 15, (DWORD_PTR)proc_CreateFileW, args_CreateFileW)) {
			m_list.AddString(TEXT("hook CreateFileW success"));
			hooked_CreateFileW = TRUE;
		}
		delete p;
		return;
	}
	case(3): {
		if (hooked_CreateFileA) {
			return;
		}
		if (args_CreateFileA == NULL) {
			args_CreateFileA = new ARG_CONTEXT;
		}
		
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("KERNELBASE.dll"), "CreateFileA", 15, (DWORD_PTR)proc_CreateFileA, args_CreateFileA)) {
			m_list.AddString(TEXT("hook CreateFileA success"));
			hooked_CreateFileA = TRUE;
		}
		delete p;
		return;
	}
	case(4): {
		if (hooked_DispatchMessageW) {
			return;
		}
		if (args_DispatchMessageW == NULL) {
			args_DispatchMessageW = new ARG_CONTEXT;
		}
		
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("user32.dll"), "DispatchMessageW", 21, (DWORD_PTR)proc_DispatchMessageW, args_DispatchMessageW)) {
			m_list.AddString(TEXT("hook DispatchMessageW success"));
			hooked_DispatchMessageW = TRUE;
		}
		delete p;
		
		return;
	}
	case(5): {
		if (hooked_VirtualQuery) {
			return;
		}
		if (args_VirtualQuery == NULL) {
			args_VirtualQuery = new ARG_CONTEXT;
		}
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("kernel32.dll"), "VirtualQuery", 19, (DWORD_PTR)proc_VirtualQuery, args_VirtualQuery)) {
			m_list.AddString(TEXT("hook VirtualQuery success"));
			hooked_VirtualQuery = TRUE;
		}
		delete p;
		return;
	}
	case(6): {
		if (hooked_ZwQueryVirtualMemory) {
			return;
		}
		if (args_ZwQueryVirtualMemory == NULL) {
			args_ZwQueryVirtualMemory = new ARG_CONTEXT;
		}
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("ntdll.dll"), "ZwQueryVirtualMemory", 16, (DWORD_PTR)proc_ZwQueryVirtualMemory, args_ZwQueryVirtualMemory)) {
			m_list.AddString(TEXT("hook ZwQueryVirtualMemory success"));
			hooked_ZwQueryVirtualMemory = TRUE;
		}
		delete p;
		return;
	}
	case(7): {
		if (hooked_VirtualQueryEx) {
			return;
		}
		if (args_VirtualQueryEx == NULL) {
			args_VirtualQueryEx = new ARG_CONTEXT;
		}
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("kernel32.dll"), "VirtualQueryEx", 19, (DWORD_PTR)proc_VirtualQueryEx, args_VirtualQueryEx)) {
			m_list.AddString(TEXT("hook VirtualQueryEx success"));
			hooked_VirtualQueryEx = TRUE;
		}
		delete p;
		return;
	}
	case(8): {
		if (hooked_ReadProcessMemory) {
			return;
		}
		if (args_ReadProcessMemory == NULL) {
			args_ReadProcessMemory = new ARG_CONTEXT;
		}
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("kernel32.dll"), "ReadProcessMemory", 14, (DWORD_PTR)proc_ReadProcessMemory, args_ReadProcessMemory)) {
			m_list.AddString(TEXT("hook ReadProcessMemory success"));
			hooked_ReadProcessMemory = TRUE;
		}
		delete p;
		return;
	}
	case(9): {
		if (hooked_ZwReadVirtualMemory) {
			return;
		}
		if (args_ZwReadVirtualMemory == NULL) {
			args_ZwReadVirtualMemory = new ARG_CONTEXT;
		}
		Patch *p = new Patch();
		if (p->hookFunc(TEXT("ntdll.dll"), "ZwReadVirtualMemory", 14, (DWORD_PTR)proc_ZwReadVirtualMemory, args_ZwReadVirtualMemory)) {
			m_list.AddString(TEXT("hook ZwReadVirtualMemory success"));
			hooked_ZwReadVirtualMemory = TRUE;
		}
		delete p;
		return;
	}
	//case(4): {
	//	if (hooked_GetMessageW) {
	//		return;
	//	}
	//	args_GetMessageW = new ARG_CONTEXT;
	//	Patch *p = new Patch();
	//	p->hookFunc(TEXT("user32.dll"), "GetMessageW", 15, (DWORD_PTR)proc_GetMessageW, args_GetMessageW);
	//	delete p;
	//	m_list.AddString(TEXT("hook GetMessageW success"));
	//	return;
	//}
	//case(5): {
	//	if (hooked_GetMessageA) {
	//		return;
	//	}
	//	args_GetMessageA = new ARG_CONTEXT;
	//	Patch *p = new Patch();
	//	p->hookFunc(TEXT("user32.dll"), "GetMessageA", 15, (DWORD_PTR)proc_GetMessageA, args_GetMessageA);
	//	delete p;
	//	m_list.AddString(TEXT("hook GetMessageA success"));
	//	return;
	//}
	default: {

	}
	}
}

