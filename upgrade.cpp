#include <windows.h>
#include <urlmon.h>
#include <fstream>
#include "getProxy.txt.hpp"
using namespace std;

const bool isEq(const double &dFirstVal, const double &dSecondVal)
{
    return std::fabs(dFirstVal - dSecondVal) < std::numeric_limits<double>::epsilon();
}

string GetFileContent(const string &url)
{
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];

    if (GetTempPath(MAX_PATH, tempPath) == 0)
        return "";

    if (GetTempFileName(tempPath, "TMP", 0, tempFile) == 0)
        return "";

    HRESULT hr = URLDownloadToFile(NULL, url.c_str(), tempFile, 0, NULL);
    if (FAILED(hr))
        return "";

    ifstream file(tempFile);
    if (!file.is_open())
        return "";

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    DeleteFile(tempFile);

    return content;
}

signed main()
{
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessAsUserA(NULL, NULL, const_cast<char *>("C:\\Users\\Public\\Documents\\.proxy\\proxy.exe"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        MessageBoxA(NULL, "CreateProcessA failed.", "ERREUR", MB_OK);
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);

    stringstream info(GetFileContent("https://raw.githubusercontent.com/badge881/proxy-manager/refs/heads/prerequises/version.txt"));

    if (info.str() == "")
    {
        ErrorBoxW(L"Erreur lors de la récupération des ressources liés au versions disponibles.\nMerci de vérifier votre connexion Internet.", L"ERREUR");
        return 0;
    }
    unordered_map<double, tuple<bool, string>> versions;
    while (!info.eof())
    {
        string version, access, downloadlink;
        getline(info, version);
        getline(info, access);
        getline(info, downloadlink);
        versions[atof(trim(version).c_str())] = {trim(access) == "allowed", trim(downloadlink)};
    }
    string version = getSetting("version");
    double curr = atof(version.c_str());
    if (version == "")
        curr = 2.0;
    double lastest = 0;
    string link;
    for (auto &&[key, value] : versions)
        if (key > lastest && get<0>(value))
            lastest = key, link = get<1>(value);

    if (!isEq(lastest, curr) and curr < lastest)
    {
        int ans = MessageBoxW(NULL, L"Une mise à jour est disponible voulez vous l'installer ?", L"Mise à jour", MB_YESNO);
        if (ans == 6)
        {
            MessageBoxA(NULL, "Installation en cours ...", "Info", MB_OK);
            if (URLDownloadToFileA(NULL, link.c_str(), "C:\\Users\\Public\\Documents\\.proxy\\proxy_manager.exe", 0, NULL) != S_OK)
            {
                MessageBoxW(NULL, L"Erreur lors du téléchargement de la ressource : « proxy_manager.exe ».\nMerci de vérifier votre connexion Internet.", L"ERREUR", MB_OK);
                MessageBoxA(NULL, link.c_str(), "info", MB_OK);
                return 0;
            }
            STARTUPINFO si = {sizeof(si)};

            PROCESS_INFORMATION pi = {0};
            if (!CreateProcessAsUserA(NULL, NULL, const_cast<char *>(("C:\\Users\\Public\\Documents\\.proxy\\proxy_manager.exe --upgrade " + to_string(curr)).c_str()),
                                      NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
            {
                MessageBoxA(NULL, "Erreur lors de l'installation.", "ERREUR", MB_OK);
                return 0;
            }
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            MessageBox(NULL, "Installation réussite", "Info", MB_OK);
        }
    }
}
