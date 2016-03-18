
#define UNICODE
#include <windows.h>
#include <windowsx.h>
#ifdef WM
#include <aygshell.h>
#endif
#include "main.h"

void trace (char *s);

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

HANDLE hPort;
HANDLE hReadThread;
DWORD PortReadThread (LPVOID lpvoid);

/***********************************************************************

  PortInitialize (LPTSTR lpszPortName)

***********************************************************************/
BOOL PortInitialize (HWND hWnd, LPTSTR lpszPortName)
{
  DWORD dwError,
        dwThreadID;
  DCB PortDCB;
  COMMTIMEOUTS CommTimeouts;

  // Open the serial port.
  hPort = CreateFile (lpszPortName, // Pointer to the name of the port
                      GENERIC_READ | GENERIC_WRITE,
                                    // Access (read/write) mode
                      0,            // Share mode
                      NULL,         // Pointer to the security attribute
                      OPEN_EXISTING,// How to open the serial port
                      0,            // Port attributes
                      NULL);        // Handle to port with attribute
                                    // to copy

  // If it fails to open the port, return FALSE.
  if ( hPort == INVALID_HANDLE_VALUE ) 
  {
    // Could not open the port.
    MessageBox (hWnd, TEXT("Unable to open the port"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return FALSE;
  }

  PortDCB.DCBlength = sizeof (DCB);     

  // Get the default port setting information.
  GetCommState (hPort, &PortDCB);

  // Change the DCB structure settings.
  PortDCB.BaudRate = 9600;              // Current baud 
  PortDCB.fBinary = TRUE;               // Binary mode; no EOF check 
  PortDCB.fParity = TRUE;               // Enable parity checking. 
  PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control 
  PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control 
  PortDCB.fDtrControl = DTR_CONTROL_ENABLE; 
                                        // DTR flow control type 
  PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
  PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
  PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control 
  PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control 
  PortDCB.fErrorChar = FALSE;           // Disable error replacement. 
  PortDCB.fNull = FALSE;                // Disable null stripping. 
  PortDCB.fRtsControl = RTS_CONTROL_ENABLE; 
                                        // RTS flow control 
  PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on 
                                        // error.
  PortDCB.ByteSize = 8;                 // Number of bits/bytes, 4-8 
  PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space 
  PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2 

  // Configure the port according to the specifications of the DCB 
  // structure.
  if (!SetCommState (hPort, &PortDCB))
  {
    // Could not create the read thread.
    MessageBox (hWnd, TEXT("Unable to configure the serial port"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return FALSE;
  }

  // Retrieve the time-out parameters for all read and write operations
  // on the port. 
  GetCommTimeouts (hPort, &CommTimeouts);

  // Change the COMMTIMEOUTS structure settings.
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;  
  CommTimeouts.ReadTotalTimeoutMultiplier = 0;  
  CommTimeouts.ReadTotalTimeoutConstant = 0;    
  CommTimeouts.WriteTotalTimeoutMultiplier = 10;  
  CommTimeouts.WriteTotalTimeoutConstant = 1000;    

  // Set the time-out parameters for all read and write operations
  // on the port. 
  if (!SetCommTimeouts (hPort, &CommTimeouts))
  {
    // Could not create the read thread.
    MessageBox (hWnd, TEXT("Unable to set the time-out parameters"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return FALSE;
  }

  // Direct the port to perform extended functions SETDTR and SETRTS.
  // SETDTR: Sends the DTR (data-terminal-ready) signal.
  // SETRTS: Sends the RTS (request-to-send) signal. 
  EscapeCommFunction (hPort, SETDTR);
  EscapeCommFunction (hPort, SETRTS);

  // Create a read thread for reading data from the communication port.
  if (hReadThread = CreateThread (NULL, 0, PortReadThread, 0, 0, 
                                  &dwThreadID))
  {
    CloseHandle (hReadThread);
  }
  else
  {
    // Could not create the read thread.
    MessageBox (hWnd, TEXT("Unable to create the read thread"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return FALSE;
  }

  return TRUE;
}


/***********************************************************************

  PortWrite (BYTE Byte)

***********************************************************************/
void PortWrite (BYTE Byte)
{
  DWORD dwError,
        dwNumBytesWritten;

  if (!WriteFile (hPort,              // Port handle
                  &Byte,              // Pointer to the data to write 
                  1,                  // Number of bytes to write
                  &dwNumBytesWritten, // Pointer to the number of bytes 
                                      // written
                  NULL))              // Must be NULL for Windows CE
  {
    // WriteFile failed. Report error.
    dwError = GetLastError ();
  }
}

void ProcessChar (BYTE c)
{
}


/***********************************************************************

  PortReadThread (LPVOID lpvoid)

***********************************************************************/
DWORD PortReadThread (LPVOID lpvoid)
{
  BYTE Byte;
  DWORD dwCommModemStatus,
        dwBytesTransferred;
  
  // Specify a set of events to be monitored for the port.
  SetCommMask (hPort, EV_RXCHAR | EV_CTS | EV_DSR | EV_RLSD | EV_RING);

  while (hPort != INVALID_HANDLE_VALUE) 
  {
    // Wait for an event to occur for the port.
    WaitCommEvent (hPort, &dwCommModemStatus, 0);

    // Re-specify the set of events to be monitored for the port.
    SetCommMask (hPort, EV_RXCHAR | EV_CTS | EV_DSR | EV_RING);

    if (dwCommModemStatus & EV_RXCHAR) 
    {
      // Loop for waiting for the data.
      do 
      {
        // Read the data from the serial port.
        ReadFile (hPort, &Byte, 1, &dwBytesTransferred, 0);

        // Display the data read.
        if (dwBytesTransferred == 1)
          ProcessChar (Byte);

      } while (dwBytesTransferred == 1);
    }

    // Retrieve modem control-register values.
    GetCommModemStatus (hPort, &dwCommModemStatus);

    // Set the indicator lights.
    // SetLightIndicators (dwCommModemStatus);
  }

  return 0;
}


/***********************************************************************

  PortClose (HANDLE hCommPort)

***********************************************************************/
BOOL PortClose (HANDLE hCommPort)
{
  DWORD dwError;

  if (hCommPort != INVALID_HANDLE_VALUE)
  {
    // Close the communication port.
    if (!CloseHandle (hCommPort))
    {
      dwError = GetLastError ();
      return FALSE;
    }
    else
    {
      hCommPort = INVALID_HANDLE_VALUE;
      return TRUE;
    }
  }

  return FALSE;
}

void LireSerie (HWND hwnd)
{
 HANDLE port_serie;
 port_serie = CreateFile (TEXT("COM5:"),
	 GENERIC_READ | GENERIC_WRITE,
	 0, NULL, OPEN_EXISTING, 0, NULL);
 if (port_serie == INVALID_HANDLE_VALUE)
	 // wnd.MessageBox (TEXT("Impossible d'ouvrir le port série"), NULL, MB_OK);
	//  wnd.afficher (TEXT("Impossible d'ouvrir le port série"));
	MessageBox (hwnd, L"Impossible d'ouvrir le port série", NULL, MB_OK);
 else
	 // wnd.MessageBox (TEXT("Port série ouvert"), NULL, MB_OK);
	 // wnd.afficher (TEXT("Port série ouvert"));
	MessageBox (hwnd, L"Port série ouvert", NULL, MB_OK);
 DCB etat_port;
 GetCommState (port_serie, &etat_port);
 etat_port.DCBlength = sizeof(DCB);
 etat_port.BaudRate = 4800;
 etat_port.fParity = FALSE;
 etat_port.ByteSize = 8;
 etat_port.StopBits = 1;

 if (SetCommState (port_serie, &etat_port))
	 // wnd.MessageBox (TEXT("Port configuré"), NULL, MB_OK);
	 // wnd.afficher (TEXT("Port configuré"));
	MessageBox (hwnd, L"Port configuré", NULL, MB_OK);
 else
	 // wnd.MessageBox (TEXT("Impossible de configurer le port"), NULL, MB_OK);
	 // wnd.afficher (TEXT("Impossible de configurer le port"));
	MessageBox (hwnd, L"Impossible de configurer le port", NULL, MB_OK);


 CloseHandle (port_serie);

}
