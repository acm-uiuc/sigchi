// Octave.h: interface for the COctave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTAVE_H__292BCB61_52E6_11D6_AA7A_00105A212DC5__INCLUDED_)
#define AFX_OCTAVE_H__292BCB61_52E6_11D6_AA7A_00105A212DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "Key.h"


class COctave:public CObj  
{
protected:
	CDC *m_dc;	// Pointer to the dialog DC
	CKey m_key[12];	// Array of 12 key objects of a octave
	CDC m_memDC;	// Memory DC	
	CBitmap m_bmp;	
	BlitKey(int numKey);
public:
	struct note_info	// note info structure to store info about keys
	{
		char note;
		int key;
		int octave;
	};
	
	COctave();
	virtual ~COctave();
	Initialize(CDC *pDC,int px,int py);
	SetRange(unsigned char LoRange);
	note_info IsKeyPressed(int x,int y);
	Draw();
	DrawOnKey(int numKey);
	Blit();
	ReleaseKey(int numKey);
	unsigned char GetKeyNote(int numKey) { return m_key[numKey].m_note; } 
};

#endif // !defined(AFX_OCTAVE_H__292BCB61_52E6_11D6_AA7A_00105A212DC5__INCLUDED_)
