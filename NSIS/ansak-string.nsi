;
; Copyright (c) 2021-2022, Arthur N. Klassen
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
;
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and-or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;
;-------------------------------------------------------------------------
;
; 2021.10.01 - First version
;
;    May you do good and not evil.
;    May you find forgiveness for yourself and forgive others.
;    May you share freely, never taking more than you give.
;
;-------------------------------------------------------------------------
;
; package.nsi -- Set up installer that populates things like this
;                (top level location configurable, include and lib hierarchies not)
; C:\ProgramData\include\ansak                 (string.hxx, _splitjoin, _trim)
;               \lib     \Win32\Debug          (ansakString.lib) (ansakString.pdb)
;                              \RelWithDebInfo        "                 "
;                              \MinSizeRel            "
;                              \Release               "
;                        \x64  \Debug                 "          (ansakString.pdb)
;                              \RelWithDebInfo        "                 "
;                              \MinSizeRel            "
;                              \Release               "
;
; base directories include\ (contains ansak\string*hxx as above)
;                  lib\ (contains hierarchy above with ansakString.lib,
;                        sometimes ansakString.pdb as needed)
;-------------------------------------------------------------------------

Name "ANSAK String Library"
!Include ansakVersion.nsh
Outfile "ansak-string-u${ANSAK_UNICODE_SUPPORT}-setup.exe"
RequestExecutionLevel admin
InstallDir C:\ProgramData

;--------------------------------
;
; Version Resource Definitions

VIAddVersionKey /LANG=1033 "Product Name" "ANSAK String Library (dev library)"
VIAddVersionKey /LANG=1033 "Comments" "Unicode validation, encoding conversion, splitting, trimming, lower-casing."
VIAddVersionKey /LANG=1033 "Company Name" "ANSAK Software Uninc."
VIAddVersionKey /LANG=1033 "LegalCopyright" "© 2015-2022 ANSAK Software Uninc."
VIAddVersionKey /LANG=1033 "FileDescription" "Installs ansak-string headers and libraries for MSVC on Windows"
VIAddVersionKey /LANG=1033 "InternalName" "ansak-string"

VIAddVersionKey /LANG=1033 "FileVersion" "${ANSAK_LIB_VERSION}"
VIFileVersion ${ANSAK_LIB_VERSION}.0
VIProductVersion ${ANSAK_LIB_VERSION}.0

;--------------------------------
;
; 1. License Page

Page license

LicenseText "Please read the following license agreement."
LicenseData "ansak-library-bsd-2clause.txt"
LicenseForceSelection checkbox "I accept"

;--------------------------------
;
; 2. Directory Page

Page directory


;--------------------------------
;
; 3. Install files

Page instfiles
Icon "ansak.ico"
UninstallIcon "ansak.ico"

;--------------------------------
; Uninstall Pages
;    -- like their install analogs

;--------------------------------
;
; 1. Uninstall confirm

UninstPage uninstConfirm un.AreYouSure

;--------------------------------
;
; 2. Uninstall progress

UninstPage instfiles

;--------------------------------
; Function -- uninstall ... are you sure?
; (function name must begin 'un.')

Function un.AreYouSure
  ; set the title of the uninstall window
  MessageBox MB_YESNO \
             "Uninstall ansak-string, headers and libraries?" \
             /SD IDNO IDNO leaveIt
  Return
  leaveIt:
  Quit
FunctionEnd

;================================================================
; Sections - install
;================================================================

Section "Headers and Libraries"
    SetOutPath "$INSTDIR"
    File FindANSAK.cmake

    SetOutPath "$INSTDIR\include\ansak"
    File include\ansak\*
    SetOutPath "$INSTDIR\lib\Win32\Debug"
    File lib\Win32\Debug\*
    SetOutPath "$INSTDIR\lib\Win32\RelWithDebInfo"
    File lib\Win32\RelWithDebInfo\*
    SetOutPath "$INSTDIR\lib\Win32\MinSizeRel"
    File lib\Win32\MinSizeRel\*
    SetOutPath "$INSTDIR\lib\Win32\Release"
    File lib\Win32\Release\*

    SetOutPath "$INSTDIR\lib\x64\Debug"
    File lib\x64\Debug\*
    SetOutPath "$INSTDIR\lib\x64\RelWithDebInfo"
    File lib\x64\RelWithDebInfo\*
    SetOutPath "$INSTDIR\lib\x64\MinSizeRel"
    File lib\x64\MinSizeRel\*
    SetOutPath "$INSTDIR\lib\x64\Release"
    File lib\x64\Release\*

    WriteUninstaller "$INSTDIR\uninstall-ansak-string.exe"
SectionEnd

Function un.isEmptyDir
  # Stack ->                    # Stack: <directory>
  Exch $0                       # Stack: $0
  Push $1                       # Stack: $1, $0
  FindFirst $0 $1 "$0\*.*"
  strcmp $1 "." 0 _notempty
    FindNext $0 $1
    strcmp $1 ".." 0 _notempty
      ClearErrors
      FindNext $0 $1
      IfErrors 0 _notempty
        FindClose $0
        Pop $1                  # Stack: $0
        StrCpy $0 1
        Exch $0                 # Stack: 1 (true)
        goto _end
     _notempty:
       FindClose $0
       ClearErrors
       Pop $1                   # Stack: $0
       StrCpy $0 0
       Exch $0                  # Stack: 0 (false)
  _end:
FunctionEnd

;================================================================
; Sections - uninstall
;================================================================

Section "Uninstall"
    Delete "$INSTDIR\include\ansak\string.hxx"
    Delete "$INSTDIR\include\ansak\string_splitjoin.hxx"
    Delete "$INSTDIR\include\ansak\string_trim.hxx"

    Delete "$INSTDIR\lib\Win32\Debug\ansakString.lib"
    Delete "$INSTDIR\lib\Win32\Debug\ansakString.pdb"
    Delete "$INSTDIR\lib\Win32\RelWithDebInfo\ansakString.lib"
    Delete "$INSTDIR\lib\Win32\RelWithDebInfo\ansakString.pdb"
    Delete "$INSTDIR\lib\Win32\MinSizeRel\ansakString.lib"
    Delete "$INSTDIR\lib\Win32\Release\ansakString.lib"

    Delete "$INSTDIR\lib\x64\Debug\ansakString.lib"
    Delete "$INSTDIR\lib\x64\Debug\ansakString.pdb"
    Delete "$INSTDIR\lib\x64\RelWithDebInfo\ansakString.lib"
    Delete "$INSTDIR\lib\x64\RelWithDebInfo\ansakString.pdb"
    Delete "$INSTDIR\lib\x64\MinSizeRel\ansakString.lib"
    Delete "$INSTDIR\lib\x64\Release\ansakString.lib"

    ; do we want to clean up a bit more?
    Push "$INSTDIR\include\ansak"
    Call un.isEmptyDir
    Pop $0
    StrCmp $0 "1" 0 notEmpty
    RmDir "$INSTDIR\include\ansak"
    Delete "$INSTDIR\FindANSAK.cmake"

notEmpty:
    Delete "$INSTDIR\uninstall-ansak-string.exe"
SectionEnd
