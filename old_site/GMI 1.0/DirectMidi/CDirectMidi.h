/*
Module : CDirectMusic.h
Purpose: Defines the header implementation for the library classes
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

#ifndef CDIRECTMIDI_H
#define CDIRECTMIDI_H


// Some required DX headers
#include <dmusicc.h>
#include <dmusici.h>
#include <dmksctrl.h>
#include <process.h>

// Macro definition for COM interfaces releases
#define SAFE_RELEASE(p)      {if(p) {(p)->Release(); (p)=NULL;}}

#define DM_FAILED				-1
// Byte constants to activate synthesizer effect (only if supported)
const BYTE SET_REVERB		=	0x1;
const BYTE SET_CHORUS		=	0x2;

// Reserved constants for internal operations
const BYTE MIDI_STRUCTURED  =	3;
const BYTE SHIFT_8			=	8;
const BYTE SHIFT_16			=	16;
const BYTE BITMASK			=	15;

// >>>>>> Main status bytes of the MIDI messages system <<<<<<

// Note off event
const BYTE NOTE_OFF			=	0x80;

// Note on event
const BYTE NOTE_ON			=	0x90;

// Program change
const BYTE PATCH_CHANGE		=	0xC0;

// Polyphonic Key Pressure (Aftertouch)
const BYTE POLY_PRESSURE	=	0xA0;

// Control Change 
const BYTE CONTROL_CHANGE	=	0xB0;

//Channel Pressure (After-touch).
const BYTE CHANNEL_PREASURE =	0xD0;

// Pitch Wheel Change
const BYTE PITCH_BEND		=	0xE0;

// >>>>> System Common messages <<<<<<

// Start of exclusive
const BYTE START_SYS_EX		=	0xF0;

// End of exclusive
const BYTE END_SYS_EX		=	0xF7;

// Song position pointer
const BYTE SONG_POINTER		=	0xF2;

// Song select
const BYTE SONG_SELECT		=	0xF3;

// Tune request
const BYTE TUNE_REQUEST		=	0xF6;

// >>>>>> System Real-Time Messages <<<<<<

// Timing clock
const BYTE TIMING_CLOCK		=	0xF8;

// Reset
const BYTE RESET			=	0xFF;

// Active sensing
const BYTE ACTIVE_SENSING   =	0xFE;

// Start the sequence
const BYTE START			=	0xFA;

// Stop the sequence
const BYTE STOP				=	0xFC;


// Infoport structure
typedef struct INFOPORT {
   char szPortDescription[DMUS_MAX_DESCRIPTION*2]; // Port description string
   DWORD dwFlags;		// Port characteristics
   DWORD dwClass;		// Class of port (Input/Output) 
   DWORD dwType;		// Type of port (See remarks) 	
   DWORD dwMemorySize;	// Amount of memory available to store DLS instruments
   DWORD dwMaxAudioChannels;  // Maximum number of audio channels that can be rendered by the port
   DWORD dwMaxVoices;		  // Maximum number of voices that can be allocated when this port is opened
   DWORD dwMaxChannelGroups ; // Maximum number of channel groups supported by this port 
   DWORD dwEffectFlags;	// Flags indicating what audio effects are available on the port
   GUID guidSynthGUID;	// Identifier of the port
} *LPINFOPORT;


// Instrument Info structure
typedef struct INSTRUMENTINFO {
	char  szInstName[MAX_PATH*2];		// Name of the instrument	
	DWORD dwPatchInCollection;		// Position of the instrument in the collection
} *LPINSTRUMENTINFO;


// Class prototypes
class CInstrument;
class CCollection;
class CDLSLoader;
class CInputPort;
class COutputPort;
class CDirectMusic;


// Definition of the class instrument
class CInstrument
{
friend CCollection;
friend COutputPort;
protected:
	IDirectMusicDownloadedInstrument8*		m_pDownLoadedInstrument; // Interface pointers
	IDirectMusicInstrument8*				m_pInstrument;	
public:
	CInstrument();
	~CInstrument();
	HRESULT SetPatch(DWORD dwdstPatchMidi);	// Sets the instrument of the collection in a MIDI program
	DWORD m_dwPatchInCollection;			// Information provided by the class
	DWORD m_dwPatchInMidi;
	char  m_strInstName[MAX_PATH*2];

};


// Definition of the class collection
class CCollection
{
friend CDLSLoader;
protected:	
	IDirectMusicCollection8*	m_pCollection;	// Interface pointer
public:	
	CCollection();
	~CCollection();
	HRESULT EnumInstrument(DWORD dwIndex,LPINSTRUMENTINFO InstInfo);	// Enumerates instruments
	HRESULT GetInstrument(CInstrument &pInstrument,LPINSTRUMENTINFO InstInfo);	// Gets the instrument
	HRESULT GetInstrument(CInstrument &pInstrument,INT nIndex);
};

// Definition of the class loader

class CDLSLoader
{
protected:	
	IDirectMusicLoader8*		m_pLoader; // Interface pointer
public:
	CDLSLoader();
	~CDLSLoader();
	HRESULT Initialize();	// Initialize the object
	HRESULT LoadDLS(LPSTR lpFileName,CCollection &pCollection);	// Loads a DLS file
	HRESULT UnLoadCollection(CCollection &pCollection);	// Unload the collection from the loader
};

// CDirectMusic main class definition

class CDirectMusic
{
friend CInputPort; 
friend COutputPort;
protected:	
	IDirectMusic8*			    m_pMusic8; // DirectMusic interface pointer
public:
	CDirectMusic();					// Constructor and the destructor of the class
	~CDirectMusic();

	HRESULT Initialize(HWND hWnd = NULL);	// Initialize object and DirectSound
		
};

// CInputPort class definition

class CInputPort
{
private:	
	HANDLE					   m_hEvent;	// Event handler	
	BOOL					   m_ProcessActive;	// Thread exit condition
protected:	
	IDirectMusicThru8*         m_pThru;		// Pointer to IdirectMusicThru interface
	IDirectMusicPort8*		   m_pPort;		// Pointer to IdirectMusicPort interface
	IDirectMusicBuffer8*	   m_pBuffer;	// Interface pointer to the buffer for incoming MIDI events 
	IDirectMusic8*			   m_pMusic8;	// Pointer to the DirectMusic interface
	HRESULT ReadBuffer();					// Read the buffer
	HRESULT ResetBuffer();					// Reset the buffer pointer
	HRESULT GetMidiEvent(REFERENCE_TIME *lprt,DWORD *lpdwGroup,DWORD *lpcb,BYTE **lppb); // Extract an event
public:
    CInputPort();										// The constructor and the destructor of the class
    ~CInputPort();
   														// Public member functions exposed by the class
	HRESULT	Initialize(CDirectMusic &pDMusic); 
	HRESULT ActivatePort(LPINFOPORT InfoPort,DWORD dwSysExSize = 32);	// Activate the MIDI port
	HRESULT GetPortInfo(DWORD dwNumPort,LPINFOPORT lpInfoPort);	// Gets port info
	DWORD	GetNumPorts();			// Returns the number of midi ports
	HRESULT ActivateNotification();	// Activate the event notification 
	HRESULT ReleasePort();			// Releases the references to the interfaces	
	HRESULT SetThru(DWORD dwSourceChannel,DWORD dwDestinationChannel,COutputPort &pdstMidiPort); //Activate Thru
	HRESULT BreakThru(DWORD dwSourceChannel,DWORD dwDestinationChannel);	//Desactivate Thru
	HRESULT TerminateNotification();				// Terminate the event notification	
	static DWORD WINAPI WaitForEvent(LPVOID lpv);	// Main thread for reading events	
	static void DecodeMidiMsg(DWORD dwMsg,BYTE *Status,BYTE *DataByte1,BYTE *DataByte2); // Decode a Midi Message		
	static void DecodeMidiMsg(DWORD Msg,BYTE *Command,BYTE *Channel,BYTE *DataByte1,BYTE *DataByte2);
	// Virtual functions to receive midi messages, structured and unstructured
	virtual void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwBytesRead,BYTE *lpBuffer)=0; 
	virtual void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwMsg)=0;
};
													
// COutputPort class definition

class COutputPort
{
friend CInputPort;
protected:	
	IReferenceClock*					m_pClock;	// Pointer to the Reference Clock
	IDirectMusicPort8*					m_pPort;	// Pointer to IDirectMusicPort interface
	IDirectMusicBuffer8*				m_pBuffer;	// Buffer to store midi messages
	IDirectMusic8*						m_pMusic8;	// Pointer to the DirectMusic interface
public:
    COutputPort();										// Constructor and the destructor of the class
    ~COutputPort();
   														
	HRESULT	Initialize(CDirectMusic &pDMusic); 
	HRESULT ActivatePort(LPINFOPORT InfoPort,DWORD dwSysExSize = 32);	// Activate the MIDI port
	HRESULT GetPortInfo(DWORD dwNumPort,LPINFOPORT lpInfoPort);	// Gets port info
	DWORD	GetNumPorts();	// Returns the number of midi ports
	HRESULT ReleasePort();	// Releases the references to the interfaces
	HRESULT DownLoadInstrument(CInstrument &pInstrument);	// Download an instrument to the port
	HRESULT UnLoadInstrument(CInstrument &pInstrument);	// Unload an instrument from the port
	HRESULT SetEffect(BOOL bActivate,BYTE nEffect);	// Activate midi effects
	HRESULT SendMidiMsg(DWORD dwMsg);	// Sends a message to the port in DWORD format
	HRESULT SendMidiMsg(LPBYTE lpMsg,DWORD dwLength,DWORD dwChannel);	// Sends an unstrucuted message to the port
	static DWORD EncodeMidiMsg(BYTE Status,BYTE DataByte1,BYTE DataByte2); // Conversion functions	
	static DWORD EncodeMidiMsg(BYTE Command,BYTE Channel,BYTE DataByte1,BYTE DataByte2);
};
													


#endif												// End of class definitions