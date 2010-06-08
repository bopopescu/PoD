/************************************************************************/
/**
 * @file
 * @brief
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*

        version number:     $LastChangedRevision$
        created by:         Anar Manafov
                            2010-03-30
        last changed by:    $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2010 GSI GridTeam. All rights reserved.
*************************************************************************/
#ifndef PBSJOBSUBMITTER_H_
#define PBSJOBSUBMITTER_H_
//=============================================================================
// gLite plug-in
#include "PbsMng.h"
// STD
#include <sstream>
#include <stdexcept>
// Qt
#include <QThread>
#include <QMutex>
#include <QMetaType>
// BOOST
#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/map.hpp>
// MiscCommon
#include "PoDUserDefaultsOptions.h"

namespace pbs_plug
{
//=============================================================================
    class CPbsJobSubmitter: public QThread
    {
            Q_OBJECT

            friend class boost::serialization::access;

        public:
            // <parent job id, num of children>
            typedef std::map<CPbsMng::jobID_t, size_t> jobslist_t;

        public:
            CPbsJobSubmitter( QObject *parent ): QThread( parent )
            {
                qRegisterMetaType<CPbsMng::jobID_t>( "CPbsMng::jobID_t" );
            }
            ~CPbsJobSubmitter()
            {
                if( isRunning() )
                    terminate();
            }

        public:
            void setUserDefaults( const PoD::CPoDUserDefaults &_ud )
            {
                m_pbs.setUserDefaults( _ud );
            }
            const jobslist_t &getParentJobsList() const
            {
                return m_parentJobs;
            }
            void setAllDefault()
            {
                m_parentJobs.clear();
            }
            void setJobScriptFilename( const std::string &_JobScriptFilename )
            {
                m_JobScriptFilename = _JobScriptFilename;
            }
            void setNumberOfWorkers( size_t _WrkN )
            {
                m_numberOfWrk = _WrkN;
            }
            void setQueue( const std::string &_queue )
            {
                m_queue = _queue;
            }
            void removeJob( const CPbsMng::jobID_t &_jobID, bool _emitSignal = true )
            {
                m_mutex.lock();
                m_parentJobs.erase( _jobID );
                m_mutex.unlock();

                if( _emitSignal )
                    emit removedJob( _jobID );
            }
            void killJob( const CPbsMng::jobID_t &_jobID )
            {
                m_pbs.killJob( _jobID );
            }
            void getQueues( CPbsMng::queueInfoContainer_t *_container ) const
            {
                m_pbs.getQueues( _container );
            }
            void jobStatusAllJobs( CPbsMng::jobInfoContainer_t *_container ) const
            {
                m_pbs.jobStatusAllJobs( _container );
            }
            void setEnvironment( const std::string &_envp )
            {
                m_pbs.setEnvironment( _envp );
            }

        signals:
            void changeProgress( int _Val );
            void newJob( const CPbsMng::jobID_t &_jobID );
            void removedJob( const CPbsMng::jobID_t &_jobID );
            void sendThreadMsg( const QString &_Msg );

        protected:
            void run()
            {
                emit changeProgress( 0 );

                // Submit a Grid Job
                try
                {
                    emit changeProgress( 30 );

                    CPbsMng::jobArray_t jobs = m_pbs.jobSubmit( m_JobScriptFilename,
                                                                m_queue,
                                                                m_numberOfWrk );

                    // get the parent index
                    if( jobs.empty() )
                        throw std::runtime_error( "Bad jobs' parent index" );

                    CPbsMng::jobID_t lastJobID = jobs[0];

                    emit changeProgress( 90 );

                    m_mutex.lock();
                    m_parentJobs.insert( jobslist_t::value_type( lastJobID, m_numberOfWrk ) );
                    m_mutex.unlock();

                    emit newJob( lastJobID );
                }
                catch( const std::exception &_e )
                {
                    emit sendThreadMsg( tr( _e.what() ) );
                    emit changeProgress( 0 );
                    return;
                }

                emit changeProgress( 100 );
            }
            //=============================================================================
            // serialization
            template<class Archive>
            void save( Archive & _ar, const unsigned int /*_version*/ ) const
            {
                _ar & BOOST_SERIALIZATION_NVP( m_parentJobs );
            }
            template<class Archive>
            void load( Archive & _ar, const unsigned int _version )
            {
                m_mutex.lock();
                _ar & BOOST_SERIALIZATION_NVP( m_parentJobs );
                m_mutex.unlock();
            }
            BOOST_SERIALIZATION_SPLIT_MEMBER()

        private:
            jobslist_t m_parentJobs;
            QMutex m_mutex;
            std::string m_JobScriptFilename;
            size_t m_numberOfWrk;
            std::string m_queue;
            CPbsMng m_pbs;
    };

}

BOOST_CLASS_VERSION( pbs_plug::CPbsJobSubmitter, 1 )

#endif /*PBSJOBSUBMITTER_H_*/