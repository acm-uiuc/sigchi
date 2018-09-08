/*Module : CDLSLoader.cpp
Purpose: Defines the code implementation for CDLSLoader class
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


// The constructor of the class

CDLSLoader::CDLSLoader()
{
	m_pLoader		= NULL;
	
}

// Initialize the object, create the instance of DirectMusicLoader object

HRESULT CDLSLoader::Initialize()
{
	HRESULT hr;
	
	if (FAILED(hr = CoCreateInstance(CLSID_DirectMusicLoader,NULL,
	   CLSCTX_INPROC_SERVER,IID_IDirectMusicLoader8,
	   (LPVOID*)&m_pLoader))) return hr;

	return hr;
}


// Loads a DLS file and initializes a Collection object

HRESULT CDLSLoader::LoadDLS(LPSTR lpFileName,CCollection &pCollection)
{
	HRESULT hr = DM_FAILED;
	DMUS_OBJECTDESC dmusdesc;
	

	// Sets to 0 the DMUS structure

	ZeroMemory(&dmusdesc,sizeof(DMUS_OBJECTDESC));
	dmusdesc.dwSize = sizeof(DMUS_OBJECTDESC);
	dmusdesc.guidClass = CLSID_DirectMusicCollection; 

	if (lpFileName == NULL)	// Load the synthesizer default GM set
	{
		dmusdesc.guidObject	= GUID_DefaultGMCollection;
		dmusdesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT;
	}
	else
	{
		mbstowcs(dmusdesc.wszFileName,lpFileName,_MAX_PATH); // Convert to wide char
		dmusdesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH;
	}

	UnLoadCollection(pCollection);	// In case the object is already with data

	if (m_pLoader)	// Load the DLS file
		hr = m_pLoader->GetObject(&dmusdesc,IID_IDirectMusicCollection,(void**)&pCollection.m_pCollection);
	
	return hr;

}

// Unload a collection from the loader

HRESULT CDLSLoader::UnLoadCollection(CCollection &pCollection)
{
	HRESULT hr = DM_FAILED;

	if ((m_pLoader) && (pCollection.m_pCollection))
	{
		hr = m_pLoader->ReleaseObjectByUnknown(pCollection.m_pCollection);
		SAFE_RELEASE(pCollection.m_pCollection);
	}
	
	return hr;
}


// Class destructor 
// Clear the references to a collection in the loader

CDLSLoader::~CDLSLoader()
{
	if (m_pLoader)
	{	
		m_pLoader->ClearCache(CLSID_DirectMusicCollection);
		// Remove from the cache objects that are no longer in use
		m_pLoader->CollectGarbage();
	}

	SAFE_RELEASE(m_pLoader);
}	
