//-----------------------------------------------------------------------------
// File: $$root$$.h
//
// Desc: Header file $$root$$ sample app
//-----------------------------------------------------------------------------
#pragma once
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file
#endif
$$IF(DPLAY)




//-----------------------------------------------------------------------------
// Player context locking defines
//-----------------------------------------------------------------------------
CRITICAL_SECTION g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) SAFE_DELETE( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );

CRITICAL_SECTION g_csWorldStateContext;
#define WORLD_LOCK()                   EnterCriticalSection( &g_csWorldStateContext ); 
#define WORLD_UNLOCK()                 LeaveCriticalSection( &g_csWorldStateContext );
$$ENDIF




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\$$root$$")

$$IF(ACTIONMAPPER || DPLAY)
// This GUID must be unique for every game, and the same for 
// every instance of this app.  // $$GUIDMSG$$
$$IF(DPLAY)
// The GUID allows DirectPlay to find other instances of the same game on
// the network.  
$$ENDIF // end DPLAY
$$IF(ACTIONMAPPER)
// The GUID allows DirectInput to remember input settings
$$ENDIF // end ACTIONMAPPER
GUID g_guidApp = $$GUIDSTRUCT$$;


$$ENDIF // end (ACTIONMAPPER|DPLAY)
$$IF(SHOW_TRIANGLE)
// Custom D3D vertex format used by the vertex buffer
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;       // vertex position
    D3DXVECTOR3 normal;         // vertex normal
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


$$ENDIF // end SHOW_TRIANGLE
$$IF(DPLAY)
// Associate a structure with every network player
struct APP_PLAYER_INFO
{
    // TODO: change as needed
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    TCHAR strPlayerName[MAX_PATH];          // Player name

$$IF(ACTIONMAPPER)
    FLOAT fAxisRotateUD;                    // State of axis for this player
    FLOAT fAxisRotateLR;                    // State of axis for this player
$$ELSE
    BOOL  bRotateUp;                       // State of up button or this player
    BOOL  bRotateDown;                     // State of down button or this player
    BOOL  bRotateLeft;                     // State of left button or this player
    BOOL  bRotateRight;                    // State of right button or this player
$$ENDIF
$$IF(DPLAYVOICE)

    BOOL  bHalfDuplex;                      // TRUE if player is in half-duplex mode
    BOOL  bTalking;                         // TRUE if player is talking
$$ENDIF

    APP_PLAYER_INFO* pNext;
    APP_PLAYER_INFO* pPrev;
};


$$ENDIF // end DPLAY
$$IF(ACTIONMAPPER)
// DirectInput action mapper reports events only when buttons/axis change
// so we need to remember the present state of relevant axis/buttons for 
// each DirectInput device.  The CInputDeviceManager will store a 
// pointer for each device that points to this struct
struct InputDeviceState
{
    // TODO: change as needed
    FLOAT fAxisRotateLR;
    BOOL  bButtonRotateLeft;
    BOOL  bButtonRotateRight;

    FLOAT fAxisRotateUD;
    BOOL  bButtonRotateUp;
    BOOL  bButtonRotateDown;
$$IF(DMUSIC || DSOUND)

    BOOL  bButtonPlaySoundButtonDown;
$$ENDIF
};


$$ENDIF
// Struct to store the current input state
struct UserInput
{
$$IF(KEYBOARD)
    BYTE diks[256];   // DirectInput keyboard state buffer 

$$ENDIF
    // TODO: change as needed
$$IF(ACTIONMAPPER)
    FLOAT fAxisRotateUD;
    FLOAT fAxisRotateLR;
$$ELSE // start !ACTIONMAPPER --> (KEYBOARD || !DINPUT)
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
$$ENDIF 
$$IF(DMUSIC || DSOUND)
    BOOL bPlaySoundButtonDown;
$$ENDIF
$$IF(ACTIONMAPPER)
    BOOL bDoConfigureInput;
    BOOL bDoConfigureDisplay;
$$ENDIF
$$IF(DPLAYVOICE)
    BOOL bDoConfigureVoice;
$$ENDIF
};


$$IF(ACTIONMAPPER)
// Input semantics used by this app
enum INPUT_SEMANTICS
{
    // Gameplay semantics
    // TODO: change as needed
    INPUT_ROTATE_AXIS_LR=1, INPUT_ROTATE_AXIS_UD,       
    INPUT_ROTATE_LEFT,      INPUT_ROTATE_RIGHT,    
    INPUT_ROTATE_UP,        INPUT_ROTATE_DOWN,
    INPUT_CONFIG_INPUT,     INPUT_CONFIG_DISPLAY,
$$IF(DPLAYVOICE)
    INPUT_CONFIG_VOICE,     
$$ENDIF
$$IF(DMUSIC || DSOUND)
    INPUT_PLAY_SOUND,       
$$ENDIF
};

// Actions used by this app
DIACTION g_rgGameAction[] =
{
    // TODO: change as needed.  Be sure to delete user map files 
    // (C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini)
    // after changing this, otherwise settings won't reset and will be read 
    // from the out of date ini files 

    // Device input (joystick, etc.) that is pre-defined by DInput, according
    // to genre type. The genre for this app is space simulators.
    { INPUT_ROTATE_AXIS_LR,  DIAXIS_3DCONTROL_LATERAL,      0, TEXT("Rotate left/right"), },
    { INPUT_ROTATE_AXIS_UD,  DIAXIS_3DCONTROL_MOVE,         0, TEXT("Rotate up/down"), },
$$IF(DMUSIC || DSOUND)
    { INPUT_PLAY_SOUND,      DIBUTTON_3DCONTROL_SPECIAL,    0, TEXT("Play sound"), },
$$ENDIF

    // Keyboard input mappings
    { INPUT_ROTATE_LEFT,     DIKEYBOARD_LEFT,               0, TEXT("Rotate left"), },
    { INPUT_ROTATE_RIGHT,    DIKEYBOARD_RIGHT,              0, TEXT("Rotate right"), },
    { INPUT_ROTATE_UP,       DIKEYBOARD_UP,                 0, TEXT("Rotate up"), },
    { INPUT_ROTATE_DOWN,     DIKEYBOARD_DOWN,               0, TEXT("Rotate down"), },
$$IF(DMUSIC || DSOUND)
    { INPUT_PLAY_SOUND,      DIKEYBOARD_F5,                 0, TEXT("Play sound"), },
$$ENDIF
    { INPUT_CONFIG_DISPLAY,  DIKEYBOARD_F2,                 DIA_APPFIXED, TEXT("Configure Display"), },    
    { INPUT_CONFIG_INPUT,    DIKEYBOARD_F3,                 DIA_APPFIXED, TEXT("Configure Input"), },    
$$IF(DPLAYVOICE)
    { INPUT_CONFIG_VOICE,    DIKEYBOARD_F4,                 DIA_APPFIXED, TEXT("Configure Voice"), },    
$$ENDIF
};

#define NUMBER_OF_GAMEACTIONS    (sizeof(g_rgGameAction)/sizeof(DIACTION))


$$ENDIF
$$IF(DPLAY)
//-----------------------------------------------------------------------------
// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------

// TODO: change or add app specific DirectPlay messages and structures as needed
#define GAME_MSGID_WORLDSTATE    1
#define GAME_MSGID_INPUTSTATE    2
#define GAME_MSGID_HOSTPAUSE     3

// Change compiler pack alignment to be BYTE aligned, and pop the current value
#pragma pack( push, 1 )

struct GAMEMSG_GENERIC
{
    // One of GAME_MSGID_* IDs so the app knows which GAMEMSG_* struct
    // to cast the msg pointer into.
    WORD nType; 
};

struct GAMEMSG_WORLDSTATE : public GAMEMSG_GENERIC
{
    FLOAT fWorldRotX;
    FLOAT fWorldRotY;
};

struct GAMEMSG_INPUTSTATE : public GAMEMSG_GENERIC
{
$$IF(ACTIONMAPPER)
    FLOAT fAxisRotateUD;
    FLOAT fAxisRotateLR;
$$ELSE
    BOOL  bRotateUp;   
    BOOL  bRotateDown; 
    BOOL  bRotateLeft; 
    BOOL  bRotateRight;
$$ENDIF
};

struct GAMEMSG_HOSTPAUSE : public GAMEMSG_GENERIC
{
    BOOL bHostPause;
};

// Pop the old pack alignment
#pragma pack( pop )


$$ENDIF


//-----------------------------------------------------------------------------
// Name: class CAppForm
// Desc: CFormView-based class which allows the UI to be created with a form
//       (dialog) resource. This class manages all the controls on the form.
//-----------------------------------------------------------------------------
class CAppForm : public CFormView, public CD3DApplication
{
public:
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
$$IF(SHOW_TRIANGLE)
    LPDIRECT3DVERTEXBUFFER9 m_pVB;                  // Vextex buffer 
$$ENDIF
$$IF(D3DFONT)
    CD3DFont*               m_pFont;                // Font for drawing text
$$ELSE
    ID3DXFont*              m_pD3DXFont;            // D3DX font    
$$ENDIF
$$IF(SHOW_TEAPOT)
    ID3DXMesh*              m_pD3DXMesh;            // D3DX mesh to store teapot
$$ENDIF

$$IF(KEYBOARD)
    LPDIRECTINPUT8          m_pDI;                  // DirectInput object
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput keyboard device
$$ENDIF
$$IF(ACTIONMAPPER)
    CInputDeviceManager*    m_pInputDeviceManager;  // DirectInput device manager
    DIACTIONFORMAT          m_diafGame;             // Action format for game play
    LPDIRECT3DSURFACE9      m_pDIConfigSurface;     // Surface for config'ing DInput devices
$$ENDIF
    UserInput               m_UserInput;            // Struct for storing user input 

$$IF(DMUSIC || DSOUND)
    FLOAT                   m_fSoundPlayRepeatCountdown; // Sound repeat timer
$$IF(DMUSIC)
    CMusicManager*          m_pMusicManager;        // DirectMusic manager class
    CMusicSegment*          m_pBounceSound;         // Bounce sound
$$ELSE // start !DMUSIC
    CSoundManager*          m_pSoundManager;        // DirectSound manager class
    CSound*                 m_pBounceSound;         // Bounce sound
$$ENDIF // end DMUSIC

$$ENDIF // end (DMUSIC || DSOUND)
$$IF(DPLAY)
    IDirectPlay8Peer*       m_pDP;                  // DirectPlay peer object
    CNetConnectWizard*      m_pNetConnectWizard;    // Connection wizard
    IDirectPlay8LobbiedApplication* m_pLobbiedApp;  // DirectPlay lobbied app 
    BOOL                    m_bWasLobbyLaunched;    // TRUE if lobby launched
    DPNID                   m_dpnidLocalPlayer;     // DPNID of local player
    LONG                    m_lNumberOfActivePlayers;        // Number of players currently in game
    TCHAR                   m_strLocalPlayerName[MAX_PATH];  // Local player name
    TCHAR                   m_strSessionName[MAX_PATH];      // Session name
    TCHAR                   m_strPreferredProvider[MAX_PATH];// Provider string
    APP_PLAYER_INFO         m_PlayInfoList;         // List of players
    APP_PLAYER_INFO*        m_pLocalPlayerInfo;     // APP_PLAYER_INFO struct for local player
    HRESULT                 m_hrNet;                // HRESULT of DirectPlay events
    FLOAT                   m_fWorldSyncTimer;      // Timer for syncing world state between players
    BOOL                    m_bHostPausing;         // Has the host paused the app?
    UserInput               m_CombinedNetworkInput; // Combined input from all network players

$$ENDIF
$$IF(DPLAYVOICE)
    CNetVoice*              m_pNetVoice;            // DirectPlay voice helper class
    DVCLIENTCONFIG          m_dvClientConfig;       // Voice client config
    GUID                    m_guidDVSessionCT;      // GUID for chosen voice compression
    BOOL                    m_bNetworkPlayersTalking; // TRUE if any of the network players are talking
    BOOL                    m_bLocalPlayerTalking; // TRUE if the local player is talking

$$ENDIF
    FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    FLOAT                   m_fWorldRotY;           // World rotation state Y-axis

private:
    HWND    m_hwndRenderWindow;
    HWND    m_hwndRenderFullScreen;
    HWND    m_hWndTopLevelParent;

    virtual HRESULT ConfirmDevice( D3DCAPS9*,DWORD,D3DFORMAT );
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT FinalCleanup();
    virtual HRESULT AdjustWindowForChange();
$$IF(DPLAY || ACTIONMAPPER)
    VOID    Pause( bool bPause );
$$ENDIF

    HRESULT RenderText();

$$IF(DINPUT)
    HRESULT InitInput( HWND hWnd );
$$ENDIF // end DINPUT
    void    UpdateInput( UserInput* pUserInput );
$$IF(DPLAY)
    HRESULT CombineInputFromAllPlayers( UserInput* pCombinedUserInput );
$$ENDIF // end DPLAY
$$IF(DINPUT)
    void    CleanupDirectInput();
$$ENDIF // end DINPUT

$$IF(DMUSIC || DSOUND)
    HRESULT InitAudio( HWND hWnd );

$$ENDIF
$$IF(DPLAY)
    HRESULT InitDirectPlay();
    void    CleanupDirectPlay();
    HRESULT ConnectViaDirectPlay();
    HRESULT SendLocalInputIfChanged();
    HRESULT SendWorldStateToAll();
    HRESULT SendPauseMessageToAll( bool bPause );

$$ENDIF
$$IF(DPLAYVOICE)
    HRESULT InitDirectPlayVoice();
    VOID    UpdateTalkingVariables();
    HRESULT UserConfigVoice();

$$ENDIF
    VOID    ReadSettings();
    VOID    WriteSettings();

    VOID    UpdateUIForDeviceCapabilites();

protected:
    DECLARE_DYNCREATE(CAppForm)

             CAppForm();
    virtual  ~CAppForm();

public:
    BOOL IsReady() { return m_bActive; }
    TCHAR* PstrFrameStats() { return m_strFrameStats; }
    VOID RenderScene() { Render3DEnvironment(); }
    HRESULT CheckForLostFullscreen();

    //{{AFX_DATA(CAppForm)
    enum { IDD = IDD_FORMVIEW };
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CAppForm)
    virtual void OnInitialUpdate();
    protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

public:
    //{{AFX_MSG(CAppForm)
    afx_msg void OnToggleFullScreen();
    afx_msg void OnChangeDevice();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
$$IF(ACTIONMAPPER)

    HRESULT InputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi );
    static HRESULT CALLBACK StaticInputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi, LPVOID pParam );   
    BOOL    ConfigureInputDevicesCB( IUnknown* pUnknown );
    static BOOL CALLBACK StaticConfigureInputDevicesCB( IUnknown* pUnknown, VOID* pUserData );
$$ENDIF
$$IF(DPLAY)

    static HRESULT WINAPI StaticDirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    HRESULT DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    static HRESULT WINAPI StaticDirectPlayLobbyMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    HRESULT DirectPlayLobbyMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
$$ENDIF
$$IF(DPLAYVOICE)

    static HRESULT WINAPI StaticDirectPlayVoiceServerMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    HRESULT DirectPlayVoiceServerMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    static HRESULT WINAPI StaticDirectPlayVoiceClientMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
    HRESULT DirectPlayVoiceClientMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
$$ENDIF
};




//-----------------------------------------------------------------------------
// Name: class CAppDoc
// Desc: Overridden CDocument class needed for the CFormView
//-----------------------------------------------------------------------------
class CAppDoc : public CDocument
{
protected:
    DECLARE_DYNCREATE(CAppDoc)

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppDoc)
    public:
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CAppDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CAppFrameWnd
// Desc: CFrameWnd-based class needed to override the CFormView's window style
//-----------------------------------------------------------------------------
class CAppFrameWnd : public CFrameWnd
{
protected:
    DECLARE_DYNCREATE(CAppFrameWnd)
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppFrameWnd)
    public:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAppFrameWnd)
    afx_msg void OnChangeDevice();
$$IF(ACTIONMAPPER)
    afx_msg void OnConfigInput();
$$ENDIF
$$IF(DPLAYVOICE)
    afx_msg void OnConfigVoice();
$$ENDIF
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CApp
// Desc: Main MFC application class derived from CWinApp.
//-----------------------------------------------------------------------------
class CApp : public CWinApp
{
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CApp)
    public:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle( LONG );
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




