// Octave.cpp: implementation of the COctave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Octave.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COctave::COctave():CObj(0,0,146,70) 
{
}

// Initializes the octave with the dialog DC in a given px py coordinates

COctave::Initialize(CDC *pDC,int px,int py)
{
	CBrush bkbrush;
	m_x = px;
	m_y = py;
	m_dc = pDC;
	m_memDC.CreateCompatibleDC(pDC);	// Creates the memory DC
	m_bmp.CreateCompatibleBitmap(pDC,150,80);
	m_memDC.SelectObject(&m_bmp);
	
	// Paints the memory DC with the dialog color brush

	bkbrush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	m_memDC.SelectObject(bkbrush);
	m_memDC.PatBlt(0,0,150,80,PATCOPY);
	
	
	// Initializes the keys of the octave in a given x,y inside 
	// the memory DC

	m_key[0].Initialize(&m_memDC,0,0,'A');
	m_key[2].Initialize(&m_memDC,21,0,'B');
	m_key[4].Initialize(&m_memDC,42,0,'C');
	m_key[1].Initialize(&m_memDC,12,0,'D');
	m_key[3].Initialize(&m_memDC,35,0,'D');
	m_key[5].Initialize(&m_memDC,63,0,'A');
	m_key[6].Initialize(&m_memDC,75,0,'D');
	m_key[7].Initialize(&m_memDC,84,0,'B');
	m_key[8].Initialize(&m_memDC,97,0,'D');
	m_key[9].Initialize(&m_memDC,105,0,'B');
	m_key[10].Initialize(&m_memDC,119,0,'D');
	m_key[11].Initialize(&m_memDC,126,0,'C');

}

// Change the note value of the octave keys from a range

COctave::SetRange(unsigned char LoRange)
{
	for (int i = 0;i <= 11;i++)
		m_key[i].SetNote(LoRange + i);
	
}

// Member function called when all keys must be drawn as normal state

COctave::Draw()
{
	for(int i = 0;i<=11;i++)
		m_key[i].DrawasNormal();
}

// Blit the entire octave

COctave::Blit()
{
	m_dc->BitBlt(m_x,m_y,m_width,m_height,&m_memDC,0,0,SRCCOPY);
}

// Blit a specific key of the octave

COctave::BlitKey(int numKey)
{
	m_key[numKey].Blit(m_x,m_y,m_dc);
}

// Returns information of the key whose octave has been pressed

COctave::note_info COctave::IsKeyPressed(int x,int y)
{
	int i = 0;
	char value = -1;
	note_info NoteInfo;
	
	while ((i<12) && ((value = m_key[i].IsPressed(x-m_x,y-m_y))<0)) i++;
	NoteInfo.note = value;
	NoteInfo.key = i;
	return NoteInfo;
}

// This member function is called when a key of the octave
// has to be drawn as pressed

COctave::DrawOnKey(int numKey)
{
	m_key[numKey].DrawOnClick();
	BlitKey(numKey);
}

// This member function is called when a key of the octave
// has to be drawn as normal (released)

COctave::ReleaseKey(int numKey)
{
	m_key[numKey].DrawasNormal();
	BlitKey(numKey);
}

COctave::~COctave()
{
}

