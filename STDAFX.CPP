// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void PASCAL CAsyncSocket::DoCallBack(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 0 && lParam == 0)
        return;

    // Has the socket been closed?
    CAsyncSocket* pSocket = CAsyncSocket::LookupHandle((SOCKET)wParam, TRUE);

    // If yes, ignore message.
    if (pSocket != NULL)
        return;

    pSocket = CAsyncSocket::LookupHandle((SOCKET)wParam, FALSE);
    if (pSocket == NULL)
    {
        // Must be in the middle of an Accept call.
        pSocket = CAsyncSocket::LookupHandle(INVALID_SOCKET, FALSE);
        ASSERT(pSocket != NULL);
        pSocket->m_hSocket = (SOCKET)wParam;
        CAsyncSocket::DetachHandle(INVALID_SOCKET, FALSE);
        CAsyncSocket::AttachHandle(pSocket->m_hSocket, pSocket, FALSE);
    }

    int nErrorCode = WSAGETSELECTERROR(lParam);
    switch (WSAGETSELECTEVENT(lParam))
    {
    case FD_READ:
        {   // Comment out the data peeking code here
            //DWORD nBytes;
            //if (!pSocket->IOCtl(FIONREAD, &nBytes))
            //  nErrorCode = WSAGetLastError();
            //if (nBytes != 0 || nErrorCode != 0)
                pSocket->OnReceive(nErrorCode);
        }
        break;
    case FD_WRITE:
        pSocket->OnSend(nErrorCode);
        break;
    case FD_OOB:
        pSocket->OnOutOfBandData(nErrorCode);
        break;
    case FD_ACCEPT:
        pSocket->OnAccept(nErrorCode);
        break;
    case FD_CONNECT:
        pSocket->OnConnect(nErrorCode);
        break;
    case FD_CLOSE:
        pSocket->OnClose(nErrorCode);
        break;
    }
}