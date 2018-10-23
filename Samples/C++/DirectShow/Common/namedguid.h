//------------------------------------------------------------------------------
// File: NamedGuid.h
//
// Desc: DirectShow sample code - helps in converting GUIDs to strings
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef NAMEDGUID_H_DEFINED
#define NAMEDGUID_H_DEFINED

// Function prototypes
void GetGUIDString(TCHAR *szString, GUID *pGUID);
void GetFormatString(TCHAR *szFormat, DMO_MEDIA_TYPE *pType, int nFormatSize);
void GetTypeSubtypeString(TCHAR *szCLSID, DMO_PARTIAL_MEDIATYPE& aList);

struct NamedGuid
{
    const GUID *pguid;
    const TCHAR *psz;
};

extern const NamedGuid rgng[];

//
// Define some Windows Media GUIDs to remove need for large WM header files.
// This is OK since GUIDs don't change.
//

// 73646976-0000-0010-8000-00AA00389B71  'vids' == WMMEDIATYPE_Video 
EXTERN_GUID(WMMEDIATYPE_Video, 
0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 73647561-0000-0010-8000-00AA00389B71  'auds' == WMMEDIATYPE_Audio 
EXTERN_GUID(WMMEDIATYPE_Audio, 
0x73647561, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 73636d64-0000-0010-8000-00AA00389B71  'scmd' == MEDIATYPE_Script 
EXTERN_GUID(WMMEDIATYPE_Script, 
0x73636d64, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 34A50FD8-8AA5-4386-81FE-A0EFE0488E31            WMMEDIATYPE_Image 
EXTERN_GUID(WMMEDIATYPE_Image, 
0x34a50fd8, 0x8aa5, 0x4386, 0x81, 0xfe, 0xa0, 0xef, 0xe0, 0x48, 0x8e, 0x31); 
// D9E47579-930E-4427-ADFC-AD80F290E470  'fxfr' == WMMEDIATYPE_FileTransfer 
EXTERN_GUID(WMMEDIATYPE_FileTransfer, 
0xd9e47579, 0x930e, 0x4427, 0xad, 0xfc, 0xad, 0x80, 0xf2, 0x90, 0xe4, 0x70); 
// 9BBA1EA7-5AB2-4829-BA57-0940209BCF3E      'text' == WMMEDIATYPE_Text 
EXTERN_GUID(WMMEDIATYPE_Text, 
0x9bba1ea7, 0x5ab2, 0x4829, 0xba, 0x57, 0x9, 0x40, 0x20, 0x9b, 0xcf, 0x3e); 

// 00000000-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_Base 
EXTERN_GUID(WMMEDIASUBTYPE_Base, 
0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// e436eb78-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB1 
EXTERN_GUID(WMMEDIASUBTYPE_RGB1, 
0xe436eb78, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb79-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB4 
EXTERN_GUID(WMMEDIASUBTYPE_RGB4, 
0xe436eb79, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb7a-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB8 
EXTERN_GUID(WMMEDIASUBTYPE_RGB8, 
0xe436eb7a, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb7b-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB565 
EXTERN_GUID(WMMEDIASUBTYPE_RGB565, 
0xe436eb7b, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb7c-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB555 
EXTERN_GUID(WMMEDIASUBTYPE_RGB555, 
0xe436eb7c, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb7d-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB24 
EXTERN_GUID(WMMEDIASUBTYPE_RGB24, 
0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// e436eb7e-524f-11ce-9f53-0020af0ba770            MEDIASUBTYPE_RGB32 
EXTERN_GUID(WMMEDIASUBTYPE_RGB32, 
0xe436eb7e, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70); 
// 30323449-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_I420 
EXTERN_GUID(WMMEDIASUBTYPE_I420, 
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 56555949-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_IYUV 
EXTERN_GUID(WMMEDIASUBTYPE_IYUV, 
0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 31313259-0000-0010-8000-00AA00389B71  'YV12' ==  MEDIASUBTYPE_YV12 
EXTERN_GUID(WMMEDIASUBTYPE_YV12, 
0x32315659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 32595559-0000-0010-8000-00AA00389B71  'YUY2' == MEDIASUBTYPE_YUY2 
EXTERN_GUID(WMMEDIASUBTYPE_YUY2, 
0x32595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 59565955-0000-0010-8000-00AA00389B71  'UYVY' ==  MEDIASUBTYPE_UYVY 
EXTERN_GUID(WMMEDIASUBTYPE_UYVY, 
0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 55595659-0000-0010-8000-00AA00389B71  'YVYU' == MEDIASUBTYPE_YVYU 
EXTERN_GUID(WMMEDIASUBTYPE_YVYU, 
0x55595659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 39555659-0000-0010-8000-00AA00389B71  'YVU9' == MEDIASUBTYPE_YVU9 
EXTERN_GUID(WMMEDIASUBTYPE_YVU9, 
0x39555659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 
// 3334504D-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_MP43 
EXTERN_GUID(WMMEDIASUBTYPE_MP43, 
0x3334504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 5334504D-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_MP4S 
EXTERN_GUID(WMMEDIASUBTYPE_MP4S, 
0x5334504D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 31564D57-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMV1 
EXTERN_GUID(WMMEDIASUBTYPE_WMV1, 
0x31564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 32564D57-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMV2 
EXTERN_GUID(WMMEDIASUBTYPE_WMV2, 
0x32564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 3153534D-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_MSS1 
EXTERN_GUID(WMMEDIASUBTYPE_MSS1, 
0x3153534D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// e06d8026-db46-11cf-b4d1-00805f6cbbea            WMMEDIASUBTYPE_MPEG2_VIDEO 
EXTERN_GUID(WMMEDIASUBTYPE_MPEG2_VIDEO, 
0xe06d8026, 0xdb46, 0x11cf, 0xb4, 0xd1, 0x00, 0x80, 0x5f, 0x6c, 0xbb, 0xea); 
// 00000001-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_PCM 
EXTERN_GUID(WMMEDIASUBTYPE_PCM, 
0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000009-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_DRM 
EXTERN_GUID(WMMEDIASUBTYPE_DRM, 
0x00000009, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000162-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMAudioV9 
EXTERN_GUID(WMMEDIASUBTYPE_WMAudioV9, 
0x00000162, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000163-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMAudio_Lossless 
EXTERN_GUID(WMMEDIASUBTYPE_WMAudio_Lossless, 
0x00000163, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 3253534D-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_MSS2 
EXTERN_GUID(WMMEDIASUBTYPE_MSS2, 
0x3253534D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 0000000A-0000-0010-8000-00AA00389B71        WMMEDIASUBTYPE_WMSP1 
EXTERN_GUID( WMMEDIASUBTYPE_WMSP1, 
0x0000000A,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71); 
// 33564D57-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMV3 
EXTERN_GUID(WMMEDIASUBTYPE_WMV3, 
0x33564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000161-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMAudioV8 
EXTERN_GUID(WMMEDIASUBTYPE_WMAudioV8, 
0x00000161, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000161-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMAudioV7 
EXTERN_GUID(WMMEDIASUBTYPE_WMAudioV7, 
0x00000161, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000161-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_WMAudioV2 
EXTERN_GUID(WMMEDIASUBTYPE_WMAudioV2, 
0x00000161, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000130-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_ACELPnet 
EXTERN_GUID(WMMEDIASUBTYPE_ACELPnet, 
0x00000130, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 00000050-0000-0010-8000-00AA00389B71            WMMEDIASUBTYPE_MP3 
EXTERN_GUID(WMMEDIASUBTYPE_MP3, 
0x00000055, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71); 
// 776257d4-c627-41cb-8f81-7ac7ff1c40cc            WMMEDIASUBTYPE_WebStream 
EXTERN_GUID(WMMEDIASUBTYPE_WebStream, 
0x776257d4, 0xc627, 0x41cb, 0x8f, 0x81, 0x7a, 0xc7, 0xff, 0x1c, 0x40, 0xcc); 

// 05589f80-c356-11ce-bf01-00aa0055595a        WMFORMAT_VideoInfo 
EXTERN_GUID(WMFORMAT_VideoInfo, 
0x05589f80, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a); 
// 05589f81-c356-11ce-bf01-00aa0055595a        WMFORMAT_WaveFormatEx 
EXTERN_GUID(WMFORMAT_WaveFormatEx, 
0x05589f81, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a); 
// 5C8510F2-DEBE-4ca7-BBA5-F07A104F8DFF        WMFORMAT_Script 
EXTERN_GUID(WMFORMAT_Script, 
0x5c8510f2, 0xdebe, 0x4ca7, 0xbb, 0xa5, 0xf0, 0x7a, 0x10, 0x4f, 0x8d, 0xff); 

// 82f38a70-c29f-11d1-97ad-00a0c95ea850        WMSCRIPTTYPE_TwoStrings 
EXTERN_GUID( WMSCRIPTTYPE_TwoStrings, 
0x82f38a70,0xc29f,0x11d1,0x97,0xad,0x00,0xa0,0xc9,0x5e,0xa8,0x50); 

// e06d80e3-db46-11cf-b4d1-00805f6cbbea        WMFORMAT_MPEG2Video 
EXTERN_GUID(WMFORMAT_MPEG2Video, 
0xe06d80e3, 0xdb46, 0x11cf, 0xb4, 0xd1, 0x00, 0x80, 0x05f, 0x6c, 0xbb, 0xea); 

EXTERN_GUID( CLSID_WMMUTEX_Language, 0xD6E22A00,0x35DA,0x11D1,0x90,0x34,0x00,0xA0,0xC9,0x03,0x49,0xBE );
EXTERN_GUID( CLSID_WMMUTEX_Bitrate, 0xD6E22A01,0x35DA,0x11D1,0x90,0x34,0x00,0xA0,0xC9,0x03,0x49,0xBE );
EXTERN_GUID( CLSID_WMMUTEX_Presentation, 0xD6E22A02,0x35DA,0x11D1,0x90,0x34,0x00,0xA0,0xC9,0x03,0x49,0xBE );
EXTERN_GUID( CLSID_WMMUTEX_Unknown, 0xD6E22A03,0x35DA,0x11D1,0x90,0x34,0x00,0xA0,0xC9,0x03,0x49,0xBE );
EXTERN_GUID( CLSID_WMBandwidthSharing_Exclusive, 0xaf6060aa,0x5197,0x11d2,0xb6,0xaf,0x00,0xc0,0x4f,0xd9,0x08,0xe9 );
EXTERN_GUID( CLSID_WMBandwidthSharing_Partial, 0xaf6060ab,0x5197,0x11d2,0xb6,0xaf,0x00,0xc0,0x4f,0xd9,0x08,0xe9 );



#endif