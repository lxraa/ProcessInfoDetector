// VEHConsole.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "VEHConsole.h"
#include "afxdialogex.h"
#include "Patch.h"
#include <TlHelp32.h>

// VEHConsole 对话框

IMPLEMENT_DYNAMIC(VEHConsole, CDialogEx)
VEHConsole *s_vehconsole = NULL;
VEHConsole::VEHConsole(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	VEHConsole::vehconsole = this;
}

VEHConsole::~VEHConsole()
{
	if (this->handler != NULL) {
		RemoveVectoredExceptionHandler(this->handler);
	}
}

void VEHConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(VEHConsole, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &VEHConsole::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &VEHConsole::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &VEHConsole::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &VEHConsole::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &VEHConsole::OnBnClickedButton5)
END_MESSAGE_MAP()


// VEHConsole 消息处理程序
LONG vehHandler(PEXCEPTION_POINTERS except_info) {
	PVOID except_address = except_info->ExceptionRecord->ExceptionAddress;
	DWORD err_code = except_info->ExceptionRecord->ExceptionCode;
	CString s;
	s.Format(TEXT("Got exception at : %p code: %u"), except_address, err_code);
	s_vehconsole->m_list.AddString(s);

	return EXCEPTION_CONTINUE_SEARCH;
}

void VEHConsole::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	this->handler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)vehHandler);
	if (this->handler != NULL) {
		MessageBox(TEXT("success"));
		s_vehconsole = this;
	}
}


void VEHConsole::OnBnClickedButton2()
{
	
	// TODO: 在此添加控件通知处理程序代码

	DWORD tid = GetDlgItemInt(IDC_EDIT1);
	Patch *p = new Patch();

	if (tid == NULL) {
		DWORD pid = GetCurrentProcessId();
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
		if (Thread32First(h, &te)) {
			do {
				if (te.th32OwnerProcessID != pid) {
					continue;
				}
				CString s;
				HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
				if (h_thread == NULL) {
					s.Format(TEXT("Open thread %u failed"),te.th32ThreadID);
					continue;
				}

				PEXCEPTION_REGISTRATION_RECORD exception_records = p->getSEHChain(h_thread);
				if (exception_records != NULL) {
					CString s;
					s.Format(TEXT("Thread : %u --------"), te.th32ThreadID);
					m_list.AddString(s);
					do {
						
						s.Format(TEXT("SEH func %p"), exception_records->Handler);
						m_list.AddString(s);
					} while (exception_records->Next != INVALID_HANDLE_VALUE);

				}
				else {
					s.Format(TEXT("Thread %u has no SEH"), te.th32ThreadID);
					m_list.AddString(s);
				}
				CloseHandle(h_thread);
		
			} while (Thread32Next(h, &te));
		}
	}
	else {
		PEXCEPTION_REGISTRATION_RECORD exception_records = p->getSEHChain(OpenThread(THREAD_ALL_ACCESS, FALSE, tid));
		if (exception_records != NULL) {
			do {
				CString s;
				s.Format(TEXT("SEH func %p"), exception_records->Handler);
				m_list.AddString(s);
			} while (exception_records->Next != INVALID_HANDLE_VALUE);
		}
		else {
			m_list.AddString(TEXT("there is no SEH in this thread"));
		}
	}
	
	
	delete p;

}


void VEHConsole::OnBnClickedButton3()
{
	
	// TODO: 在此添加控件通知处理程序代码
	
	UINT tid = GetDlgItemInt(IDC_EDIT1);
	if (tid) {
		CString s;
		HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
		if (-1 == SuspendThread(h_thread)) {
			s.Format(TEXT("Suspend thread %u failed"), tid);
			m_list.AddString(s);
		}
		else {
			s.Format(TEXT("Suspend thread %u success"), tid);
			m_list.AddString(s);
		}
		CloseHandle(h_thread);
	}
	else {
		CString s;
		DWORD current_tid = GetCurrentThreadId();
		DWORD current_pid = GetCurrentProcessId();
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
		if (INVALID_HANDLE_VALUE == h) {
			m_list.AddString(TEXT("create thread snapshot failed"));
			return;
		}
		if (Thread32First(h, &te)) {
			do {
				if (current_tid != te.th32ThreadID && current_pid == te.th32OwnerProcessID) {
					HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
					if (h_thread == INVALID_HANDLE_VALUE) {
						s.Format(TEXT("open thread %u failed"), te.th32ThreadID);
						m_list.AddString(s);
						continue;
					}
					if (SuspendThread(h_thread) != -1) {
						s.Format(TEXT("suspend thread %u success"), te.th32ThreadID);
						m_list.AddString(s);
					}
					else {
						s.Format(TEXT("suspend thread %u failed"), te.th32ThreadID);
						m_list.AddString(s);
					}
					CloseHandle(h_thread);
				}
			} while (Thread32Next(h, &te));
		}
		CloseHandle(h);
	}

}


void VEHConsole::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD cid = GetCurrentProcessId();
	CString s;
	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (INVALID_HANDLE_VALUE == h) {
		m_list.AddString(TEXT("create thread snapshot failed"));
		return;
	}
	s.Format(TEXT("Current thread id:%u"), GetCurrentThreadId());
	m_list.AddString(s);
	m_list.AddString(TEXT("searching......."));
	
	if (Thread32First(h, &te)) {
		do {
			if (te.th32OwnerProcessID == cid) {
				
				s.Format(TEXT("Thread: tid:%u"), te.th32ThreadID);
				m_list.AddString(s);
			}

		} while (Thread32Next(h, &te));
	}
	m_list.AddString(TEXT("done"));
	CloseHandle(h);
}


void VEHConsole::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT tid = GetDlgItemInt(IDC_EDIT1);
	if (tid) {
		CString s;
		HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
		if (-1 == ResumeThread(h_thread)) {
			s.Format(TEXT("Resume thread %u failed"), tid);
			m_list.AddString(s);
		}
		else {
			s.Format(TEXT("Resume thread %u success"), tid);
			m_list.AddString(s);
		}
		CloseHandle(h_thread);
	}
	else {
		CString s;
		DWORD current_tid = GetCurrentThreadId();
		DWORD current_pid = GetCurrentProcessId();
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
		if (INVALID_HANDLE_VALUE == h) {
			m_list.AddString(TEXT("Create thread snapshot failed"));
			return;
		}
		if (Thread32First(h, &te)) {
			do {
				if (current_tid != te.th32ThreadID && current_pid == te.th32OwnerProcessID) {
					HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
					if (h_thread == INVALID_HANDLE_VALUE) {
						s.Format(TEXT("Open thread %u failed"), te.th32ThreadID);
						m_list.AddString(s);
						continue;
					}
					if (ResumeThread(h_thread) != -1) {
						s.Format(TEXT("Resume thread %u success"), te.th32ThreadID);
						m_list.AddString(s);
					}
					else {
						s.Format(TEXT("Resume thread %u failed"), te.th32ThreadID);
						m_list.AddString(s);
					}
					CloseHandle(h_thread);
				}
			} while (Thread32Next(h, &te));
		}
		CloseHandle(h);

	}

}
