$wifiName = $(Get-NetConnectionProfile).Name;
$data = Get-Content -Path "$PSScriptRoot\proxy.json" | out-string | ConvertFrom-Json;
function Enable-Proxy {
    param ( $proxy );
    Set-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Internet Settings" -Name ProxyEnable -Value 1;
    Set-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Internet Settings" -Name ProxyServer -Value $proxy;
};
function Disable-Proxy {
    Set-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Internet Settings" -Name ProxyEnable -Value 0;
};
if ($null -eq $data.$wifiName) {
    Disable-Proxy;
} else {
    Enable-Proxy $data.$wifiName;
}
