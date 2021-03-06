

// ServerDlg.h: 헤더 파일
//
#define MAX_USER_COUNT 100
#include "mmsystem.h" //MCI 사용 헤더
#pragma comment (lib,"winmm.lib")


struct UserData {
	SOCKET h_socket;
	char ip_address[16];
};

// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	afx_msg void SendFrameData(SOCKET ah_socket, char a_message_id, unsigned short int a_body_size, char* ap_send_data);
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	void CServerDlg::OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void CServerDlg::AddEventString(const char *ap_string);
	
private:
	CListBox m_event_list;
	SOCKET mh_listen_socket;
	UserData m_user_list[MAX_USER_COUNT];

protected:
	afx_msg LRESULT On25001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On25002(WPARAM wParam, LPARAM lParam);
	
public :
	void CServerDlg::Play1();
	void CServerDlg::Play2();
	/*void CServerDlg::Play3();
	void CServerDlg::Play4();
	void CServerDlg::Play5();
	void CServerDlg::Play6();
	void CServerDlg::Play7();
	void CServerDlg::Play8();
	void CServerDlg::Play9();
	void CServerDlg::Play10();
	void CServerDlg::Play11();
	void CServerDlg::Play12();
	void CServerDlg::Play13();
	void CServerDlg::Play14();
	void CServerDlg::Play15();
	void CServerDlg::Play16();
	*/
	int i = 0;

	MCI_OPEN_PARMS      mciOpen; //파일을 로드
	MCI_PLAY_PARMS       mciPlay; //파일을 재생
	MCI_STATUS_PARMS   mciStatus; //파일의 상태

	UINT wDeviceID = 0;

	DWORD LoadWAV(HWND hWnd, LPCTSTR lpszWave)  //파일 불러오기에 관한 함수
{
		DWORD Result;

		mciOpen.lpstrDeviceType = (LPCSTR)L"WaveAudio";

		//WaveAudio = WAV   MPEGVideo = mp3 

		mciOpen.lpstrElementName = lpszWave;

		Result = mciSendCommand(wDeviceID, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&mciOpen);

		if (Result)
			return Result;

		wDeviceID = mciOpen.wDeviceID;

		mciPlay.dwCallback = (DWORD)hWnd;

		if (Result)
			return Result;

		return 0;
	}

	DWORD BassDrum = LoadWAV(NULL, (LPCTSTR)L"BassDrum.WAV");
	DWORD DrumSnare = LoadWAV(NULL, (LPCTSTR)L"DrumSnare.WAV");

	SOCKET mh_socket;
	char m_connect_flag;


	void Sclose() { //파일 불러오기 종료

		if (wDeviceID > 0)

		{
			mciSendCommand(1, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
			mciSendCommand(2, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
		}
	}
};
