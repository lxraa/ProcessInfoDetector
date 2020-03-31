#pragma once
#include <map>

// PatchConsole 对话框

class PatchConsole : public CDialogEx
{
	DECLARE_DYNAMIC(PatchConsole)

public:
	PatchConsole(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PatchConsole();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_combox;
	afx_msg void OnBnClickedButton1();
private:
	BOOL hooked_DebugCheck = FALSE;
	BOOL hooked_CreateToolhelp32Snapshot = FALSE;
public:
	CListBox m_list;
	afx_msg void OnCbnDropdownCombo1();
	std::map<CString, INT> kv;
};
