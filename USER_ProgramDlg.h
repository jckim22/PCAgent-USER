// USER_ProgramDlg.h: 헤더 파일
//

#pragma once
#include "IpcResponseListener.h"
#include "StressTest.h"  // StressTest 헤더 추가

#define WM_START_IPC (WM_USER + 1)
#define WM_SYSTEM_RESPONSE (WM_USER + 100)
class StressTest;
// CUSERProgramDlg 대화 상자
class CUSERProgramDlg : public CDialogEx
{
	// 생성입니다.
public:
	CUSERProgramDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.	
	afx_msg LRESULT OnSystemResponse(WPARAM w, LPARAM l);
	afx_msg LRESULT OnStartIpc(WPARAM, LPARAM);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_PROGRAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CButton m_FirstCheck;
	CButton m_SecondCheck;
	CButton m_ThirdCheck;
	IpcResponseListener listener;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

private:
	// StressTest 객체 (멤버 변수로 관리)
	StressTest* m_pStressTest;
};