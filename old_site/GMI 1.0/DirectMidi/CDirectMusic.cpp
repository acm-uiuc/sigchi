/*Module : CDirectMusic.cpp
Purpose: Defines the code implementation for CDirectMusic class
Created: CJP / 08-02-2002
History: CJP / 20-09-2002 
	
	1. Improved class destructors

	2. Check member variables for NULL values before performing an operation
		
	3. Restructured the class system
	
	4. Better method to enumerate and select MIDI ports
	
	5. Adapted the external thread to a pure virtual function

	6. SysEx reception and sending enabled

	7. Added flexible conversion functions

	8. Find out how to activate the Microsoft software synth. (Thanks to DirectSound)

	9. Added DLS support

Copyright (c) 2002 by C. Jiménez de Parga  
All rights reserved.
*/

#include "stdafx.h"
#include "CDirectMidi.h"

// The constructor of the class, member variables initialization

CDirectMusic::CDirectMusic()
{
    CoInitialize(NULL);	// Initialize COM
	m_pMusic8			= NULL; 

}


// Intialization of CDirectMusic object.
// Create the instance of the DirectMusic object

HRESULT CDirectMusic::Initialize(HWND hWnd)
{
	HRESULT hr;
	
	if (FAILED(hr = CoCreateInstance(CLSID_DirectMusic,NULL,
		CLSCTX_INPROC,IID_IDirectMusic8,(void**)&m_pMusic8))) return hr;
	
	if (FAILED(hr = m_pMusic8->SetDirectSound(NULL,hWnd))) return hr;

	return S_OK;
}
      
// The class destructor

CDirectMusic::~CDirectMusic()
{
	SAFE_RELEASE(m_pMusic8);	
	CoUninitialize();	// Uninitialize COM
}	

