#pragma once


// VEHConsole 对话框

class VEHConsole : public CDialogEx
{
	DECLARE_DYNAMIC(VEHConsole)

public:
	VEHConsole(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~VEHConsole();
	PVOID handler = NULL;
	
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton1();
	CListBox m_list;
	VEHConsole *vehconsole;
	afx_msg void OnBnClickedButton2();
	CEdit m_edit;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
