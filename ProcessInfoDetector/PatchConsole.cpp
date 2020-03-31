// PatchConsole.cpp: 实现文件
//

#include "stdafx.h"
#include "ProcessInfoDetector.h"
#include "PatchConsole.h"
#include "afxdialogex.h"
#include "patch.h"

// PatchConsole 对话框

IMPLEMENT_DYNAMIC(PatchConsole, CDialogEx)

PatchConsole::PatchConsole(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{
	CString s(TEXT("Debug-Check-Func"));
	kv.insert(std::pair<CString, INT>(s, 0));
	s = TEXT("CreateToolhelp32Snapshot");
	kv.insert(std::pair<CString, INT>(s, 1));


}

PatchConsole::~PatchConsole()
{
}

void PatchConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(PatchConsole, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &PatchConsole::OnBnClickedButton1)
	ON_CBN_DROPDOWN(IDC_COMBO1, &PatchConsole::OnCbnDropdownCombo1)
END_MESSAGE_MAP()


// PatchConsole 消息处理程序




void PatchConsole::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString k;
	m_combox.GetLBText(m_combox.GetCurSel(), k);
	
	switch (kv[k]) {
	case(0): {
		if (hooked_DebugCheck) {
			return;
		}
		Patch *p = new Patch();
		if (p->removeDebuggerCheck_R3x64() && p->removePebDebuggerFlag_R3x64()) {
			m_list.AddString(TEXT("patch debugger function success"));
			hooked_DebugCheck = TRUE;
		}
		
		delete p;
		
		return;
	}
	case(1): {
		if (hooked_CreateToolhelp32Snapshot) {
			return;
		}
		Patch *p = new Patch();
		if (p->patchCreateToolhelp32Snapshot()) {
			m_list.AddString(TEXT("patch CreateToolhelp32Snapshot function success"));
			hooked_CreateToolhelp32Snapshot = TRUE;
		}
		delete p;
		return;
	}
	default: {
		return;
	}
	}
	
}


void PatchConsole::OnCbnDropdownCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_combox.ResetContent();
	for (auto iter = kv.begin(); iter != kv.end(); iter++) {
		m_combox.AddString(iter->first);
	}
}
