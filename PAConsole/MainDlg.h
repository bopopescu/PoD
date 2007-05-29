/************************************************************************/
/**
 * @file MainDlg.h
 * @brief Main dialog declaration
 * @author Anar Manafov A.Manafov@gsi.de
 */ /*
 
        version number:   $LastChangedRevision$
        created by:          Anar Manafov
                                  2007-05-23
        last changed by:   $LastChangedBy$ $LastChangedDate$
 
        Copyright (c) 2007 GSI GridTeam. All rights reserved.
*************************************************************************/
#ifndef CMAINDLG_H_
#define CMAINDLG_H_

// Qt
#include <QTimer>

// Qt autogen. file
#include "ui_maindlg.h"

// OUR
#include "MiscUtils.h"

class CMainDlg:
            public QDialog
{
        Q_OBJECT

    public:
        CMainDlg( QDialog *_Parent = 0 );
        ~CMainDlg()
        {
            if ( m_Timer )
            {
                m_Timer->stop();
                delete m_Timer;
            }
        }

    private slots:
        // Server's slots
        void on_btnStatusServer_clicked();
        void on_btnStartServer_clicked();
        void on_btnStopServer_clicked();
        void on_btnBrowsePIDDir_clicked();
        // Client's slots
        void on_btnStartClient_clicked( bool Checked = false );
        // Timer
        void update();

    private:
        Ui::MainDlg m_ui;
        QTimer *m_Timer;
};

#endif /*CMAINDLG_H_*/
