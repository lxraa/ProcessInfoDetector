// HookDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "HookDialog.h"
#include "afxdialogex.h"
#include "patch.h"
#include <TlHelp32.h>

HookDialog *hookdialog = NULL;
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
END_MESSAGE_MAP()


// HookDialog 消息处理程序
void proc() {
	/*CString s(TEXT("Invoke CreateToolhelp32SnapShot"));
	hookdialog->info_box.AddString(s);*/
	return;
}


void HookDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	Patch *p = new Patch();
	p->hookFunc(TEXT("kernel32.dll"), "CreateToolhelp32Snapshot", 15, (DWORD_PTR)proc);
	this->info_box.AddString(TEXT("hook CreateToolhelp32Snapshot success"));
	delete p;
	return;
}

VOID proc_NtTerminateProcess() {
	//CString s(TEXT("Invoke NtTerminateProcess"));
	//hookdialog->info_box.AddString(s);
	//MessageBox(NULL, TEXT("PAUSE"), TEXT("0"), MB_OK);
	return;
}
void HookDialog::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	Patch *p = new Patch();
	p->hookFunc(TEXT("ntdll.dll"), "NtTerminateProcess", 16, (DWORD_PTR)proc_NtTerminateProcess);
	this->info_box.AddString(TEXT("hook NtTerminateProcess success"));
	delete p;
	TerminateProcess(GetCurrentProcess(), 1);
	return;
}

void HookDialog::OnLbnSelchangeList2()
{
	// TODO: 在此添加控件通知处理程序代码
}



