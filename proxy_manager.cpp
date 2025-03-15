#include <fstream>
#include <vector>
#include <windows.h>

bool InstallProxyManager(const std::vector<std::pair<std::string, std::string>> &ssid_ip_list)
{
    std::string path = "C:\\Users\\Public\\Documents\\.proxy";
    CreateDirectoryA(path.c_str(), NULL);
    MessageBoxA(NULL, "Installation en cours.", "Info", MB_OK);

    if (S_OK != URLDownloadToFile(NULL, "https://raw.githubusercontent.com/badge881/proxy-manager/refs/heads/prerequises/V2/proxy.exe", (path + "\\proxy.exe").c_str(), 0, NULL))
    {
        MessageBoxW(NULL, L"Erreur lors du téléchargement de la ressource : « proxy.exe ».\nMerci de vérifier votre connexion Internet.", L"ERREUR", MB_OK);
        return false;
    }

    std::ofstream data(path + "\\proxy.txt");
    for (const auto &[SSID, proxy] : ssid_ip_list)
        data << SSID << "\n"
             << proxy << "\n";

    STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    if (!CreateProcessAsUserA(NULL, NULL, const_cast<char *>("powershell -Command $($WshShell = New-Object -ComObject WScript.Shell;\
$Shortcut = $WshShell.CreateShortcut(\\\"$env:USERPROFILE\\Desktop\\proxy_manager.lnk\\\");\
$Shortcut.TargetPath = \\\"C:\\Users\\Public\\Documents\\.proxy\\proxy.exe\\\";\
$Shortcut.WindowStyle = 7;\
$Shortcut.Description = \\\"proxy manager\\\";\
$Shortcut.IconLocation = \\\"%SystemRoot%\\System32\\SHELL32.dll,9\\\";\
$Shortcut.Save();)"),
                              NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        MessageBoxA(NULL, "Erreur lors de la creation du lien sur le bureau.", "ERREUR", MB_OK);
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    MessageBoxA(NULL, "Installation finie.", "Info", MB_OK);
    return true;
}

const char g_szClassName[] = "MainWindow";
HWND hEditSSID, hEditIP, hList;
std::vector<std::pair<std::string, std::string>> ssid_ip_list;

void CreateHelpWindow()
{
    MessageBoxW(NULL, L"Manuel d'utilisation :\n\
1) Saisissez la paire, à savoir le nom du réseau Wi-Fi et l'adresse IP du proxy, par exemple : « Wi-Fi-Free » et « 1.1.1.1:3128 ».\n\
2) Validez la sélection en cliquant sur le bouton correspondant.\n\
3) Une fois la programmation terminée, veuillez cliquer sur « Fermer ». Dans le cas contraire, veuillez continuer à l'étape 1.", L"Aide", MB_OK);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateWindowW(L"static", L"Saisissez le nom du Wi-Fi et son adresse IP.", WS_VISIBLE | WS_CHILD, 10, 10, 345, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("button", "Aide", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 360, 10, 60, 20, hwnd, (HMENU)3, NULL, NULL);
        CreateWindowA("static", "WiFi: ", WS_VISIBLE | WS_CHILD, 10, 40, 70, 20, hwnd, NULL, NULL, NULL);
        hEditSSID = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 90, 40, 200, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("static", "IP Proxy: ", WS_VISIBLE | WS_CHILD, 10, 70, 70, 20, hwnd, NULL, NULL, NULL);
        hEditIP = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 90, 70, 200, 20, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"button", L"Valider", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 300, 37, 120, 55, hwnd, (HMENU)1, NULL, NULL);
        hList = CreateWindowA("listbox", "", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY | LBS_STANDARD, 10, 100, 410, 120, hwnd, NULL, NULL, NULL);
        CreateWindowA("button", "Modifier", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 220, 200, 25, hwnd, (HMENU)4, NULL, NULL);
        CreateWindowA("button", "Supprimer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 220, 220, 200, 25, hwnd, (HMENU)5, NULL, NULL);
        CreateWindowA("button", "Fermer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 300, 250, 120, 25, hwnd, (HMENU)2, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            char ssid[256], ip[256];
            GetWindowTextA(hEditSSID, ssid, 256);
            GetWindowTextA(hEditIP, ip, 256);
            ssid_ip_list.insert(ssid_ip_list.begin(), make_pair(std::string(ssid), std::string(ip)));
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)(ssid + std::string(" - ") + ip).c_str());
            SetWindowTextA(hEditSSID, "");
            SetWindowTextA(hEditIP, "");
        }
        else if (LOWORD(wParam) == 2)
        {
            if (InstallProxyManager(ssid_ip_list))
                DestroyWindow(hwnd);
            else
                MessageBoxW(NULL, L"L'installation a échouée", L"ERREUR", MB_OK);
        }
        else if (LOWORD(wParam) == 3)
        {
            CreateHelpWindow();
        }
        else if (LOWORD(wParam) == 4)
        {
            int index = SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (index != LB_ERR)
            {
                char buffer[256];
                SendMessageA(hList, LB_GETTEXT, index, (LPARAM)buffer);
                std::string item(buffer);
                size_t pos = item.find(" - ");
                if (pos != std::string::npos)
                {
                    std::string ssid = item.substr(0, pos);
                    std::string ip = item.substr(pos + 3);
                    SetWindowTextA(hEditSSID, ssid.c_str());
                    SetWindowTextA(hEditIP, ip.c_str());
                    ssid_ip_list.erase(ssid_ip_list.begin() + index);
                    SendMessageA(hList, LB_DELETESTRING, index, 0);
                }
            }
        }
        else if (LOWORD(wParam) == 5)
        {
            int index = SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (index != LB_ERR)
            {
                ssid_ip_list.erase(ssid_ip_list.begin() + index);
                SendMessageA(hList, LB_DELETESTRING, index, 0);
            }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&wc))
    {
        MessageBoxA(NULL, "Erreur code 15.", "ERREUR", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Proxy Manager Installer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 330,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBoxA(NULL, "Erreur code 16.", "ERREUR", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessageA(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return Msg.wParam;
}