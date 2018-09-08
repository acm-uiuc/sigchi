/*
Module : COutputPort.cpp
Purpose: Defines the code implementation for the COutput port class
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


// Class constructor, member variables initialization

COutputPort::COutputPort()
{
	m_pPort			= NULL;
	m_pBuffer		= NULL;
	m_pClock		= NULL;
	m_pMusic8		= NULL;
}	


// Initialize object

HRESULT COutputPort::Initialize(CDirectMusic &pDMusic)
{
	if (pDMusic.m_pMusic8 == NULL) return DM_FAILED;
	m_pMusic8 = pDMusic.m_pMusic8;	
	return S_OK;
}


// Returns the number of MIDI ports

DWORD COutputPort::GetNumPorts()
{
	DMUS_PORTCAPS portinf;
	DWORD dwIndex = 0, dwNum = 0;

	// Checks member variables initialization

	if (!m_pMusic8) return DM_FAILED;
		
	// Set to 0 the DMUS_PORTCAPS structure  
	
	ZeroMemory(&portinf,sizeof(portinf));
	portinf.dwSize = sizeof(DMUS_PORTCAPS);
     
	// Call the DirectMusic8 member function to enumerate systems ports
	
	while (m_pMusic8->EnumPort(dwIndex++,&portinf)==S_OK)
		if (portinf.dwClass == DMUS_PC_OUTPUTCLASS) dwNum++;
	
	return dwNum;
}
	

// Gets the port info given the number of the port, returns a pointer to an INFOPORT structure 

HRESULT COutputPort::GetPortInfo(DWORD dwNumPort,LPINFOPORT lpInfoPort)
{
	HRESULT hr;
	DMUS_PORTCAPS portinf;
	DWORD dwIndex = 0, dwNum = 0;

	
	// Checks member variables initialization

	if (!m_pMusic8)  return DM_FAILED;
		
	// Set to 0 the DMUS_PORTCAPS structure  
	
	ZeroMemory(&portinf,sizeof(portinf));
	portinf.dwSize = sizeof(DMUS_PORTCAPS);
    
	while ((hr = m_pMusic8->EnumPort(dwIndex++,&portinf))==S_OK) 
	{	
		if (portinf.dwClass == DMUS_PC_OUTPUTCLASS) dwNum++;
		if (dwNum == dwNumPort) break;
	}
											 

	if (SUCCEEDED(hr))
	{
		// Converts port description to char string 
	
		WideCharToMultiByte(CP_ACP,0,portinf.wszDescription,-1,
                        lpInfoPort->szPortDescription,
                        sizeof(lpInfoPort->szPortDescription)/
						sizeof(lpInfoPort->szPortDescription[0]),0,0);


		// Copy the GUID of DMUS_PORTCAP structure to an INFOPORT structure
   
		CopyMemory(&(lpInfoPort->guidSynthGUID),&portinf.guidPort,sizeof(GUID));
   
		lpInfoPort->dwClass					= portinf.dwClass;
		lpInfoPort->dwEffectFlags			= portinf.dwEffectFlags;
		lpInfoPort->dwFlags	 				= portinf.dwFlags;
		lpInfoPort->dwMemorySize			= portinf.dwMemorySize;
		lpInfoPort->dwMaxAudioChannels	    = portinf.dwMaxAudioChannels;
		lpInfoPort->dwMaxChannelGroups		= portinf.dwMaxChannelGroups;
		lpInfoPort->dwMaxVoices				= portinf.dwMaxVoices;
		lpInfoPort->dwType					= portinf.dwType;
	}
	
	return hr;
}
	


// Function to select and activate an Input MIDI port given an INFOPORT structure

HRESULT COutputPort::ActivatePort(LPINFOPORT InfoPort,DWORD dwSysExSize)
{
	HRESULT hr;
	DMUS_PORTPARAMS dmus;	
	
	
	// Checks member variables initialization

	if (!(m_pMusic8 || m_pPort)) return DM_FAILED;
	
	// Sets to 0 the port capabilities structure
	
	ZeroMemory(&dmus,sizeof(DMUS_PORTPARAMS8));
	
	// Sets the params for this port
	
	dmus.dwSize = sizeof(DMUS_PORTPARAMS);
	dmus.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
	dmus.dwChannelGroups = 1;
	
	
	// The midi port is created here 
	
	if (FAILED(hr = m_pMusic8->CreatePort(InfoPort->guidSynthGUID ,&dmus, 
        &m_pPort,NULL))) return hr;
	
	// We have to activate it
	
	if (FAILED(hr = m_pPort->Activate(TRUE))) return hr;
	
	if (FAILED(hr = m_pPort->GetLatencyClock(&m_pClock))) return hr;
	
	
	DMUS_BUFFERDESC BufferDesc;					// Create the DirectMusic buffer to store MIDI messages
	ZeroMemory(&BufferDesc,sizeof(DMUS_BUFFERDESC));
	BufferDesc.dwSize = sizeof(DMUS_BUFFERDESC);
	BufferDesc.guidBufferFormat = GUID_NULL;
	BufferDesc.cbBuffer = DMUS_EVENT_SIZE(dwSysExSize);	// at least 32 bytes to store messages
	if (FAILED(hr = m_pMusic8->CreateMusicBuffer(&BufferDesc,&m_pBuffer,NULL))) return hr;
   	
	// Instructs the port to compact DLS or wave-table memory 
	m_pPort->Compact();

	return S_OK;
}



// This function removes the selected port 
// and releases all their asociated interfaces for a new port selection

HRESULT COutputPort::ReleasePort()
{
	HRESULT hr = DM_FAILED;
	
	if (m_pPort)
		hr = m_pPort->Activate(FALSE);
	
	SAFE_RELEASE(m_pPort);
	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pClock);
	
	return hr;
	
}


// Download an instrument to the port wave-table memory

HRESULT COutputPort::DownLoadInstrument(CInstrument &pInstrument)
{
	HRESULT hr = DM_FAILED;

	if ((m_pPort) && (pInstrument.m_pInstrument))
	{	
		
		UnLoadInstrument(pInstrument);	// Unload the instrument object in case is downloaded
		
		// Download the instrument to the port and keep a reference to
		// this instrument in the port with the m_pDownloadedInstrument

		if (FAILED(hr = m_pPort->DownloadInstrument(pInstrument.m_pInstrument,
			&pInstrument.m_pDownLoadedInstrument,NULL,0))) return hr;
	}
	return hr;

}


// Unload an instrument from the port

HRESULT COutputPort::UnLoadInstrument(CInstrument &pInstrument)
{
	HRESULT hr = DM_FAILED;

	if ((m_pPort) && (pInstrument.m_pDownLoadedInstrument))
	{
		if (FAILED(hr = m_pPort->UnloadInstrument(pInstrument.m_pDownLoadedInstrument))) return hr;
		SAFE_RELEASE(pInstrument.m_pDownLoadedInstrument);
	}
	
	return hr;
}	


// Sends an unstructured MIDI message

HRESULT COutputPort::SendMidiMsg(LPBYTE lpMsg,DWORD dwLength,DWORD dwChannel)
{
	HRESULT hr = DM_FAILED;
	REFERENCE_TIME rt;
	if ((m_pPort) && (m_pBuffer) && (m_pClock))
	{
		if (FAILED(hr = m_pClock->GetTime(&rt))) return hr;	// Gets the exact time to play it
		if (FAILED(hr = m_pBuffer->PackUnstructured(rt,dwChannel,dwLength,lpMsg))) return hr; 
		if (FAILED(hr = m_pPort->PlayBuffer(m_pBuffer))) return hr; // Sends the data
		if (FAILED(hr = m_pBuffer->Flush())) return hr;	// Discards all data in the buffer
	}

	return hr;
}	

// Function to send a MIDI normal message to the selected output port

HRESULT COutputPort::SendMidiMsg(DWORD dwMsg)
{
	HRESULT hr = DM_FAILED;
	REFERENCE_TIME rt;
	if ((m_pPort) && (m_pBuffer) && (m_pClock))
	{
		if (FAILED(hr = m_pClock->GetTime(&rt))) return hr; // Gets the exact time to play it
		if (FAILED(hr = m_pBuffer->PackStructured(rt,dwMsg & 0x0f,dwMsg))) return hr; 
		if (FAILED(hr = m_pPort->PlayBuffer(m_pBuffer))) return hr; // Sends the data
		if (FAILED(hr = m_pBuffer->Flush())) return hr; // Discards all data in the buffer
	}

	return hr;
	
}


// Function to activate an effect in the port

HRESULT COutputPort::SetEffect(BOOL bActivate,BYTE Effect)
{
	IKsControl   *pControl;
    KSPROPERTY   ksp;
    DWORD        dwEffects=0;
    ULONG        cb;
	HRESULT      hr=DM_FAILED;

    // Get the IKsControl interface
	if (m_pPort)
	{
		hr = m_pPort->QueryInterface(IID_IKsControl,(void**)&pControl);

		if (SUCCEEDED(hr))
		{
			ZeroMemory(&ksp, sizeof(ksp));

        
			if (!bActivate)
				dwEffects = 0;
			else
			{
				// Select an audio effect
				switch(Effect)
				{
					case 0x1:
						dwEffects = DMUS_EFFECT_REVERB;
						break;
					case 0x2:
						dwEffects = DMUS_EFFECT_CHORUS;
						break;
					case 0x3:
						dwEffects = DMUS_EFFECT_CHORUS | DMUS_EFFECT_REVERB;
						break;
				}		
			}
		
		ksp.Set = GUID_DMUS_PROP_Effects;
		ksp.Id = 0;
		ksp.Flags = KSPROPERTY_TYPE_SET;
	
                            
		pControl->KsProperty(&ksp,
			                 sizeof(ksp),
                             (LPVOID)&dwEffects,
                             sizeof(dwEffects),
                             &cb);

		SAFE_RELEASE(pControl);
	
		}
	}
	return hr;

}

// Code a MIDI message given the three bytes of a MIDI message

DWORD COutputPort::EncodeMidiMsg(BYTE Status,BYTE DataByte1,BYTE DataByte2)
{
	DWORD dwMsg;
	dwMsg = Status;
    dwMsg |= DataByte1 << SHIFT_8;
    dwMsg |= DataByte2 << SHIFT_16;
	return dwMsg;
}

// Code a MIDI message given the Command and the Channel

DWORD COutputPort::EncodeMidiMsg(BYTE Command,BYTE Channel,BYTE DataByte1,BYTE DataByte2)
{
    DWORD dwMsg;
	dwMsg = Command | Channel;
    dwMsg |= DataByte1 << SHIFT_8;
    dwMsg |= DataByte2 << SHIFT_16;
	return dwMsg;
}


// Class destructor
// Release all the interfaces

COutputPort::~COutputPort()
{
	ReleasePort();
}

