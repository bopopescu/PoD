/************************************************************************/
/**
 * @file PARes.h
 * @brief Resource file of the PROOFAgent project.
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*

        version number:     $LastChangedRevision$
        created by:         Anar Manafov
                            2007-03-01
        last changed by:    $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2007 GSI GridTeam. All rights reserved.
*************************************************************************/
#ifndef PARES_H_
#define PARES_H_

// OUR
#include "def.h"

namespace PROOFAgent
{

    const MiscCommon::LPCTSTR g_szPROTOCOL_VERSION = _T("PAprotocol:0.1.0");
    const MiscCommon::LPCTSTR g_szRESPONSE_OK = _T("PA_OK");
    const MiscCommon::LPCTSTR g_szSEND_USERNAME = _T("PA_CLT_USER:");

    const MiscCommon::LPCTSTR g_SIGNAL_PIPE_PATH = _T("$POD_LOCATION/signal_pipe");

    const size_t g_nBUF_SIZE = 1024;

    typedef enum{ Unknown, Server, Client }EAgentMode_t;

};

#endif /*PARES_H_*/
