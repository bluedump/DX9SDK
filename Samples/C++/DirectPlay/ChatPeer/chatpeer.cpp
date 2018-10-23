//----------------------------------------------------------------------------
// File: ChatPeer.cpp
//
// Desc: The main game file for the ChatPeer sample.  It connects 
//       players together with two dialog boxes to prompt users on the 
//       connection settings to join or create a session. After the user 
//       connects to a session, the sample displays a multiplayer stage. 
// 
//       After a new game has started the sample begins a very simplistic 
//       chat session where users can send text to each other.
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef STRICT
#define STRICT
#endif // !STRICT

#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dplay8.h>
#include <dplobby8.h>
#include <dxerr9.h>
#include "NetConnect.h"
#include "DXUtil.h"
#include "SessionInfo.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Platform-dependent includes
//-----------------------------------------------------------------------------
#ifdef UNDER_CE
    #include <aygshell.h>
#else
    #include <richedit.h>
#endif // UNDER_CE


//-----------------------------------------------------------------------------
// Player context locking defines
//-----------------------------------------------------------------------------
CRITICAL_SECTION g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) SAFE_DELETE( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define DPLAY_SAMPLE_KEY        TEXT("Software\\Microsoft\\DirectX DirectPlay Samples")
#define MAX_PLAYER_NAME         14
#define MAX_CHAT_STRINGS        50
#define MAX_CHAT_STRING_LENGTH  508
#define WM_APP_UPDATE_STATS     (WM_APP + 0)
#define WM_APP_CHAT             (WM_APP + 1)
#define CHATPEER_PORT           2502


// This GUID allows DirectPlay to find other instances of the same game on
// the network.  So it must be unique for every game, and the same for 
// every instance of that game.  // {876A3036-FFD7-46bc-9209-B42F617B9BE7}
GUID g_guidApp = { 0x876a3036, 0xffd7, 0x46bc, { 0x92, 0x9, 0xb4, 0x2f, 0x61, 0x7b, 0x9b, 0xe7 } };

struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    TCHAR strPlayerName[MAX_PLAYER_NAME];   // Player name
};



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
IDirectPlay8Peer*  g_pDP                         = NULL;    // DirectPlay peer object
CNetConnectWizard* g_pNetConnectWizard           = NULL;    // Connection wizard
CSessionInfo*      g_pSessionInfo                = NULL;    // Session Information
IDirectPlay8LobbiedApplication* g_pLobbiedApp    = NULL;    // DirectPlay lobbied app 
BOOL               g_bWasLobbyLaunched           = FALSE;   // TRUE if lobby launched
HINSTANCE          g_hInst                       = NULL;    // HINST of app
HWND               g_hDlg                        = NULL;    // HWND of main dialog
DPNID              g_dpnidLocalPlayer            = 0;       // DPNID of local player
DPNID              g_dpnidHostPlayer             = 0;       // DPNID of host player
LONG               g_lNumberOfActivePlayers      = 0;       // Number of players currently in game
TCHAR              g_strAppName[256]             = TEXT("ChatPeer");
HRESULT            g_hrDialog;                              // Exit code for app 
TCHAR              g_strLocalPlayerName[MAX_PATH];          // Local player name
TCHAR              g_strSessionName[MAX_PATH];              // Session name
TCHAR              g_strPreferredProvider[MAX_PATH];        // Provider string




//-----------------------------------------------------------------------------
// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------
#define GAME_MSGID_CHAT    1

// Change compiler pack alignment to be BYTE aligned, and pop the current value
#pragma pack( push, 1 )

struct GAMEMSG_GENERIC
{
    BYTE nType;
};

struct GAMEMSG_CHAT : public GAMEMSG_GENERIC
{
    char strChatString[MAX_CHAT_STRING_LENGTH];
};

// Pop the old pack alignment
#pragma pack( pop )


struct APP_QUEUED_DATA 
{
    GAMEMSG_CHAT* pChatMsg;
    DPNHANDLE hBufferHandle;
};



//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
HRESULT WINAPI DirectPlayLobbyMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
INT_PTR CALLBACK ChatDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
HRESULT        InitDirectPlay();
HRESULT        OnInitDialog( HWND hDlg );
HRESULT        SendChatMessage( HWND hDlg );
HRESULT        RefreshStatusBarText( DPNID idLocal, DPNID idHost );
HRESULT        GetHostnamePortString( TCHAR* str, LPDIRECTPLAY8ADDRESS pAddress, DWORD dwBufferLen );




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application.  Since we use a simple dialog for 
//       user interaction we don't need to pump messages.
//-----------------------------------------------------------------------------
INT APIENTRY _tWinMain( HINSTANCE hInst, HINSTANCE hPrevInst, 
                        LPTSTR pCmdLine, INT nCmdShow )
{
    HRESULT hr;
    HKEY    hDPlaySampleRegKey;
    BOOL    bConnectSuccess = FALSE;

#ifdef UNDER_CE
    // This is needed along with a hidden control in the RC file to make the
    // soft keyboard pop up when the user selects an edit control.
    SHInitExtraControls();
#endif // UNDER_CE

    InitCommonControls();

    g_hInst = hInst;
    InitializeCriticalSection( &g_csPlayerContext );

    // Read persistent state information from registry
    RegCreateKeyEx( HKEY_CURRENT_USER, DPLAY_SAMPLE_KEY, 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, 
                    &hDPlaySampleRegKey, NULL );
    DXUtil_ReadStringRegKeyCch( hDPlaySampleRegKey, TEXT("Player Name"), 
                             g_strLocalPlayerName, MAX_PATH, TEXT("TestPlayer") );
    DXUtil_ReadStringRegKeyCch( hDPlaySampleRegKey, TEXT("Session Name"), 
                             g_strSessionName, MAX_PATH, TEXT("TestGame") );
    DXUtil_ReadStringRegKeyCch( hDPlaySampleRegKey, TEXT("Preferred Provider"), 
                             g_strPreferredProvider, MAX_PATH, TEXT("DirectPlay8 TCP/IP Service Provider") );

    // Init COM so we can use CoCreateInstance
    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    // Create helper class
    g_pNetConnectWizard = new CNetConnectWizard( hInst, NULL, g_strAppName, &g_guidApp );

    if( FAILED( hr = InitDirectPlay() ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("InitDirectPlay"), hr );
        MessageBox( NULL, TEXT("Failed initializing IDirectPlay8Peer. ")
                    TEXT("The sample will now quit."),
                    TEXT("DirectPlay Sample"), MB_OK | MB_ICONERROR );
        goto LCleanup;
    }

    // Create session information helper
    g_pSessionInfo = new CSessionInfo( g_pDP );

    // Check if we were launched from a lobby client
    if( g_bWasLobbyLaunched && g_pNetConnectWizard->HaveConnectionSettingsFromLobby() )
    {
        // If were lobby launched then DPL_MSGID_CONNECT has already been
        // handled, so we can just tell the wizard to connect to the lobby
        // that has sent us a DPL_MSGID_CONNECT msg.
        if( FAILED( hr = g_pNetConnectWizard->ConnectUsingLobbySettings() ) )
        {
            DXTRACE_ERR_MSGBOX( TEXT("ConnectUsingLobbySettings"), hr );
            MessageBox( NULL, TEXT("Failed to connect using lobby settings. ")
                        TEXT("The sample will now quit."),
                        TEXT("DirectPlay Sample"), MB_OK | MB_ICONERROR );

            bConnectSuccess = FALSE;
        }
        else
        {
            // Read information from g_pNetConnectWizard
            _tcsncpy( g_strLocalPlayerName, g_pNetConnectWizard->GetPlayerName(), MAX_PATH-1 );
            g_strLocalPlayerName[ MAX_PATH-1 ] = 0;

            bConnectSuccess = TRUE; 
        }
    }
    else
    {
        // If not lobby launched, prompt the user about the network 
        // connection and which session they would like to join or 
        // if they want to create a new one.

        // Setup connection wizard
        g_pNetConnectWizard->SetPlayerName( g_strLocalPlayerName );
        g_pNetConnectWizard->SetSessionName( g_strSessionName );
        g_pNetConnectWizard->SetPreferredProvider( g_strPreferredProvider );
        g_pNetConnectWizard->SetDefaultPort( CHATPEER_PORT );

        // Do the connection wizard
        hr = g_pNetConnectWizard->DoConnectWizard( FALSE );        
        if( FAILED( hr ) ) 
        {
            DXTRACE_ERR_MSGBOX( TEXT("DoConnectWizard"), hr );
            MessageBox( NULL, TEXT("Multiplayer connect failed. ")
                        TEXT("The sample will now quit."),
                        TEXT("DirectPlay Sample"), MB_OK | MB_ICONERROR );
            bConnectSuccess = FALSE;
        } 
        else if( hr == NCW_S_QUIT ) 
        {
            // The user canceled the Multiplayer connect, so quit 
            bConnectSuccess = FALSE;
        }
        else
        {
            bConnectSuccess = TRUE; 

            // Read information from g_pNetConnectWizard
            _tcsncpy( g_strLocalPlayerName, g_pNetConnectWizard->GetPlayerName(), MAX_PATH-1 );
            g_strLocalPlayerName[ MAX_PATH-1 ] = 0;

            _tcsncpy( g_strSessionName, g_pNetConnectWizard->GetSessionName(), MAX_PATH-1 );
            g_strSessionName[ MAX_PATH-1 ] = 0;

            _tcsncpy( g_strPreferredProvider, g_pNetConnectWizard->GetPreferredProvider(), MAX_PATH-1 );
            g_strPreferredProvider[ MAX_PATH-1 ] = 0;

            // Write information to the registry
            DXUtil_WriteStringRegKey( hDPlaySampleRegKey, TEXT("Player Name"), g_strLocalPlayerName );
            DXUtil_WriteStringRegKey( hDPlaySampleRegKey, TEXT("Session Name"), g_strSessionName );
            DXUtil_WriteStringRegKey( hDPlaySampleRegKey, TEXT("Preferred Provider"), g_strPreferredProvider );
        }
    }

    if( bConnectSuccess )
    {
        // App is now connected via DirectPlay, so start the game.  

        // For this sample, we just start a simple dialog box game.
        g_hrDialog = S_OK;
        DialogBox( hInst, MAKEINTRESOURCE(IDD_MAIN_GAME), NULL, (DLGPROC) ChatDlgProc );

        if( FAILED( g_hrDialog ) )
        {
            if( g_hrDialog == DPNERR_CONNECTIONLOST )
            {
                MessageBox( NULL, TEXT("The DirectPlay session was lost. ")
                            TEXT("The sample will now quit."),
                            TEXT("DirectPlay Sample"), MB_OK | MB_ICONERROR );
            }
            else
            {
                DXTRACE_ERR_MSGBOX( TEXT("DialogBox"), g_hrDialog );
                MessageBox( NULL, TEXT("An error occured during the game. ")
                            TEXT("The sample will now quit."),
                            TEXT("DirectPlay Sample"), MB_OK | MB_ICONERROR );
            }
        }
    }

LCleanup:
    // Cleanup DirectPlay and helper classes
    g_pNetConnectWizard->Shutdown();

    if( g_pDP )
    {
        g_pDP->Close(0);
        SAFE_RELEASE( g_pDP );
    }

    if( g_pLobbiedApp )
    {
        g_pLobbiedApp->Close( 0 );
        SAFE_RELEASE( g_pLobbiedApp );
    }    

    // Don't delete the wizard until we know that 
    // DirectPlay is out of its message handlers.
    // This will be true after Close() has been called. 
    SAFE_DELETE( g_pNetConnectWizard );
    SAFE_DELETE( g_pSessionInfo );

    RegCloseKey( hDPlaySampleRegKey );
    DeleteCriticalSection( &g_csPlayerContext );
    CoUninitialize();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: InitDirectPlay()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT InitDirectPlay()
{
    DPNHANDLE hLobbyLaunchedConnection = NULL;
    HRESULT hr;

    // Create IDirectPlay8Peer
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Peer, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8Peer, 
                                       (LPVOID*) &g_pDP ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );

    // Create IDirectPlay8LobbiedApplication
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8LobbiedApplication, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8LobbiedApplication, 
                                       (LPVOID*) &g_pLobbiedApp ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );

    // Init the helper class, now that g_pDP and g_pLobbiedApp are valid
    g_pNetConnectWizard->Init( g_pDP, g_pLobbiedApp );

    // Turn off parameter validation in release builds
#ifdef _DEBUG
    const DWORD dwInitFlags = 0;
#else
    const DWORD dwInitFlags = DPNINITIALIZE_DISABLEPARAMVAL;
#endif // _DEBUG

    // Init IDirectPlay8Peer
    if( FAILED( hr = g_pDP->Initialize( NULL, DirectPlayMessageHandler, dwInitFlags ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );

    // Init IDirectPlay8LobbiedApplication.  Before this Initialize() returns 
    // a DPL_MSGID_CONNECT msg may come in to the DirectPlayLobbyMessageHandler 
    // so be prepared ahead of time.
    if( FAILED( hr = g_pLobbiedApp->Initialize( NULL, DirectPlayLobbyMessageHandler, 
                                                &hLobbyLaunchedConnection, dwInitFlags ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );

    // IDirectPlay8LobbiedApplication::Initialize returns a handle to a connnection
    // if we have been lobby launced.  Initialize is guanteeded to return after 
    // the DPL_MSGID_CONNECT msg has been processed.  So unless a we are expected 
    // multiple lobby connections, we do not need to remember the lobby connection
    // handle since it will be recorded upon the DPL_MSGID_CONNECT msg.
    g_bWasLobbyLaunched = ( hLobbyLaunchedConnection != NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ChatDlgProc()
// Desc: Handles dialog messages
//-----------------------------------------------------------------------------
INT_PTR CALLBACK ChatDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg ) 
    {
        case WM_INITDIALOG:
        {
#if defined(WIN32_PLATFORM_PSPC) && (_WIN32_WCE >= 300)
            SHINITDLGINFO   shidi;
            memset( &shidi, 0, sizeof(SHINITDLGINFO) );
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
            shidi.hDlg = hDlg;

            SetForegroundWindow( hDlg );
            SHInitDialog( &shidi );
#endif // WIN32_PLATFORM_PSPC

            g_hDlg = hDlg;
            if( FAILED( g_hrDialog = OnInitDialog( hDlg ) ) )
            {
                DXTRACE_ERR_MSGBOX( TEXT("OnInitDialog"), g_hrDialog );
                EndDialog( hDlg, 0 );
            }
            break;
        }

        case WM_APP_UPDATE_STATS:
        {
            // Update the number of players in the game
            TCHAR strNumberPlayers[32];

            wsprintf( strNumberPlayers, TEXT("%d"), g_lNumberOfActivePlayers );
            SetDlgItemText( hDlg, IDC_NUM_PLAYERS, strNumberPlayers );

            // Update the status bar text
            RefreshStatusBarText( g_dpnidLocalPlayer, g_dpnidHostPlayer );
            break;
        }

        case WM_APP_CHAT:
        {
            HRESULT          hr;
            DPNID            dpnidPlayer = (DPNID) wParam;
            APP_PLAYER_INFO* pPlayerInfo = NULL;

            PLAYER_LOCK(); // enter player context CS

            // Get the player context accosicated with this DPNID
            hr = g_pDP->GetPlayerContext( dpnidPlayer, 
                                          (LPVOID* const) &pPlayerInfo,
                                          0);

            PLAYER_ADDREF( pPlayerInfo ); // addref player, since we are using it now
            PLAYER_UNLOCK(); // leave player context CS

            APP_QUEUED_DATA* pQueuedData = (APP_QUEUED_DATA*) lParam;

            if( FAILED(hr) || pPlayerInfo == NULL )
            {
                // The player who sent this may have gone away before this 
                // message was handled, so just ignore it
                if( pQueuedData )
                {
                     g_pDP->ReturnBuffer( pQueuedData->hBufferHandle,0 );
                     SAFE_DELETE( pQueuedData );
                }
                break;
            }
            
            // Add the message to the local listbox
            HWND hWndChatBox = GetDlgItem( hDlg, IDC_CHAT_LISTBOX );
            int nCount = (int)SendMessage( hWndChatBox, LB_GETCOUNT, 0, 0 );
            if( nCount > MAX_CHAT_STRINGS )
                SendMessage( hWndChatBox, LB_DELETESTRING, 0, 0 );

            // Make the chat string from the player's name and the edit box string
            TCHAR strChatBuffer[MAX_PLAYER_NAME + MAX_CHAT_STRING_LENGTH + 32];
            wsprintf( strChatBuffer, TEXT("<%s> %hs"), pPlayerInfo->strPlayerName, pQueuedData->pChatMsg->strChatString );

            PLAYER_LOCK(); // enter player context CS
            PLAYER_RELEASE( pPlayerInfo );  // Release player and cleanup if needed
            PLAYER_UNLOCK(); // leave player context CS            

            // Add it, and make sure it is visible
            int nIndex = (int)SendMessage( hWndChatBox, LB_ADDSTRING, 0, (LPARAM)strChatBuffer );
            SendMessage( hWndChatBox, LB_SETTOPINDEX, nIndex, 0 );

            // Done with the buffer, so return it DirectPlay, 
            // so that the memory can be reused
            g_pDP->ReturnBuffer( pQueuedData->hBufferHandle,0 );
            SAFE_DELETE( pQueuedData );
            break;
        }

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_CHAT_EDIT:
                    if( HIWORD(wParam) == EN_UPDATE )
                    {
                        BOOL bEnableSend;
                        if( 0 == GetWindowTextLength( GetDlgItem( hDlg, IDC_CHAT_EDIT ) ) )
                            bEnableSend = FALSE;
                        else
                            bEnableSend = TRUE;

                        EnableWindow( GetDlgItem( hDlg, IDC_SEND ), bEnableSend );
                    }
                    break;

                case IDC_SEND:
                    // The enter key was pressed, so send out the chat message
                    if( FAILED( g_hrDialog = SendChatMessage( hDlg ) ) )
                    {
                        DXTRACE_ERR_MSGBOX( TEXT("SendChatMessage"), g_hrDialog );
                        EndDialog( hDlg, 0 );
                    }
                    break;

                case IDC_INFO:
                    if( g_pSessionInfo )
                        g_pSessionInfo->ShowDialog( hDlg );
                    return TRUE;

                case IDCANCEL:
                    g_hrDialog = S_OK;
                    EndDialog( hDlg, 0 );
                    return TRUE;
            }
            break;
    }

    return FALSE; // Didn't handle message
}




//-----------------------------------------------------------------------------
// Name: OnInitDialog()
// Desc: Inits the dialog for the chat client.
//-----------------------------------------------------------------------------
HRESULT OnInitDialog( HWND hDlg )
{
    // Load and set the icon
    HICON hIcon = LoadIcon( g_hInst, MAKEINTRESOURCE( IDI_MAIN ) );
    SendMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM) hIcon );  // Set big icon
    SendMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon );  // Set small icon

    // Display local player's name
    SetDlgItemText( hDlg, IDC_PLAYER_NAME, g_strLocalPlayerName );

    PostMessage( hDlg, WM_APP_UPDATE_STATS, 0, 0 );
    SetFocus( GetDlgItem( hDlg, IDC_CHAT_EDIT ) );

#ifndef UNDER_CE
    SendMessage( GetDlgItem( hDlg, IDC_CHAT_EDIT ), EM_SETEVENTMASK, 0, ENM_UPDATE );
#endif // !UNDER_CE
    EnableWindow( GetDlgItem( hDlg, IDC_SEND ), FALSE );

    if( g_pNetConnectWizard->IsHostPlayer() )
        SetWindowText( hDlg, TEXT("ChatPeer (Host)") );
    else
        SetWindowText( hDlg, TEXT("ChatPeer") );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DirectPlayMessageHandler
// Desc: Handler for DirectPlay messages.  This function is called by
//       the DirectPlay message handler pool of threads, so be care of thread
//       synchronization problems with shared memory
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, 
                                         DWORD dwMessageId, 
                                         PVOID pMsgBuffer )
{
    // Try not to stay in this message handler for too long, otherwise
    // there will be a backlog of data.  The best solution is to 
    // queue data as it comes in, and then handle it on other threads
    // as this sample shows

    // This function is called by the DirectPlay message handler pool of 
    // threads, so be care of thread synchronization problems with shared memory
    
    // Sift this message through the SessionInfo helper class
    if( g_pSessionInfo && g_pSessionInfo->MessageHandler( dwMessageId, pMsgBuffer ) )
        return S_OK;

    HRESULT hReturn = S_OK;

    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
            HRESULT hr;
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
            pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            if( NULL == pPlayerInfo )
                break;

            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
            pPlayerInfo->lRefCount   = 1;

            // Get the peer info and extract its name
            DWORD dwSize = 0;
            DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
            hr = DPNERR_CONNECTING;
            
            // GetPeerInfo might return DPNERR_CONNECTING when connecting, 
            // so just keep calling it if it does
            while( hr == DPNERR_CONNECTING ) 
                hr = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );                                
                
            if( hr == DPNERR_BUFFERTOOSMALL )
            {
                pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
                if( NULL == pdpPlayerInfo )
                {
                    // Out of memory
                    SAFE_DELETE( pPlayerInfo )
                    break;
                }

                ZeroMemory( pdpPlayerInfo, dwSize );
                pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
                
                hr = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
                if( SUCCEEDED(hr) )
                {
                    // This stores a extra TCHAR copy of the player name for 
                    // easier access.  This will be redundent copy since DPlay 
                    // also keeps a copy of the player name in GetPeerInfo()
                    DXUtil_ConvertWideStringToGenericCch( pPlayerInfo->strPlayerName, 
                                                       pdpPlayerInfo->pwszName, MAX_PLAYER_NAME );    
                                                       
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
                        g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_HOST )
                        g_dpnidHostPlayer = pCreatePlayerMsg->dpnidPlayer;
                }

                SAFE_DELETE_ARRAY( pdpPlayerInfo );
            }
                
            // Tell DirectPlay to store this pPlayerInfo 
            // pointer in the pvPlayerContext.
            pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedIncrement( &g_lNumberOfActivePlayers );
            if( g_hDlg != NULL )
                PostMessage( g_hDlg, WM_APP_UPDATE_STATS, 0, 0 );

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
            pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;

            PLAYER_LOCK();                  // enter player context CS
            PLAYER_RELEASE( pPlayerInfo );  // Release player and cleanup if needed
            PLAYER_UNLOCK();                // leave player context CS

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedDecrement( &g_lNumberOfActivePlayers );
            if( g_hDlg != NULL )
                PostMessage( g_hDlg, WM_APP_UPDATE_STATS, 0, 0 );

            break;
        }

        case DPN_MSGID_HOST_MIGRATE:
        {
            PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
            pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;

            if( pHostMigrateMsg->dpnidNewHost == g_dpnidLocalPlayer )
                SetWindowText( g_hDlg, TEXT("ChatPeer (Host)") );

            g_dpnidHostPlayer = pHostMigrateMsg->dpnidNewHost;
            if( g_hDlg != NULL )
                PostMessage( g_hDlg, WM_APP_UPDATE_STATS, 0, 0 );

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            g_hrDialog = DPNERR_CONNECTIONLOST;
            EndDialog( g_hDlg, 0 );
            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;           
            if( NULL == pPlayerInfo )
                break;

            // Validate incoming data: A malicious user could modify or create an application
            // to send bogus information; to help guard against logical errors and denial 
            // of service attacks, the size of incoming data should be checked against what
            // is expected.
            if( pReceiveMsg->dwReceiveDataSize < sizeof(GAMEMSG_GENERIC) )
                break;

            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            if( pMsg->nType == GAME_MSGID_CHAT )
            {
                // Validate incoming data
                if( pReceiveMsg->dwReceiveDataSize > sizeof(GAMEMSG_CHAT) )
                    break;

                // This message is sent when a player has send a chat message to us, so 
                // post a message to the dialog thread to update the UI.  
                // This keeps the DirectPlay threads from blocking, and also
                // serializes the recieves since DirectPlayMessageHandler can
                // be called simultaneously from a pool of DirectPlay threads.
                GAMEMSG_CHAT* pChatMessage = (GAMEMSG_CHAT*) pMsg;

                // Make certain the passed string is null terminated
                DWORD dwStringLength = pReceiveMsg->dwReceiveDataSize - sizeof(GAMEMSG_GENERIC);
                pChatMessage->strChatString[ dwStringLength - 1 ] = 0;

                // Record the buffer handle so the buffer can be returned later 
                APP_QUEUED_DATA* pQueuedData = new APP_QUEUED_DATA;
                if( NULL == pQueuedData )
                    break;

                pQueuedData->hBufferHandle = pReceiveMsg->hBufferHandle;
                pQueuedData->pChatMsg      = pChatMessage;

                // Pass the APP_QUEUED_DATA to the main dialog thread, so it can
                // process it.  It will also cleanup the struct
                PostMessage( g_hDlg, WM_APP_CHAT, 
                             pPlayerInfo->dpnidPlayer, (LPARAM) pQueuedData );

                // Tell DirectPlay to assume that ownership of the buffer 
                // has been transferred to the application, and so it will 
                // neither free nor modify it until ownership is returned 
                // to DirectPlay through the ReturnBuffer() call.
                hReturn = DPNSUCCESS_PENDING;
            }
            break;
        }
    }

    // Make sure the DirectPlay MessageHandler calls the CNetConnectWizard handler, 
    // so it can be informed of messages such as DPN_MSGID_ENUM_HOSTS_RESPONSE.
    if( hReturn != DPNSUCCESS_PENDING && SUCCEEDED(hReturn) && g_pNetConnectWizard )
        hReturn = g_pNetConnectWizard->MessageHandler( pvUserContext, dwMessageId, pMsgBuffer );
    
    return hReturn;
}




//-----------------------------------------------------------------------------
// Name: DirectPlayLobbyMessageHandler
// Desc: Handler for DirectPlay lobby messages.  This function is called by
//       the DirectPlay lobby message handler pool of threads, so be careful of 
//       thread synchronization problems with shared memory
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayLobbyMessageHandler( PVOID pvUserContext, 
                                              DWORD dwMessageId, 
                                              PVOID pMsgBuffer )
{
    switch( dwMessageId )
    {
        case DPL_MSGID_CONNECT:
        {
            PDPL_MESSAGE_CONNECT pConnectMsg;
            pConnectMsg = (PDPL_MESSAGE_CONNECT)pMsgBuffer;

            // The CNetConnectWizard will handle this message for us,
            // so there is nothing we need to do here for this simple
            // sample.
            break;
        }

        case DPL_MSGID_DISCONNECT:
        {
            PDPL_MESSAGE_DISCONNECT pDisconnectMsg;
            pDisconnectMsg = (PDPL_MESSAGE_DISCONNECT)pMsgBuffer;

            // We should free any data associated with the lobby 
            // client here, but there is none.
            break;
        }

        case DPL_MSGID_RECEIVE:
        {
            PDPL_MESSAGE_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPL_MESSAGE_RECEIVE)pMsgBuffer;

            // The lobby client sent us data.  This sample doesn't
            // expected data from the client, but it is useful 
            // for more complex apps.
            break;
        }

        case DPL_MSGID_CONNECTION_SETTINGS:
        {
            PDPL_MESSAGE_CONNECTION_SETTINGS pConnectionStatusMsg;
            pConnectionStatusMsg = (PDPL_MESSAGE_CONNECTION_SETTINGS)pMsgBuffer;

            // The lobby client has changed the connection settings.  
            // This simple sample doesn't handle this, but more complex apps may
            // want to.
            break;
        }
    }

    // Make sure the DirectPlay MessageHandler calls the CNetConnectWizard handler, 
    // so the wizard can be informed of lobby messages such as DPL_MSGID_CONNECT
    if( g_pNetConnectWizard )
        return g_pNetConnectWizard->LobbyMessageHandler( pvUserContext, dwMessageId, 
                                                         pMsgBuffer );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SendChatMessage()
// Desc: Create chat string based on the editbox and send it to everyone 
//-----------------------------------------------------------------------------
HRESULT SendChatMessage( HWND hDlg )
{
    // Get length of item text
    DWORD dwEditboxBufferSize = (DWORD)SendDlgItemMessage( hDlg, IDC_CHAT_EDIT, 
                                                           WM_GETTEXTLENGTH, 0, 0 );
    if( dwEditboxBufferSize == 0 )
        return S_OK;  // Don't do anything for blank messages 

    GAMEMSG_CHAT msgChat;
    msgChat.nType = GAME_MSGID_CHAT;
#ifndef UNICODE
    GetDlgItemText( hDlg, IDC_CHAT_EDIT, msgChat.strChatString, MAX_CHAT_STRING_LENGTH );
#else
    WCHAR szTempBuffer[MAX_CHAT_STRING_LENGTH];
    GetDlgItemText( hDlg, IDC_CHAT_EDIT, szTempBuffer, MAX_CHAT_STRING_LENGTH );
    DXUtil_ConvertWideStringToAnsiCch(msgChat.strChatString, szTempBuffer, MAX_CHAT_STRING_LENGTH);
#endif // UNICODE

    // Set the dwBufferSize to only send the string that has valid text in it.  
    // Otherwise bandwidth is wasted.
    DPN_BUFFER_DESC bufferDesc;
    bufferDesc.dwBufferSize = sizeof(msgChat.nType) + 
                              (DWORD) (strlen(msgChat.strChatString)+1);
    bufferDesc.pBufferData  = (BYTE*) &msgChat;

    // Send it to all of the players include the local client
    // DirectPlay will tell via the message handler 
    // if there are any severe errors, so ignore any errors 
    DPNHANDLE hAsync;
    g_pDP->SendTo( DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1,
                   0, NULL, &hAsync, DPNSEND_GUARANTEED );

    // Blank out edit box
    SetDlgItemText( hDlg, IDC_CHAT_EDIT, TEXT("") );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RefreshStatusBarText()
// Desc: Refresh the current status bar text based on connection state
//-----------------------------------------------------------------------------
HRESULT RefreshStatusBarText( DPNID idLocal, DPNID idHost )
{
    HRESULT hr = S_OK; 
    TCHAR strStatus[ 1024 ] = {0};
    TCHAR strAddress[ 256 ] = {0};
    DWORD dwNumAddresses = 0; // Count of address objects
    LPDIRECTPLAY8ADDRESS *rpAddresses = NULL; // Range of addresses    
    DWORD i = 0; // Loop variable

    // Margin the status text
    _tcsncpy( strStatus, TEXT("   "), 1023 );
    strStatus[ 1023 ] = 0;

    if( idLocal == NULL || idHost == NULL )
    {
        // If the host or local dpnid is not set, this should mean the player
        // creation messages have not yet arrived
        _tcsncat( strStatus, TEXT("Not connected"), 1023 - lstrlen( strStatus ) );
        strStatus[ 1023 ] = 0;
        dwNumAddresses = 0;
    }
    else if( idLocal == idHost )
    {
        // Else if the host and local dpnid's are equal, the local player is
        // the session host
        _tcsncat( strStatus, TEXT("Hosting"), 1023 - lstrlen( strStatus ) );
        strStatus[ 1023 ] = 0;
        dwNumAddresses = 0;

        // Determine the buffer size needed to hold the addresses
        hr = g_pDP->GetLocalHostAddresses( rpAddresses, &dwNumAddresses, 0 );
        if( DPNERR_BUFFERTOOSMALL != hr )
            goto LCleanReturn;  

        // Allocate the array
        rpAddresses = new LPDIRECTPLAY8ADDRESS[ dwNumAddresses ];
        if( NULL == rpAddresses )
        {
            hr = E_OUTOFMEMORY;
            goto LCleanReturn;
        }

        ZeroMemory( rpAddresses, dwNumAddresses * sizeof(LPDIRECTPLAY8ADDRESS) );

        // Retrieve the addresses
        hr = g_pDP->GetLocalHostAddresses( rpAddresses, &dwNumAddresses, 0 );
        if( FAILED(hr) )
            goto LCleanReturn;  
    }
    else
    {
        // Else the local player is connected to a remote session
        _tcsncat( strStatus, TEXT("Connected"), 1023 - lstrlen( strStatus ) );
        strStatus[ 1023 ] = 0;
        dwNumAddresses = 1;

        // Allocate the array
        rpAddresses = new LPDIRECTPLAY8ADDRESS[ dwNumAddresses ];
        if( NULL == rpAddresses )
        {
            hr = E_OUTOFMEMORY;
            goto LCleanReturn;
        }

        ZeroMemory( rpAddresses, dwNumAddresses * sizeof(LPDIRECTPLAY8ADDRESS) );

        // Retrieve the address
        hr = g_pDP->GetPeerAddress( idHost, &rpAddresses[0], 0 );
        if( FAILED(hr) )
            goto LCleanReturn;
    }

    // If we have addresses to report, determine a reasonable way to output them
    if( dwNumAddresses > 0 )
    {
        // Get the provider from the DirectPlay address
        GUID guidSP;
        hr = rpAddresses[0]->GetSP( &guidSP );
        if( FAILED(hr) )
            goto LCleanReturn;

        TCHAR strProvider[ 256 ] = {0};
  
        if( CLSID_DP8SP_IPX == guidSP )
            _tcsncpy( strProvider, TEXT(" via IPX"), 256 );
        else if( CLSID_DP8SP_TCPIP == guidSP )
            _tcsncpy( strProvider, TEXT(" via TCP/IP"), 256 );
        else if( CLSID_DP8SP_MODEM == guidSP )
            _tcsncpy( strProvider, TEXT(" via Modem"), 256 );
        else if( CLSID_DP8SP_SERIAL == guidSP )
            _tcsncpy( strProvider, TEXT(" via Serial"), 256 );
        else if( CLSID_DP8SP_BLUETOOTH == guidSP )
            _tcsncpy( strProvider, TEXT(" via Bluetooth"), 256 );
        
        _tcsncat( strStatus, strProvider, 1023 - lstrlen( strStatus ) );
        strStatus[ 1023 ] = 0;

        // If we are using TCP/IP or IPX, display the address list
        if( CLSID_DP8SP_IPX == guidSP ||
            CLSID_DP8SP_TCPIP == guidSP )
        {
            // Append the addresses to the status text
            for( i=0; i < dwNumAddresses; i++ )
            {
                // Get the IP address
                if( FAILED( GetHostnamePortString( strAddress, rpAddresses[ i ], 256 ) ) )
                    break;

                // Add formatting characters
                TCHAR* strSpacing = ( i != 0 ) ? TEXT(", ") : 
                                    ( idLocal == idHost ) ? TEXT(" at ") : TEXT(" to ");                      
        
                _tcsncat( strStatus, strSpacing, 1023 - lstrlen( strStatus ) );
                strStatus[ 1023 ] = 0;


                // Add the IP address string
                _tcsncat( strStatus, strAddress, 1023 - lstrlen( strStatus ) );
                strStatus[ 1023 ] = 0;
     
            }
        }
    }

    // Margin the status text
    _tcsncat( strStatus, TEXT("   "), 1023 - lstrlen( strStatus ) );
    strStatus[ 1023 ] = 0;

    // Set the text
    SendDlgItemMessage( g_hDlg, IDC_STATUS_BAR_TEXT, WM_SETTEXT, 0, (LPARAM) strStatus );
    hr = S_OK;
  
LCleanReturn:
    for( i=0; i < dwNumAddresses; i++ )
        SAFE_RELEASE( rpAddresses[i] );

    SAFE_DELETE_ARRAY( rpAddresses );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: GetHostnamePortString
// Desc: Stores the hostname and port number of the given DirectPlay 
//       address in the provided string.
//-----------------------------------------------------------------------------
HRESULT GetHostnamePortString( TCHAR* str, LPDIRECTPLAY8ADDRESS pAddress, DWORD dwBufferLen )
{
    HRESULT hr = S_OK;

    // Sanity check
    if( NULL == str || NULL == pAddress )
        return E_FAIL;

    // Get the hostname string from the DirectPlay address
    WCHAR wstrHostname[ 256 ] = {0};
    DWORD dwSize = sizeof(wstrHostname);
    DWORD dwDataType = 0;
    hr = pAddress->GetComponentByName( L"hostname", wstrHostname, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Convert from wide character to generic
    TCHAR strHostname[ 256 ] = {0};
    DXUtil_ConvertWideStringToGenericCch( strHostname, wstrHostname, 256 );

    // Get the port value from the DirectPlay address
    DWORD dwPort = 0;
    dwSize = sizeof(DWORD);
    hr = pAddress->GetComponentByName( L"port", &dwPort, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Copy the address string and null terminate the result
    _sntprintf( str, dwBufferLen, TEXT("%s:%d"), strHostname, dwPort );
    str[ dwBufferLen-1 ] = TEXT('\0');

    return S_OK;
}