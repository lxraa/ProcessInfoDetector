// VEHConsole.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "VEHConsole.h"
#include "afxdialogex.h"


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
	DDX_Control(pDX, IDC_LIST1, info_box);
}


BEGIN_MESSAGE_MAP(VEHConsole, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &VEHConsole::OnBnClickedButton1)
END_MESSAGE_MAP()


// VEHConsole 消息处理程序
LONG vehHandler(PEXCEPTION_POINTERS except_info) {
	PVOID except_address = except_info->ExceptionRecord->ExceptionAddress;
	DWORD err_code = except_info->ExceptionRecord->ExceptionCode;
	CString s;
	s.Format(TEXT("Got exception at : %p code: %u"), except_address, err_code);
	s_vehconsole->info_box.AddString(s);

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
