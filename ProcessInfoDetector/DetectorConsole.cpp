// DetectorConsole.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "DetectorConsole.h"
#include "afxdialogex.h"
#include "patch.h"
#include <TlHelp32.h>
#include <psapi.h>
#include "HookDialog.h"
#include "VEHConsole.h"
// DetectorConsole 对话框
typedef NTSTATUS (WINAPI*ZWQUERYINFORMATIONTHREAD)(
_In_      HANDLE          ThreadHandle,
_In_      THREADINFOCLASS ThreadInformationClass,
_In_      PVOID           ThreadInformation,
_In_      ULONG           ThreadInformationLength,
_Out_opt_ PULONG          ReturnLength
);

IMPLEMENT_DYNAMIC(DetectorConsole, CDialogEx)



DetectorConsole::DetectorConsole(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

DetectorConsole::~DetectorConsole()
{
}

void DetectorConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(DetectorConsole, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &DetectorConsole::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &DetectorConsole::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &DetectorConsole::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &DetectorConsole::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &DetectorConsole::OnBnClickedButton4)
END_MESSAGE_MAP()


// DetectorConsole 消息处理程序


void DetectorConsole::OnBnClickedButton1()
{

	m_list.ResetContent();
	HINSTANCE h_ntdll = GetModuleHandle(TEXT("ntdll"));
	ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread = (ZWQUERYINFORMATIONTHREAD)GetProcAddress(h_ntdll, "ZwQueryInformationThread");

	THREADENTRY32 t;
	t.dwSize = sizeof(THREADENTRY32);
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (INVALID_HANDLE_VALUE == h) {
		MessageBox(TEXT("创建线程快照失败"));
		return;
	}
	if (Thread32First(h,&t)) {
		do {
			CString c;
			PVOID addr;
			HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, t.th32ThreadID);
			ZwQueryInformationThread(h_thread, ThreadQuerySetWin32StartAddress, &addr, sizeof(addr), NULL);
			THREAD_BASIC_INFORMATION tbi;
			TCHAR modname[MAX_PATH];
			ZwQueryInformationThread(h_thread, ThreadBasicInformation, &tbi, sizeof(tbi), NULL);
			GetMappedFileName(OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)tbi.ClientId.UniqueProcess), addr, modname, MAX_PATH);

			c.Format(TEXT("thread id:%d-%s"), t.th32ThreadID, modname);
			m_list.AddString(c);
		} while (Thread32Next(h,&t));
	}
	SuspendThread(GetCurrentProcess());
}


void DetectorConsole::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (is_debug_patched) {
		return;
	}
	Patch *p = new Patch();
	p->removeDebuggerCheck_R3x64();
	p->removePebDebuggerFlag_R3x64();
	m_list.AddString(TEXT("Patch success"));
	delete p;
}


void DetectorConsole::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_list.ResetContent();
	MODULEENTRY32 m;
	m.dwSize = sizeof(MODULEENTRY32);
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (INVALID_HANDLE_VALUE == h) {
		MessageBox(TEXT("Create snapshot failed"));
		return;
	}
	if (Module32First(h,&m)) {
		do {
			CString s;
			s.Format(TEXT("module:%s    addr:%p    size:%x"), m.szExePath, m.modBaseAddr, m.modBaseSize);

			m_list.AddString(s);
			
		} while (Module32Next(h,&m));
	}


}


void DetectorConsole::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	HookDialog *h = new HookDialog();
	h->DoModal();
	delete h;
}




void DetectorConsole::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	VEHConsole *v = new VEHConsole();
	v->DoModal();
	delete v;
}


