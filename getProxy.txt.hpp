#include <bits/stdc++.h>
using namespace std;

void ErrorBoxA(string message, string windowName = "ERREUR")
{
    MessageBoxA(NULL, message.c_str(), windowName.c_str(), MB_OK);
}
void ErrorBoxW(wstring message, wstring windowName = L"ERREUR")
{
    MessageBoxW(NULL, message.c_str(), windowName.c_str(), MB_OK);
}

#define ErrorBox  __MINGW_NAME_AW(ErrorBox)

const char *delVals = " \t\n\r\f\v";

inline string trim(string s, const char *t = delVals)
{
    s.erase(0, s.find_first_not_of(t));
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

template<typename K, typename V>
V get(const unordered_map<K, V>& map, const K& key, const V& defaultValue = V()) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    return defaultValue;
}

unordered_map<string, string> getSettings()
{
    ifstream data("C:\\Users\\Public\\Documents\\.proxy\\proxy.txt");
    unordered_map<string, string> settings;
    while (!data.eof())
    {
        string name, info;
        getline(data, name);
        getline(data, info);
        settings[name] = info;
    }
    return settings;
}
string getSetting(string name)
{
    ifstream data("C:\\Users\\Public\\Documents\\.proxy\\proxy.txt");
    unordered_map<string, string> settings;
    while (!data.eof())
    {
        string name, info;
        getline(data, name);
        getline(data, info);
        settings[name] = info;
    }
    return get(settings, name, string("not defined"));
}