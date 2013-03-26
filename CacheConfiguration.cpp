#include "StdAfx.h"
#include "CacheConfiguration.h"

extern HMODULE g_hCurrentInstance;

CCacheConfiguration::CCacheConfiguration()
{
    m_DefaultBehavior = CachingBehaviorIgnore;
    m_pExceptions = NULL;
    m_nExceptions = 0;
    m_dwSitePrefixLen = 0;
    m_szSitePrefix = NULL;
}

CCacheConfiguration::~CCacheConfiguration()
{
}

void CCacheConfiguration::Initialize(PWSTR szConfigFilePath)
{
	if (NULL != szConfigFilePath)
	{
		ParseConfigurationFile(szConfigFilePath);
		return;
	}

    // Compose configuration file path, should be the same as module file path except for extension.
    WCHAR szConfigFile[MAX_PATH];
    DWORD dwSize = GetModuleFileName(g_hCurrentInstance, szConfigFile, MAX_PATH);
    if (0 == dwSize)
        return;
    WCHAR* pExtension = StrRChr(szConfigFile, szConfigFile + dwSize, L'.');
    if (NULL == pExtension)
        return;
    pExtension++; // skip the '.' and point to 'dll'
    dwSize = dwSize - (DWORD)(pExtension - szConfigFile) + 1;
    HRESULT hr = StringCchCopy(pExtension, (size_t) dwSize, L"ini");
    if (FAILED(hr))
        return;

    ParseConfigurationFile(szConfigFile);
}

void CCacheConfiguration::ParseConfigurationFile(PCWSTR szFilePath)
{
    DWORD dwStringSize;
    WCHAR szBuf[MAX_PATH];
    HRESULT hr;

    // Read default behavior
    m_DefaultBehavior = (CachingBehavior)GetPrivateProfileInt(L"Global", L"Default", 0, szFilePath);

    // Read site prefix
    dwStringSize = GetPrivateProfileString(L"Global", L"SitePrefix", NULL, szBuf, MAX_PATH, szFilePath);
    if (dwStringSize > 0)
    {
        m_szSitePrefix = new WCHAR[dwStringSize + sizeof(WCHAR)];
        StringCchCopy(m_szSitePrefix, dwStringSize + sizeof(WCHAR), szBuf);
	    m_dwSitePrefixLen = dwStringSize;
    }

    // Read exceptions section
    dwStringSize = GetPrivateProfileSection(L"Exceptions", szBuf, MAX_PATH, szFilePath);
    if (dwStringSize < 1)
        return;

    for (DWORD index=0 ; index < dwStringSize ; index++)
	{
        if (L'\0' == szBuf[index])
            m_nExceptions++;
	}

	m_pExceptions = new CachingException[m_nExceptions];
	if (NULL == m_pExceptions)
	{
		m_nExceptions = 0;
		return;
	}

	int offset = 0;
	int len = 0;
	int exceptionIndex = 0;
	CachingException* pExp = &m_pExceptions[exceptionIndex];

	while (true)
	{
		if (L'\0' == szBuf[offset + len])
		{
			pExp->Behavior = (CachingBehavior)StrToInt((LPCWSTR)(szBuf + offset));

			if (L'\0' == szBuf[offset + len + 1])
				break;
			else
			{
				offset += len + 1;
				len = -1;
				pExp = &m_pExceptions[++exceptionIndex];
			}
		}
		else if (L'=' == szBuf[offset + len])
		{
			pExp->szUrlPart = new WCHAR[len + 2];
			pExp->nChars = len + 2;
			hr = StringCchCopy(pExp->szUrlPart, len + 1, szBuf + offset);
			if (STRSAFE_E_INSUFFICIENT_BUFFER != hr)
			{
				delete[] pExp->szUrlPart;
				pExp->szUrlPart = NULL;
				break;
			}
			offset += len + 1;
			len = -1;
		}
		len++;
	}
}

void CCacheConfiguration::Terminate()
{
	for (int index = 0; index < m_nExceptions; index++)
		delete[] m_pExceptions[index].szUrlPart;

	if (NULL != m_pExceptions)
		delete[] m_pExceptions;

    if (NULL != m_szSitePrefix)
        delete[] m_szSitePrefix;
}

CachingBehavior CCacheConfiguration::GetCachePolicy(PCWSTR szUrl)
{
    PCWSTR szOffset = szUrl;
    if (m_dwSitePrefixLen > 0)
        szOffset += m_dwSitePrefixLen - 1;

    for (USHORT index = 0; index < m_nExceptions ; index++)
    {
        if (0 == StrCmpNI(szOffset, m_pExceptions[index].szUrlPart, m_pExceptions[index].nChars - 2))
            return m_pExceptions[index].Behavior;
    }

	return m_DefaultBehavior;
}

BOOL CCacheConfiguration::UrlHasSitePrefix(PCWSTR szUrl)
{
    if (NULL == m_szSitePrefix)
        return TRUE;

    return 0 == StrCmpNI(szUrl, m_szSitePrefix, m_dwSitePrefixLen);
}