/*
 *  logEngine.h
 *  pod-ssh
 *
 *  Created by Anar Manafov on 31.08.10.
 *  Copyright 2010 Anar Manafov <Anar.Manafov@gmail.com>. All rights reserved.
 *
 */
#ifndef LOGENGINE_H
#define LOGENGINE_H
//=============================================================================
#include <boost/thread/thread.hpp>
//=============================================================================
class CLogEngine
{
    public:
        ~CLogEngine();
        void start();
        void stop();
        void operator()( const std::string &_msg, const std::string &_id = "**" ) const;

    private:
        void thread_worker( int _fd, const std::string & _pipename );

    private:
        int m_fd;
        boost::thread m_thread;
        volatile sig_atomic_t m_stopLogEngine;
        std::string m_pipeName;
};
//=============================================================================
#endif
