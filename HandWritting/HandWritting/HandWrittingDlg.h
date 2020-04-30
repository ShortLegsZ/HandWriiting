
// HandWrittingDlg.h: 头文件
//

#pragma once


// CHandWrittingDlg 对话框
class CHandWrittingDlg : public CDialogEx
{
// 构造
public:
	CHandWrittingDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HANDWRITTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void readTrainData();
	void initSVM();
	void trainData();
	void predictData(int i, int j);

public:
	CStatic m_picture;
	CString m_show_predict;
	afx_msg void OnBnTrainbtn();
	afx_msg void OnBnPredictbtn();
	CString m_prompt_message;
	afx_msg void OnBnReaddatabtn();
	CString m_train_sample;
	CString m_test_name;
	CString m_test_sample_num;
	afx_msg void OnBnConfirmbtn();
	afx_msg void OnBnLastbtn();
	afx_msg void OnBnNextbtn();
	afx_msg void OnBnUsemode();
	CEdit m_CCheck;
	CEdit m_Npre;
	CString m_NPre;
};
