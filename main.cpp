#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>

#include"resource.h"


/*Declare Variables*/
HWND g_hwnd;
HWND g_hEdit;
HFONT g_hFont;
COLORREF g_editcolor=RGB(0,0,0);
HBRUSH g_hbrbackground=CreateSolidBrush(RGB(255,255,255));

char curfile[MAX_PATH];
bool isopened=false;
bool needsave=false;


//Declare Functions

void LoadFileToEdit();

void SaveTextFileFromEdit();

bool GetFileNameForSave();

void checksave();

void ChooseFontForEdit();

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


/*  Make the class name into a global variable  */
char szClassName[ ] = "AbhinavApp";
char strTitle[ ]="MyPad 1.0 [Untitled]";

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;
    MSG msg;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_WIN));
    wincl.hIconSm = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_TITLE));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDC_MENU);                 /* menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           NULL,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "MyPad 1.0 [Untitled]",            /* Title Text */
           WS_VISIBLE |
           WS_SYSMENU |
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           640,                 /* The programs width */
           480,                  /* and height in pixels */
           NULL,                 /* The window is not a child-window */
           0,                /*  menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    if(!hwnd)
    {
        MessageBox(NULL,"Could not create the window...\nThe application will now exit...","Error",MB_OK | MB_ICONERROR);
        return 0;
    }
    g_hwnd=hwnd;
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);


    HACCEL hAccel=LoadAccelerators(hThisInstance,MAKEINTRESOURCE(IDC_ACCEL));
    bool done=false;
    /* Run the message loop. It will run until GetMessage() returns 0 */
   while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;;							// If So done=TRUE
			}
			else  if(!TranslateAccelerator(g_hwnd,hAccel,&msg))									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}

    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}


void LoadFileToEdit()
{
    curfile[0]='\0';
    OPENFILENAME ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.hwndOwner=g_hwnd;
    ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All File(*.*)\0*.*\0";
    ofn.lpstrFile=curfile;
    ofn.nMaxFile=MAX_PATH;
    ofn.Flags=OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt="txt";
    if(!GetOpenFileName(&ofn))
      return;
    HANDLE hFile;
    bool bsucces=false;
    hFile=CreateFile(curfile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if(hFile!=INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;
        dwFileSize=GetFileSize(hFile,NULL);
        if(dwFileSize!=0xFFFFFFFF)
        {
            LPSTR tempftext;
            tempftext=(char*)GlobalAlloc(GPTR,dwFileSize+1);
            if(tempftext!=NULL)
            {
                DWORD dwRead;
                if(ReadFile(hFile,tempftext,dwFileSize,&dwRead,NULL))
               {
                   tempftext[dwFileSize]=0;
                   if(SetWindowText(g_hEdit,tempftext))
                     bsucces=true;
               }
               GlobalFree(tempftext);
           }
       }
       CloseHandle(hFile);
    }
    if(!bsucces)
    {
        MessageBox(g_hwnd,"The File could not be loaded!!","Error",MB_OK | MB_ICONERROR);
        return;
    }
    SetWindowText(g_hwnd,curfile);
    needsave=false;
    isopened=true;
}

void SaveTextFileFromEdit()
{
    HANDLE hFile;
    bool bsucces=false;
    hFile=CreateFile(curfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(hFile!=INVALID_HANDLE_VALUE)
    {
       DWORD dwTextLength;
       dwTextLength=GetWindowTextLength(g_hEdit);
       LPSTR pszText;
       DWORD dwBufferSize=dwTextLength+1;
       pszText=(char*)GlobalAlloc(GPTR,dwBufferSize);
       if(pszText!=NULL)
       {
           if(GetWindowText(g_hEdit,pszText,dwBufferSize))
           {
            DWORD dwWritten;
            if(WriteFile(hFile,pszText,dwTextLength,&dwWritten,NULL))
               bsucces=true;
           }
           GlobalFree(pszText);
       }

       CloseHandle(hFile);
    }
    if(!bsucces)
    {
        MessageBox(g_hwnd,"The File could not be saved!!!","Error",MB_OK | MB_ICONERROR);
        return;
    }
    isopened=true;
    needsave=false;
}

bool GetFileNameForSave()
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn,sizeof(OPENFILENAME));
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.hwndOwner=g_hwnd;
    ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0All File(*.*)\0*.*\0";
    ofn.lpstrFile=curfile;
    ofn.nMaxFile=MAX_PATH;
    ofn.Flags=OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt="txt";
    if(!GetSaveFileName(&ofn))
      return false;
    return true;
}

void checksave()
{
    if(needsave)
            {
                int res;
                res=MessageBox(g_hwnd,"The File has been changed!!!\nDo you want to save it before continueing?","Save File before continueing!!",MB_YESNOCANCEL | MB_ICONINFORMATION);
                if(res==IDCANCEL)
                   return ;
                if(GetFileNameForSave())
                      SaveTextFileFromEdit();

            }
}
void ChooseFontForEdit()
{
    CHOOSEFONT cf={sizeof(CHOOSEFONT)};
    LOGFONT lf;
    GetObject(g_hFont,sizeof(LOGFONT),&lf);
    cf.Flags=CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
    cf.hwndOwner=g_hwnd;
    cf.lpLogFont=&lf;
    cf.rgbColors=g_editcolor;
    if(!ChooseFont(&cf))
      return;
    HFONT hf=CreateFontIndirect(&lf);
    if(hf)
    {
        g_hFont=hf;
        SendMessage(g_hEdit,WM_SETFONT,(WPARAM)g_hFont,TRUE);
    }
    g_editcolor=cf.rgbColors;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
        {
            HWND hEdit;
            HFONT hFont;
           //For Edit Control...
           hEdit = CreateWindowEx (
                       WS_EX_CLIENTEDGE,                   /* Extended p ossibilites for variation */
                       "EDIT",         /* Classname */
                       "",       /* Title Text */
                       WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_AUTOHSCROLL|ES_AUTOVSCROLL , /* default window */
                       1,       /* Windows decides the position */
                       1,       /* where the window ends up on the screen */
                       100,                 /* The programs width */
                       100,                 /* and height in pixels */
                       hwnd,        /* The window is a child-window to hwnd */
                      (HMENU) IDC_EDIT,                /* No menu */
                       GetModuleHandle(NULL),       /* Program Instance handler */
                       NULL                 /* No Window Creation data */
                        );
           if(hEdit==NULL)
           {
               MessageBox(g_hwnd,"Could not Create Edit control!!","Error",MB_OK | MB_ICONERROR);
               PostQuitMessage(0);
           }

           hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
           SendMessage(hEdit,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(FALSE,0));

           g_hEdit=hEdit;
           g_hFont=hFont;

           RECT rcClient;
            GetClientRect(g_hwnd,&rcClient);
            SetWindowPos(g_hEdit,NULL,0,0,rcClient.right,rcClient.bottom,SWP_NOZORDER);

        }

        break;
        case WM_SIZE:
        {
            RECT rcClient;
            GetClientRect(g_hwnd,&rcClient);
            SetWindowPos(g_hEdit,NULL,0,0,rcClient.right,rcClient.bottom,SWP_NOZORDER);
        }
        break;
        case WM_CLOSE:
        {
            if(needsave)
            {
                int res;
                res=MessageBox(g_hwnd,"The File has been changed!!!\nDo you want to save it before exit?","Save File before Existing!!",MB_YESNOCANCEL | MB_ICONINFORMATION);
                if(res==IDCANCEL)
                   return 0;
                if(res==IDYES)
                {
                    if(GetFileNameForSave())
                      SaveTextFileFromEdit();
                }
            }
            if(MessageBox(g_hwnd,"Are you sure you want to exit!!!","Sure Exit?",MB_YESNO | MB_ICONQUESTION)==IDNO)
              return 0;
            PostQuitMessage(0);
        }

        break;
        case WM_CTLCOLOREDIT:
        {
            HDC hdcedit=(HDC)wParam;
            SetTextColor(hdcedit,g_editcolor);
            SetBkMode(hdcedit,TRANSPARENT);
            return (LONG)g_hbrbackground;
        }
        break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_FNEW:
                {
                    checksave();
                    isopened=false;
                    needsave=false;
                    SetWindowText(g_hEdit,"");
                    SetWindowText(g_hwnd,"MyPad 1.0 [Untitled]");
                }
                break;
                case IDM_FOPEN:
                {
                    checksave();
                    LoadFileToEdit();
                }
                break;
                case IDM_FSAVE:
                {
                    if(needsave)
                    {
                        if(isopened)
                          SaveTextFileFromEdit();
                        else if(GetFileNameForSave())
                          SaveTextFileFromEdit();
                    }
                }
                break;
                case IDM_FSAVEAS:
                {
                    if(GetFileNameForSave())
                      SaveTextFileFromEdit();
                }
                break;
                case IDM_FEXIT:
                  PostMessage(hwnd,WM_CLOSE,0,0);
                break;
                case IDM_ECUT:
                  SendMessage(g_hEdit,WM_CUT,0,0);
                break;
                case IDM_ECOPY:
                  SendMessage(g_hEdit,WM_COPY,0,0);
                break;
                case IDM_EPASTE:
                  SendMessage(g_hEdit,WM_PASTE,0,0);
                break;
                case IDM_FOCHANGE:
                  ChooseFontForEdit();
                break;
                case IDM_APROG:
                  MessageBox(g_hwnd,"Programmer:Abhinav Tripathi\nCreated:8 to 13 October,2010\nRemarks:Created as a hobby to program...\nAbout Programmer:Started programming in c++ from August 2008 and liked it so much that it's now my PASSION and HOBBY both...\n\nI know all the programming languages but like just c++ and hence have made it in c++..","Programmer",MB_OK | MB_ICONINFORMATION);
                break;
                case IDM_AMPAD:
                  MessageBox(g_hwnd,"Software:MyPad\nVersion:1.0\nDated:8 to 13 October,2010\nRemarks:Created as a base for providing GUI in a compiler program also as my first windows app...","MyPad 1.0",MB_OK | MB_ICONINFORMATION);
                break;
                case IDC_EDIT:
                {
                    switch(HIWORD(wParam))
                    {
                        case EN_CHANGE:
                           needsave=true;
                        break;
                    }
                }
                break;
           }
           break;
     default:                      /* for messages that we don't deal with */
            break;
    }

   return DefWindowProc (hwnd, message, wParam, lParam);
}
