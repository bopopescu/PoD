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
// BOOST
#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
// PAConsole
#include "JobSubmitter.h"
#include "TreeItemContainer.h"

typedef std::auto_ptr<CJobSubmitter> JobSubmitterPtr_t;

class CGridDlg: public QWidget
{
        Q_OBJECT

        friend class boost::serialization::access;

    public:
        CGridDlg( QWidget *parent = NULL );
        virtual ~CGridDlg();

    public:
        CJobSubmitter *getJobSubmitter()
        {
            return m_JobSubmitter.get();
        }

    public slots:
        void on_btnSubmitClient_clicked();
        void updateJobsTree();
        void recieveThreadMsg( const QString &_Msg);
        void setProgress( int _Val );
        void on_btnBrowseJDL_clicked();
        void on_edtJDLFileName_textChanged( const QString & /*_text*/ );

    private slots:
        void copyJobID() const;
        void cancelJob();
        void getJobOutput();

    protected:
        void contextMenuEvent( QContextMenuEvent *event );

    private:
        void createActions();
        void UpdateEndpoints();
        void UpdateAfterLoad();

        template<class Archive>
        void save(Archive & _ar, const unsigned int /*_version*/) const
        {
            _ar & BOOST_SERIALIZATION_NVP(m_JDLFileName);
        }
        template<class Archive>
        void load(Archive & _ar, const unsigned int /*_version*/)
        {
            _ar & BOOST_SERIALIZATION_NVP(m_JDLFileName);
            UpdateAfterLoad();
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

    private:
        Ui::wgGrid m_ui;
        QTimer *m_Timer;
        JobSubmitterPtr_t m_JobSubmitter;
        QAction *copyJobIDAct;
        QAction *cancelJobAct;
        QAction *getJobOutputAct;
        CTreeItemContainer m_TreeItems;
        QClipboard *clipboard;
        std::string m_JDLFileName;
};

BOOST_CLASS_VERSION(CGridDlg, 1)

#endif /*GRIDDLG_H_*/
