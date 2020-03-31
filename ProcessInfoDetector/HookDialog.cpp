// HookDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "HookDialog.h"
#include "afxdialogex.h"
#include "patch.h"
#include <TlHelp32.h>

HookDialog *hookdialog = NULL;
PARG_CONTEXT args_CreateToolhelp32SnapShot = NULL;
PARG_CONTEXT args_NtTerminateProcess = NULL;
PARG_CONTEXT args_CreateFileW = NULL;
PARG_CONTEXT args_CreateFileA = NULL;

BOOL hooked_CreateToolhelp32SnapShot = FALSE;
BOOL hooked_NtTerminateProcess = FALSE;
BOOL hooked_CreateFileW = FALSE;
BOOL hooked_CreateFileA = FALSE;
// HookDialog 对话框

IMPLEMENT_DYNAMIC(HookDialog, CDialogEx)



HookDialog::HookDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{
	hookdialog = this;
}

HookDialog::~HookDialog()
{
}

void HookDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, info_box);
}


BEGIN_MESSAGE_MAP(HookDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &HookDialog::OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST2, &HookDialog::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON2, &HookDialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &HookDialog::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &HookDialog::OnBnClickedButton4)
END_MESSAGE_MAP()


// HookDialog 消息处理程序
void proc() {

	CString s;
	s.Format(TEXT("Invoke CreateToolhelp32SnapShot ret_address: %p param:%p %p %p %p"), args_CreateToolhelp32SnapShot->ret_address, args_CreateToolhelp32SnapShot->rcx, args_CreateToolhelp32SnapShot->rdx, args_CreateToolhelp32SnapShot->r8, args_CreateToolhelp32SnapShot->r9);
	hookdialog->info_box.AddString(s);
	return;
}


void HookDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hooked_CreateToolhelp32SnapShot) {
		return;
	}
	args_CreateToolhelp32SnapShot = new ARG_CONTEXT;
	Patch *p = new Patch();
	p->hookFunc(TEXT("kernel32.dll"), "CreateToolhelp32Snapshot", 15, (DWORD_PTR)proc,args_CreateToolhelp32SnapShot);
	this->info_box.AddString(TEXT("hook CreateToolhelp32Snapshot success"));
	delete p;
	hooked_CreateToolhelp32SnapShot = TRUE;
	return;
}

VOID proc_NtTerminateProcess() {
	CString s(TEXT("Invoke NtTerminateProcess"));
	hookdialog->info_box.AddString(s);

	MessageBox(NULL, TEXT("PAUSE"), TEXT("0"), MB_OK);
	return;
}
void HookDialog::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hooked_NtTerminateProcess) {
		return;
	}
	args_NtTerminateProcess = new ARG_CONTEXT;

	Patch *p = new Patch();
	p->hookFunc(TEXT("ntdll.dll"), "NtTerminateProcess", 16, (DWORD_PTR)proc_NtTerminateProcess, args_NtTerminateProcess);
	this->info_box.AddString(TEXT("hook NtTerminateProcess success"));
	delete p;
	hooked_NtTerminateProcess = TRUE;
	return;
}

void HookDialog::OnLbnSelchangeList2()
{
	// TODO: 在此添加控件通知处理程序代码
}



VOID proc_CreateFileW() {
	CString s;
	s.Format(TEXT("Invoke CreateFileW:%s %p %p %p"), args_CreateFileW->rcx, args_CreateFileW->rdx, args_CreateFileW->r8, args_CreateFileW->r9);

	hookdialog->info_box.AddString(s);
	return;
}

void HookDialog::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hooked_CreateFileW) {
		return;
	}
	args_CreateFileW = new ARG_CONTEXT;
	Patch *p = new Patch();
	p->hookFunc(TEXT("KERNELBASE.dll"), "CreateFileW", 15, (DWORD_PTR)proc_CreateFileW, args_CreateFileW);
	delete p;
	hookdialog->info_box.AddString(TEXT("hook CreateFileW success"));
	return;
}


VOID proc_CreateFileA() {
	CString s;
	s.Format(TEXT("Invoke CreateFileW:%p %s %p %p %p"),args_CreateFileA->ret_address, args_CreateFileA->rcx, args_CreateFileA->rdx, args_CreateFileA->r8, args_CreateFileA->r9);

	hookdialog->info_box.AddString(s);
	return;
}

void HookDialog::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hooked_CreateFileA) {
		return;
	}
	args_CreateFileA = new ARG_CONTEXT;
	Patch *p = new Patch();
	p->hookFunc(TEXT("KERNELBASE.dll"), "CreateFileA", 15, (DWORD_PTR)proc_CreateFileA, args_CreateFileA);
	delete p;
	hookdialog->info_box.AddString(TEXT("hook CreateFileA success"));
	return;
}
