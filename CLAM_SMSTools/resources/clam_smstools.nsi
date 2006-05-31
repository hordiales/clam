; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "CLAM-SMSTools"
!define PRODUCT_VERSION "${VERSION}"
!define PRODUCT_PUBLISHER "CLAM devel"
!define PRODUCT_WEB_SITE "http://clam.iua.upf.edu"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\SMSTools.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

!define ALL_USERS

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "../COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "Catalan"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Spanish"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\${PRODUCT_NAME}-${PRODUCT_VERSION}_setup.exe"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show


Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
   ReadEnvStr $0 CLAM_PATH
   StrCmp $0 "" 0 +6
   MessageBox MB_ICONEXCLAMATION|MB_YESNO "CLAM external dependencies have not been installed. Do you want to continue?" IDYES +2
   Abort
   MessageBox MB_ICONEXCLAMATION|MB_OK "The application won't work until you install the external libraries package located at ${PRODUCT_WEB_SITE}"
   StrCpy $INSTDIR "$PROGRAMFILES\CLAM\SMSTools"
   Goto +2
   StrCpy $INSTDIR "$0\SMSTools"
   
FunctionEnd

Section "Principal" SEC01
  SetOutPath "$INSTDIR\bin"
  SetOverwrite ifnewer
  File "..\SMSTools.exe"
  CreateDirectory "$SMPROGRAMS\CLAM\SMSTools"
  CreateShortCut "$SMPROGRAMS\CLAM\SMSTools\SMSTools.lnk" "$INSTDIR\bin\SMSTools.exe"
  CreateShortCut "$DESKTOP\SMSTools.lnk" "$INSTDIR\bin\SMSTools.exe"
  File '${EXTERNALDLLDIR}\libsndfile.dll'
  File '${EXTERNALDLLDIR}\ogg.dll'
  File '${EXTERNALDLLDIR}\pthreadVCE.dll'
  File '${EXTERNALDLLDIR}\qt-mt322.dll'
  File '${EXTERNALDLLDIR}\vorbis.dll'
  File '${EXTERNALDLLDIR}\vorbisenc.dll'
  File '${EXTERNALDLLDIR}\vorbisfile.dll'
  File '${EXTERNALDLLDIR}\xerces-c_2_3_0.dll'
  File '${VCRUNTIMEDIR}\msvcp71.dll'
  File '${VCRUNTIMEDIR}\msvcr71.dll'

  SetOutPath "$INSTDIR\example-data\"
  File "..\example-data\*"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\CLAM\SMSTools\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\CLAM\SMSTools\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\SMSTools.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\SMSTools.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "La desinstalaci�n de $(^Name) finaliz� satisfactoriamente."
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "�Est� completamente seguro que desea desinstalar $(^Name) junto con todos sus componentes?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\bin\SMSTools.exe"

  Delete "$SMPROGRAMS\CLAM\SMSTools\Uninstall.lnk"
  Delete "$SMPROGRAMS\CLAM\SMSTools\Website.lnk"
  Delete "$DESKTOP\SMSTools.lnk"
  Delete "$SMPROGRAMS\CLAM\SMSTools\SMSTools.lnk"

  RMDir "$SMPROGRAMS\CLAM\SMSTools"
  RMDir "$INSTDIR\resources"
  Delete "$INSTDIR\bin\libsndfile.dll"
  Delete "$INSTDIR\bin\ogg.dll"
  Delete "$INSTDIR\bin\pthreadVCE.dll"
  Delete "$INSTDIR\bin\qt-mt322.dll"
  Delete "$INSTDIR\bin\vorbis.dll"
  Delete "$INSTDIR\bin\vorbisenc.dll"
  Delete "$INSTDIR\bin\vorbisfile.dll"
  Delete "$INSTDIR\bin\xerces-c_2_3_0.dll"
  Delete "$INSTDIR\bin\msvcp71.dll"
  Delete "$INSTDIR\bin\msvcr71.dll"
  Delete "$SMPROGRAMS\CLAM\SMSTools\Uninstall.lnk"
  Delete "$SMPROGRAMS\CLAM\SMSTools\Website.lnk"
  Delete "$DESKTOP\SMSTools.lnk"
  Delete "$SMPROGRAMS\CLAM\SMSTools\SMSTools.lnk"

  RMDir "$SMPROGRAMS\CLAM\SMSTools"
  RMDir "$SMPROGRAMS\CLAM"
  RMDir "$INSTDIR\bin"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
