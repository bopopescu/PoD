/************************************************************************/
/**
 * @file $$File name$$
 * @brief $$File comment$$
 * @author Anar Manafov A.Manafov@gsi.de
 *//*

        version number:    $LastChangedRevision$
        created by:        Anar Manafov
                           2007-08-24
        last changed by:   $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2007-2008 GSI GridTeam. All rights reserved.
*************************************************************************/
#ifndef GRIDDLG_H_
#define GRIDDLG_H_

// Qt autogen. file
#include "ui_wgGrid.h"
// MiscCommon
#include "def.h"
// PAConsole
#include "IJobManager.h"
#include "JobSubmitter.h"
#include "TreeItemContainer.h"

class CGridDlg: public QWidget, IJobManager
{
        Q_OBJECT
        Q_INTERFACES( IJobManager )

        friend class boost::serialization::access;

    public:
        CGridDlg( QWidget *parent = NULL );
        virtual ~CGridDlg();

    public:
        // IJobManager interface
        QString getName() const
        {
            return QString( "gLite\nJob Manager" );
        }
        QWidget *getWidget()
        {
            return this;
        }
        QIcon getIcon()
        {
            return QIcon( ":/images/grid.png" );
        }
        void startUpdTimer( int _JobStatusUpdInterval )
        {
            // start or restart the timer
            m_Timer->start( _JobStatusUpdInterval * 1000 );
        }
        int getJobsCount() const
        {
            return m_JobsCount;
        }

    signals:
        void changeNumberOfJobs( int _count );

    public:
        /**
         *
         * @brief The setAllDefault() method sets all CGridDlg to default values.
         *
         */
        void setAllDefault();

    public slots:
        void on_btnSubmitClient_clicked();
        void updateJobsTree();
        void recieveThreadMsg( const QString &_Msg );
        void setProgress( int _Val );
        void on_btnBrowseJDL_clicked();
        void on_edtJDLFileName_textChanged( const QString & /*_text*/ );
        void setNumberOfJobs( int _count );


    private slots:
        void copyJobID() const;
        void cancelJob();
        void getJobOutput();
        void getJobLoggingInfo();
        void removeJob();

    protected:
        void contextMenuEvent( QContextMenuEvent *event );

    private:
        void createActions();
        void UpdateEndpoints( bool _Msg = true );
        void UpdateAfterLoad();

        // serialization
        template<class Archive>
        void save( Archive & _ar, const unsigned int /*_version*/ ) const
        {
            _ar
            & BOOST_SERIALIZATION_NVP( m_JDLFileName )
            & BOOST_SERIALIZATION_NVP( m_JobSubmitter );
        }
        template<class Archive>
        void load( Archive & _ar, const unsigned int /*_version*/ )
        {
            _ar
            & BOOST_SERIALIZATION_NVP( m_JDLFileName )
            & BOOST_SERIALIZATION_NVP( m_JobSubmitter );
            UpdateAfterLoad();
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

    private:
        Ui::wgGrid m_ui;
        QTimer *m_Timer;
        CJobSubmitter m_JobSubmitter;
        QAction *copyJobIDAct;
        QAction *cancelJobAct;
        QAction *getJobOutputAct;
        QAction *getJobLoggingInfoAct;
        QAction *removeJobAct;
        CTreeItemContainer m_TreeItems;
        QClipboard *clipboard;
        std::string m_JDLFileName;
        int m_JobsCount;
};

BOOST_CLASS_VERSION( CGridDlg, 1 )

#endif /*GRIDDLG_H_*/
