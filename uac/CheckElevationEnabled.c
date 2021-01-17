#ifndef UNICODE
  #define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <locale.h>

#define AddrToFunc(T) ((T)(GetProcAddress(GetModuleHandle(L"kernel32.dll"), (&((#T)[1])))))
/*
 * kernel32!CheckElevationEnabled is just a wrapper of ntdll!RtlQueryElevationFlags
 * Signature:
 * DWORD CheckElevationEnabled(
 *    _In_Out_ PBOOL pEnabled
 * );
 */
typedef DWORD (WINAPI *pCheckElevationEnabled)(PBOOL);

void getlasterror(DWORD ecode) {
  HLOCAL msg = NULL;
  DWORD size = FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPWSTR)&msg, 0, NULL
  );

  if (0 == size)
    wprintf(L"[?] Unknown error has been occured.\n");
  else
    wprintf(L"[!] %.*s\n", (INT)(size - sizeof(WCHAR)), (LPWSTR)msg);

  if (NULL != LocalFree(msg))
    wprintf(L"LocalFree (%d) fatal error.\n", GetLastError());
}

int wmain(void) {
  _wsetlocale(LC_CTYPE, L"");

  DWORD err = ERROR_SUCCESS;
  BOOL  uac = FALSE;

  pCheckElevationEnabled CheckElevationEnabled = AddrToFunc(pCheckElevationEnabled);
  if (NULL == CheckElevationEnabled) {
    err = GetLastError();
    getlasterror(err);
    return err;
  }

  err = CheckElevationEnabled(&uac);
  if (ERROR_SUCCESS != err) {
    getlasterror(err);
    return err;
  }

  wprintf(L"UAC is %sabled.\n", uac ? L"en" : L"dis");

  return 0;
}
