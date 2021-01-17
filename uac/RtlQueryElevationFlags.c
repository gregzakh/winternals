#ifndef UNICODE
  #define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <locale.h>

typedef LONG NTSTATUS;

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0L)
#define AddrToFunc(T) ((T)GetProcAddress(GetModuleHandle(L"ntdll.dll"), (&((#T)[1]))))

typedef union _RTL_ELVATION_FLAGS {
   ULONG Flags;
   struct { // first three fields are derived from KUSER_SHARED_DATA (+0x2F0)
      ULONG ElevationEnabled : 1;
      ULONG VirtualizationEnabled : 1;
      ULONG InstallerDetectEanbled : 1;
      ULONG ReservedBits : 29;
   };
} RTL_ELVATION_FLAGS, *PRTL_ELVATION_FLAGS;

typedef NTSTATUS (__stdcall *pRtlQueryElevationFlags)(PRTL_ELVATION_FLAGS);
typedef ULONG    (__stdcall *pRtlNtStatusToDosError)(NTSTATUS);

pRtlQueryElevationFlags RtlQueryElevationFlags;
pRtlNtStatusToDosError RtlNtStatusToDosError;

void getlasterror(NTSTATUS nts) {
  HLOCAL msg = NULL;
  DWORD size = FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    NULL, 0L != nts ? RtlNtStatusToDosError(nts) : GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&msg, 0, NULL
  );

  if (0 == size)
    wprintf(L"[?] Unknown error has been occured.\n");
  else
    wprintf(L"[!] %.*s\n", (INT)(size - sizeof(WCHAR)), (LPWSTR)msg);

  if (NULL != LocalFree(msg))
    wprintf(L"LocalFree (%d) fatal error.\n", GetLastError());
}

BOOLEAN LocateSignatures(void) {
  RtlQueryElevationFlags = AddrToFunc(pRtlQueryElevationFlags);
  if (NULL == RtlQueryElevationFlags) return FALSE;

  RtlNtStatusToDosError = AddrToFunc(pRtlNtStatusToDosError);
  if (NULL == RtlNtStatusToDosError) return FALSE;

  return TRUE;
}

int wmain(void) {
  _wsetlocale(LC_CTYPE, L"");
  if (!LocateSignatures()) {
    getlasterror(0L);
    return 1;
  }

  RTL_ELVATION_FLAGS flags = {0};
  NTSTATUS nts = RtlQueryElevationFlags(&flags);
  if (!NT_SUCCESS(nts)) {
    getlasterror(nts);
    return 1;
  }
  wprintf(L"UAC is %sabled.\n", flags.ElevationEnabled ? L"en" : L"dis");

  return 0;
}
