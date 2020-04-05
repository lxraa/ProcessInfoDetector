#pragma once

#include <map>
// HookDialog 对话框

class HookDialog : public CDialogEx
{
	DECLARE_DYNAMIC(HookDialog)

public:
	HookDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~HookDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList2();
	CListBox m_list;
	CComboBox m_combox;
	std::map<CString, INT> kv;
	afx_msg void OnCbnDropdownCombo1();
	afx_msg void OnBnClickedButton5();
};
