#pragma once


// DetectorConsole 对话框

class DetectorConsole : public CDialogEx
{

	DECLARE_DYNAMIC(DetectorConsole)

public:
	DetectorConsole(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DetectorConsole();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	CListBox m_list;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();


public:
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton1();
};