/************************************************************************/
/**
 * @file AgentServer.cpp
 * @brief Packet Forwarder's implementation
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*

        version number:     $LastChangedRevision$
        created by:         Anar Manafov
                            2009-10-02
        last changed by:    $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2009 GSI GridTeam. All rights reserved.
*************************************************************************/
// BOOST
#include <boost/thread/mutex.hpp>
// MiscCommon
#include "ErrorCode.h"
#include "INet.h"
// PROOFAgent
#include "AgentServer.h"
//=============================================================================
using namespace std;
using namespace MiscCommon;
namespace inet = MiscCommon::INet;
//=============================================================================
const size_t g_READ_READY_INTERVAL = 4;
extern sig_atomic_t graceful_quit;
//=============================================================================
namespace PROOFAgent
{

//=============================================================================
    CAgentServer::CAgentServer( const SOptions_t &_data ): CAgentBase( _data.m_podOptions.m_server.m_common )
    {
        m_Data = _data.m_podOptions.m_server;
        m_serverInfoFile = _data.m_serverInfoFile;

        //InfoLog( MiscCommon::erOK, "Agent Server configuration:" ) << m_Data;
    }

//=============================================================================
    CAgentServer::~CAgentServer()
    {
        deleteServerInfoFile();
    }

//=============================================================================
    void CAgentServer::AddPF( inet::Socket_t _ClientSocket,
                              unsigned short _nNewLocalPort,
                              const string &_sPROOFCfgString )
    {
        boost::mutex::scoped_lock lock( m_PFList_mutex );
        m_PFList.add( _ClientSocket, _nNewLocalPort, _sPROOFCfgString );
    }

//=============================================================================
    void CAgentServer::CleanDisconnectsPF( const string &_sPROOFCfg )
    {
        m_PFList.clean_disconnects( _sPROOFCfg );
    }

//=============================================================================
//------------------------- Agent SERVER ------------------------------------------------------------
    void CAgentServer::ThreadWorker()
    {
        DebugLog( erOK, "Creating a PROOF configuration file..." );
        CreatePROOFCfg( m_commonOptions.m_proofCFG );
        try
        {
            readServerInfoFile( m_serverInfoFile );

            inet::CSocketServer server;
            server.Bind( m_agentServerListenPort );
            server.Listen( 100 ); // TODO: Move this number of queued clients to config
            server.GetSocket().set_nonblock(); // Nonblocking server socket

            // Add main server's socket to the list of sockets to select
            m_socksToSelect.insert( server.GetSocket().get() );
            while ( true )
            {
                // TODO: we need to check real PROOF port here (from cfg)
                if ( !IsPROOFReady( 0 ) )
                {
                    FaultLog( erError, "Can't connect to PROOF/XRD service." );
                    graceful_quit = 1;
                }

                // Checking whether signal has arrived
                if ( graceful_quit )
                {
                    InfoLog( erOK, "STOP signal received." );
                    return ;
                }

                // TODO: Needs to be optimized. Maybe moved to a different thread
                // cleaning all PF which are in disconnect state
                //CleanDisconnectsPF( m_commonOptions.m_proofCFG );

                // ------------------------
                // A Global "select"
                // ------------------------�
                fd_set readset;
                FD_ZERO( &readset );

                // TODO: implement poll or check that a number of sockets is not higher than 1024 (limitations of "select" )
                Sockets_type::const_iterator iter = m_socksToSelect.begin();
                Sockets_type::const_iterator iter_end = m_socksToSelect.end();
                for ( ; iter != iter_end; ++iter )
                {
                    FD_SET( *iter, &readset );
                }

                // Setting time-out
                timeval timeout;
                timeout.tv_sec = g_READ_READY_INTERVAL;
                timeout.tv_usec = 0;

                int fd_max = *( m_socksToSelect.rbegin() );
                // TODO: Send errno to log
                int retval = ::select( fd_max + 1, &readset, NULL, NULL, &timeout );
                if ( retval < 0 )
                {
                    FaultLog( erError, "Server socket got error while calling \"select\": " + errno2str() );
                    return;
                }

                if ( 0 == retval )
                    continue;

                // check whether agent's client tries to connect..
                if ( FD_ISSET( server.GetSocket().get(), &readset ) )
                {
                    inet::smart_socket socket( server.Accept() );
                    createClientNode( socket );
                }

                // check whether a proof server tries to connect to proof workers
                iter = m_socksToSelect.begin();
                iter_end = m_socksToSelect.end();
                for ( ; iter != iter_end; ++iter )
                {
                    if ( FD_ISSET( *iter, &readset ) )
                    {
                        // if yes, then we need to activate this node and
                        // add it to the packetforwarder
                        // TODO:
                    }
                }
            }
        }
        catch ( exception & e )
        {
            FaultLog( erError, e.what() );
        }
    }

//=============================================================================
    void CAgentServer::createClientNode( MiscCommon::INet::smart_socket &_sock )
    {
        // checking protocol version
        string sReceive;
        inet::receive_string( _sock, &sReceive, g_nBUF_SIZE );
        DebugLog( erOK, "Server received: " + sReceive );

        // TODO: Implement protocol version check
        string sOK( g_szRESPONSE_OK );
        DebugLog( erOK, "Server sends: " + sOK );
        inet::send_string( _sock, sOK );

        // TODO: Receiving user name -- now we always assume that this is a user name -- WE NEED to implement a simple protocol!!!
        string sUsrName;
        receive_string( _sock, &sUsrName, g_nBUF_SIZE );
        DebugLog( erOK, "Server received: " + sUsrName );

        DebugLog( erOK, "Server sends: " + sOK );
        send_string( _sock, sOK );

        string strSocketInfo;
        inet::socket2string( _sock, &strSocketInfo );
        string strSocketPeerInfo;
        inet::peer2string( _sock, &strSocketPeerInfo );
        stringstream ss;
        ss
        << "Accepting connection on : " << strSocketInfo
        << " for peer: " << strSocketPeerInfo;
        InfoLog( erOK, ss.str() );

        const int port = inet::get_free_port( m_Data.m_agentLocalClientPortMin, m_Data.m_agentLocalClientPortMax );
        if ( 0 == port )
            throw runtime_error( "Can't find any free port from the given range." );

        // Add a worker to PROOF cfg
        string strRealWrkHost;
        string strPROOFCfgString;
        inet::peer2string( _sock, &strRealWrkHost );

        //AddWrk2PROOFCfg( m_commonOptions.m_proofCFG, sUsrName, port, strRealWrkHost, &strPROOFCfgString );

        // Spawn PortForwarder
        //   AddPF( _sock.detach(), port, strPROOFCfgString );

        // Now when we got a connection from our worker, we need to create a local server (for that worker)
        // which actually will emulate a local worker node for a proof server
        // Listening for PROOF master connections
        // Whenever he tries to connect to its clients we will catch it and redirect it
        inet::CSocketServer localPROOFclient;
        localPROOFclient.Bind( port );
        localPROOFclient.Listen( 1 );
        localPROOFclient.GetSocket().set_nonblock();

        // Then we add this node to a nodes container
        CNodeContainer::node_type node( new SNode( _sock.detach(), localPROOFclient.GetSocket().detach(), strPROOFCfgString ) );
        node->disable();
        m_nodes.addNode( node );
        // Update proof.cfg according to a current number of active workers

        // add new worker's localPROOFServer socket to the main "select"
        m_socksToSelect.insert( node->m_second->get() );
    }
//=============================================================================
    void CAgentServer::deleteServerInfoFile()
    {
        // TODO: check error code
        unlink( m_serverInfoFile.c_str() );
    }
}
