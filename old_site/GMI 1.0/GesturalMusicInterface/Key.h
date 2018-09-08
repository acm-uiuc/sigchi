// Key.h: interface for the CKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEY_H__9605DF10_4F30_11D6_AA7A_00105A212DC5__INCLUDED_)
#define AFX_KEY_H__9605DF10_4F30_11D6_AA7A_00105A212DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

class CKey : public CObj 
{
protected:
	CDC *m_dc;				// Pointer to the memory DC
	unsigned char m_type;	// Type of key
	CRect m_CRect1,m_CRect2;	// CRect objects to define the geometry of the key	
public:
	unsigned char m_note;	// Value of the note
	
	CKey();
	virtual ~CKey();
	Initialize(CDC *dc,int px,int py,char type);
	SetNote(unsigned char nNote);
	char IsPressed(int x,int y);
	void DrawasNormal();
	void DrawOnClick();
	void Blit(int x,int y,CDC *pmemDC);
};

#endif // !defined(AFX_KEY_H__9605DF10_4F30_11D6_AA7A_00105A212DC5__INCLUDED_)
