
extern HANDLE hPort;
extern HANDLE hReadThread;

BOOL PortInitialize (HWND hWnd, LPTSTR lpszPortName);
void PortWrite (BYTE Byte);
DWORD PortReadThread (LPVOID lpvoid);
BOOL PortClose (HANDLE hCommPort);
void LireSerie (HWND hwnd);
