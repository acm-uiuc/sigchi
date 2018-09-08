/*Module : CInstrument.cpp
Purpose: Defines the code implementation for CInstrument class
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

// Class constructor, initializes member variables

CInstrument::CInstrument()
{
	m_pDownLoadedInstrument = NULL;
	m_pInstrument			= NULL;
	m_dwPatchInCollection	= -1;
	m_dwPatchInMidi			= -1;
	m_strInstName[0]		= '\0';	
}


// Sets the destination of the instrument in a Synth. MIDI program 

HRESULT CInstrument::SetPatch(DWORD dwdstPatchMidi)
{
	HRESULT hr = DM_FAILED;
	
	if (m_pInstrument)
	{	
		if (FAILED(hr = m_pInstrument->SetPatch(dwdstPatchMidi))) return hr;
		m_dwPatchInMidi = dwdstPatchMidi;
	}
	
	return hr;
}	

// Class destructor

CInstrument::~CInstrument()
{
	// Release the reference to the interface that contains information about
	// the downloaded instrument
	SAFE_RELEASE(m_pDownLoadedInstrument); 
	SAFE_RELEASE(m_pInstrument);
}