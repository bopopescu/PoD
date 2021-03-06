/************************************************************************/
/**
 * @file Test_ProtocolCommands.cpp
 * @brief Unit tests of ProtocolCommands
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*

        version number:     $LastChangedRevision$
        created by:         Anar Manafov
                            2009-12-26
        last changed by:    $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2009 GSI GridTeam. All rights reserved.
*************************************************************************/
// STD
#include <stdexcept>
// BOOST: tests
// Defines test_main function to link with actual unit test code.
#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN    // Boost 1.33
#define BOOST_TEST_MAIN

// FIX: silence a warning until BOOST fix it
// boost/test/floating_point_comparison.hpp:251:25: warning: unused variable 'check_is_close' [-Wunused-variable]
// boost/test/floating_point_comparison.hpp:273:25: warning: unused variable 'check_is_small' [-Wunused-variable]
// clang
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif
#include <boost/test/auto_unit_test.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

using boost::unit_test::test_suite;
// pod-agent
#include "ProtocolCommands.h"
//=============================================================================
using namespace std;
using namespace PROOFAgent;

BOOST_AUTO_TEST_SUITE( pod_agent_ProtocolCommands );
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SVersionCmd )
{
    SVersionCmd a;
    a.m_version = 34;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    SVersionCmd b;
    b.convertFromData( data );

    BOOST_CHECK_EQUAL( a, b );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SVersionCmd_BadData )
{
    SVersionCmd a;
    a.m_version = 34;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    // making data array shorter
    data.resize( data.size() - 2 );

    SVersionCmd b;
    BOOST_CHECK_THROW( b.convertFromData( data ), runtime_error );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SHostInfoCmd )
{
    SHostInfoCmd a;
    a.m_username = "testuser";
    a.m_host = "test.host.de";
    a.m_xpdPort = 129;
    a.m_xpdPid = 198442;
    a.m_agentPort = 224;
    a.m_agentPid = 12442;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    SHostInfoCmd b;
    b.convertFromData( data );

    BOOST_CHECK_EQUAL( a, b );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SHostInfoCmd_BadData )
{
    SHostInfoCmd a;
    a.m_username = "testuser";
    a.m_host = "test.host.de";
    a.m_xpdPort = 129;
    a.m_xpdPid = 198442;
    a.m_agentPort = 224;
    a.m_agentPid = 12442;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    // making data array shorter
    data.resize( data.size() - 2 );

    SHostInfoCmd b;
    BOOST_CHECK_THROW( b.convertFromData( data ), runtime_error );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SIdCmd )
{
    SIdCmd a;
    a.m_id = 236734;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    SIdCmd b;
    b.convertFromData( data );

    BOOST_CHECK_EQUAL( a, b );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SIdCmd_BadData )
{
    SIdCmd a;
    a.m_id = 34;
    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    // making data array shorter
    data.resize( data.size() - 2 );

    SIdCmd b;
    BOOST_CHECK_THROW( b.convertFromData( data ), runtime_error );
}
//=============================================================================
BOOST_AUTO_TEST_CASE( test_SWnListCmd )
{
    SWnListCmd a;
    a.m_container.push_back( "test1" );
    a.m_container.push_back( "test dsfg" );
    a.m_container.push_back( "sdg t43454 35" );
    a.m_container.push_back( "test2" );
    a.m_container.push_back( "test4 dfgd rt test5" );

    MiscCommon::BYTEVector_t data;
    a.convertToData( &data );

    SWnListCmd b;
    b.convertFromData( data );

    BOOST_CHECK_EQUAL( a, b );
}
BOOST_AUTO_TEST_SUITE_END();
