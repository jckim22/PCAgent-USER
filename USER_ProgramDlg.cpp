// USER_ProgramDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "USER_Program.h"
#include "USER_ProgramDlg.h"
#include "afxdialogex.h"
#include "madCHook.h"
#include "IpcClient.h"
#include "StressTest.h"  // StressTest 헤더 추가

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSERProgramDlg 대화 상자

CUSERProgramDlg::CUSERProgramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_PROGRAM_DIALOG, pParent)
	, m_pStressTest(nullptr)  // StressTest 포인터 초기화
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSERProgramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_FirstCheck);
	DDX_Control(pDX, IDC_CHECK2, m_SecondCheck);
	DDX_Control(pDX, IDC_CHECK3, m_ThirdCheck);
}

BEGIN_MESSAGE_MAP(CUSERProgramDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUSERProgramDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_SYSTEM_RESPONSE, &CUSERProgramDlg::OnSystemResponse)
	ON_MESSAGE(WM_START_IPC, &CUSERProgramDlg::OnStartIpc)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON2, &CUSERProgramDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CUSERProgramDlg 메시지 처리기

BOOL CUSERProgramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitializeMadCHook();
	HWND hwnd = GetSafeHwnd();
	listener.Start(hwnd);

	// StressTest 객체 생성 (기본 설정 사용)
	StressTest::TestConfig cfg;
	cfg.threadCount = 10;         // 필요 시 조정 가능
	cfg.requestsPerThread = 20;   // 필요 시 조정 가능
	m_pStressTest = new StressTest(cfg);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUSERProgramDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUSERProgramDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUSERProgramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUSERProgramDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL opt1 = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
	BOOL opt2 = (IsDlgButtonChecked(IDC_CHECK2) == BST_CHECKED);
	BOOL opt3 = (IsDlgButtonChecked(IDC_CHECK3) == BST_CHECKED);

	IpcClient client;
	client.SendOptions(opt1, opt2, opt3);
}

LRESULT CUSERProgramDlg::OnSystemResponse(WPARAM w, LPARAM l)
{
	char* text = (char*)l;

	AfxMessageBox(CA2W(text)); // MFC 메시지박스, CA2W -> ANSI 문자열(char -> 유니코드 문자열(wchar_t) 변환기

	free(text);  // strdup 해제

	return 0;
}

LRESULT CUSERProgramDlg::OnStartIpc(WPARAM, LPARAM)
{
	return 0;
}

void CUSERProgramDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.	
}

void CUSERProgramDlg::OnBnClickedButton2()
{
	// 테스트 시작 확인
	int result = AfxMessageBox(
		_T("IPC 동시 요청 스트레스 테스트를 시작합니다.\n\n")
		_T("- 10개 스레드가 동시에 실행됩니다\n")
		_T("- 각 스레드는 20개 요청을 보냅니다\n")
		_T("- 총 200개의 IPC 메시지가 전송됩니다\n")
		_T("- 각 요청마다 고유한 SEQ 번호가 부여됩니다\n\n")
		_T("계속하시겠습니까?"),
		MB_YESNO | MB_ICONQUESTION);

	if (result != IDYES) return;

	// 버튼 비활성화
	CWnd* pButton = GetDlgItem(IDC_BUTTON2);
	if (pButton) pButton->EnableWindow(FALSE);

	// 테스트 실행
	try
	{
		StressTest::TestConfig config;
		config.threadCount = 10;         // 동시 스레드 수
		config.requestsPerThread = 20;   // 스레드당 요청 수

		StressTest test(config);
		auto testResult = test.runConcurrentTest();

		// 필요 시 콘솔에 간단 출력 (총합/성공/실패/에러율)
		testResult.print(); // 혹은 제거 가능

		// 결과 메시지박스(간소화)
		CString msg;
		msg.Format(
			_T("테스트 완료\n\n")
			_T("총 요청: %d\n")
			_T("성공: %d\n")
			_T("실패: %d\n"),
			testResult.totalRequests,
			testResult.successCount,
			testResult.errorCount			
		);

		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	catch (...)
	{
		AfxMessageBox(_T("테스트 중 오류가 발생했습니다."), MB_ICONERROR);
	}

	// 버튼 다시 활성화
	if (pButton) pButton->EnableWindow(TRUE);
}