#ifndef UNICODE
  #define UNICODE
#endif

#include <windows.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include <memory>
#include <locale>

#pragma comment(lib, "ntdll.lib")

typedef LONG NTSTATUS;

#define IOCTL_BEEP CTL_CODE(FILE_DEVICE_BEEP, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _BEEP {
   ULONG Frequency;
   ULONG Duration;
} BEEP;

struct AutoHndl {
  using pointer = HANDLE;

  void operator()(HANDLE instance) const {
    std::wcout << (
      CloseHandle(instance) ?
        L"[*] success" :
        L"[!] CloseHandle fatal error."
      ) << std::endl;
  }
};

int wmain(void) {
  using namespace std;
  locale::global(locale(""));

  auto getlasterror = [](NTSTATUS nts) {
    HLOCAL loc{};
    DWORD size = FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
      nullptr, 0 != nts ? RtlNtStatusToDosError(nts) : GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPWSTR>(&loc), 0, nullptr
    );

    if (!size)
      wcout << L"Unknown error has been occured." << endl;
    else {
      wstring msg(reinterpret_cast<LPWSTR>(loc));
      wcout << L"[!] " << msg.substr(0, size - sizeof(WCHAR)) << endl;
    }

    if (nullptr != LocalFree(loc))
      wcout << L"LocalFree (" << GetLastError() << ") fatal error." << endl;
  };

  UNICODE_STRING name{};
  RtlInitUnicodeString(&name, L"\\Device\\Beep");

  OBJECT_ATTRIBUTES attr{};
  InitializeObjectAttributes(&attr, &name, 0, nullptr, nullptr);

  IO_STATUS_BLOCK isb{};
  HANDLE hndl{};
  NTSTATUS nts = NtCreateFile(
    &hndl, GENERIC_READ, &attr, &isb, nullptr, FILE_ATTRIBUTE_NORMAL,
    FILE_SHARE_READ, FILE_OPEN, 0, nullptr, 0
  );
  if (!NT_SUCCESS(nts)) {
    getlasterror(nts);
    return 1;
  }
  unique_ptr<HANDLE, AutoHndl> driver(hndl);

  DWORD bytes{};
  BEEP beep{};
  beep.Frequency = 1000;
  beep.Duration  = 700;
  if (!DeviceIoControl(
    driver.get(), IOCTL_BEEP, &beep, sizeof(beep), nullptr, 0, &bytes, nullptr
  )) {
    getlasterror(0L);
    return 1;
  }

  return 0;
}
