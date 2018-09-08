// Record.cpp: interface for the CRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Record.h"
#include "time.h"

CRecord::CRecord()
{
	nTotal = TOTAL_MEMORY;	//Initializes the total memory counter
}

CRecord::~CRecord()
{
}

CRecord::GetStartTime()		// Stores the record start time
{
	GetSystemTime(&Start);
	nIndex = 0;				// Start counting from 0
}


CRecord::StoreMidiMsg(DWORD dwMsg)	// Stores the current message
{
	DWORD dwdiff;
	
	GetSystemTime(&Current);		// Calculates the elapsed time
	dwdiff = (Current.wMilliseconds+Current.wSecond*1000+Current.wMinute*60000+Current.wHour*3600000)-
			(Start.wMilliseconds+Start.wSecond*1000+Start.wMinute*60000+Start.wHour*3600000);
	
	if (Start.wHour>Current.wHour) // In case 0:0:0:0 hours UTC
		dwdiff = 86400000+dwdiff;

	NoteArray[nIndex].dwTime = dwdiff;			// Stores the time when the message was generated
	NoteArray[nIndex].dwMsg = dwMsg;			// Stores the message
	nMsgRecorded = ++nIndex;				    // Increments the message counter and stores the total recorded
}


// Returns the index 

DWORD CRecord::GetIndex()					
{
	return nIndex;					
}

// Returns the total recorded messages

DWORD CRecord::GetTotalRecorded()
{
	return nMsgRecorded;
}

// Set index to 0, start counting

void CRecord::ResetIndex()
{
	nIndex = 0;
}

// Advance index in one unit

void CRecord::StepIndex()
{
	nIndex++;
}

// Gets the message in the array pointed by the index

DWORD CRecord::GetStoredMessage()
{
	return NoteArray[nIndex].dwMsg;
}

// Returns the space memory for recording

DWORD CRecord::GetTotalMsgMemory()
{
	return nTotal;
}

// Returns the time of a stored message given the index

DWORD CRecord::GetTime(DWORD dwIndex)
{
	return NoteArray[dwIndex].dwTime;
}