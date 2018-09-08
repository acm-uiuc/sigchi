// Key.cpp: implementation of the CKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Key.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKey::CKey()
{

}

// Initilializes the key object given the memory DC

CKey::Initialize(CDC *dc,int px,int py,char type)
{
	m_dc = dc;
	m_x = px;
	m_y= py;
	m_type = type;	
	switch(m_type)		// Creates the key depending on the type
	{
	case 'A':			
		m_CRect1.SetRect(0+m_x,0+m_y,10+m_x,40+m_y);
		m_CRect2.SetRect(0+m_x,40+m_y,20+m_x,70+m_y);
		break;
	case 'B':
		m_CRect1.SetRect(8+m_x,0+m_y,12+m_x,40+m_y);
		m_CRect2.SetRect(0+m_x,40+m_y,20+m_x,70+m_y);
		break;
	case 'C':
		m_CRect1.SetRect(10+m_x,0+m_y,20+m_x,70+m_y);
		m_CRect2.SetRect(0+m_x,40+m_y,20+m_x,70+m_y);
		break;
	case 'D':
		m_CRect1.SetRect(0+m_x,0+m_y,15+m_x,38+m_y);
		break;
	default:
		break;
	}
}

// Dinamically assigns a note value to a given key

CKey::SetNote(unsigned char nNote)
{
	m_note = nNote;
}

// Draws the key in normal state on the respective memory DC 

void CKey::DrawasNormal()
{
	CBrush fillbrush,*oldbrush;
	(m_type != 'D')?fillbrush.CreateStockObject(WHITE_BRUSH) 
		: fillbrush.CreateStockObject(BLACK_BRUSH); 
	oldbrush = m_dc->SelectObject(&fillbrush);
	m_dc->FillRect(&m_CRect1,&fillbrush);
	if (m_type != 'D') m_dc->FillRect(&m_CRect2,&fillbrush);
	m_dc->SelectObject(oldbrush);
}

// Detects when a given coordinates are in the key range
// if it is in the key range inmediately returns the note value

char CKey::IsPressed(int x,int y)
{
	switch(m_type)
	{
	case 'D':
		if (m_CRect1.PtInRect(CPoint(x,y)))  return m_note;
			else return -1;
		break;
	default:
		if (m_CRect1.PtInRect(CPoint(x,y)) || m_CRect2.PtInRect(CPoint(x,y))) return m_note; 
			else return -1;
		break;
	}
}

// Draws the key when it is pressed on the corresponding memory DC

void CKey::DrawOnClick()
{
	CBrush fillbrush,*oldbrush;
	(m_type != 'D')?fillbrush.CreateSolidBrush(RGB(154,169,239)) 
		: fillbrush.CreateSolidBrush(RGB(52,78,199)); 
	oldbrush = m_dc->SelectObject(&fillbrush);
	m_dc->FillRect(&m_CRect1,&fillbrush);
	if (m_type != 'D') m_dc->FillRect(&m_CRect2,&fillbrush);
	m_dc->SelectObject(oldbrush);
	fillbrush.DeleteObject();
}


// Blits the part of the key on the memory DC to the application DC

void CKey::Blit(int x,int y,CDC *pDC)
{
	CPoint PointTL = m_CRect1.TopLeft();
	CPoint PointTL2 = m_CRect2.TopLeft();
	if (m_type!='D')
	{
		pDC->BitBlt(x+PointTL.x,y+PointTL.y,m_CRect1.Width(),m_CRect1.Height(),m_dc,PointTL.x,PointTL.y,SRCCOPY);
		pDC->BitBlt(x+PointTL2.x,y+PointTL2.y,m_CRect2.Width(),m_CRect2.Height(),m_dc,PointTL2.x,PointTL2.y,SRCCOPY);
	}
	else 
		pDC->BitBlt(x+PointTL.x,y+PointTL.y,m_CRect1.Width(),m_CRect1.Height(),m_dc,PointTL.x,PointTL.y,SRCCOPY);

}

CKey::~CKey()
{

}
