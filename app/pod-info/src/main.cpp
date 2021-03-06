/************************************************************************/
/**
 * @file main.cpp
 * @brief main file
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*

        version number:     $LastChangedRevision$
        created by:         Anar Manafov
                            2011-01-17
        last changed by:    $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2011 GSI, Scientific Computing division. All rights reserved.
*************************************************************************/
// STD
#include <stdexcept>
// MiscCommon
#include "BOOSTHelper.h"
#include "Process.h"
#include "SysHelper.h"
#include "PoDSysFiles.h"
#include "PoDUserDefaultsOptions.h"
// pod-info
#include "version.h"
#include "Server.h"
#include "Options.h"
#include "SrvInfo.h"
#include "SSHTunnel.h"
//=============================================================================
using namespace MiscCommon;
using namespace std;
namespace bpo = boost::program_options;
namespace boost_hlp = MiscCommon::BOOSTHelper;
//=============================================================================
string version( const CPoDEnvironment &_env, const pod_info::CServer &_srv )
{
    bool noServer( false );
    PROOFAgent::SHostInfoCmd srvHostInfo;
    try
    {
        _srv.getSrvHostInfo( &srvHostInfo );
    }
    catch( ... )
    {
        noServer = true;
    }
    ostringstream ss;
    ss
            << "PoD location: " << _env.PoDPath() << "\n"
            << "Local Version: " << _env.version() << "\n";

    if( noServer )
    {
        ss << "PoD server is NOT found.";
    }
    else
    {
        ss
                << "Server PoD location: "
                << srvHostInfo.m_username << "@" << srvHostInfo.m_host << ":"
                << srvHostInfo.m_PoDPath << "\n"
                << "Server Version: " << srvHostInfo.m_version;
    }
    return ( ss.str() );
}
//=============================================================================
size_t listWNs( string *_output, const pod_info::CServer &_srv,
                const SOptions &_opt )
{
    PROOFAgent::SWnListCmd lst;
    try
    {
        _srv.getListOfWNs( &lst );
    }
    catch( exception &_e )
    {
        string msg;
        msg += "PoD server is NOT found.\n";
        if( _opt.m_debug )
            msg += _e.what();
        throw runtime_error( msg );
    }
    if( _output )
    {
        stringstream ss;
        std::ostream_iterator< std::string > output( ss, "\n" );
        std::copy( lst.m_container.begin(), lst.m_container.end(), output );
        *_output = ss.str();
    }
    return ( lst.m_container.size() );
}
//=============================================================================
void retrieveRemoteServerInfo( const SOptions &_opt,
                               const string &_destinationFile )
{
    // delete first the remote srv info file
    unlink( _destinationFile.c_str() );

    StringVector_t arg;
    string sourceFile( _opt.m_remotePath );
    sourceFile += "etc/server_info.cfg";
    arg.push_back( "-s" + sourceFile );
    arg.push_back( "-l " + _opt.m_sshConnectionStr );
    arg.push_back( "-f " + _destinationFile );
    if( _opt.m_debug )
        arg.push_back( "-d" );
    if( _opt.m_batchMode )
        arg.push_back( "-b" );
    string cmd( "$POD_LOCATION/bin/private/pod-remote-srv-info" );
    smart_path( &cmd );
    string stdout;
    do_execv( cmd, arg, 60, NULL );
    if( !file_exists( _destinationFile ) )
    {
        stringstream ss;
        ss << "Remote PoD server is NOT running.";
        throw runtime_error( ss.str() );
    }
}
//=============================================================================
int main( int argc, char *argv[] )
{
    CPoDEnvironment env;
    EPoDServerType srvType( SrvType_Unknown );

    try
    {
        env.init();

        SOptions options;
        if( !parseCmdLine( argc, argv, &options ) )
            return 0;

        // An SSH tunnel object
        CSSHTunnel sshTunnel;

        CSrvInfo srvInfo( &env );
        srvInfo.getInfo();

        // Short info about locals
        if( options.m_xpdPid )
        {
            if( 0 == srvInfo.xpdPid() )
                return 1;

            cout << srvInfo.xpdPid() << endl;
            return 0;
        }
        if( options.m_xpdPort )
        {
            if( 0 == srvInfo.xpdPort() )
                return 1;

            cout << srvInfo.xpdPort() << endl;
            return 0;
        }
        if( options.m_agentPid )
        {
            if( 0 == srvInfo.agentPid() )
                return 1;

            cout << srvInfo.agentPid() << endl;
            return 0;
        }
        if( options.m_agentPort )
        {
            if( !srvInfo.processServerInfoCfg() )
                return 1;

            if( 0 == srvInfo.agentPort() )
                return 1;

            cout << srvInfo.agentPort() << endl;
            return 0;
        }

        string srvHost;
        size_t agentPort( 0 );

        // Check PoD server's Type
        srvType = ( options.m_sshConnectionStr.empty() ) ? SrvType_Local : SrvType_Remote;

        // >>> REMOTE SERVER <<<
        // if the type of the server is remote, than we need to get a remote
        // server info file
        // use SSH to retrieve server_info.cfg
        if( SrvType_Remote == srvType )
        {
            string outfile( env.srvInfoFileRemote() );
            retrieveRemoteServerInfo( options, outfile );
            srvInfo.processServerInfoCfg( &outfile );
            // now we can delete the remote server file
            // we can't reuse it in next sessions, since the PoD port could change
            unlink( outfile.c_str() );

            // we tunnel the connection to PoD server
            agentPort = inet::get_free_port( env.getUD().m_server.m_agentPortsRangeMin,
                                             env.getUD().m_server.m_agentPortsRangeMax );
            if( 0 == agentPort )
            {
                throw runtime_error( "Can't find any free port to tunnel PoD services" );
            }
            // TODO: Be careful, tunnelAgentPidFile can be busy by pod-remote
            // check before using it.
            sshTunnel.setPidFile( env.tunnelAgentPidFile() );
            sshTunnel.create( options.m_sshConnectionStr, agentPort,
                              srvInfo.agentPort(), options.m_openDomain );

            // if we tunnel pod-agent's port, than we need to connect to a localhost
            srvHost = "localhost";
        }
        else
        {
            // >>> REMOTE MANAGED SERVER <<<
            // Check for pod-remote daemons
#if defined (BOOST_PROPERTY_TREE)
            PoD::SPoDRemoteOptions opt_file;
            if( srvInfo.processPoDRemoteCfg( &opt_file ) )
            {
                srvType = SrvType_RemoteManaged;
                srvHost = "localhost";
                agentPort = opt_file.m_localAgentPort;
            }
#endif
            // >>> LOCAL SERVER <<<
            if( SrvType_Local == srvType )
            {
                // Process a local server-info.
                // If "--version" is given, than we don't throw,
                // because we need a version info in anyway, even without any server
                if( !srvInfo.processServerInfoCfg() && !options.m_version )
                {
                    string msg;
                    msg += "PoD server is NOT running.";
                    if( options.m_debug )
                    {
                        msg += "\nCan't process server info: ";
                        msg += env.srvInfoFile();
                    }
                    throw runtime_error( msg );
                }
                srvHost = srvInfo.serverHost();
                agentPort = srvInfo.agentPort();
            }
        }

        // Print type of the server - only in debug mode
        if( options.m_debug )
        {
            cout << "PoD Server Type: ";
            switch( srvType )
            {
                case SrvType_Local:
                    cout << "local";
                    break;
                case SrvType_Remote:
                    cout << "remote";
                    break;
                case SrvType_RemoteManaged:
                    cout << "remote (managed by pod-remote)";
                    break;
                default:
                    cout << "unknown";
                    break;
            }
            cout << endl;
        }

        // Show version information
        if( options.m_version )
        {
            pod_info::CServer srv( srvHost, agentPort );
            cout << version( env, srv ) << endl;
            return 0;
        }

        // PoD Server status
        if( options.m_status || options.m_connectionString )
        {
            string localHostName;
            get_hostname( &localHostName );
            // If we run locally it could mean, we run on a shared file system as well.
            // We therefore need to check, whether we run on the same machine as the server
            if( srvType == SrvType_Local && localHostName == srvHost )
            {
                srvInfo.getInfo();
            }
            else
            {
                pod_info::CServer srv( srvHost, agentPort );
                srvInfo.getInfo( &srv );
            }

            if( options.m_status )
            {
                srvInfo.printInfo( cout );
                if( srvInfo.getStatus() != CSrvInfo::srvStatus_OK )
                {
                    return srvInfo.getStatus();
                }
            }
            if( options.m_connectionString )
            {
                srvInfo.printConnectionString( cout );
            }
        }

        // list of and a number of available WNs
        if( options.m_countWNs || options.m_listWNs )
        {
            pod_info::CServer srv( srvHost, agentPort );
            string lst;
            size_t n = listWNs(( options.m_listWNs ? &lst : NULL ), srv, options );

            if( options.m_countWNs )
            {
                cout << n << endl;
            }
            if( options.m_listWNs )
            {
                cout << lst;
                cout.flush();
            }
        }
    }
    catch( exception& e )
    {
        cerr << PROJECT_NAME << ": " << e.what() << endl;
        return 1;
    }
    return 0;
}
