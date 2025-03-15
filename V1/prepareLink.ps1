$WshShell = New-Object -ComObject WScript.Shell;
$Shortcut = $WshShell.CreateShortcut("$env:USERPROFILE\Desktop\proxy_manager.lnk");
$Shortcut.TargetPath = "powershell.exe";
$Shortcut.Arguments = "-File " + $PSScriptRoot + "\proxy.ps1";
$Shortcut.WindowStyle = 7;
$Shortcut.WorkingDirectory = $PSScriptRoot;
$Shortcut.Description = "proxy manager";
$Shortcut.IconLocation = "%SystemRoot%\System32\SHELL32.dll,9";
$Shortcut.Save();