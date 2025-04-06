#include <iostream>
#include <fstream>
#include <windows.h>
#include "getProxy.txt.hpp"
using namespace std;

int version = 3;

string Get_SSID()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &saAttr, 0))
    {
        throw runtime_error("Failed to create pipe");
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.hStdInput = hRead;
    ZeroMemory(&pi, sizeof(pi));

    const char *command = "powershell -Command \"(Get-NetConnectionProfile).Name\"";
    if (!CreateProcessAsUserA(NULL, NULL, const_cast<char *>(command), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        CloseHandle(hWrite);
        CloseHandle(hRead);
        throw runtime_error("Failed to create process");
    }

    CloseHandle(hWrite);

    // Read the output from the process
    string result;
    char buffer[128];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    CloseHandle(hRead);

    return result;
}

void SetProxyEnable(string proxy)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS)
    {
        MessageBoxW(NULL, (L"Erreur lors de l'activation du proxy (error code: 1" + to_wstring(result) + L")").c_str(), L"ERREUR", MB_OK);
        return;
    }

    DWORD value = 1;
    result = RegSetValueEx(hKey, "ProxyEnable", 0, REG_DWORD, (const BYTE *)&value, sizeof(value));
    if (result != ERROR_SUCCESS)
    {
        MessageBoxW(NULL, (L"Erreur lors de l'activation du proxy (error code: 2" + to_wstring(result) + L")").c_str(), L"ERREUR", MB_OK);
        return;
    }
    result = RegSetValueEx(hKey, "ProxyServer", 0, REG_SZ, (const BYTE *)proxy.c_str(), proxy.size() + 1);
    if (result != ERROR_SUCCESS)
    {
        MessageBoxW(NULL, (L"Erreur lors de l'activation du proxy (error code: 3" + to_wstring(result) + L")").c_str(), L"ERREUR", MB_OK);
        return;
    }
    RegCloseKey(hKey);
}

void SetProxyDisable()
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS)
    {
        MessageBoxW(NULL, (L"Erreur lors de la désactivation du proxy (error code: 1" + to_wstring(result) + L")").c_str(), L"ERREUR", MB_OK);
        return;
    }

    DWORD value = 0;
    result = RegSetValueEx(hKey, "ProxyEnable", 0, REG_DWORD, (const BYTE *)&value, sizeof(value));
    if (result != ERROR_SUCCESS)
    {
        MessageBoxW(NULL, (L"Erreur lors de la désactivation du proxy (error code: 2" + to_wstring(result) + L")").c_str(), L"ERREUR", MB_OK);
        return;
    }
    RegCloseKey(hKey);
}

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        MessageBoxW(NULL, L"Ce programme ne prend pas d'arguments", L"ERREUR", MB_OK);
        return 0;
    }

    string curr_ssid = trim(Get_SSID());
    string ipProxy = getSetting("proxy: " + curr_ssid);
    if (ipProxy == "")
    {
        SetProxyDisable();
        MessageBoxW(NULL, L"Le proxy a bien été désactivé.", L"Info", MB_OK);
    }
    else
    {
        SetProxyEnable(ipProxy);
        MessageBoxW(NULL, L"Le proxy a bien été activé.", L"Info", MB_OK);
    }
    return 0;
}