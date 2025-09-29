#pragma once
#include <afxsock.h>
class CConnectSocket :
    public CSocket
{
public:
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
};
