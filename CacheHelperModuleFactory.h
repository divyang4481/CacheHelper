#include "stdafx.h"
#include "CacheHelperModule.h"

#pragma once

class CCacheHelperModuleFactory : public IHttpModuleFactory
{
public:
    CCacheHelperModuleFactory() { m_pConfig = NULL; }

    HRESULT GetHttpModule(OUT CHttpModule** ppModule, IN IModuleAllocator* pAllocator)
    {
        UNREFERENCED_PARAMETER(pAllocator);

        if (NULL == m_pConfig)
        {
            m_pConfig = new CCacheConfiguration();
            m_pConfig->Initialize();
        }

        CCacheHelperModule * pModule = new CCacheHelperModule(m_pConfig);

        if (!pModule)
            return HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY );
        else
        {
            *ppModule = pModule;
            pModule = NULL;

            return S_OK;
        }            
    }

    void Terminate()
    {
        if (NULL != m_pConfig)
            delete m_pConfig;
        delete this;
    }

private:
    CCacheConfiguration* m_pConfig;
};