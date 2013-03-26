#include "stdafx.h"

#pragma once

/*
Lists caching behaviors that this module supports.
*/
enum CachingBehavior
{
    CachingBehaviorFirst = 0,

    // Indicates that request should be ignored by this module.
    CachingBehaviorIgnore = 0,

    // Indicates that ETag, Expires and Last-Modified headers should be removed and Cache-Control
    // header should be set to private.
    CachingBehaviorPrivate = 1,

    // Indicates that Expires and Last-Modified headers should be removed, ETag header should be
    // ignored and Cache-Control set to public.
    CachingBehaviorConditional = 2,

    // Indicates that ETag, Expires and Last-Modified headers should be removed and Cache-Control
    // set to public.
    CachingBehaviorPublic = 3,

    CachingBehaviorLast = 3
};

class CCacheConfiguration
{
public:
    CCacheConfiguration();
    ~CCacheConfiguration();

    void Initialize(PWSTR szConfigFilePath = NULL);
    void Terminate();
    BOOL UrlHasSitePrefix(PCWSTR szUrl);
    CachingBehavior GetCachePolicy(PCWSTR szUrl);

private:
    void ParseConfigurationFile(PCWSTR szFilePath);

    struct CachingException
    {
        PWSTR szUrlPart;
        USHORT nChars;
		CachingBehavior Behavior;
    };

    CachingBehavior m_DefaultBehavior;
    PWSTR m_szSitePrefix;
    DWORD m_dwSitePrefixLen;

    CachingException* m_pExceptions;
    USHORT m_nExceptions;
};