// Object.cpp: implementation of the CObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Object.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor, initializes the dimensions of the physical object

CObj::CObj(int x,int y,int width,int height)
{
	m_x = x;				
	m_y = y;
	m_width = width;
	m_height = height;
}

CObj::~CObj()
{

}
