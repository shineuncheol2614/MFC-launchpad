
// ServerDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "mmsystem.h" //MCI 사용 헤더

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment (lib,"winmm.lib")
#endif
#pragma warning(disable: 4996)


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
	ON_WM_DESTROY()
END_MESSAGE_MAP()
// CServerDlg 대화 상자
CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_event_list);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CServerDlg::On25001)
	ON_MESSAGE(25002, &CServerDlg::On25002)
END_MESSAGE_MAP()


// CServerDlg 메시지 처리기

void CServerDlg::AddEventString(const char *ap_string) {
	while (m_event_list.GetCount() > 3000) {
		m_event_list.DeleteString(0);
	}
	int index = m_event_list.InsertString(-1, ap_string);
	m_event_list.SetCurSel(index);
}

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

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

	 

	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET : 인터넷 기본 TCP 주소, 0은 2번째 인자 버전과 자동으로 프로토콜을 연결해줌
	sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//ip는 할당된 ip에 따른다: cmd->ipconfig
	srv_addr.sin_port = htons(18000);//htonl=host byte order to network order, s=unsigned short int

	bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	//소켓을 네트워크 카드에 연결
	AddEventString("서비스를 시작합니다.");//리스트박스에 출력
	listen(mh_listen_socket, 1);
	//몇 개 까지 받을거냐(동시에 처리하는 것이 아님,)빠르게 만들려면 소켓을 여러개 만들어 리슨도 여러개 만듬

	WSAAsyncSelect(mh_listen_socket, m_hWnd, 25001, FD_ACCEPT);//비동기함수
	//accept를 사용하면 실제 사용자가 접속할 때까지 프로그램이 응답상태에 빠지는 단점이있음
	//h_socket에서 FD_ACCEPT 이벤트가 발생하면 25001메세지를 이 대화상자에 보내라는 것->응답없음 상태에 빠지지않음


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::Play1() {
	BassDrum = mciSendCommand(1, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL);
	BassDrum = mciSendCommand(1, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay);
}

void CServerDlg::Play2() {
	DrumSnare = mciSendCommand(2, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL);
	DrumSnare = mciSendCommand(2, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay);
}

afx_msg LRESULT CServerDlg::On25001(WPARAM wParam, LPARAM lParam)
{//wParam -> 이벤트가 발생한 소켓의 핸들이 넘어옴, lParam에 소켓에 발생한 메세지가 옴
	//FD_ACCEPT 
	sockaddr_in client_addr;
	int sockaddr_in_size = sizeof(sockaddr_in);
	SOCKET h_socket = accept(mh_listen_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size);
	//소켓 자리에 wParam을 적어도 됨
	//2번 째 인자는 접속하는 client의 ip를 알 수 있게하는 역할(발신자 표시)
	//h_socket은 클론 소켓으로, listen 소켓을 복제함. listen 소켓은 사용되지않고 accept 소켓만 사용됨
	int i;
	for (i = 0; i < MAX_USER_COUNT; i++) {//비어있는 소켓을 찾는다
		if (m_user_list[i].h_socket == 0)
			break;
	}

	if (i < MAX_USER_COUNT) {//빈 방이 있는 상태
		m_user_list[i].h_socket = h_socket;//해당 소켓을 저장
		strcpy(m_user_list[i].ip_address, inet_ntoa(client_addr.sin_addr));
		//client address 복사
		WSAAsyncSelect(m_user_list[i].h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
		//FD_WRITE는 서버에서 데이터 보낼떄 이 메세지에는 비동기를 걸지 않음-> 보내는 시점을 서버가 알기 때문
		//FD_READ 상대편이 데이터를 보낼 때 발생되는 메세지
		//FD_CLOSE 상대편이 끊을 때
		// | 비트 연산자를 사용해야함, or를 하면 오버라이트 되서 마지막에 쓰인 메세지만 받게됨
		CString str;
		str.Format("%s에서 접속했습니다.", m_user_list[i].ip_address);
		AddEventString(str);

	}

	else {//꽉 찬 상태
		AddEventString("관리 최대 인원 초과!\n");
		closesocket(h_socket);//accept용 소켓 클로즈->필요가 없으므로
		//끊어진 client 다시 접속 시도, -> 끊고,, 악순환 반복 따라서 리슨 소켓을 닫으면 아예 못들어옴.
	}


	return 0;
}


afx_msg LRESULT CServerDlg::On25002(WPARAM wParam, LPARAM lParam)
{
	//lParam에는 FD_READ or FD_CLOSE 의 정보를 담고있음.
	 //wParam과 socket의 형은 unsigned int형
	CString str;

	if (WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(wParam, m_hWnd, 25002, FD_CLOSE);
		char key;
		recv(wParam, &key, 1, 0);
		if (key == 27) {
			char message_id;
			recv(wParam, &message_id, 1, 0);

			unsigned short body_size;
			recv(wParam, (char *)&body_size, 2, 0);

			char* p_body_data = NULL;
			if (body_size > 0) {
				p_body_data = new char[body_size];
				int total = 0, x, retry = 0;
				while (total < body_size) {
					x = recv(wParam, p_body_data + total, body_size - total, 0);
					if (x == SOCKET_ERROR)break;
					total = total + x;
					if (total < body_size) {
						Sleep(50);
						retry++;
						if (retry > 5)break;
					}
				}

			}

			//새로 추가된 부분
			if (message_id == 1) {
				for (i = 0; i < MAX_USER_COUNT; i++) {//어느 소켓에서 보냈는지 찾는과정
					if (m_user_list[i].h_socket == wParam)
						break;
				}
				//i번째 소켓에서 보낸 것을 확인함

				CString str2;//i번째 소켓의 주소와 받은 데이터를 str2에 연결함
				str2.Format("%s : %s", m_user_list[i].ip_address, p_body_data);
				AddEventString(str2);//서버 리스트 박스에 출력

				//접속한 모든 클라이언트에서 보냄(broad casting)
				for (i = 0; i < MAX_USER_COUNT; i++) {
					if (m_user_list[i].h_socket != INVALID_SOCKET) {//유효하지않은 소켓이 아니라면
						SendFrameData(m_user_list[i].h_socket, 1, str2.GetLength() + 1, (char *)(const char*)p_body_data);  //클라이언트 들에거 데이터 전송 Str2
						//바디 사이즈는 NULL공간을 포함해야하므로 +1
						//CString의 char*캐스팅은 (char *)(const char *)로 가능
						//여기서 i는 위의 i의 관계없음
					}
				}
			}
			//여기까지 기존에서 새로 추가된 부분
			

			WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
		}

	else {//FD_CLOSE
		//서버와 다른 부분-2
		closesocket(mh_socket);//클라이언트 소켓 클로즈
		mh_socket = INVALID_SOCKET;
		m_connect_flag = 0;//연결 해제 flag 변경
		//AddEventString("서버가 연결을 해제 했습니다.~");
		//-여기까지-2
	}

	//if (p_body_data != NULL)delete[] p_body_data;
	WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE)


}
	else {//FD_CLOSE
		closesocket(wParam);
		for (int i = 0; i < MAX_USER_COUNT; i++) {
			if (m_user_list[i].h_socket == wParam) {
				m_user_list[i].h_socket = INVALID_SOCKET;
				str.Format("사용자가 종료했습니다.: %s", m_user_list[i].ip_address);
				AddEventString(str);
				break;
			}
		}
	}
	
	return 0;
}


void CServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	for (int j = 0; j < MAX_USER_COUNT; j++) {
		if (m_user_list[j].h_socket != 0) {
			closesocket(m_user_list[j].h_socket);
		}
	}

	if (wDeviceID > 0)

	{
		mciSendCommand(1, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
		mciSendCommand(2, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
	}

}

void CServerDlg::SendFrameData(SOCKET ah_socket, char a_message_id,unsigned short int a_body_size, char* ap_send_data)//헤더와 데이터의 content를 보내는 함수
{
	char *p_send_data = new char[4 + a_body_size];//헤더 4 byte
	*p_send_data = 27;//key의 고유값을 27로 통일
	*(p_send_data + 1) = a_message_id;//message_id
	*(unsigned short *)(p_send_data + 2) = a_body_size;
	//일시적으로 char*를 short*에 맞게 변위를 조절

	memcpy(p_send_data + 4, ap_send_data, a_body_size);//데이터를 복사
	send(ah_socket, p_send_data, a_body_size + 4, 0);// 클라이언트에게 전송
													 //send(동기화함수->다 진행될때까지 벗어나지 못함)
	delete[] p_send_data;//전송후 삭제
}