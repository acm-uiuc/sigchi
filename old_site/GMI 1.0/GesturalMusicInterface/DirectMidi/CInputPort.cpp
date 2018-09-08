/*
Module : CInputPort.cpp
Purpose: Defines the code implementation for the CInputport class
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

CInputPort::CInputPort()
{
	m_pThru			= NULL;
	m_hEvent		= NULL;
	m_pPort			= NULL;
	m_pBuffer		= NULL;
	m_pMusic8		= NULL;
	m_ProcessActive = FALSE;
}	


// Initialize object

HRESULT CInputPort::Initialize(CDirectMusic &pDMusic)
{
	if (pDMusic.m_pMusic8 == NULL) return DM_FAILED;
	m_pMusic8 = pDMusic.m_pMusic8;	
	return S_OK;
}
	
// Function to return the number of input midi ports

DWORD CInputPort::GetNumPorts()
{
	DMUS_PORTCAPS portinf;
	DWORD dwIndex = 0, dwNum = 0;

	if (!m_pMusic8) return DM_FAILED;
	
	// Set to 0 the DMUS_PORTCAPS structure  
	
	ZeroMemory(&portinf,sizeof(portinf));
	portinf.dwSize = sizeof(DMUS_PORTCAPS);
     
	// Call the DirectMusic8 member function to enumerate systems ports
	
	while (m_pMusic8->EnumPort(dwIndex++,&portinf)==S_OK)
		if (portinf.dwClass == DMUS_PC_INPUTCLASS) dwNum++;
	
	return dwNum;
}
	
// Gets the port info given the number of the port, returns a pointer to an INFOPORT structure

HRESULT CInputPort::GetPortInfo(DWORD dwNumPort,LPINFOPORT lpInfoPort)
{
	HRESULT hr;
	DMUS_PORTCAPS portinf;
	DWORD dwIndex = 0, dwNum = 0;

	if (!m_pMusic8) return DM_FAILED;
	
	// Set to 0 the DMUS_PORTCAPS structure  
	
	ZeroMemory(&portinf,sizeof(portinf));
	portinf.dwSize = sizeof(DMUS_PORTCAPS);
    
	while ((hr = m_pMusic8->EnumPort(dwIndex++,&portinf)) == S_OK) 
	{	
		if (portinf.dwClass == DMUS_PC_INPUTCLASS) dwNum++;
		if (dwNum == dwNumPort) break;
	}
											 

	if (SUCCEEDED(hr))
	{
		// Converts port description to char string 
	
		WideCharToMultiByte(CP_ACP,0,portinf.wszDescription,-1,
                        lpInfoPort->szPortDescription,
                        sizeof(lpInfoPort->szPortDescription)/
						sizeof(lpInfoPort->szPortDescription[0]),0,0);


		// Copy the GUID of DMUS_PORTCAP structure to the INFOPORT structure
   
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
	


// Function to select and activate an Input MIDI port given a LPINFOPORT structure

HRESULT CInputPort::ActivatePort(LPINFOPORT InfoPort,DWORD dwSysExSize)
{
	HRESULT hr;
	DMUS_PORTPARAMS8 dmus;	
	
	// Checks member variables initialization

	if (!(m_pMusic8 || m_pPort)) return DM_FAILED;
	
	// Sets to 0 the port capabilities structure
	
	ZeroMemory(&dmus,sizeof(DMUS_PORTPARAMS));
	
	// Sets the params for this port (16 channels group)
	
	dmus.dwSize = sizeof(DMUS_PORTPARAMS);
	dmus.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
	dmus.dwChannelGroups = 1; 
	
	// The midi port is created here 
	
	if (FAILED(hr = m_pMusic8->CreatePort(InfoPort->guidSynthGUID,&dmus, 
        &m_pPort,NULL))) return hr;
	
	// We have to activate it
	
	if (FAILED(hr = m_pPort->Activate(TRUE))) return hr;
	

	DMUS_BUFFERDESC BufferDesc;					// Create the DirectMusic buffer to store MIDI messages
	ZeroMemory(&BufferDesc,sizeof(DMUS_BUFFERDESC));
	BufferDesc.dwSize = sizeof(DMUS_BUFFERDESC);
	BufferDesc.guidBufferFormat = GUID_NULL;
	BufferDesc.cbBuffer = DMUS_EVENT_SIZE(dwSysExSize);		// at least 32 bytes to store messages
	if (FAILED(hr = m_pMusic8->CreateMusicBuffer(&BufferDesc,&m_pBuffer,NULL))) return hr;
   	
	// Gets the thru interface

	if (FAILED(hr = m_pPort->QueryInterface(IID_IDirectMusicThru8,(void**)&m_pThru))) return hr; 
	
	return S_OK;
}



// This function removes the selected port 
// and releases all their asociated interfaces for a new port selection

HRESULT CInputPort::ReleasePort()
{
	HRESULT hr = DM_FAILED;
	
	if (m_pPort)
		hr = m_pPort->Activate(FALSE);
	
		
	SAFE_RELEASE(m_pPort);
	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pThru);
	
	return hr;

}


// Main thread for processing incoming MIDI events

DWORD CInputPort::WaitForEvent(LPVOID lpv)
{
    HRESULT hr;
    REFERENCE_TIME rt;	// System time when the event is received
    DWORD dwGroup;		// Channel group
    DWORD dwcb,dwMsg;	// Byte counter, Midi message	
    BYTE *pb;			// Pointer to the data

	CInputPort *ptrPort=(CInputPort*)lpv;
	
	while(ptrPort->m_ProcessActive)	// Check for active
	{
		WaitForMultipleObjects(1,&ptrPort->m_hEvent, FALSE, INFINITE); // Waits for an event
		while (ptrPort->m_ProcessActive)
		{
			hr = ptrPort->ReadBuffer();	// Read incoming midi messages
			if (hr == S_FALSE)
			{
				break;  // No more messages to read into the buffer
			}
			
			ptrPort->ResetBuffer();// Set the buffer pointer to the start
					
			while (ptrPort->m_ProcessActive)	
			{
				hr = ptrPort->GetMidiEvent(&rt,&dwGroup,&dwcb,&pb); // Extract midi data
				if (hr == S_OK)
				{
					if (dwcb>MIDI_STRUCTURED)	// In case it is SysEx data
						ptrPort->RecvMidiMsg(rt,dwGroup,dwcb,pb);
					else
					{
						CopyMemory(&dwMsg,pb,dwcb); // Structured data (Standard format)	
						ptrPort->RecvMidiMsg(rt,dwGroup,dwMsg);
					}
					// pb points to the data structure for the message, and
				}	// you can do anything that you want with it
				else if (hr == S_FALSE)
				{
					break;  // No more messages in the buffer
				}
			}  // Done with the buffer
				
		}  // Done reading pending events
	}
		
	return 0;
}


// Activate the notification handler
// Begins the worker thread execution
 
HRESULT CInputPort::ActivateNotification()
{
	HRESULT hr = DM_FAILED;
	DWORD	dwThId;
	
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	if (m_hEvent)
	{
	
		if (m_pPort)	// Activate the event notification
		{
			if (FAILED(hr = m_pPort->SetReadNotificationHandle(m_hEvent))) return hr;
			m_ProcessActive = TRUE;
			// Create the main thread	
			if (CreateThread(NULL,0,WaitForEvent,this,0,&dwThId) == NULL) return DM_FAILED;
		}
	}
	
	return hr;

}

// Activates the MIDI thru from an Input MIDI port to a given Output MIDI port

HRESULT CInputPort::SetThru(DWORD dwSourceChannel,DWORD dwDestinationChannel,COutputPort &pdstMidiPort)
{
	HRESULT hr = DM_FAILED;
	
	if (m_pThru)
		hr = m_pThru->ThruChannel(1,dwSourceChannel,1,dwDestinationChannel,pdstMidiPort.m_pPort);
			
	return hr;
}    

// Desactivates the MIDI thru 

HRESULT CInputPort::BreakThru(DWORD dwSourceChannel,DWORD dwDestinationChannel)
{
	HRESULT hr = DM_FAILED;
	
	if (m_pThru)
	{
		hr = m_pThru->ThruChannel(1,dwSourceChannel,1,dwDestinationChannel,NULL);
			
	}
	
	return hr;
}    


// Ends the notification and terminate the thread

HRESULT CInputPort::TerminateNotification()
{
	HRESULT hr = DM_FAILED;
	if (m_pPort)
	{
		hr = m_pPort->SetReadNotificationHandle(NULL); // Ends the read notification
		m_ProcessActive = FALSE;					   // Tells the thread to exit	
		SetEvent(m_hEvent);	 
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	return hr;
}


// Function to fill a buffer with incoming MIDI data from the port

HRESULT CInputPort::ReadBuffer()
{
	if (m_pPort)
		return m_pPort->Read(m_pBuffer);
	else return S_FALSE;
}


// Sets the read pointer to the start of the data in the buffer

HRESULT CInputPort::ResetBuffer()
{	
	if (m_pBuffer)
		return m_pBuffer->ResetReadPtr();
	else return S_FALSE;
}

// Returns information about the next message in the buffer and advances the read pointer

HRESULT CInputPort::GetMidiEvent(REFERENCE_TIME *lprt,DWORD *lpdwGroup,DWORD *lpcb,BYTE **lppb)
{
	if (m_pBuffer)
		return m_pBuffer->GetNextEvent(lprt,lpdwGroup,lpcb,lppb);
	else return S_FALSE;
}


// Extract from a DWORD a MIDI 1.0 message

void CInputPort::DecodeMidiMsg(DWORD dwMsg,BYTE *Status,BYTE *DataByte1,BYTE *DataByte2)
{
	*Status = (BYTE)dwMsg;
    *DataByte1 = (BYTE)(dwMsg >> SHIFT_8);
    *DataByte2 = (BYTE)(dwMsg >> SHIFT_16);
}

// Extract from a DWORD a MIDI 1.0 message separating Command and Channel

void CInputPort::DecodeMidiMsg(DWORD dwMsg,BYTE *Command,BYTE *Channel,BYTE *DataByte1,BYTE *DataByte2)
{
    *Command = (BYTE)(dwMsg & ~BITMASK);
    *Channel = (BYTE)(dwMsg & BITMASK);
    *DataByte1 = (BYTE)(dwMsg >> SHIFT_8);
    *DataByte2 = (BYTE)(dwMsg >> SHIFT_16);
}

// Class destructor
// Release all the interfaces
// Call TerminateNotification in case the thread is active 

CInputPort::~CInputPort()
{
	if (m_hEvent) TerminateNotification();
	ReleasePort();
}

