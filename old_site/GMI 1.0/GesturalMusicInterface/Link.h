#if !defined(AFX_LINK_H__B49752A1_94D1_11D6_B3BC_00105A212DC5__INCLUDED_)
#define AFX_LINK_H__B49752A1_94D1_11D6_B3BC_00105A212DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Link.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLink window

class CLink : public CStatic
{
// Construction
public:
	CLink();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLink)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLink();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLink)
	HBRUSH CLink::CtlColor(CDC* pDC, UINT nCtlColor); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINK_H__B49752A1_94D1_11D6_B3BC_00105A212DC5__INCLUDED_)
