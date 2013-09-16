; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=SetPara
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Robot.h"
LastPage=0

ClassCount=6
Class1=CRobotApp
Class2=CRobotDoc
Class3=CRobotView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Class6=SetPara
Resource3=IDD_DIALOGPARA

[CLS:CRobotApp]
Type=0
HeaderFile=Robot.h
ImplementationFile=Robot.cpp
Filter=N

[CLS:CRobotDoc]
Type=0
HeaderFile=RobotDoc.h
ImplementationFile=RobotDoc.cpp
Filter=N

[CLS:CRobotView]
Type=0
HeaderFile=RobotView.h
ImplementationFile=RobotView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=Robot.cpp
ImplementationFile=Robot.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_MENUSTART
Command3=ID_MENUPARA
Command4=ID_APP_EXIT
Command5=ID_MENUERASE
Command6=ID_MENUADD
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_APP_ABOUT
CommandCount=9

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_BTSTART
Command2=ID_BTPAUSE
Command3=ID_BTERASE
Command4=ID_BTADD
Command5=ID_BTPARA
Command6=ID_APP_ABOUT
CommandCount=6

[DLG:IDD_DIALOGPARA]
Type=1
Class=SetPara
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDITHEIGHT,edit,1350631553
Control4=IDC_EDITWIDTH,edit,1350631553
Control5=IDC_EDITCROSS,edit,1350631553
Control6=IDC_EDITMUTATION,edit,1350631553
Control7=IDC_EDITPOP,edit,1350631553
Control8=IDC_EDITGENNUM,edit,1350631553
Control9=IDC_STATICHEIGHT,static,1342308865
Control10=IDC_STATICWIDTH,static,1342308865
Control11=IDC_STATICCROSS,static,1342308865
Control12=IDC_STATICMUTATION,static,1342308865
Control13=IDC_STATICPOP,static,1342308865
Control14=IDC_STATICGENNUM,static,1342308865

[CLS:SetPara]
Type=0
HeaderFile=SetPara.h
ImplementationFile=SetPara.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_APP_ABOUT
VirtualFilter=dWC

