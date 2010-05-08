/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include "LibCommon/Log.h"

#include "MergeDatedDatums.h"

//
// CMergeDatedDatums
//

CMergeDatedDatums::CMergeDatedDatums(void) 
: m_state( eInit ), m_request( eUnknown )
{
}

CMergeDatedDatums::~CMergeDatedDatums(void) {
  while ( !m_vCarriers.Empty() ) {
    CMergeCarrierBase *p = m_vCarriers.RemoveEnd();
    delete p;
  }
}

void CMergeDatedDatums::Add( CTimeSeries<CQuote> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.Append( new CMergeCarrier<CQuote>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CTrade> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.Append( new CMergeCarrier<CTrade>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CBar> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.Append( new CMergeCarrier<CBar>( pSeries, function ) );
}

void CMergeDatedDatums::Add( CTimeSeries<CMarketDepth> *pSeries, CMergeDatedDatums::OnDatumHandler function) {
  m_vCarriers.Append( new CMergeCarrier<CMarketDepth>( pSeries, function ) );
}

// http://www.codeguru.com/forum/archive/index.php/t-344661.html

/*
struct SortByMergeCarrier {
public:
  SortByMergeCarrier( std::vector<CMergeCarrierBase *> *v ): m_v( v ) {};
  bool operator() ( size_t lhs, size_t rhs ) { return (*m_v)[lhs]->GetDateTime() < (*m_v)[rhs]->GetDateTime(); };
protected:
  std::vector<CMergeCarrierBase *> *m_v;
};
*/

// be aware that this maybe running in alternate thread
// the thread is not created in this class 
// for example, see CSimulationProvider
void CMergeDatedDatums::Run() {
  m_request = eRun;
  size_t cntCarriers = m_vCarriers.Size();
  LOG << "#carriers: " << cntCarriers;  // need cross thread writing 
  CMergeCarrierBase *pCarrier;
  m_cntProcessedDatums = 0;
  m_cntReorders = 0;
  m_state = eRunning;
  while ( ( 0 != cntCarriers ) && ( eRun == m_request ) ) {  // once all series have been depleted, end of run
    pCarrier = m_vCarriers.GetRoot();
    ProcessCarrier( pCarrier );
    //pCarrier->ProcessDatum();  // automatically loads next datum when done
    ++m_cntProcessedDatums;
    if ( NULL == pCarrier->GetDatedDatum() ) {
      // retire the consumed carrier
      m_vCarriers.ArchiveRoot();
      --cntCarriers;
    }
    else {
      // reorder the carriers
      m_vCarriers.SiftDown();
    }
  }
  m_state = eStopped;
  LOG << "Merge stats: " << m_cntProcessedDatums << ", " << m_cntReorders;
}

void CMergeDatedDatums::ProcessCarrier( CMergeCarrierBase *pCarrier ) {
  pCarrier->ProcessDatum();  // automatically loads next datum when done
}

void CMergeDatedDatums::Stop( void ) {
  m_request = eStop;
}
