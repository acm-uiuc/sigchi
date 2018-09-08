/*Module : CCollection.cpp
Purpose: Defines the code implementation for CCollection class
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

// Class constructor

CCollection::CCollection()
{
	m_pCollection = NULL;
}
	
// Enumerate instruments stored in a collection given the index in the collection
// returns a pointer to an INSTRUMENTINFO

HRESULT CCollection::EnumInstrument(DWORD dwIndex,LPINSTRUMENTINFO InstInfo)
{
	HRESULT hr = DM_FAILED;
    WCHAR wszName[MAX_PATH];	
    DWORD dwPatch;

	if (m_pCollection)
		hr = m_pCollection->EnumInstrument(dwIndex,&dwPatch,wszName,MAX_PATH);
    
	if (hr == S_OK)
	{
		wcstombs(InstInfo->szInstName,wszName,MAX_PATH);	// Name of the instrument
		InstInfo->dwPatchInCollection = dwPatch; // Patch in the collection	
	}

	return hr;	
}


// Get the instrument given a pointer to an INSTRUMENTINFO structure

HRESULT CCollection::GetInstrument(CInstrument &pInstrument,LPINSTRUMENTINFO InstInfo)
{
	HRESULT hr = DM_FAILED;

	if (m_pCollection)
	{
		
		SAFE_RELEASE(pInstrument.m_pInstrument); // Release the old reference to the interface
		
		// Gets the instrument given the patch in the collection
		
		if (FAILED(hr = m_pCollection->GetInstrument(InstInfo->dwPatchInCollection,&pInstrument.m_pInstrument))) return hr;
		
		// Fills with information the class public member variables

		pInstrument.m_dwPatchInCollection = InstInfo->dwPatchInCollection;
		strcpy(pInstrument.m_strInstName,InstInfo->szInstName);
	}
	
	return hr;
}

// Get the instrument given the index in the collection

HRESULT CCollection::GetInstrument(CInstrument &pInstrument,INT nIndex)
{
	HRESULT hr = DM_FAILED;
	
	INSTRUMENTINFO InstInfo;

	if (m_pCollection)
	{
		
		SAFE_RELEASE(pInstrument.m_pInstrument); // Release the old reference to the interface
		
		// Gets the instrument associated with the index
		
		if (FAILED(hr = EnumInstrument(nIndex,&InstInfo))) return hr;
		
		// Gets the instrument given the patch in the collection
		
		if (FAILED(hr = m_pCollection->GetInstrument(InstInfo.dwPatchInCollection,&pInstrument.m_pInstrument))) return hr;
		
		// Fills with information the class public member variables

		pInstrument.m_dwPatchInCollection = InstInfo.dwPatchInCollection;
		strcpy(pInstrument.m_strInstName,InstInfo.szInstName);
	}
	
	return hr;
}

// The class destructor

CCollection::~CCollection()
{
	SAFE_RELEASE(m_pCollection);
}