/*
MidiStation -> Gestural Music Interface

Module : KeybDlg.cpp
Purpose: Implementation for the CKeybDlg class
Created: CJP / 10-4-2002

P5 Integration, Copyright (c) 2003 by SIG CHI of ACM@UIUC	

Copyright (c) 2002 by C. Jiménez de Parga  
All rights reserved.
*/

#include "stdafx.h"	
#include "Keyb.h"	
#include "KeybDlg.h"
#include "Octave.h"	// Class definition of the COctave object
#include "GMdef.h"	// List of GM instruments, notes and keys
#include "Record.h"	// Class definition for record object
#include "p5dll.h"
#include "p5bend.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]  =  __FILE__;
#endif


char msgStatus[1024];
CP5DLL P5;
							// Declaration of the main objects
CDirectMusic CMusic8;
CDirectInputPort CInPort;
COutputPort COutPort;
CDLSLoader	DLSLoader;
CCollection Collection;
CInstrument Instrument;

COctave Keyboard[4];		// Object Keyboard with 4 octaves
CRecord rec;				// Object record
CDC *pDC;					// Pointer to the main DC	
CCriticalSection CS;		// Critical Section
CFont *pFont,*pModFont;		// Fonts of the CAbout dialog

KeybState State;			// Keyboard state enum variable	
BOOL bLastKey = FALSE;		// Flags for keyboard control
BOOL KeyPressed[MAX_KEYS];		// Flags of keys state
bool downPrev[5]={false,false,false,false,false};
COctave::note_info ReleaseInfo; // Information of the last pressed octave key 


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
//	DDX_Control(pDX, IDC_IMAGELOGO, m_ImageLogo);
	DDX_Control(pDX, IDC_STATIC_URL, m_Link);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_STATIC_URL, OnStaticUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeybDlg dialog

// Main dialog class constuctor
// Initializes member variables

CKeybDlg::CKeybDlg(CWnd* pParent /* = NULL*/)
: CDialog(CKeybDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeybDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon			=	AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bAbandon		=	FALSE;	
	m_bExitPlayBack =	TRUE;
	State			=	STOPPED;
	m_bSwSynth		=	FALSE;
}


void CKeybDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeybDlg)
	DDX_Control(pDX, IDC_NOTE_RANGE, m_NoteRange);
	DDX_Control(pDX, IDC_MEMPROG, m_MemProgress);
	DDX_Control(pDX, IDC_STOP, m_Stop);
	DDX_Control(pDX, IDC_REC, m_Rec);
	DDX_Control(pDX, IDC_PLAY, m_Play);
	DDX_Control(pDX, IDC_OUTPUT_PORTS, m_OutPortList);
	DDX_Control(pDX, IDC_INPUT_PORTS, m_InPortList);
	DDX_Control(pDX, IDC_MESSAGES_LIST, m_MessagesList);
	DDX_Control(pDX, IDC_INSTRUMENTS, m_InstList);

	DDX_Control(pDX, IDC_THUMBEDGE, m_ThumbEdge);
	DDX_Control(pDX, IDC_RINGEDGE, m_RingEdge);
	DDX_Control(pDX, IDC_PINKYEDGE, m_PinkyEdge);
	DDX_Control(pDX, IDC_MIDDLEEDGE, m_MiddleEdge);
	DDX_Control(pDX, IDC_INDEXEDGE, m_IndexEdge);
/*
	DDX_Control(pDX, IDC_THUMBSLIDER, m_ThumbSlider);
	DDX_Control(pDX, IDC_RINGSLIDER, m_RingSlider);
	DDX_Control(pDX, IDC_PINKYSLIDER, m_PinkySlider);
	DDX_Control(pDX, IDC_MIDDLESLIDER, m_MiddleSlider);
	DDX_Control(pDX, IDC_INDEXSLIDER, m_IndexSlider);
*/
	DDX_Control(pDX, IDC_THUMBCLICKSTATE, m_ThumbState);
	DDX_Control(pDX, IDC_RINGCLICKSTATE, m_RingState);
	DDX_Control(pDX, IDC_PINKYCLICKSTATE, m_PinkyState);
	DDX_Control(pDX, IDC_MIDDLECLICKSTATE, m_MiddleState);
	DDX_Control(pDX, IDC_INDEXCLICKSTATE, m_IndexState);

	DDX_Control(pDX, IDC_THUMB, m_Thumb);
	DDX_Control(pDX, IDC_RING, m_Ring);
	DDX_Control(pDX, IDC_PINKY, m_Pinky);
	DDX_Control(pDX, IDC_MIDDLE, m_Middle);
	DDX_Control(pDX, IDC_INDEX, m_Index);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeybDlg, CDialog)
	//{{AFX_MSG_MAP(CKeybDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_CBN_SELCHANGE(IDC_INSTRUMENTS, OnSelchangeInstruments)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_REC, OnRec)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_CBN_SELCHANGE(IDC_OUTPUT_PORTS, OnSelchangeOutputPorts)
	ON_CBN_SELCHANGE(IDC_INPUT_PORTS, OnSelchangeInputPorts)
	ON_CBN_SELCHANGE(IDC_NOTE_RANGE, OnSelchangeNoteRange)
	ON_COMMAND(IDR_HELP, OnHelp)
	ON_WM_ACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_INITMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeybDlg message handlers

// Function to enumerate midi I/O ports

void CKeybDlg::EnumPorts()
{
	INFOPORT Info;
	DWORD dwNumOutPorts = COutPort.GetNumPorts();
	DWORD dwNumInPorts = CInPort.GetNumPorts();
	DWORD dwCountInPorts = 0;
	BOOL bSelected = FALSE;

	// List all output ports
	
	for(DWORD nPortCount = 1;nPortCount<=dwNumOutPorts;nPortCount++)
	{
		COutPort.GetPortInfo(nPortCount,&Info);
		m_OutPortList.AddString(Info.szPortDescription);

		if (!bSelected) // Select the port
		{	
			COutPort.ActivatePort(&Info);
			bSelected = TRUE;
			nOutPortSel = nPortCount - 1;
			m_OutPortList.SetCurSel(nOutPortSel);
			if (Info.dwFlags & DMUS_PC_SOFTWARESYNTH)
			{	
				m_bSwSynth = TRUE; // It's working with a Sw. Synth.
				PrepareSoftwareSynth();
			}
		}
				
	}

	bSelected = FALSE;

	// List all input ports

	for(nPortCount = 1;nPortCount<=dwNumInPorts;nPortCount++)
	{
		CInPort.GetPortInfo(nPortCount,&Info);
		if (Info.dwType != DMUS_PORT_KERNEL_MODE)
		{
			m_InPortList.AddString(Info.szPortDescription);
			if (!bSelected) // Select the port
			{	
				CInPort.ActivatePort(&Info);
				bSelected = TRUE;
				nInPortSel = dwCountInPorts;
				m_InPortList.SetCurSel(nInPortSel);
			}
			dwCountInPorts++;
		}
	}
}	


// Overriden member function for receiving messages

void CDirectInputPort::RecvMidiMsg(REFERENCE_TIME lprt,DWORD dwChannel,DWORD dwMsg)
{
	BYTE Command,Channel,Note,Velocity;
	
	CSingleLock csl(&CS);
	
	csl.Lock(); // Protect the shared resource
	
	CInputPort::DecodeMidiMsg(dwMsg,&Command,&Channel,&Note,&Velocity);
	
	csl.Unlock();

	if ((Command == NOTE_ON)) //Channel #1 Note-On
    {                    
		csl.Lock();			// Enter into the critical section

		if (State == RECORDING) // In case it is recording
		{		
			if (rec.GetIndex()<rec.GetTotalMsgMemory())	// If there is space
			{	
				rec.StoreMidiMsg(dwMsg);	// Store the message in the array
				
				csl.Unlock();	// Exit critical section
				
				m_pKeybDlg->m_MemProgress.SetPos(rec.GetIndex());	
			} else						// The maximum space is finished
			{
				csl.Unlock();			// Exit critical section
				
				m_pKeybDlg->StopRecord();	//stops the recording
			}
		} else csl.Unlock();
							
		csl.Lock();
		
		m_pKeybDlg->RecvNote(Note);		//Displays the note in the keyboard
		
		csl.Unlock();
		
		m_pKeybDlg->AddListCtrl(Note,Velocity);	// Add the in	
	
	} else if (Command == NOTE_OFF)    //Channel #0 Note-Off
    {
		csl.Lock();	// Enter critial section
		
		if (State == RECORDING)	// In case it is recording
		{	
			if (rec.GetIndex() < rec.GetTotalMsgMemory())
			{	
				rec.StoreMidiMsg(dwMsg);	// Store the message in the array
				
				csl.Unlock();				// Leaves critical section
				
				m_pKeybDlg->m_MemProgress.SetPos(rec.GetIndex());
			} else	// no more space
			{
				csl.Unlock();				//Leaves critical section
				
				m_pKeybDlg->StopRecord();	//Stop recording
			}
		} else csl.Unlock();
		
		csl.Lock();						// Enter crtical section
		
		m_pKeybDlg->ReleaseNote(Note);	// Displays the note-off in the keyboard
		
		csl.Unlock();					// Leave critical section	
    }
}


// Add a subitem into the List Control

void CKeybDlg::StoresInformation(LVITEM &lvi,int nSubItem,CString strCad)
{
	lvi.mask =  LVIF_TEXT;
	lvi.iSubItem = nSubItem;
	lvi.pszText = (LPTSTR)(LPCTSTR)strCad;
	m_MessagesList.SetItem(&lvi);
}	

// Funtion to add MIDI information into the List Control

BOOL CKeybDlg::AddListCtrl(unsigned char note,unsigned char velocity)
{
	CString strEng,strEsp,strVelocity,strValue;
	
	strValue.Format("%i",note);
	strEng = GetEngValue(note);
	strEsp = GetEspValue(note);
	strVelocity.Format("%i",velocity);
	
	if (m_MessagesList.GetItemCount()>MAX_LIST_ITEMS)  // Prevent exceeding the capacity
		m_MessagesList.DeleteAllItems();

	// Stores the information

	LVITEM lvi;
	lvi.mask =  LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)"1";
	m_MessagesList.InsertItem(&lvi);

	StoresInformation(lvi,1,strEng);
	StoresInformation(lvi,2,strEsp);
	StoresInformation(lvi,3,strValue);
	StoresInformation(lvi,4,strVelocity);
	
	// Sets the selection line
	m_MessagesList.SetItemState(0,LVIS_SELECTED ,LVIS_SELECTED);

	return TRUE;
}

// Initialize CMidiPort library
// Detects DirectX8.0

BOOL CKeybDlg::InitializeLibrary()
{
	if FAILED(CMusic8.Initialize(m_hWnd))
	{
		CString strMsg = "Can't initialize DirectMusic objects.";
		strMsg+= " Ensure you have an audio card device and DirectX 8.0 or above ";
		strMsg+= "installed in your system.";
		AfxMessageBox(strMsg);
		return false;
	}
	return true;
	
}	

// Initialize DirectMusic objects

void CKeybDlg::InitializeDirectMusic()
{
	CInPort.Initialize(CMusic8);   // Initialize the input port	
	COutPort.Initialize(CMusic8);  // Initialize the output port	
	DLSLoader.Initialize();		   // Initialize the Loader object
	CInPort.SetDlgReference(this); // Set the reference to the dialog object
	EnumPorts();				   // Enumerate ports and select the default one
	CInPort.ActivateNotification();	// Activates event notification
	CInPort.SetThru(0,0,COutPort);	// Activates the Midi thru with the default output port
}	

// Initializes keyboard object

void CKeybDlg::InitializeKeyboard()
{
	pDC = GetDC();
	Keyboard[0].Initialize(pDC,113+27,155+138);
	Keyboard[0].Draw();			// Draw the octave into the memory DC		
	Keyboard[1].Initialize(pDC,113+174,155+138);
	Keyboard[1].Draw();
	Keyboard[2].Initialize(pDC,113+321,155+138);
	Keyboard[2].Draw();
	Keyboard[3].Initialize(pDC,113+468,155+138);
	Keyboard[3].Draw();	
	Keyboard[0].SetRange(36);	// Sets the octaves range
	Keyboard[1].SetRange(48);
	Keyboard[2].SetRange(60);
	Keyboard[3].SetRange(72);
	nLoNote = 36;
	nHiNote = 83;
	for (int nKey = 0;nKey < 12;nKey++) // Sets the keys state
		KeyPressed[nKey]=FALSE;
}		



// Populate the instruments combo box  

void CKeybDlg::InitializeInstruments()
{
	for (int nInst = 0;nInst<128;nInst++)
		m_InstList.AddString(GMInstruments[nInst]);
	m_InstList.SetCurSel(0);

}

// Insert the respective columns in the control list

void CKeybDlg::InitializeMessageList()
{
	m_MessagesList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	m_MessagesList.InsertColumn(0,"Channel",LVCFMT_LEFT,130);
	m_MessagesList.InsertColumn(1,"Note",LVCFMT_LEFT,130);
	m_MessagesList.InsertColumn(2,"Nota",LVCFMT_LEFT,130);
	m_MessagesList.InsertColumn(3,"Value",LVCFMT_LEFT,130);
	m_MessagesList.InsertColumn(4,"Velocity",LVCFMT_LEFT,130);
}	


// Initialize some required controls

void CKeybDlg::InitializeControls()
{
	m_Play.EnableWindow(FALSE);
	m_Stop.EnableWindow(FALSE);
	m_NoteRange.SetCurSel(0);
	m_Play.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PLAY),IMAGE_ICON,0,0,0));
	m_Stop.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_STOP),IMAGE_ICON,0,0,0));
	m_Rec.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_RECORD),IMAGE_ICON,0,0,0));
//	m_Image.SetBitmap((HBITMAP)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_LOGO),IMAGE_BITMAP,0,0,
//	LR_LOADMAP3DCOLORS |LR_LOADTRANSPARENT));

}

// Create the playback thread event

void CKeybDlg::InitializeEventHandler()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

// Initdialog message handler 
// Initialize the application objects

BOOL CKeybDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0)  ==  IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu  =  GetSystemMenu(FALSE);
	if (pSysMenu !=  NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Call initialization member functions
	

	if (InitializeLibrary())
	{		
		InitializeDirectMusic();
		InitializeKeyboard();
		if (!m_bSwSynth)
			InitializeInstruments();
		InitializeMessageList();
		InitializeControls();
		InitializeEventHandler();
		SetTimer(1,300,NULL);
	} else EndDialog(0);	// The user doesn't have DirectX installed, 
							// then leaves application


	if (P5.P5_Init() != TRUE)
	{
		sprintf(msgStatus, "No P5 detected...\r\n");
		GetDlgItem(IDC_STATUSMSG)->SetWindowText(msgStatus);
	}
	else
	{
		strcat(msgStatus, "P5 Found...\r\n");
		GetDlgItem(IDC_STATUSMSG)->SetWindowText(msgStatus);

		P5Bend_Init(&P5, 0);
		P5.P5_SetMouseState(0, false);	
	}

	SetTimer(3100, 10, NULL);

	m_Index.SetRange(0,63);
	m_Index.SetStep(1);

	m_Middle.SetRange(0,63);
	m_Middle.SetStep(1);
	
	m_Ring.SetRange(0,63);
	m_Ring.SetStep(1);
	
	m_Pinky.SetRange(0,63);
	m_Pinky.SetStep(1);
	
	m_Thumb.SetRange(0,63);
	m_Thumb.SetStep(1);

/*
	m_ThumbSlider.SetRange(0, 30);
	m_ThumbSlider.SetPos(nBendSensitivity[P5_THUMB]);
	m_ThumbSlider.SetTicFreq(5);

	m_IndexSlider.SetRange(0, 30);
	m_IndexSlider.SetPos(nBendSensitivity[P5_INDEX]);
	m_IndexSlider.SetTicFreq(5);

	m_MiddleSlider.SetRange(0, 30);
	m_MiddleSlider.SetPos(nBendSensitivity[P5_MIDDLE]);
	m_MiddleSlider.SetTicFreq(5);

	m_RingSlider.SetRange(0, 30);
	m_RingSlider.SetPos(nBendSensitivity[P5_RING]);
	m_RingSlider.SetTicFreq(5);

	m_PinkySlider.SetRange(0, 30);
	m_PinkySlider.SetPos(nBendSensitivity[P5_PINKY]);
	m_PinkySlider.SetTicFreq(5);

*/	


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKeybDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0)  ==  IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// On paint function
// Paints the dialog object and the keyboard concurrently
// another process could be also using the blit function of the Keyboard object

void CKeybDlg::OnPaint() 
{
	CSingleLock csl(&CS);
	csl.Lock();
	Keyboard[0].Blit();
	Keyboard[1].Blit();
	Keyboard[2].Blit();
	Keyboard[3].Blit();
	csl.Unlock();
	//CDialog::OnPaint();
	
	// yo Josh, liable for bugs here
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

}

// The system calls this to obtain the cursor to display while the user drags
// the minimized window.

HCURSOR CKeybDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


// Member function called when a note-on display is required

void CKeybDlg::RecvNote(unsigned char note)
{
	int nOctave,nNote;
	if ((note >= nLoNote) && (note <= nHiNote))  // Detects if the note falls in the actual range
	{	
		nOctave = int((note-nLoNote)/12);
		nNote = note%12;
		Keyboard[nOctave].DrawOnKey(nNote);
	}
}

// Member function called when note-off display is required

void CKeybDlg::ReleaseNote(unsigned char note)
{
	int nOctave,nNote;
	if ((note >= nLoNote) && (note <= nHiNote))
	{	
		nOctave = int((note-nLoNote)/12);
		nNote = note%12;
		Keyboard[nOctave].ReleaseKey(nNote);
	}
}	

// Function called when the keyboard sends a note the the port

void CKeybDlg::SendMidiMsg(unsigned char Note,BOOL bOn)
{
	DWORD dwMsg;
	
	CSingleLock csl(&CS);
	
	csl.Lock();							// Protect the code section, some resources are shared
	
	dwMsg = COutputPort::EncodeMidiMsg(((bOn) ? NOTE_ON : NOTE_OFF),0,Note,0x7f);
	
	COutPort.SendMidiMsg(dwMsg);		// Send it
	
	if (State == RECORDING)							// If recording
	{	
		if (rec.GetIndex() < rec.GetTotalMsgMemory())		// If there is enough space
		{	
			rec.StoreMidiMsg(dwMsg);	// Store the note in the record object
			csl.Unlock();		
			m_MemProgress.SetPos(rec.GetIndex());
		} else							// The limit has been reached
		{
			csl.Unlock();				// Stop recording and leaves the critical section
			StopRecord();
		}
	} else csl.Unlock();
}


// Member function called when the mouse left button is pressed
// A key-on must be marked

void CKeybDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSingleLock csl(&CS);
	
	int nOct = 0;
	
	do					// Detects which key has been pressed
	{
		csl.Lock();		// Enter into the critical section
		ReleaseInfo = Keyboard[nOct].IsKeyPressed(point.x,point.y); 
		csl.Unlock();	// Frees the critical section
		nOct++;
	} while ((nOct < MAX_OCTAVES) && (ReleaseInfo.note<0));	// Check all octaves
	
	if (ReleaseInfo.note >= 0)					// A key has been pressed
	{											// ** note_info.note is -1 when no key is pressed **
		SendMidiMsg(ReleaseInfo.note,TRUE);		// Send the note to the port
		AddListCtrl(ReleaseInfo.note,127);		// Show it in the list control
		csl.Lock();
		ReleaseInfo.octave = nOct-1;					// Store the last pressed key
		Keyboard[ReleaseInfo.octave].DrawOnKey(ReleaseInfo.key);	//Blits the key on 
		csl.Unlock();							
		bLastKey = TRUE;							// There is a key to release
	};
	CDialog::OnLButtonDown(nFlags, point);
}

// Member function called when the mouse left button is pressed and moved
// A key-on must be marked and another key must be released

void CKeybDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CSingleLock csl(&CS);
	COctave::note_info LastInfo;
	
	if (nFlags & MK_LBUTTON)	// If the left button is pressed when moving
	{	
		int nOct = 0;
		do
		{
			csl.Lock();			// Detects which key has been pressed
			LastInfo = Keyboard[nOct].IsKeyPressed(point.x,point.y); 
			csl.Unlock();
			nOct++;
		} while ((nOct<MAX_OCTAVES) && (LastInfo.note<0));
		if ((LastInfo.key != ReleaseInfo.key)) // Avoid redundace over the key
		{
			if (bLastKey)					// If there was a key to release
			{
				SendMidiMsg(ReleaseInfo.note,FALSE);	// Turn off the note
				
				csl.Lock();
				Keyboard[ReleaseInfo.octave].ReleaseKey(ReleaseInfo.key);	// Release the key
				csl.Unlock();
				
				bLastKey = FALSE;
			}
			if (LastInfo.note >= 0)		// In case there is a new key pressed
			{	
				SendMidiMsg(LastInfo.note,TRUE);	// Sends the note
				AddListCtrl(LastInfo.note,127);	// Reports to the control list
				csl.Lock();
				LastInfo.octave = nOct-1;				// Blits the new key
				Keyboard[LastInfo.octave].DrawOnKey(LastInfo.key);
				csl.Unlock();
				
				ReleaseInfo = LastInfo;				// Store the last pressed key
				bLastKey = TRUE;					// There is a key to release stored in ReleaseInfo
				
			} else 	
			{	
				ReleaseInfo.key = 12;			   // Key outside a valid zone
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

// Member function called when double click
// A key-on must be marked

void CKeybDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags,point);
	CDialog::OnLButtonDblClk(nFlags, point);
}

// Member function called when the mouse left button is released
// A key-off must be marked

void CKeybDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CSingleLock csl(&CS);

	if (bLastKey)	// If there was a key to release
	{
		SendMidiMsg(ReleaseInfo.note,FALSE);	// Turn off the note
		
		csl.Lock();	//Displays the key turned off
		Keyboard[ReleaseInfo.octave].ReleaseKey(ReleaseInfo.key);	
		csl.Unlock();
		
		bLastKey = FALSE;	// There is no key to relase
	}
	CDialog::OnLButtonUp(nFlags, point);
}




// This member function handles the main dialog onfocus message
// and proceeds to release the keyboard octave(a computer key could be pressed when
// the main dialog hasn't got the focus)

void CKeybDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	

	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	CSingleLock csl(&CS);
	
	if (nState==WA_INACTIVE)  
	{	
		csl.Lock();
		for (int nKeyCount = 0;nKeyCount < 24;nKeyCount++)
			Keyboard[int(nKeyCount/12)+2].ReleaseKey(nKeyCount%12); // Disable the non focused keyboard
		
		csl.Unlock();
		
		COutPort.SendMidiMsg(COutputPort::EncodeMidiMsg(ALLNOTES_OFF,0,123,0));		
	}

}


// In this case we prevent the keyboard keys remain pressed when the 
// user initializes the menu

void CKeybDlg::OnInitMenu(CMenu* pMenu) 
{
	CDialog::OnInitMenu(pMenu);
	
	OnActivate(WA_INACTIVE,NULL,FALSE);
}


// Member function to handle keyboard events

void CKeybDlg::KeyMessage(int nPosKey,BOOL NoteType,int nOctave)
{
	unsigned char Note;
	
	CSingleLock csl(&CS);
	
	switch(NoteType)
	{
		case TRUE:	// Key pressed
			if (!KeyPressed[nPosKey])
			{	
				
				csl.Lock();				// Access to a shared resource
						
				Keyboard[nOctave+2].DrawOnKey(nPosKey%12);
				Note = Keyboard[nOctave+2].GetKeyNote(nPosKey%12);
			
				csl.Unlock();

				SendMidiMsg(Note,NoteType);	//Plays the note
				AddListCtrl(Note,127);		// Adds the note to the list
				KeyPressed[nPosKey] = TRUE; // The key holds pressed
			}
		break;
		case FALSE:	// Key released
			
			csl.Lock();
			
			Keyboard[nOctave+2].ReleaseKey(nPosKey%12);
			
			csl.Unlock();
			
			SendMidiMsg(Keyboard[nOctave+2].GetKeyNote(nPosKey%12),NoteType);
			KeyPressed[nPosKey] = FALSE;	// The key is not already pressed
		break;
	}

}


// Member function to extract the pressed key index in the array

void CKeybDlg::OnKeyRecv(char KeyCode,BOOL NoteType)
{
	int nPosKey = strchr(KeyboardNotes,KeyCode)-KeyboardNotes;
	if (nPosKey >= 0) 
		KeyMessage(nPosKey,NoteType,int(nPosKey/12));
}	


// Pretranslate message function handler
// Detects a keyboard message before translating it

BOOL CKeybDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
		case WM_SYSKEYDOWN:	// A system key, normaly the ALT key
			OnActivate(WA_INACTIVE,NULL,FALSE);
		break;
		case WM_KEYDOWN:
			OnKeyRecv((char)pMsg->wParam,TRUE);
		break;

		case WM_KEYUP:
			OnKeyRecv((char)pMsg->wParam,FALSE);
		break;
		default:
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


// Instrument selection message handler

void CKeybDlg::OnSelchangeInstruments() 
{
	
	BYTE bInstrument = (BYTE)m_InstList.GetCurSel();
	INSTRUMENTINFO InstInfo;

	if (m_bSwSynth)
	{
		Collection.EnumInstrument(bInstrument,&InstInfo);
		Collection.GetInstrument(Instrument,&InstInfo);
		Instrument.SetPatch(0);
		COutPort.DownLoadInstrument(Instrument);
		bInstrument = 0;
	} 
	
	COutPort.SendMidiMsg(COutputPort::EncodeMidiMsg(PATCH_CHANGE,0,bInstrument,0));
}


// Gets the note in english format

CString CKeybDlg::GetEngValue(unsigned char note)
{
	CString strNote;
	strNote = MIDINotesEng[note%12];
	return strNote;
}

// Gets the note in spanish format

CString CKeybDlg::GetEspValue(unsigned char note)
{
	CString strNote;
	strNote = MIDINotesEsp[note%12];
	return strNote;
}


// Member function called when the application is closed

void CKeybDlg::CloseDown()
{
	ReleaseAllKeys();
	CloseHandle(m_hEvent);
	CInPort.BreakThru(0,0);
	CInPort.TerminateNotification();
	DLSLoader.UnLoadCollection(Collection);
	COutPort.DownLoadInstrument(Instrument);
	CDialog::EndDialog(0);
}


// Timer to detect when the application is ready to exit
// The user wants to abandon and is not in playback mode

void CKeybDlg::OnTimer(UINT nIDEvent) 
{
	if ((m_bExitPlayBack) && (m_bAbandon)) 
		CloseDown();

//more bugs coming because of closedown above
	if (P5.m_P5Devices!=NULL)
	{
		int value;

		value=((unsigned char)P5.m_P5Devices[0].m_byBendSensor_Data[P5_INDEX]);
		m_Index.SetPos(63-value);
		
		value=((unsigned char)P5.m_P5Devices[0].m_byBendSensor_Data[P5_MIDDLE]);
		m_Middle.SetPos(63-value);
		
		value=((unsigned char)P5.m_P5Devices[0].m_byBendSensor_Data[P5_RING]);
		m_Ring.SetPos(63-value);
		
		value=((unsigned char)P5.m_P5Devices[0].m_byBendSensor_Data[P5_PINKY]);
		m_Pinky.SetPos(63-value);

		value=((unsigned char)P5.m_P5Devices[0].m_byBendSensor_Data[P5_THUMB]);
		m_Thumb.SetPos(63-value);

/*		P5Bend_SetClickSensitivity(P5_THUMB, m_ThumbSlider.GetPos());
		P5Bend_SetClickSensitivity(P5_INDEX, m_IndexSlider.GetPos());
		P5Bend_SetClickSensitivity(P5_MIDDLE, m_MiddleSlider.GetPos());
		P5Bend_SetClickSensitivity(P5_RING, m_RingSlider.GetPos());
		P5Bend_SetClickSensitivity(P5_PINKY, m_PinkySlider.GetPos());
*/
		P5Bend_SetClickSensitivity(P5_THUMB, 5);
		P5Bend_SetClickSensitivity(P5_INDEX, 5);
		P5Bend_SetClickSensitivity(P5_MIDDLE, 5);
		P5Bend_SetClickSensitivity(P5_RING, 5);
		P5Bend_SetClickSensitivity(P5_PINKY, 5);


		P5Bend_Process();
		m_IndexState.SetCheck(bP5ClickLevel[P5_INDEX]);
		m_MiddleState.SetCheck(bP5ClickLevel[P5_MIDDLE]);
		m_RingState.SetCheck(bP5ClickLevel[P5_RING]);
		m_PinkyState.SetCheck(bP5ClickLevel[P5_PINKY]);
		m_ThumbState.SetCheck(bP5ClickLevel[P5_THUMB]);

		sprintf (msgStatus, "%d", nP5ClickEdge[P5_INDEX]);
		m_IndexEdge.SetWindowText(msgStatus);

		sprintf (msgStatus, "%d", nP5ClickEdge[P5_MIDDLE]);
		m_MiddleEdge.SetWindowText(msgStatus);

		sprintf (msgStatus, "%d", nP5ClickEdge[P5_RING]);
		m_RingEdge.SetWindowText(msgStatus);

		sprintf (msgStatus, "%d", nP5ClickEdge[P5_PINKY]);
		m_PinkyEdge.SetWindowText(msgStatus);

		sprintf (msgStatus, "%d", nP5ClickEdge[P5_THUMB]);
		m_ThumbEdge.SetWindowText(msgStatus);

		for(int fingers=0; fingers<5; fingers++)
		{
			//if(!downPrev[fingers])
			if(bP5ClickLevel[fingers]) // if a finger is clickstate or not
					{
					/*downPrev[fingers]=TRUE;*/ // a "key" was pressed
					//OnKeyRecv(char(fingers+'A')%12,TRUE);
					switch(fingers)
					{
						case 0:
							OnKeyRecv('B',TRUE);
							break;
						case 1:
							OnKeyRecv('N',TRUE);
							break;
						case 2:
							OnKeyRecv('M',TRUE);
							break;
						case 3:
							OnKeyRecv('Q',TRUE);
							break;
						case 4:
							OnKeyRecv('W',TRUE);
							break;
					}
				}
			else if((!bP5ClickLevel[fingers]) /*&& downPrev[fingers]*/) //[fingers] 
				{
					downPrev[fingers]=FALSE;
					//OnKeyRecv((char(fingers)+'A')%12,FALSE);
					switch(fingers)
					{
						case 0:
							OnKeyRecv('B',FALSE);
							break;
						case 1:
							OnKeyRecv('N',FALSE);
							break;
						case 2:
							OnKeyRecv('M',FALSE);
							break;
						case 3:
							OnKeyRecv('Q',FALSE);
							break;
						case 4:
							OnKeyRecv('W',FALSE);
							break;
					}
				}
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}


// A simple playback thread 

UINT ProcessMidiRecord(LPVOID lpParam)
{
	DWORD dwMsg; // Millisecond counter
	BYTE Note,Velocity,Channel,Command;
		
	CSingleLock csl(&CS);
	CKeybDlg *ptr = (CKeybDlg*)lpParam;
	
	// Executes the thread while there are messages in the record object array
	// and the user doesn't want to exit
		
	rec.ResetIndex();

	while ((rec.GetIndex() <  rec.GetTotalRecorded() - 1) && (!ptr->m_bAbandon) && (!ptr->m_bExitPlayBack))
	{
		
		csl.Lock();						// Protects the critical section													
				
		dwMsg = rec.GetStoredMessage();
		COutPort.SendMidiMsg(dwMsg);	//Sends the stored message to the port
		CInputPort::DecodeMidiMsg(dwMsg,&Command,&Channel,&Note,&Velocity);
				
		if (Command == NOTE_ON)
			ptr->RecvNote(Note);	// Sends the note to the drawing function
		else
			ptr->ReleaseNote(Note);	
		csl.Unlock();
		
		if (Command == NOTE_ON) ptr->AddListCtrl(Note,Velocity); // Adds the messages to the list control								
							
		// Waits for the elapsed time between messages

		WaitForSingleObject(ptr->m_hEvent, rec.GetTime(rec.GetIndex() + 1) - rec.GetTime(rec.GetIndex()));

		rec.StepIndex();
	
		ptr->m_MemProgress.SetPos(rec.GetIndex());	
	}
	
	ptr->StopRecord();	// Calls the stop record member function
	
	return 0;
	
}	


// Record button message handler
// Called when the user wants to record

void CKeybDlg::OnRec() 
{
	
	m_MemProgress.SetRange32(0,rec.GetTotalMsgMemory());
	rec.GetStartTime();
	m_Rec.EnableWindow(FALSE);
	m_Play.EnableWindow(FALSE);
	m_Stop.EnableWindow(TRUE);
	State = RECORDING;
	SetFocus();
}

// Play button message handler
// Called when the user wants to playback

void CKeybDlg::OnPlay() 
{
	
	m_Rec.EnableWindow(FALSE);
	m_Play.EnableWindow(FALSE);
	m_MemProgress.SetRange32(0,rec.GetTotalRecorded()-1);
	m_bExitPlayBack = FALSE;
	State = PLAYING;
	
	AfxBeginThread(ProcessMidiRecord,this);  // Begins the playback thread
}



// Member function called when a note off general message must be sent
// All notes must be released

void CKeybDlg::ReleaseAllKeys()
{
	int i,j;

	for(i = 0;i<4;i++)
		for (j = 0;j<12;j++)
			Keyboard[i].ReleaseKey(j);

	COutPort.SendMidiMsg(COutputPort::EncodeMidiMsg(ALLNOTES_OFF,0,123,0));
}


// StopRecord member function
// Called when the user stops either the record or the playback

void CKeybDlg::StopRecord()
{

	CSingleLock csl(&CS);

	csl.Lock();
	
	if (State == RECORDING)
		State = STOPPED;
	else 
	{
		m_bExitPlayBack = TRUE;
		SetEvent(m_hEvent);
		ResetEvent(m_hEvent);
		ReleaseAllKeys();	
	}	
	
	csl.Unlock();
	
	m_Rec.EnableWindow(TRUE);
	m_Play.EnableWindow(TRUE);
	SendMessage(WM_SETFOCUS,0,0);
}

// Stop button message handler
// Called when the user want to stop either the playback or the recording

void CKeybDlg::OnStop() 
{
	
	StopRecord();	
}

// On close message handler

void CKeybDlg::OnClose() 
{
	m_bAbandon = TRUE; // Notify the timer that the user wants to exit 
	SetEvent(m_hEvent);

//	CDialog::OnDestroy();

	P5.P5_SetMouseState(0, true);
	
}

// Clear the button message handler
// Called to clear the control list items

void CKeybDlg::OnClear() 
{
	m_MessagesList.DeleteAllItems();
}


// Prepares the built-in instrument set of the selected
// software sytnhesizer and activates port effects

void CKeybDlg::PrepareSoftwareSynth()
{
	DWORD dwIndex = 0;
	INSTRUMENTINFO InstInfo;
	CString strInst;

	DLSLoader.LoadDLS(NULL,Collection);	// Loads the standard GM set
	m_InstList.ResetContent();
	
	while (Collection.EnumInstrument(dwIndex++,&InstInfo) == S_OK)
	{	
		strInst.Format(" %d ",dwIndex);	// List the instruments		
		strInst+=InstInfo.szInstName;	// and adds them to the list 
		m_InstList.AddString(strInst);
	}
	
	m_InstList.SetCurSel(0);
	OnSelchangeInstruments();		    // Selects the first one
	COutPort.SetEffect(TRUE,SET_REVERB | SET_CHORUS);	// Activate effects
}		


// Output ports combo box message handler 
// to select the new output port

void CKeybDlg::OnSelchangeOutputPorts() 
{
	
	// Get the item index on the list 
	INFOPORT Info;
	DWORD nSel = (DWORD)m_OutPortList.GetCurSel();
	if (nSel != nOutPortSel)	// Check if it is a different port
	{
		COutPort.ReleasePort();				// Destroys the last output port	
		COutPort.GetPortInfo(nSel + 1 ,&Info);
		COutPort.ActivatePort(&Info);
		if (Info.dwFlags & DMUS_PC_SOFTWARESYNTH)
		{
			m_bSwSynth = TRUE;
			PrepareSoftwareSynth();
		}
		else 
		{	
			m_bSwSynth = FALSE;
			m_InstList.ResetContent();
			InitializeInstruments();		
		}
		
		CInPort.BreakThru(0,0);				// Closedown the thru	
		CInPort.SetThru(0,0,COutPort);		// Sets the new thru
		m_InstList.SetCurSel(0);			// Reset the instrument selection
		nOutPortSel = nSel;					// Stores the last selected port
	}
	
	SetFocus();
}


// Input ports combo box message handler 
// to select the new input port

void CKeybDlg::OnSelchangeInputPorts() 
{
	
	// Get the item index on the list 
	INFOPORT Info;
	DWORD nSel = (DWORD)m_InPortList.GetCurSel();

	if (nSel != nInPortSel)		// Check if it is a different port
	{
		CInPort.TerminateNotification();	// Terminates the notification in the input port
		CInPort.BreakThru(0,0);				// Closedown the thru
		CInPort.ReleasePort();				// Destroys the port
		CInPort.GetPortInfo(nSel + 1,&Info);
		CInPort.ActivatePort(&Info);	// Activates the new input port in DirectMusic objects
		CInPort.ActivateNotification();
		CInPort.SetThru(0,0,COutPort);			// Set the thru with the new output port
		nInPortSel = nSel;					// Stores the last selected port
	}	
	
	SetFocus();
}


// Note range combo box
// called to select a new note range in the keyboard octaves

void CKeybDlg::OnSelchangeNoteRange() 
{
	
	int nSel = m_NoteRange.GetCurSel();
	
	CSingleLock csl(&CS);
	
	csl.Lock();		// Protect the Keyboard object, is shared
	
	ReleaseAllKeys();	// Turn off all the keys
	
	switch(nSel)
	{
		case 0:
			Keyboard[0].SetRange(36);	// Octave3 --> Octave6
			Keyboard[1].SetRange(48);
			Keyboard[2].SetRange(60);
			Keyboard[3].SetRange(72);
			nLoNote = 36;
			nHiNote = 83;
			break;
		case 1:
			Keyboard[0].SetRange(24);	// Octave1 --> Octave 4
			Keyboard[1].SetRange(36);
			Keyboard[2].SetRange(48);
			Keyboard[3].SetRange(60);
			nLoNote = 24;
			nHiNote = 71;
			break;
		case 2:	
			Keyboard[0].SetRange(72);	// Octave5 --> Octave 8
			Keyboard[1].SetRange(84);
			Keyboard[2].SetRange(96);
			Keyboard[3].SetRange(108);
			nLoNote = 72;
			nHiNote = 119;
			break;
		case 3:	
			Keyboard[0].SetRange(0);	// Octave -1 --> Octave 2
			Keyboard[1].SetRange(12);
			Keyboard[2].SetRange(24);
			Keyboard[3].SetRange(36);
			nLoNote = 0;
			nHiNote = 47;
			break;
		default:
			break;
	}

	csl.Unlock();
}		

// Help menu item message handler

void CKeybDlg::OnHelp() 
{
	CAboutDlg CAbout;	//Invoke the About dialog box 
	CAbout.DoModal();
}



// Enter point for AboutBox initialization

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	HBITMAP hbm;
	LOGFONT lf;
	HFONT hFont;
/*	hbm = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_CODEPROJECT),IMAGE_BITMAP,0,0,
	LR_LOADMAP3DCOLORS |LR_LOADTRANSPARENT);
	m_Image.SetBitmap(hbm);
	hbm = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_LOGO),IMAGE_BITMAP,0,0,
	LR_LOADMAP3DCOLORS |LR_LOADTRANSPARENT);
	m_ImageLogo.SetBitmap(hbm);
*/
	pFont = m_Link.GetFont();
	pFont->GetObject(sizeof(lf),&lf);
	lf.lfUnderline = TRUE;
	hFont = CreateFontIndirect(&lf);
	pModFont = pFont->FromHandle(hFont);
	m_Link.SetFont(pModFont);
	return TRUE;  
}


void CAboutDlg::OnStaticUrl() 
{
	
	// Executes the URL in the default web browser

	ShellExecute(NULL,_T("open"),"http://www.acm.uiuc.edu/sigchi", NULL,NULL,SW_SHOWNORMAL);
}


