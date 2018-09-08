// Keyb.h : main header file for the KEYB application
//

#if !defined(AFX_KEYB_H__3BE1DA05_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_)
#define AFX_KEYB_H__3BE1DA05_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKeybApp:
// See Keyb.cpp for the implementation of this class
//

class CKeybApp : public CWinApp
{
public:
	CKeybApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeybApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKeybApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYB_H__3BE1DA05_610B_11D6_B3BC_EC5E2C31CF36__INCLUDED_)
