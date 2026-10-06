; Установщик Qt-клиента для Windows (Inno Setup 6). Версия передаётся из CI: ISCC /DAppVersion=1.0.1
#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif

[Setup]
AppId={{6F3B2C1E-9A4D-4E7B-8C21-5D0F7A9B3E64}
AppName=Quiz Client
AppVersion={#AppVersion}
AppPublisher=Деев Егор Викторович
AppPublisherURL=https://github.com/EDeev/quizlab
DefaultDirName={autopf}\Quiz Client
DefaultGroupName=Quiz Client
UninstallDisplayIcon={app}\quizlab-client.exe
OutputDir=..\..
OutputBaseFilename=quizlab-client-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequiredOverridesAllowed=dialog

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\..\dist\quizlab-client\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Quiz Client"; Filename: "{app}\quizlab-client.exe"
Name: "{group}\{cm:UninstallProgram,Quiz Client}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Quiz Client"; Filename: "{app}\quizlab-client.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\quizlab-client.exe"; Description: "{cm:LaunchProgram,Quiz Client}"; Flags: nowait postinstall skipifsilent
