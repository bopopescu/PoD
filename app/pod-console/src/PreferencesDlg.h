/************************************************************************/
/**
 * @file $$File name$$
 * @brief $$File comment$$
 * @author Anar Manafov A.Manafov@gsi.de
 *//*

        version number:    $LastChangedRevision$
        created by:        Anar Manafov
                           2008-07-15
        last changed by:   $LastChangedBy$ $LastChangedDate$

        Copyright (c) 2008 GSI GridTeam. All rights reserved.
*************************************************************************/
#ifndef PREFERENCESDLG_H_
#define PREFERENCESDLG_H_
//=============================================================================
// Qt autogen. file
#include "ui_wgPreferences.h"
// BOOST
#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
//=============================================================================
class CPreferencesDlg: public QWidget
{
        Q_OBJECT

        friend class boost::serialization::access;

    public:
        CPreferencesDlg( QWidget *_parent = NULL );
        virtual ~CPreferencesDlg();

    public:
        int getJobStatusUpdInterval()
        {
            return m_JobStatusUpdInterval;
        }

    signals:
        void changedJobStatusUpdInterval( int _val );

    private slots:
        void _changedJobStatusUpdInterval( int _val );

    private:
        void UpdateAfterLoad();

        // serialization
        template<class Archive>
        void save( Archive & _ar, const unsigned int /*_version*/ ) const
        {
            _ar
            & BOOST_SERIALIZATION_NVP( m_JobStatusUpdInterval );
        }
        template<class Archive>
        void load( Archive & _ar, const unsigned int /*_version*/ )
        {
            _ar
            & BOOST_SERIALIZATION_NVP( m_JobStatusUpdInterval );
            UpdateAfterLoad();
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

    private:
        Ui::wgPreferences m_ui;
        int m_JobStatusUpdInterval;
};

BOOST_CLASS_VERSION( CPreferencesDlg, 2 )

#endif /* PREFERENCESDLG_H_ */