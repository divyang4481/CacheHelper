#include "stdafx.h"
#include "CacheConfiguration.h"

#pragma once

class CCacheHelperModule : public CHttpModule
{
public:
    CCacheHelperModule(CCacheConfiguration* pConfig);

    REQUEST_NOTIFICATION_STATUS OnSendResponse(IN IHttpContext * pHttpContext, 
                                               IN ISendResponseProvider * pProvider);

private:
    void MakeUncacheable(IHttpResponse* pHttpResponse)
    {
        pHttpResponse->DeleteHeader(HttpHeaderEtag);
        pHttpResponse->DeleteHeader(HttpHeaderLastModified);
        pHttpResponse->DeleteHeader(HttpHeaderExpires);

        pHttpResponse->SetHeader(HttpHeaderCacheControl,
                                 (PCSTR) "private, max-age=0", 18, TRUE);
    }

    void MakeConditionallyCacheable(IHttpResponse* pHttpResponse)
    {
        pHttpResponse->DeleteHeader(HttpHeaderLastModified);
        pHttpResponse->DeleteHeader(HttpHeaderExpires);

        pHttpResponse->SetHeader(HttpHeaderCacheControl,(PCSTR) "public", 6, TRUE);
    }

    void MakeCacheable(IHttpResponse* pHttpResponse)
    {
        pHttpResponse->DeleteHeader(HttpHeaderEtag);
        pHttpResponse->DeleteHeader(HttpHeaderLastModified);
        pHttpResponse->DeleteHeader(HttpHeaderExpires);

        pHttpResponse->SetHeader(HttpHeaderCacheControl,
                                 (PCSTR) "public, max-age=31536000", 24, TRUE);
    }

    CCacheConfiguration* m_pConfig;
};