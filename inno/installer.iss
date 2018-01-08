; INNO installer script for Process Event Vulnerability Assessment Tool

#define AppVersion GetFileVersion(AddBackslash(SourcePath) + "x86\pevat.exe")

; ==============================================================================================================

[Setup]

AppName=Process Event Vulnerability Assessment Tool
AppVersion={#AppVersion}
AppVerName=Process Event Vulnerability Assessment Tool ({#AppVersion})
AppCopyright=Copyright © 2009-2011 Software Integrity, LLC. All rights reserved.
AppPublisher=Software Integrity, LLC
AppPublisherURL=http://www.softwareintegrity.com/
AppComments=Test applications for Windows synchronization event vulnerabilities.
AppContact=http://www.softwareintegrity.com/

UsePreviousGroup=yes
UsePreviousAppDir=yes

LicenseFile=common\pevat-license.rtf

DefaultGroupName=Software Integrity\Process Event Vulnerability Assessment Tool
DefaultDirName={pf}\Software Integrity\Process Event Vulnerability Assessment Tool

; Need admin (rather than poweruser) due to VC++ runtime. See http://www.jrsoftware.org/iskb.php?vc.
PrivilegesRequired=admin

UninstallDisplayIcon={app}\pevat.exe

Compression=lzma2/max
SolidCompression=yes

AllowNoIcons=yes

OutputDir=.
OutputBaseFilename="pevat-setup"

VersionInfoCompany=Software Integrity, LLC
VersionInfoCopyright=Copyright © 2009-2011 Software Integrity, LLC. All rights reserved.
VersionInfoDescription=Process Event Vulnerability Assessment Tool installation program.
VersionInfoProductName=Process Event Vulnerability Assessment Tool
VersionInfoProductVersion={#AppVersion}
VersionInfoVersion={#AppVersion}

; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
; On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64

; Note: We don't set ProcessorsAllowed because we want this
; installation to run on all architectures (including Itanium,
; since it's capable of running 32-bit code too).

; ==============================================================================================================

[Types]

Name: "full"; Description: "Full installation (program and documentation)"
Name: "compact"; Description: "Compact installation (program only)"

; ==============================================================================================================

[Components]

Name: "program"; Description: "Program Files"; Types: full compact; Flags: fixed
Name: "documentation"; Description: "Documentation Files"; Types: full; ExtraDiskSpaceRequired: 112000;

; ==============================================================================================================

[Files]

Source: "x64\pevat.exe"; DestDir: "{app}"; Components: program; Check: Is64BitInstallMode
Source: "x64\vcredist_x64.exe"; DestDir: "{app}"; Components: program; Flags: dontcopy restartreplace uninsneveruninstall sharedfile; Check: Is64BitInstallMode

Source: "x86\pevat.exe"; DestDir: "{app}"; Components: program; Check: not Is64BitInstallMode
Source: "x86\vcredist_x86.exe"; DestDir: "{app}"; Components: program; Flags: dontcopy restartreplace uninsneveruninstall sharedfile; Check: not Is64BitInstallMode

Source: "common\pevat-license.rtf"; DestDir: "{app}"; Components: program;

; Source: "common\pevat-user.pdf"; DestDir: "{app}"; Components: documentation;

; ==============================================================================================================

[Icons]

Name: "{group}\Process Event Vulnerability Assessment Tool"; WorkingDir: "{userdocs}"; Comment: "Securely store documents and files using password based cryptography and authenticated encryption."; Filename: "{app}\pevat.exe"
Name: "{group}\User Guide"; Comment: "Read the user guide."; Filename: "{app}\pevat-user.pdf"

Name: "{commonprograms}\Process Event Vulnerability Assessment Tool"; Comment: "Test applications for Windows synchronization event vulnerabilities."; WorkingDir: "{userdocs}"; Filename: "{app}\pevat.exe"
Name: "{commondesktop}\Process Event Vulnerability Assessment Tool"; Comment: "Test applications for Windows synchronization event vulnerabilities."; WorkingDir: "{userdocs}"; Filename: "{app}\pevat.exe"

; ==============================================================================================================

[Code]
const

  QuitMessageReboot = 'The installation of the Microsoft VC++ Runtime redistributable was not completed. You will need to restart your computer to complete the installation.';
  QuitMessageError = 'Error while installing the Microsoft VC++ Runtime redistributable. Setup cannot continue.';

function DetectAndInstallPrerequisites: Boolean;
var
  ResultCode: Integer;

begin  

  (*** Place your prerequisite detection and installation code below. ***)
  (*** Return False if missing prerequisites were detected but their installation failed, else return True. ***)

  if Is64BitInstallMode then begin
    ExtractTemporaryFile('vcredist_x64.exe');
    Exec(ExpandConstant('{tmp}\vcredist_x64.exe'), '/q', '.', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end
  else begin
    ExtractTemporaryFile('vcredist_x86.exe');
    Exec(ExpandConstant('{tmp}\vcredist_x86.exe'), '/q', '.', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;

  if ResultCode <> 0 then begin
    MsgBox('Failed to install or update the Microsoft VC++ Runtime redistributable.', mbError, MB_OK);
    Result := False;
  end
  else begin
    Result := True;
  end;

end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  ChecksumBefore, ChecksumAfter: String;

begin

  ChecksumBefore := MakePendingFileRenameOperationsChecksum;
  if DetectAndInstallPrerequisites then begin
    ChecksumAfter := MakePendingFileRenameOperationsChecksum;
    if ChecksumBefore <> ChecksumAfter then begin
      Result := QuitMessageReboot;
    end;
  end else
    Result := QuitMessageError;
end;
