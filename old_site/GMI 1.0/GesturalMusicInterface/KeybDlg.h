/*
MidiStation -> Gestural Music Interface

Module : KeybDlg.cpp
Purpose: Implementation for the CKeybDlg class
Created: CJP / 10-4-2002
History: CJP / 4-10-2002 

  
P5 Integration, Copyright (c) 2003 by SIG CHI of ACM@UIUC	
	
Copyright (c) 2002 by C. Jiménez de Parga  
All rights reserved.
*/

// Some required headers

#include <afxmt.h>
#include ".\\DirectMidi\\CDirectMidi.h" // The DirectMidi wrapper library
#include "Link.h"	// Class for URL hyperlink

#if !defined(AFX_KEYBDLG_H__3BE1DA07_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_)
#define AFX_KEYBDLG_H__3BE1DA07_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LIST_ITEMS 1000	// Maximum list items 
#define MAX_OCTAVES 4 
#define MAX_KEYS 24

const BYTE ALLNOTES_OFF = 0xB0; // Turn off all current playing notes

enum KeybState { RECORDING, STOPPED , PLAYING }; // Keyboard state

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD  =  IDD_ABOUTBOX };
	CStatic	m_ImageLogo;
	CLink	m_Link;
	CStatic	m_Image;
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStaticUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CKeybDlg dialog

class CKeybDlg : public CDialog
{
// Construction
public:
	CKeybDlg(CWnd* pParent = NULL);	// standard constructor
	void EnumPorts();
	void InitializeDirectMusic();
	void InitializeKeyboard();
	void InitializeInstruments();
	void InitializeMessageList();
	void InitializeControls();
	void InitializeEventHandler();
	
	void StoresInformation(LVITEM &lvi,int nSubItem,CString strCad);
	void CloseDown();

	void RecvNote(unsigned char note);
	void ReleaseNote(unsigned char note);
	void SendMidiMsg(unsigned char note,BOOL bOn);
	void StopRecord();
	void ReleaseAllKeys();
	void PrepareSoftwareSynth();
	void KeyMessage(int nCode,BOOL NoteType,int nOctave);
	void OnKeyRecv(char KeyCode,BOOL NoteType);

	BOOL AddListCtrl(unsigned char note,unsigned char velocity);
	BOOL InitializeLibrary();

	CString GetEngValue(unsigned char note);
	CString GetEspValue(unsigned char note);
	
	BOOL m_bAbandon,m_bExitPlayBack,m_bSwSynth;
	DWORD nInPortSel,nOutPortSel;
	UCHAR nLoNote,nHiNote;
	HANDLE m_hEvent;
	// Dialog Data
	//{{AFX_DATA(CKeybDlg)
	enum { IDD = IDD_KEYB_DIALOG };
//	CStatic			m_Image;
	CComboBox		m_NoteRange;
	CProgressCtrl	m_MemProgress;
	CButton			m_Stop;
	CButton			m_Rec;
	CButton			m_Play;
	CComboBox		m_OutPortList;
	CComboBox		m_InPortList;
	CListCtrl		m_MessagesList;
	CComboBox		m_InstList;

	CStatic	m_ThumbEdge;
	CStatic	m_RingEdge;
	CStatic	m_PinkyEdge;
	CStatic	m_MiddleEdge;
	CStatic	m_IndexEdge;
	CSliderCtrl	m_ThumbSlider;
	CSliderCtrl	m_RingSlider;
	CSliderCtrl	m_PinkySlider;
	CSliderCtrl	m_MiddleSlider;
	CSliderCtrl	m_IndexSlider;
	CButton	m_ThumbState;
	CButton	m_RingState;
	CButton	m_PinkyState;
	CButton	m_MiddleState;
	CButton	m_IndexState;
	CProgressCtrl	m_Thumb;
	CProgressCtrl	m_Ring;
	CProgressCtrl	m_Pinky;
	CProgressCtrl	m_Middle;
	CProgressCtrl	m_Index;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeybDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CKeybDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeInstruments();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRec();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnClose();
	afx_msg void OnClear();
	afx_msg void OnSelchangeOutputPorts();
	afx_msg void OnSelchangeInputPorts();
	afx_msg void OnSelchangeNoteRange();
	afx_msg void OnHelp();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnInitMenu(CMenu* pMenu);


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Derivated class from CInputport

class CDirectInputPort:public CInputPort
{
	CKeybDlg* m_pKeybDlg;
public:
	void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwBytesRead,BYTE *lpBuffer) {};
	void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwMsg);
	void SetDlgReference(CKeybDlg *pKeybDlg) { m_pKeybDlg = pKeybDlg; } // Pointer to the main dialog object
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBDLG_H__3BE1DA07_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_)
