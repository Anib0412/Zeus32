/* this ALWAYS GENERATED file contains the RPC server stubs */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Sep 21 13:48:02 2005
 */
/* Compiler settings for D:\Source\Zeus32\GisRPCInterface.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#include <string.h>
#include "GisRPCInterface.h"

#define TYPE_FORMAT_STRING_SIZE   423                               
#define PROC_FORMAT_STRING_SIZE   179                               

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;

extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;

/* Standard interface: __MIDL_itf_GisRPCInterface_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Standard interface: GISRPCInterface, ver. 1.0,
   GUID={0x91705248,0xc79c,0x4b6d,{0xb1,0x98,0xa7,0xf5,0xe1,0xf8,0x3b,0x47}} */


extern RPC_DISPATCH_TABLE GISRPCInterface_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE GISRPCInterface___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x91705248,0xc79c,0x4b6d,{0xb1,0x98,0xa7,0xf5,0xe1,0xf8,0x3b,0x47}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &GISRPCInterface_v1_0_DispatchTable,
    0,
    0,
    0,
    0,
    0
    };
RPC_IF_HANDLE GISRPCInterface_v1_0_s_ifspec = (RPC_IF_HANDLE)& GISRPCInterface___RpcServerInterface;

extern const MIDL_STUB_DESC GISRPCInterface_StubDesc;

void __RPC_STUB
GISRPCInterface_GISRPCGetServerVersion(
    PRPC_MESSAGE _pRpcMessage )
{
    int _M2;
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iVersion;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iVersion = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        iVersion = &_M2;
        
        _RetVal = GISRPCGetServerVersion(iVersion);
        
        _StubMsg.BufferLength = 4U + 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( int __RPC_FAR * )_StubMsg.Buffer)++ = *iVersion;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetNumDevices(
    PRPC_MESSAGE _pRpcMessage )
{
    int _M3;
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iNumDevices;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iNumDevices = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        iNumDevices = &_M3;
        
        _RetVal = GISRPCGetNumDevices(iNumDevices);
        
        _StubMsg.BufferLength = 4U + 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( int __RPC_FAR * )_StubMsg.Buffer)++ = *iNumDevices;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetDevInfo(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    GISRPC_DEVINFO _pDevInfoM;
    int __RPC_FAR *iIdx;
    GISRPC_DEVINFO __RPC_FAR *pDevInfo;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iIdx = 0;
    ( GISRPC_DEVINFO __RPC_FAR * )pDevInfo = 0;
    ( PCONTEXT_DOC_HANDLE __RPC_FAR * )phDevice = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[6] );
            
            iIdx = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        pDevInfo = &_pDevInfoM;
        pDevInfo -> hDevice = 0;
        phDevice = NDRSContextUnmarshall( (char *)0, _pRpcMessage->DataRepresentation ); 
        
        
        _RetVal = GISRPCGetDevInfo(
                           iIdx,
                           pDevInfo,
                           ( PCONTEXT_DOC_HANDLE __RPC_FAR * )NDRSContextValue(phDevice));
        
        _StubMsg.BufferLength = 0U + 27U + 7U;
        NdrComplexStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                    (unsigned char __RPC_FAR *)pDevInfo,
                                    (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrComplexStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                  (unsigned char __RPC_FAR *)pDevInfo,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        NdrServerContextMarshall(
                            ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                            ( NDR_SCONTEXT  )phDevice,
                            ( NDR_RUNDOWN  )PCONTEXT_DOC_HANDLE_rundown);
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pDevInfo,
                        &__MIDL_TypeFormatString.Format[10] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetCanvasInfo(
    PRPC_MESSAGE _pRpcMessage )
{
    GISRPC_CANVAS __RPC_FAR *GISCanvas;
    GISRPC_CANVAS _GISCanvasM;
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iIdx;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iIdx = 0;
    ( GISRPC_CANVAS __RPC_FAR * )GISCanvas = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iIdx = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        GISCanvas = &_GISCanvasM;
        
        _RetVal = GISRPCGetCanvasInfo(
                              ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                              iIdx,
                              GISCanvas);
        
        _StubMsg.BufferLength = 0U + 11U;
        NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char __RPC_FAR *)GISCanvas,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[78] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char __RPC_FAR *)GISCanvas,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[78] );
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCSetBackPrintLine(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iLine;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iLine = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[34] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iLine = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCSetBackPrintLine(( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),iLine);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetDevOptionName(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    GISRPC_DEVSTRING _pOptionNameM;
    int __RPC_FAR *iOption;
    GISRPC_DEVSTRING __RPC_FAR *pOptionName;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iOption = 0;
    ( GISRPC_DEVSTRING __RPC_FAR * )pOptionName = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[44] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iOption = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        pOptionName = &_pOptionNameM;
        
        _RetVal = GISRPCGetDevOptionName(
                                 ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                                 iOption,
                                 pOptionName);
        
        _StubMsg.BufferLength = 0U + 11U;
        NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char __RPC_FAR *)pOptionName,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char __RPC_FAR *)pOptionName,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetDevOptionSettingName(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    GISRPC_DEVSTRING _pOptionNameM;
    int __RPC_FAR *iOption;
    int __RPC_FAR *iSetting;
    GISRPC_DEVSTRING __RPC_FAR *pOptionName;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iOption = 0;
    ( int __RPC_FAR * )iSetting = 0;
    ( GISRPC_DEVSTRING __RPC_FAR * )pOptionName = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[58] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iOption = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            iSetting = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        pOptionName = &_pOptionNameM;
        
        _RetVal = GISRPCGetDevOptionSettingName(
                                        ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                                        iOption,
                                        iSetting,
                                        pOptionName);
        
        _StubMsg.BufferLength = 0U + 11U;
        NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char __RPC_FAR *)pOptionName,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char __RPC_FAR *)pOptionName,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCGetDevOption(
    PRPC_MESSAGE _pRpcMessage )
{
    int _M4;
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iOption;
    int __RPC_FAR *iSetting;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iOption = 0;
    ( int __RPC_FAR * )iSetting = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[76] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iOption = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        iSetting = &_M4;
        
        _RetVal = GISRPCGetDevOption(
                             ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                             iOption,
                             iSetting);
        
        _StubMsg.BufferLength = 4U + 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( int __RPC_FAR * )_StubMsg.Buffer)++ = *iSetting;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCSetDevOption(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int __RPC_FAR *iOption;
    int __RPC_FAR *iSetting;
    NDR_SCONTEXT phDevice;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( int __RPC_FAR * )iOption = 0;
    ( int __RPC_FAR * )iSetting = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[90] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            iOption = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            iSetting = ( int __RPC_FAR * )_StubMsg.Buffer;
            _StubMsg.Buffer += sizeof( int  );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCSetDevOption(
                             ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                             iOption,
                             iSetting);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCOpenJob(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    unsigned char __RPC_FAR *jobName;
    NDR_SCONTEXT phDevice;
    NDR_SCONTEXT phJob;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( unsigned char __RPC_FAR * )jobName = 0;
    ( PCONTEXT_JOB_HANDLE __RPC_FAR * )phJob = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[104] );
            
            phDevice = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrConformantStringUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char __RPC_FAR * __RPC_FAR *)&jobName,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[110],
                                           (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        phJob = NDRSContextUnmarshall( (char *)0, _pRpcMessage->DataRepresentation ); 
        
        
        _RetVal = GISRPCOpenJob(
                        ( PCONTEXT_DOC_HANDLE  )*NDRSContextValue(phDevice),
                        jobName,
                        ( PCONTEXT_JOB_HANDLE __RPC_FAR * )NDRSContextValue(phJob));
        
        _StubMsg.BufferLength = 20U + 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrServerContextMarshall(
                            ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                            ( NDR_SCONTEXT  )phJob,
                            ( NDR_RUNDOWN  )PCONTEXT_JOB_HANDLE_rundown);
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCCloseJob(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NDR_SCONTEXT hJob;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[118] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCCloseJob(( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob));
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCPrintJob(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NDR_SCONTEXT hJob;
    GISRPC_PRINTSPECS __RPC_FAR *pPrintSpecs;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( GISRPC_PRINTSPECS __RPC_FAR * )pPrintSpecs = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[124] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char __RPC_FAR * __RPC_FAR *)&pPrintSpecs,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[128],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCPrintJob(( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob),pPrintSpecs);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pPrintSpecs,
                        &__MIDL_TypeFormatString.Format[124] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCAddJobSeparator(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NDR_SCONTEXT hJob;
    GISRPC_JOBSEPARATOR __RPC_FAR *pJobSepinfo;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( GISRPC_JOBSEPARATOR __RPC_FAR * )pJobSepinfo = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[134] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char __RPC_FAR * __RPC_FAR *)&pJobSepinfo,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[182],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCAddJobSeparator(( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob),pJobSepinfo);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pJobSepinfo,
                        &__MIDL_TypeFormatString.Format[152] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCPutFile(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NDR_SCONTEXT hJob;
    GISRPC_SENDFILEINFO __RPC_FAR *pSendFileInfo;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( GISRPC_SENDFILEINFO __RPC_FAR * )pSendFileInfo = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[144] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char __RPC_FAR * __RPC_FAR *)&pSendFileInfo,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[252],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCPutFile(( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob),pSendFileInfo);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pSendFileInfo,
                        &__MIDL_TypeFormatString.Format[248] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCAddPhoto(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    GISRPC_DEVSTRING _pOutFileNameM;
    NDR_SCONTEXT hJob;
    GISRPC_DEVSTRING __RPC_FAR *pOutFileName;
    GISRPC_PHOTOSPECS __RPC_FAR *pPhotoSpecs;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( GISRPC_PHOTOSPECS __RPC_FAR * )pPhotoSpecs = 0;
    ( GISRPC_DEVSTRING __RPC_FAR * )pOutFileName = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[154] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char __RPC_FAR * __RPC_FAR *)&pPhotoSpecs,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[328],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        pOutFileName = &_pOutFileNameM;
        
        _RetVal = GISRPCAddPhoto(
                         ( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob),
                         pPhotoSpecs,
                         pOutFileName);
        
        _StubMsg.BufferLength = 0U + 11U;
        NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char __RPC_FAR *)pOutFileName,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char __RPC_FAR *)pOutFileName,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[98] );
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pPhotoSpecs,
                        &__MIDL_TypeFormatString.Format[280] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
GISRPCInterface_GISRPCAddPhoto2(
    PRPC_MESSAGE _pRpcMessage )
{
    long _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NDR_SCONTEXT hJob;
    GISRPC_PHOTOSPECS2 __RPC_FAR *pPhotoSpecs;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &GISRPCInterface_StubDesc);
    
    ( GISRPC_PHOTOSPECS2 __RPC_FAR * )pPhotoSpecs = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[168] );
            
            hJob = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char __RPC_FAR * __RPC_FAR *)&pPhotoSpecs,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[376],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = GISRPCAddPhoto2(( PCONTEXT_JOB_HANDLE  )*NDRSContextValue(hJob),pPhotoSpecs);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( long __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)pPhotoSpecs,
                        &__MIDL_TypeFormatString.Format[372] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}


static const MIDL_STUB_DESC GISRPCInterface_StubDesc = 
    {
    (void __RPC_FAR *)& GISRPCInterface___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static RPC_DISPATCH_FUNCTION GISRPCInterface_table[] =
    {
    GISRPCInterface_GISRPCGetServerVersion,
    GISRPCInterface_GISRPCGetNumDevices,
    GISRPCInterface_GISRPCGetDevInfo,
    GISRPCInterface_GISRPCGetCanvasInfo,
    GISRPCInterface_GISRPCSetBackPrintLine,
    GISRPCInterface_GISRPCGetDevOptionName,
    GISRPCInterface_GISRPCGetDevOptionSettingName,
    GISRPCInterface_GISRPCGetDevOption,
    GISRPCInterface_GISRPCSetDevOption,
    GISRPCInterface_GISRPCOpenJob,
    GISRPCInterface_GISRPCCloseJob,
    GISRPCInterface_GISRPCPrintJob,
    GISRPCInterface_GISRPCAddJobSeparator,
    GISRPCInterface_GISRPCPutFile,
    GISRPCInterface_GISRPCAddPhoto,
    GISRPCInterface_GISRPCAddPhoto2,
    0
    };
RPC_DISPATCH_TABLE GISRPCInterface_v1_0_DispatchTable = 
    {
    16,
    GISRPCInterface_table
    };

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/*  2 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/*  4 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  6 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/*  8 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 10 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 12 */	NdrFcShort( 0xa ),	/* Type Offset=10 */
/* 14 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 16 */	NdrFcShort( 0x3e ),	/* Type Offset=62 */
/* 18 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 20 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 22 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 24 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 26 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 28 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 30 */	NdrFcShort( 0x4a ),	/* Type Offset=74 */
/* 32 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 34 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 36 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 38 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 40 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 42 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 44 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 46 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 48 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 50 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 52 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 54 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */
/* 56 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 58 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 60 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 62 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 64 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 66 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 68 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 70 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 72 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */
/* 74 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 76 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 78 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 80 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 82 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 84 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 86 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 88 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 90 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 92 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 94 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 96 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 98 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 100 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 102 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 104 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 106 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 108 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 110 */	NdrFcShort( 0x6c ),	/* Type Offset=108 */
/* 112 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 114 */	NdrFcShort( 0x70 ),	/* Type Offset=112 */
/* 116 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 118 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 120 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 122 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 124 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 126 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 128 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 130 */	NdrFcShort( 0x7c ),	/* Type Offset=124 */
/* 132 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 134 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 136 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 138 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 140 */	NdrFcShort( 0x98 ),	/* Type Offset=152 */
/* 142 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 144 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 146 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 148 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 150 */	NdrFcShort( 0xf8 ),	/* Type Offset=248 */
/* 152 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 154 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 156 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 158 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 160 */	NdrFcShort( 0x118 ),	/* Type Offset=280 */
/* 162 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 164 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */
/* 166 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 168 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 170 */	NdrFcShort( 0x78 ),	/* Type Offset=120 */
/* 172 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 174 */	NdrFcShort( 0x174 ),	/* Type Offset=372 */
/* 176 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  8 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 12 */	NdrFcShort( 0x8 ),	/* Offset= 8 (20) */
/* 14 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 16 */	NdrFcShort( 0x100 ),	/* 256 */
/* 18 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 20 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 22 */	NdrFcShort( 0x430 ),	/* 1072 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x20 ),	/* Offset= 32 (58) */
/* 28 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 30 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffef ),	/* Offset= -17 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 34 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffeb ),	/* Offset= -21 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 38 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe7 ),	/* Offset= -25 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 42 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe3 ),	/* Offset= -29 (14) */
			0x8,		/* FC_LONG */
/* 46 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 48 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 50 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 52 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 54 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 56 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 58 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 60 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 62 */	
			0x11, 0x0,	/* FC_RP */
/* 64 */	NdrFcShort( 0x2 ),	/* Offset= 2 (66) */
/* 66 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 68 */	0x0,		/* 0 */
			0x2,		/* 2 */
/* 70 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 72 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 74 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 80 */	NdrFcShort( 0x20c ),	/* 524 */
/* 82 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 84 */	NdrFcShort( 0xffffffba ),	/* Offset= -70 (14) */
/* 86 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 88 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (14) */
/* 90 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 92 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 94 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 96 */	NdrFcShort( 0x2 ),	/* Offset= 2 (98) */
/* 98 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 100 */	NdrFcShort( 0x100 ),	/* 256 */
/* 102 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 104 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (14) */
/* 106 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 108 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 110 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 112 */	
			0x11, 0x0,	/* FC_RP */
/* 114 */	NdrFcShort( 0x2 ),	/* Offset= 2 (116) */
/* 116 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 118 */	0x1,		/* 1 */
			0x2,		/* 2 */
/* 120 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 122 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 124 */	
			0x11, 0x0,	/* FC_RP */
/* 126 */	NdrFcShort( 0x2 ),	/* Offset= 2 (128) */
/* 128 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 130 */	NdrFcShort( 0x21c ),	/* 540 */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* Offset= 0 (134) */
/* 136 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 138 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 140 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 142 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 144 */	NdrFcShort( 0xffffff7e ),	/* Offset= -130 (14) */
/* 146 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 148 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff79 ),	/* Offset= -135 (14) */
			0x5b,		/* FC_END */
/* 152 */	
			0x11, 0x0,	/* FC_RP */
/* 154 */	NdrFcShort( 0x1c ),	/* Offset= 28 (182) */
/* 156 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 158 */	NdrFcShort( 0x600 ),	/* 1536 */
/* 160 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 162 */	NdrFcShort( 0xffffff6c ),	/* Offset= -148 (14) */
/* 164 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 166 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 168 */	NdrFcShort( 0x1400 ),	/* 5120 */
/* 170 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 172 */	NdrFcShort( 0xffffff62 ),	/* Offset= -158 (14) */
/* 174 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 176 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 178 */	NdrFcShort( 0xc ),	/* 12 */
/* 180 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 182 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 184 */	NdrFcShort( 0x1f70 ),	/* 8048 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x0 ),	/* Offset= 0 (188) */
/* 190 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 192 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 194 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 196 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 198 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 200 */	NdrFcShort( 0xffffff46 ),	/* Offset= -186 (14) */
/* 202 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 204 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (156) */
/* 206 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 208 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (166) */
/* 210 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 212 */	NdrFcShort( 0xffffff3a ),	/* Offset= -198 (14) */
/* 214 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 216 */	NdrFcShort( 0xffffff36 ),	/* Offset= -202 (14) */
/* 218 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 220 */	NdrFcShort( 0xffffff32 ),	/* Offset= -206 (14) */
/* 222 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 224 */	NdrFcShort( 0xffffff2e ),	/* Offset= -210 (14) */
/* 226 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 228 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (176) */
/* 230 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 232 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 234 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 236 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 238 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 240 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 242 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 244 */	0x8,		/* FC_LONG */
			0xa,		/* FC_FLOAT */
/* 246 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 248 */	
			0x11, 0x0,	/* FC_RP */
/* 250 */	NdrFcShort( 0x2 ),	/* Offset= 2 (252) */
/* 252 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 254 */	NdrFcShort( 0x31c ),	/* 796 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* Offset= 0 (258) */
/* 260 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 262 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff07 ),	/* Offset= -249 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 266 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff03 ),	/* Offset= -253 (14) */
			0x8,		/* FC_LONG */
/* 270 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 272 */	NdrFcShort( 0xfffffefe ),	/* Offset= -258 (14) */
/* 274 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 276 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 278 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 280 */	
			0x11, 0x0,	/* FC_RP */
/* 282 */	NdrFcShort( 0x2e ),	/* Offset= 46 (328) */
/* 284 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 286 */	NdrFcShort( 0x5 ),	/* 5 */
/* 288 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 292 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 296 */	0xd,		/* FC_ENUM16 */
			0x5b,		/* FC_END */
/* 298 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 300 */	NdrFcShort( 0x14 ),	/* 20 */
/* 302 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 304 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 306 */	NdrFcShort( 0x2c ),	/* 44 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x0 ),	/* Offset= 0 (310) */
/* 312 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 314 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffe1 ),	/* Offset= -31 (284) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 318 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffeb ),	/* Offset= -21 (298) */
			0x5b,		/* FC_END */
/* 322 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 324 */	NdrFcShort( 0x7d00 ),	/* 32000 */
/* 326 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 328 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 330 */	NdrFcShort( 0x8064 ),	/* -32668 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x0 ),	/* Offset= 0 (334) */
/* 336 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 338 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffebb ),	/* Offset= -325 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 342 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffeb7 ),	/* Offset= -329 (14) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 346 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd5 ),	/* Offset= -43 (304) */
			0x8,		/* FC_LONG */
/* 350 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 352 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffead ),	/* Offset= -339 (14) */
			0x8,		/* FC_LONG */
/* 356 */	0x8,		/* FC_LONG */
			0xd,		/* FC_ENUM16 */
/* 358 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 360 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 362 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 364 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 366 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd3 ),	/* Offset= -45 (322) */
			0x8,		/* FC_LONG */
/* 370 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 372 */	
			0x11, 0x0,	/* FC_RP */
/* 374 */	NdrFcShort( 0x2 ),	/* Offset= 2 (376) */
/* 376 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 378 */	NdrFcShort( 0x8774 ),	/* -30860 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* Offset= 0 (382) */
/* 384 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 386 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (328) */
/* 388 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 390 */	NdrFcShort( 0xfffffe88 ),	/* Offset= -376 (14) */
/* 392 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 394 */	NdrFcShort( 0xfffffe84 ),	/* Offset= -380 (14) */
/* 396 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 398 */	NdrFcShort( 0xfffffe80 ),	/* Offset= -384 (14) */
/* 400 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 402 */	NdrFcShort( 0xfffffe7c ),	/* Offset= -388 (14) */
/* 404 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 406 */	NdrFcShort( 0xfffffe78 ),	/* Offset= -392 (14) */
/* 408 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 410 */	NdrFcShort( 0xfffffe74 ),	/* Offset= -396 (14) */
/* 412 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 414 */	NdrFcShort( 0xfffffe70 ),	/* Offset= -400 (14) */
/* 416 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 418 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 420 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */

			0x0
        }
    };
