// Object.h: interface for the CObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT_H__9605DF0F_4F30_11D6_AA7A_00105A212DC5__INCLUDED_)
#define AFX_OBJECT_H__9605DF0F_4F30_11D6_AA7A_00105A212DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CObj  
{
public:
	int m_x,m_y,m_width,m_height;
	CObj(int x = 0,int y = 0,int width = 0,int height = 0);
	virtual ~CObj();

};

#endif // !defined(AFX_OBJECT_H__9605DF0F_4F30_11D6_AA7A_00105A212DC5__INCLUDED_)
