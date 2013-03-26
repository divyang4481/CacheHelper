#include "StdAfx.h"
#include <httptrace.h>
#include "CacheHelperModule.h"
#include "CacheConfiguration.h"

CCacheHelperModule::CCacheHelperModule(CCacheConfiguration* pConfig)
{
    m_pConfig = pConfig;
}

REQUEST_NOTIFICATION_STATUS CCacheHelperModule::OnSendResponse(
    IN IHttpContext* pHttpContext, IN ISendResponseProvider* pProvider)
{
    UNREFERENCED_PARAMETER(pProvider);

    IHttpResponse* pHttpResponse = pHttpContext->GetResponse();
    IHttpRequest* pHttpRequest = pHttpContext->GetRequest();

    if ((pHttpResponse != NULL) && (pHttpRequest != NULL))
    {
        HTTP_COOKED_URL url = pHttpRequest->GetRawHttpRequest()->CookedUrl;

        if (!m_pConfig->UrlHasSitePrefix(url.pAbsPath))
            return RQ_NOTIFICATION_CONTINUE;

        CachingBehavior behavior = CachingBehaviorIgnore;

        USHORT cchXCache;
        PCSTR szXCache = pHttpResponse->GetHeader((PCSTR)"X-Cache", &cchXCache);
        if (cchXCache > 0)
        {
            behavior = (CachingBehavior)StrToIntA(szXCache);
            pHttpResponse->DeleteHeader((PCSTR)"X-Cache");
        }
        else
        {
            behavior = m_pConfig->GetCachePolicy(url.pAbsPath);
        }
        
        switch (behavior)
        {
        case CachingBehaviorPrivate:
            MakeUncacheable(pHttpResponse);
            break;
        case CachingBehaviorConditional:
            MakeConditionallyCacheable(pHttpResponse);
            break;
        case CachingBehaviorPublic:
            MakeCacheable(pHttpResponse);
            break;
        }
    }

    return RQ_NOTIFICATION_CONTINUE;
}