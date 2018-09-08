// Record.h: implementation for the CRecord class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CRECORD_H
#define CRECORD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const DWORD TOTAL_MEMORY = 10000 ;	// Maximum memory size for recording (~80KB)


class CRecord
{
	struct Msg					// Structure to store midi messages
	{	
		DWORD dwMsg;			// MIDI 1.0 message
		DWORD dwTime;			// Time of the message in milliseconds
	};
	
	SYSTEMTIME Start;			// Record start time
	SYSTEMTIME Current;			// Record current time
	Msg NoteArray[TOTAL_MEMORY];	// Array of messages
	DWORD nIndex,nTotal,nMsgRecorded;	// Index of the message in the array
public:	
	CRecord();
	~CRecord();
	GetStartTime();					// Reset the recording
	StoreMidiMsg(DWORD dwMsg);		// Store a given midi message in current time
	DWORD GetIndex();				// Get the index in the array
	DWORD GetStoredMessage();		// Get stored Midi DWORD pointed by the index	
	DWORD GetTotalMsgMemory();		// Returns the total memory allocated for recording
	void ResetIndex();				// Set the index to 0 (Start)
	void StepIndex();				// Step the index one unit
	DWORD GetTotalRecorded();		// Returns number of messages recorded
	DWORD GetTime(DWORD dwIndex);	// Returns the time of a given index
};	

#endif

