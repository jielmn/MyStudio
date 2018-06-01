/*
 * libjingle
 * Copyright 2004--2006, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products 
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "talk/xmpp/xmpptask.h"
#include "talk/xmpp/xmppclient.h"
#include "talk/xmpp/xmppengine.h"
#include "talk/xmpp/constants.h"
#include "talk/xmpp/ratelimitmanager.h"
#include "talk/examples/login/xmpppump.h"
#include "talk/examples/login/xmppthread.h"
#include "talk/base/pathutils.h"
#include "talk/base/fileutils.h"
#include "talk/base/stream.h"
#include "talk/base/tarstream.h"
#include "talk/base/httpcommon.h"

#include "libjingle/libjingle.h"
#include "libjingle/libjingle_inner.h"

#include <time.h>
#include <assert.h>

#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <iterator>
#endif

namespace buzz {

RateLimitManager task_rate_manager;



XmppTask::XmppTask(Task* parent, XmppEngine::HandlerLevel level)
    : Task(parent), client_(NULL) {
#ifdef _DEBUG
  debug_force_timeout_ = false;
#endif

  XmppClient* client = (XmppClient*)parent->GetParent(XMPP_CLIENT_TASK_CODE);
  client_ = client;
  id_ = client->NextId();
  client->AddXmppTask(this, level);
  client->SignalDisconnected.connect(this, &XmppTask::OnDisconnect);
}

XmppTask::~XmppTask() {
  StopImpl();
  ClearVector( m_vIqIds );
}

bool  XmppTask::AddIqId( const TIqId *  ptIqId )
{
    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        const TIqId * p = *it;
        if ( 0 == strcmp( p->szIqId, ptIqId->szIqId ) )
        {
            return false;
        }
    }

    TIqId * ptNewIqId = new TIqId;
    memcpy( ptNewIqId, ptIqId, sizeof(TIqId) );

    m_vIqIds.push_back( ptNewIqId );
    return true;
}

bool  XmppTask::DelIqId( const TIqId *  ptIqId )
{
    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        TIqId * p = *it;
        if ( 0 == strcmp( p->szIqId, ptIqId->szIqId ) )
        {
            delete p;
            m_vIqIds.erase( it );
            return true;
        }
    }
    return false;
}


bool  XmppTask::IfExistIqId( const TIqId *  ptIqId )
{
    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        TIqId * p = *it;
        if ( 0 == strcmp( p->szIqId, ptIqId->szIqId ) )
        {
            return true;
        }
    }
    return false;
}

bool   XmppTask::IfExistIqId( const char * szIq, bool bDelete, TIqId * ptIqId /*= 0*/ )
{
    assert( szIq );

    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        TIqId * p = *it;
        if ( 0 == strcmp( p->szIqId, szIq ) )
        {
            if ( bDelete )
            {
                if ( ptIqId )
                {
                    memcpy( ptIqId, p, sizeof(TIqId) );
                }
                delete p;
                m_vIqIds.erase( it );
            }
            return true;
        }
    }
    return false;
}


void XmppTask::StopImpl() {
  while (NextStanza() != NULL) {}
  if (client_) {
    client_->RemoveXmppTask(this);
    client_->SignalDisconnected.disconnect(this);
    client_ = NULL;
  }
}

XmppReturnStatus XmppTask::SendStanza(const XmlElement* stanza) {
  if (client_ == NULL)
    return XMPP_RETURN_BADSTATE;
  return client_->SendStanza(stanza);
}

XmppReturnStatus XmppTask::SendStanzaError(const XmlElement* element_original,
                                           XmppStanzaError code,
                                           const std::string& text) {
  if (client_ == NULL)
    return XMPP_RETURN_BADSTATE;
  return client_->SendStanzaError(element_original, code, text);
}

void XmppTask::Stop() {
  StopImpl();
  Task::Stop();
}

void XmppTask::OnDisconnect() {
  Error();
}

void XmppTask::QueueStanza(const XmlElement* stanza) {
#ifdef _DEBUG
  if (debug_force_timeout_)
    return;
#endif

  stanza_queue_.push_back(new XmlElement(*stanza));
  Wake();
}

const XmlElement* XmppTask::NextStanza() {
  XmlElement* result = NULL;
  if (!stanza_queue_.empty()) {
    result = stanza_queue_.front();
    stanza_queue_.pop_front();
  }
  next_stanza_.reset(result);
  return result;
}

XmlElement* XmppTask::MakeIq(const std::string& type,
                             const buzz::Jid& to,
                             const std::string id) {
  XmlElement* result = new XmlElement(QN_IQ);
  if (!type.empty())
    result->AddAttr(QN_TYPE, type);
  if (to != JID_EMPTY)
    result->AddAttr(QN_TO, to.Str());
  if (!id.empty())
    result->AddAttr(QN_ID, id);
  return result;
}

XmlElement* XmppTask::MakeIqResult(const XmlElement * query) {
  XmlElement* result = new XmlElement(QN_IQ);
  result->AddAttr(QN_TYPE, STR_RESULT);
  if (query->HasAttr(QN_FROM)) {
    result->AddAttr(QN_TO, query->Attr(QN_FROM));
  }
  result->AddAttr(QN_ID, query->Attr(QN_ID));
  return result;
}

bool XmppTask::MatchResponseIq(const XmlElement* stanza,
                               const Jid& to,
                               const std::string& id) {
  if (stanza->Name() != QN_IQ)
    return false;

  if (stanza->Attr(QN_ID) != id)
    return false;

  Jid from(stanza->Attr(QN_FROM));
  if (from == to)
    return true;

  // We address the server as "", check if we are doing so here.
  if (to != JID_EMPTY)
    return false;

  // It is legal for the server to identify itself with "domain" or
  // "myself@domain"
  Jid me = client_->jid();
  return (from == Jid(me.domain())) || (from == me.BareJid());
}

bool XmppTask::MatchRequestIq(const XmlElement* stanza,
                              const std::string& type,
                              const QName& qn) {
  if (stanza->Name() != QN_IQ)
    return false;

  if (stanza->Attr(QN_TYPE) != type)
    return false;

  if (stanza->FirstNamed(qn) == NULL)
    return false;

  return true;
} 

bool XmppTask::VerifyTaskRateLimit(const std::string task_name, int max_count, 
                                   int per_x_seconds) {
  return task_rate_manager.VerifyRateLimit(task_name, max_count, 
                                           per_x_seconds);
}






void MG_AddGroup( CLibJinglexApp * app, PTMemberGroup & p, const LibJingleEx::PTJid ptJid, const char * szGroupName, BOOL bFirst )
{
    if ( 0 == p )
    {
        vMemberGroups::iterator it_find = find_if(
            app->m_MemberGroups.begin(), app->m_MemberGroups.end(), FindMemberGroupObj( ptJid ) ) ;

        if ( it_find == app->m_MemberGroups.end() )
        {
            p = new TMemberGroup;
            memcpy( &p->m_tJid, ptJid, sizeof( LibJingleEx::TJid ) );

            app->m_MemberGroups.push_back( p );
        }
        else
        {
            p = *it_find;
        }
    }

    if ( bFirst )
    {
        p->m_Groups.clear();
    }

    std::vector<const char *>::iterator it_findstr = find_if( p->m_Groups.begin(),
        p->m_Groups.end(), FindStringObj( szGroupName ) );

    if ( it_findstr == p->m_Groups.end() )
    {
        p->m_Groups.push_back( szGroupName );
    }
}

//LibJingleEx::PTGroup AddGroup( LibJingleEx::PTGroup & pFirstGroup, const char * szGroupName, BOOL * pbAdded, DWORD & dwGroupId )
//{
//    assert( szGroupName );
//
//    BOOL  bSpecialGroup = FALSE;
//
//    // ����ǡ�δ������ϵ�ˡ���������������Ϊ��
//    if ( 0 == strcmp( szGroupName, SPECIAL_GROUP_NAME ) || '\0' == szGroupName[0] )
//    {
//        szGroupName   = "";
//        bSpecialGroup = TRUE;
//    }
//
//    BOOL bAdded = FALSE;
//
//    LibJingleEx::PTGroup  pLast = 0;
//    LibJingleEx::PTGroup  pCur  = pFirstGroup;
//
//    while( pCur )
//    {
//        if ( 0 == strcmp( pCur->szName, szGroupName ) )
//        {
//            break;
//        }
//
//        pLast = pCur;
//        pCur  = pCur->pNext;
//    }
//
//    if ( 0 == pCur )
//    {
//        LibJingleEx::PTGroup pNewGroup = new LibJingleEx::TGroup;
//        memset( pNewGroup, 0, sizeof(LibJingleEx::TGroup) );
//
//        strncpy( pNewGroup->szName, szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN - 1 );
//        pNewGroup->szName[LibJingleEx::MAX_GROUP_NAME_LEN - 1] = '\0';
//
//        if ( bSpecialGroup )
//        {
//            pNewGroup->dwGroupId = SPECIAL_GROUP_SN;
//        }
//        else
//        {
//            dwGroupId++;
//            while( dwGroupId == SPECIAL_GROUP_SN )
//            {
//                dwGroupId++;
//            }
//            pNewGroup->dwGroupId = dwGroupId;
//        }
//        
//
//        if ( 0 == pLast )
//        {
//            pFirstGroup = pNewGroup;
//        }
//        else
//        {
//            pLast->pNext = pNewGroup;
//        }
//
//        bAdded = TRUE;
//        if ( 0 != pbAdded )
//        {
//            *pbAdded = bAdded;
//        }
//
//        return pNewGroup;
//    }
//    else
//    {
//        bAdded = FALSE;
//        if ( 0 != pbAdded )
//        {
//            *pbAdded = bAdded;
//        }
//        return pCur;
//    }
//}

int Subscription2int( const char * szSubscription )
{
    if ( 0 == strcmp( szSubscription, "from" ) )
    {
        return SUB_STATE_SUBSCRIBE;
    }
    else if ( 0 == strcmp( szSubscription, "to" ) )
    {
        return SUB_STATE_SUBSCRIBED;
    }
    else if ( 0 == strcmp( szSubscription, "both" ) )
    {
        return ( SUB_STATE_SUBSCRIBE | SUB_STATE_SUBSCRIBED );
    }
    else if ( 0 == strcmp( szSubscription, "remove" ) )
    {
        return -1;
    }

    return SUB_STATE_NONE;
}

int Ask2int( const char * szAsk )
{
    if ( 0 == strcmp( "subscribe", szAsk ) )
    {
        return ASK_STATE_SUBSCRIBE;
    }

    return ASK_STATE_NONE;
}


bool MyRosterTask::HandleStanza(const XmlElement * stanza)
{
    // ��Ϊ��ǰ�ɰ汾��dms�����ϵ�ˣ�Ҳ����׼roster���������ϵ��
    // ���Ե�ɾ��dms��ϵ��ʱ��unscribe��ϵ�ˣ������ɾ������ϵ�˵�״̬�����
#if !(ROSTER_SWITCH)
    return false;
#endif
    
    
    // stanza�����Ʊ���Ϊiq
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }


    XmppClient * c = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)c->m_hApp;
    cFuncLock cLock( &app->m_lock );
    char buf[MAX_BUFF_SIZE];



    if ( stanza->Attr( QN_TYPE ) == "set" )
    {
        const XmlElement * element = stanza->FirstElement();
        // �������roster_queryԪ�أ��˳� 
        if ( 0 == element || element->Name() != QN_ROSTER_QUERY )
        {
            return false;
        }

        // �����û�л�ȡ��roster����
        if ( !app->m_bGetRosterData )
        {
            // ����result
            buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
            iq->SetAttr( buzz::QN_TYPE, buzz::STR_RESULT );
            iq->SetAttr( buzz::QN_ID, stanza->Attr( QN_ID ) );
            c->SendStanza( iq );
            delete iq;
            return true;
        }

        // ��¼roster version
        strncpy( app->m_szRosterVer, element->Attr( QN_VER ).c_str(), sizeof(app->m_szRosterVer) );
        app->m_bRosterChanged = TRUE;
       

        LibJingleEx::PTGroup & pFirstGroup = app->m_pFirstGroup;

        const XmlElement * item = element->FirstElement();
        while( 0 != item )
        {
            // ����subscription����
            int nSubscription = Subscription2int( item->Attr( QName("", "subscription") ).c_str() );

            // ����jid����(bare jid)
            LibJingleEx::TJid tJid;
            LibJingleEx::StrJid2TJid( Utf8ToAnsi( item->Attr( QName("", "jid") ).c_str(), buf, MAX_BUFF_SIZE ), tJid );

            // ɾ������
            if ( -1 == nSubscription )
            {
                app->RemoveFriend(&tJid);
                item = item->NextElement();
                continue;
            }


            // ��������
            char szMemberName[LibJingleEx::MAX_MEMBER_NAME_LEN];
            Utf8ToAnsi( item->Attr( QName("", "name") ).c_str(), szMemberName, LibJingleEx::MAX_MEMBER_NAME_LEN );

            // ����ask����
            int nAsk = Ask2int( item->Attr( QName("", "ask") ).c_str() );

            // ��������
            char szBitRate[LibJingleEx::MAX_MEMBER_BITRATE_LEN] = { 0 };
            const XmlElement * bitrate = item->FirstNamed( QN_KEDA_ROSTER_BITRATE );
            if ( bitrate )
            {
                Utf8ToAnsi( bitrate->Attr(QN_VALUE).c_str(), szBitRate, LibJingleEx::MAX_MEMBER_BITRATE_LEN );
            }

            // ����
            int nType = 0;
            const XmlElement * type_ele = item->FirstNamed( QN_KEDA_ROSTER_TYPE );
            if ( type_ele )
            {
                sscanf( type_ele->Attr(QN_VALUE).c_str(), " %d", &nType );
            }


            PTMemberGroup pMg = 0;
            std::vector<const char*> vGroups;

            vMemberGroups::iterator it_find = find_if( app->m_MemberGroups.begin(), app->m_MemberGroups.end(), FindMemberGroupObj(&tJid) );
            if ( it_find != app->m_MemberGroups.end() )
            {
                pMg = *it_find;
                std::copy( pMg->m_Groups.begin(), pMg->m_Groups.end(), back_inserter(vGroups) );

                pMg->m_Groups.clear();
                memset( &pMg->m_tData, 0, sizeof(pMg->m_tData) );
            }
            else
            {
                pMg = new TMemberGroup;
                app->m_MemberGroups.push_back( pMg );
            }


            memcpy( &pMg->m_tJid, &tJid, sizeof( LibJingleEx::TJid ) );
            strcpy( pMg->m_tData.szName, szMemberName );
            pMg->m_tData.nSubscription = nSubscription;
            pMg->m_tData.nAsk          = nAsk;
            pMg->m_tData.pData         = 0;
            strcpy( pMg->m_tData.szBitRate, szBitRate );
            pMg->m_tData.nType         = nType;


            const XmlElement * child   = item->FirstElement();
            unsigned long dwGroupIndex = 0;

            // �е�rosterû��group��Ԫ�أ������Ƿ���ȱʡ������Ϊ�յ�����
            while ( 0 != child || 0 == dwGroupIndex )
            {
                BOOL  bHandleGroupElement = FALSE;
                char  szGroupName[LibJingleEx::MAX_GROUP_NAME_LEN] = { 0 };

                if ( 0 == child )
                {
                    bHandleGroupElement = TRUE;
                }
                else if ( child && child->Name().LocalPart() == "group" )
                {
                    Utf8ToAnsi( child->BodyText().c_str(), szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );
                    bHandleGroupElement = TRUE;
                }

                if ( bHandleGroupElement )
                {
                    // ��������������(��������������ڵĻ�)
                    BOOL bAdded = FALSE;
                    LibJingleEx::PTGroup pGroup = app->AddGroup( szGroupName, &bAdded );

#if !DMS_CONTACTS_SWITCH
                    if ( bAdded )
                    {
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::TDmsGroup  tDmsGroup;
                            tDmsGroup.dwGroupId  = pGroup->dwGroupId;
                            tDmsGroup.dwParentId = 0;
                            strncpy( tDmsGroup.szGroupName, pGroup->szName, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

                            g_pfnOnMessage( LibJingleEx::ON_NOTIFY_ADD_DMS_GROUP, app, app->m_pUserData,
                                            (unsigned char *)&tDmsGroup,  sizeof(LibJingleEx::TDmsGroup) );
                        }
                    }
#endif

                    LibJingleEx::TMember  tMember;
                    LibJingleEx::PTMember pMember = &tMember;

                    strcpy( pMember->szName, szMemberName );
                    pMember->nSubscription = nSubscription;
                    pMember->nAsk = nAsk;
                    memcpy( &pMember->tJid, &tJid, sizeof( LibJingleEx::TJid ) );
                    strcpy( pMember->szBitRate, szBitRate );
                    pMember->nType = nType;



                    LibJingleEx::TMemberItem tCallBackParam;
                    strcpy( tCallBackParam.szName, szMemberName );
                    tCallBackParam.nSubscription = pMember->nSubscription;
                    tCallBackParam.nAsk          = pMember->nAsk;
                    tCallBackParam.pData         = 0;
                    strcpy( tCallBackParam.szBitRate, szBitRate );
                    tCallBackParam.nType         = nType;




                    LibJingleEx::TMember tOld;
                    BOOL  bMemberUiChanged = FALSE;
                    BOOL  bDmsAdded = FALSE;
                    BOOL  bDmsDeleted = FALSE;

                    // ���������ĳ������������ϵ��
                    if ( !app->AddMember( pGroup, pMember, &tOld, &bMemberUiChanged, &bDmsAdded, &bDmsDeleted ) )
                    {
                        // ��Ա����
                        if ( g_tEvents.pOnRosterChange )
                        {
                            g_tEvents.pOnRosterChange( app, szGroupName, &tJid, &tCallBackParam );
                        }
                        else if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData msgData;

                            msgData.Append( (unsigned char *)szGroupName,     sizeof(szGroupName) );
                            msgData.Append( (unsigned char *)&tJid,           sizeof(tJid) );
                            msgData.Append( (unsigned char *)&tCallBackParam, sizeof(tCallBackParam) );

                            g_pfnOnMessage( LibJingleEx::ON_ROSTER_CHANGE, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                        }
                        // 

                        std::vector<const char*>::iterator it_sub;
                        for ( it_sub = vGroups.begin(); it_sub != vGroups.end(); it_sub++ )
                        {
                            if ( 0 == strcmp( *it_sub, szGroupName ) )
                            {
                                vGroups.erase( it_sub );
                                break;
                            }
                        }
                    }
                    // ������µĳ�Ա
                    else
                    {
                        if ( g_tEvents.pOnRosterAdd )
                        {
                            g_tEvents.pOnRosterAdd( app, szGroupName, &tJid, &tCallBackParam );
                        }
                        else if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData msgData;

                            msgData.Append( (unsigned char *)szGroupName, sizeof(szGroupName) );
                            msgData.Append( (unsigned char *)&tJid,       sizeof(tJid) );
                            msgData.Append( (unsigned char *)&tCallBackParam,     sizeof(tCallBackParam) );

                            g_pfnOnMessage( LibJingleEx::ON_ROSTER_ADD, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                        }
                    }


#if !DMS_CONTACTS_SWITCH
                    if ( bDmsAdded )
                    {
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::TDmsMember  tDmsMember;
                            tDmsMember.dwMemberId = pMember->dwId;
                            tDmsMember.dwGroupId  = pGroup->dwGroupId;
                            tDmsMember.eType      = (LibJingleEx::EDmsMemberType)pMember->nType;
                            LibJingleEx::TJid2StrJid( pMember->tJid, tDmsMember.szJid, LibJingleEx::MAX_JID_LEN );
                            strncpy( tDmsMember.szName, pMember->szName, LibJingleEx::MAX_DMS_MEMBER_NAME_LEN );
                            strncpy( tDmsMember.szReserved[RESERVERD_BITRATE_INDEX], pMember->szBitRate, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN);

                            g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_ADD_DMS_MEMBER, app, app->m_pUserData, 
                                (unsigned char *)&tDmsMember, sizeof(LibJingleEx::TDmsMember) );
                        }
                    }
                    else if ( bDmsDeleted )
                    {
                        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_DEL_DMS_MEMBER, app, app->m_pUserData, 
                                (unsigned char *)&pMember->dwId,  sizeof(pMember->dwId) );
                    }
                    else if ( bMemberUiChanged )
                    {
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::TDmsMember  tDmsMember;
                            tDmsMember.dwMemberId = pMember->dwId;
                            tDmsMember.dwGroupId  = pGroup->dwGroupId;
                            tDmsMember.eType      = (LibJingleEx::EDmsMemberType)pMember->nType;
                            LibJingleEx::TJid2StrJid( pMember->tJid, tDmsMember.szJid, LibJingleEx::MAX_JID_LEN );
                            strncpy( tDmsMember.szName, pMember->szName, LibJingleEx::MAX_DMS_MEMBER_NAME_LEN );
                            strncpy( tDmsMember.szReserved[RESERVERD_BITRATE_INDEX], pMember->szBitRate, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN);

                            g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_MODIFY_DMS_MEMBER, app, app->m_pUserData, 
                                (unsigned char *)&tDmsMember, sizeof(LibJingleEx::TDmsMember) );
                        }
                    }
#endif


                    std::vector<const char*>::iterator it_str = find_if( pMg->m_Groups.begin(), 
                        pMg->m_Groups.end(), FindStringObj(pGroup->szName) );
                    if ( it_str == pMg->m_Groups.end() )
                    {
                        pMg->m_Groups.push_back( pGroup->szName );
                    }
                }

                if ( child )
                {
                    child = child->NextElement();
                }

                if ( bHandleGroupElement )
                {
                    dwGroupIndex++;
                }
            }

            std::vector<const char*>::iterator it_sub;
            for ( it_sub = vGroups.begin(); it_sub != vGroups.end(); it_sub++ )
            {
                // ɾ����ĳЩ��Ա
                if ( g_tEvents.pOnRosterDel )
                {
                    g_tEvents.pOnRosterDel( app, *it_sub, &tJid );
                }
                else if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData msgData;
                    msgData.Append( (unsigned char *)*it_sub,   LibJingleEx::MAX_GROUP_NAME_LEN );
                    msgData.Append( (unsigned char *)&tJid,     sizeof(tJid) );

                    g_pfnOnMessage( LibJingleEx::ON_ROSTER_DEL, app, app->m_pUserData, 
                                    msgData.GetMsgData(), msgData.GetMsgLength() );
                }

                LibJingleEx::PTGroup pGroup = app->m_pFirstGroup;
                LibJingleEx::PTGroup pLastGroup = 0;

                while( pGroup )
                {
                    if ( 0 == strcmp( pGroup->szName, *it_sub ) )
                    {
                        break;
                    }

                    pLastGroup = pGroup;
                    pGroup = pGroup->pNext;
                }
                // assert( pGroup );
                if ( 0 == pGroup )
                {
                    break;
                }

                LibJingleEx::PTMember pMember = pGroup->pChild;
                LibJingleEx::PTMember pLastMember = 0;
                while( pMember )
                {
                    if ( IfJidEqual( pMember->tJid, tJid ) )
                    {
                        break;
                    }

                    pLastMember = pMember;
                    pMember = pMember->pNext;
                }
                // assert( pMember );
                if ( 0 == pMember )
                {
                    break;
                }

#if !DMS_CONTACTS_SWITCH
                if ( !IfUnscribedContact(pMember) )
                {
                    if ( g_pfnOnMessage )
                    {
                        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_DEL_DMS_MEMBER, app, app->m_pUserData, 
                                         (unsigned char *)&pMember->dwId,  sizeof(pMember->dwId) );
                    }
                }
#endif

                if ( 0 == pLastMember )
                {
                    pGroup->pChild = pMember->pNext;
                }
                else
                {
                    pLastMember->pNext = pMember->pNext;
                }
                delete pMember;

#if !DMS_CONTACTS_SWITCH
                if ( pGroup->bToBeDeleted && 0 == pGroup->pChild )
                {
                    // ֪ͨ��ɾ��
                    if ( g_pfnOnMessage )
                    {
                        g_pfnOnMessage( LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP, app, app->m_pUserData, 
                                        (unsigned char *)&pGroup->dwGroupId,  sizeof(pGroup->dwGroupId) );
                    }
                    // END(֪ͨ��ɾ��)
                    app->RemoveGroup( pGroup );
                }
#endif              
            }

            item = item->NextElement();
        }

        // ����result
        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_RESULT );
        iq->SetAttr( buzz::QN_ID, stanza->Attr( QN_ID ) );
        GetClient()->SendStanza( iq );
        delete iq;

        app->SaveRoster2File();
    }
    else
    {
        // ������Ͳ���result
        if ( stanza->Attr( QN_TYPE ) != "result" )
        {
            return false;
        }


        // �������ƽ̨�Ļ�Ӧresult
        if ( !IfExistIqId( stanza->Attr(QN_ID).c_str(), true ) )
        {
            return false;
        }

        const XmlElement * element = stanza->FirstElement();
        //if ( 0 == element || element->Name() != QN_ROSTER_QUERY )
        //{
        //    return false;
        //}

        LibJingleEx::PTGroup & pFirstGroup = app->m_pFirstGroup;
        const XmlElement * item = 0;

        // ����汾�����
        if ( app->m_roster_root && ( 0 == element || (element->Name() == QN_ROSTER_QUERY && element->Attr(QN_VER) == app->m_roster_root->Attr(QN_VER)) ) )
        {
            const XmlElement * groups_element   = app->m_roster_root->FirstNamed( QName("", "groups") );
            const XmlElement * contacts_element = app->m_roster_root->FirstNamed( QName("", "contacts") );    

            // ������
            if ( groups_element )
            {
                const XmlElement * group_element = groups_element->FirstElement();
                while( group_element )
                {
                    LibJingleEx::PTGroup pGroup = app->AddGroup( Utf8ToAnsi( group_element->BodyText().c_str(), buf, MAX_BUFF_SIZE ), 0 );
                    group_element = group_element->NextElement();
                }
            }

            ////// ��ϵ������
            // û����ϵ������
            if ( 0 == contacts_element )
            {
                return true;
            }

            item = contacts_element->FirstElement();
        }
        // ����汾�Ų�ͬ
        else
        {
            if ( 0 == element || element->Name() != QN_ROSTER_QUERY )
            {
                return false;
            }

            strncpy( app->m_szRosterVer, element->Attr(QN_VER).c_str(), sizeof(app->m_szRosterVer) );
            app->m_bRosterChanged = TRUE;
            item = element->FirstElement();
        }

        // �������Ϊ�յ���(δ����)
        app->AddGroup( "", 0 );


        while( 0 != item )
        {
            // ��������
            char szMemberName[LibJingleEx::MAX_MEMBER_NAME_LEN];
            Utf8ToAnsi( item->Attr( QName("", "name") ).c_str(), szMemberName, LibJingleEx::MAX_MEMBER_NAME_LEN );

            // ����subscription����
            int nSubscription = Subscription2int( item->Attr( QName("", "subscription") ).c_str() );
            // ����ask����
            int nAsk = Ask2int( item->Attr( QName("", "ask") ).c_str() );
            // ����jid����(bare jid)
            LibJingleEx::TJid tJid;
            LibJingleEx::StrJid2TJid( Utf8ToAnsi(item->Attr( QName("", "jid") ).c_str(), buf, MAX_BUFF_SIZE), tJid );

            // ��������
            char  szBitRate[LibJingleEx::MAX_MEMBER_BITRATE_LEN] = { 0 };
            const XmlElement * bitrate = GetFirstNamedElementByLocalPart( item, "bitrate" );
            if ( bitrate )
            {
                Utf8ToAnsi( bitrate->Attr(QN_VALUE).c_str(), szBitRate, LibJingleEx::MAX_MEMBER_BITRATE_LEN );
            }

            int  nType = 0;
            const XmlElement * type_ele = GetFirstNamedElementByLocalPart( item, "type" );
            if ( type_ele )
            {
                sscanf( type_ele->Attr(QN_VALUE).c_str(), " %d",  &nType );
            }


            PTMemberGroup pMg = 0;

            const XmlElement * child        = item->FirstElement();
            unsigned long dwGroupIndex      = 0;
            BOOL               bFirstGroup  = TRUE;

            while( 0 != child || 0 == dwGroupIndex )
            {
                BOOL  bHandleGroupElement = FALSE;
                char  szGroupName[LibJingleEx::MAX_GROUP_NAME_LEN] = { 0 };

                if ( 0 == child )
                {
                    bHandleGroupElement = TRUE;
                }
                else if ( child->Name().LocalPart() == "group" )
                {
                    Utf8ToAnsi( child->BodyText().c_str(), szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );
                    bHandleGroupElement = TRUE;
                }
                
                if ( bHandleGroupElement )
                {
                    LibJingleEx::PTGroup pGroup = app->AddGroup( szGroupName, 0 );

                    LibJingleEx::TMember tMember;
                    LibJingleEx::PTMember pMember = &tMember;

                    // ��������
                    strcpy( pMember->szName, szMemberName );
                    // ����subscription����
                    pMember->nSubscription = nSubscription;
                    // ����ask����
                    pMember->nAsk = nAsk;
                    // ����jid����(bare jid)
                    memcpy( &pMember->tJid, &tJid, sizeof(LibJingleEx::TJid) );
                    // bit rate
                    strcpy( pMember->szBitRate, szBitRate );
                    // ����
                    pMember->nType = nType;

                    if ( !app->AddMember( pGroup, pMember ) )
                    {
                        MYLOG("== Failed to AddMember(), %s, %u \n", __FILE__, __LINE__ );
                    }

                    // for MemberGroup
                    MG_AddGroup( app, pMg, &tJid, pGroup->szName, bFirstGroup );
                    // end for MemberGroup
                    bFirstGroup = FALSE;
                }

                if ( child )
                {
                    child = child->NextElement();
                }

                if ( bHandleGroupElement )
                {
                    dwGroupIndex++;
                }
            } // end of while( 0 != group )


            assert( pMg );

            strcpy( pMg->m_tData.szName, szMemberName );
            pMg->m_tData.nSubscription = nSubscription;
            pMg->m_tData.nAsk          = nAsk;
            pMg->m_tData.pData         = 0;
            strcpy( pMg->m_tData.szBitRate, szBitRate );
            pMg->m_tData.nType         = nType;



            // ��ȫ����û�й�ע����û�������ע����ɾ���ú���
            if ( 0 == nSubscription && 0 == nAsk && LibJingleEx::DMS_MEMBER_TYPE_JID == nType )
            {
                char  szId[MAX_RAND_ID_LENGTH];

                buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
                iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
                iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

                buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
                iq->AddElement( query );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ROSTER_ITEM, true );
                query->AddElement( item );

                item->SetAttr( buzz::QN_JID, AnsiToUtf8( GetJid(tJid), buf, MAX_BUFF_SIZE ) );
                item->SetAttr( buzz::QN_SUBSCRIPTION, "remove" );

                c->SendStanza( iq );
                delete iq;
            }


            item = item->NextElement();
        } // end of while( 0 != item )

        if ( g_tEvents.pOnRosterInfo )
        {
            g_tEvents.pOnRosterInfo( app, pFirstGroup );
        }
        else if ( g_pfnOnMessage )
        {
            g_pfnOnMessage( LibJingleEx::ON_ROSTER_INFO, app, app->m_pUserData, (unsigned char *)&pFirstGroup, sizeof(pFirstGroup) );
        }

        // �ѻ�ȡ��roster����
        app->m_bGetRosterData = TRUE;

        // ����roster���ݵ��ļ�
        app->SaveRoster2File();

#if !DMS_CONTACTS_SWITCH
        if ( g_pfnOnMessage )
        {
            g_pfnOnMessage( LibJingleEx::ON_MEMBERS_DATA_READY, app, app->m_pUserData, 0, 0  );
        }
#endif

    }	

    return true;
}

// <iq type='get' id='purplec2262867'><query xmlns='jabber:iq:roster'/></iq>
void MyRosterTask::GetRoster( const char * szVersion /*= 0*/ )
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);

    buzz::XmlElement * roster_query = new buzz::XmlElement(buzz::QN_ROSTER_QUERY, true);
    iq->AddElement( roster_query );

    if ( szVersion )
    {
        roster_query->SetAttr( QN_VER, szVersion );
    }

    client->SendStanza( iq );
    delete iq;


    TIqId tIqId;
    tIqId.dwType = IQ_TYPE_ROSTER;
    strcpy( tIqId.szIqId, randid );
    tIqId.dwSubType = ROSTER_GET;

    AddIqId( &tIqId );
}

void  OnDmsAddGroup( CLibJinglexApp * app, const XmlElement * item )
{
    unsigned long dwId = 0;
    unsigned long dwParentId = 0;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &dwId );
    sscanf( item->Attr(QN_DMS_PARENT_ID).c_str(), " %u", &dwParentId );

    // �����֪ͨ����id���ǵ�ǰ���group id + 1
    if ( app->m_dwDmsMaxGroupId + 1 != dwId )
    {
        return;
    }

    char buf[MAX_BUFF_SIZE];
    vDmsGroup & v = app->m_vDmsGroups;
    vDmsGroup::iterator it;

    if ( dwParentId > 0 )
    {
        // ��ѯ���޸ø�����
        it = find_if( v.begin(), v.end(), FindDmsGroupObj( dwParentId ) );
        // û�иø�����
        if ( it == v.end() )
        {
            return;
        }
    }

    // ��ѯ���޸���
    it = find_if( v.begin(), v.end(), FindDmsGroupObj( dwId ) );
    // ����и���
    if ( it != v.end() )
    {
        return;
    }

    app->m_dwDmsMaxGroupId++;

    LibJingleEx::PTDmsGroup pNewGroup = new LibJingleEx::TDmsGroup;
    pNewGroup->dwGroupId  = dwId;
    pNewGroup->dwParentId = dwParentId;
    strncpy( pNewGroup->szGroupName, Utf8ToAnsi(item->Attr( QN_DMS_NAME ).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_GROUP_NAME_LEN );
    v.push_back( pNewGroup );

#if DMS_CONTACTS_SWITCH
    // ֪ͨ���������������
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage( LibJingleEx::ON_NOTIFY_ADD_DMS_GROUP, app, app->m_pUserData, 
            (unsigned char *)pNewGroup, sizeof(LibJingleEx::TDmsGroup) );
    }
#endif
}

void  OnDmsDeleteGroup( CLibJinglexApp * app, const XmlElement * item, BOOL bMemberReserverd = FALSE )
{
    unsigned long dwId = 0;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &dwId );


    vDmsGroup & v = app->m_vDmsGroups;
    vDmsGroup::iterator it, it_find;

    // ��ѯ���޸���
    it_find = find_if( v.begin(), v.end(), FindDmsGroupObj( dwId ) );
    // û�и���
    if ( it_find == v.end() )
    {
        return;
    }

    // �鿴�����Ƿ�������
    for ( it = v.begin(); it != v.end(); it++ )
    {
        LibJingleEx::PTDmsGroup  p = *it;

        // ������
        if ( p->dwParentId == dwId )
        {
            return;
        }
    }

    // ɾ�������µ�������ϵ��(���߱�����Ա��δ����)
    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = app->m_vDmsMembers[i];
        vDmsMember::iterator it_m;

        // ����˽�п�i
        for ( it_m = vMembers.begin(); it_m != vMembers.end(); )
        {
            LibJingleEx::PTDmsMember pMember = *it_m;

            // ����������ĳ�Ա
            if ( pMember->dwGroupId == dwId )
            {
                // ���Ҫ��������δ���顱
                if ( bMemberReserverd )
                {
                    // ����ڡ�δ���顱���ҵ��ó�Ա
                    if ( app->IfExistDmsMemberInTheGroup( SPECIAL_GROUP_SN, pMember->szJid ) )
                    {
                        it_m = vMembers.erase( it_m );
                        delete pMember;
                    }
                    else
                    {
                        pMember->dwGroupId = SPECIAL_GROUP_SN;
                        it_m++;
                    }
                }
                else
                {
                    it_m = vMembers.erase( it_m );
                    delete pMember;
                }
            }
            else
            {
                it_m++;
            }
        }
    }
    

    // ɾ������
    delete *it_find;
    v.erase( it_find );

#if DMS_CONTACTS_SWITCH
    // ֪ͨ�����ɾ������
    if ( g_pfnOnMessage )
    {
        LibJingleEx::CMsgData   msgData;
        msgData.Append( (unsigned char *)&dwId,             sizeof(dwId) );
        msgData.Append( (unsigned char *)&bMemberReserverd, sizeof(bMemberReserverd) );

        g_pfnOnMessage( LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP, app, app->m_pUserData, 
                        msgData.GetMsgData(), msgData.GetMsgLength() );
    }
#endif
}

void  OnDmsModifyGroup( CLibJinglexApp * app, const XmlElement * item )
{
    char buf[MAX_BUFF_SIZE];
    LibJingleEx::TDmsGroup tGroup;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &tGroup.dwGroupId );
    strncpy( tGroup.szGroupName, Utf8ToAnsi(item->Attr(buzz::QN_DMS_NAME).c_str(), buf, MAX_BUFF_SIZE), 
             LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

    vDmsGroup & v = app->m_vDmsGroups;
    vDmsGroup::iterator it_find;

    // ��ѯ���޸���
    it_find = find_if( v.begin(), v.end(), FindDmsGroupObj( tGroup.dwGroupId ) );
    // û�и���
    if ( it_find == v.end() )
    {
        return;
    }


    // �޸�������
    LibJingleEx::PTDmsGroup pGroup = *it_find;
    strcpy( pGroup->szGroupName, tGroup.szGroupName );

#if DMS_CONTACTS_SWITCH
    // ֪ͨ������޸�����
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage( LibJingleEx::ON_NOTIFY_MODIFY_DMS_GROUP, app, app->m_pUserData, 
                        (unsigned char *)&tGroup, sizeof(tGroup) );
    }
#endif
}

void  OnDmsMoveGroup( CLibJinglexApp * app, const XmlElement * item )
{

}

void  OnDmsAddMember( CLibJinglexApp * app, const XmlElement * item )
{
    char buf[MAX_BUFF_SIZE];
    LibJingleEx::TDmsMember    tMember;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &tMember.dwMemberId );
    sscanf( item->Attr(QN_DMS_PARENT_ID).c_str(), " %u", &tMember.dwGroupId );
    sscanf( item->Attr(QN_DMS_TYPE).c_str(),      " %u", &tMember.eType );
    strncpy( tMember.szName, Utf8ToAnsi(item->Attr(QN_DMS_NAME).c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_NAME_LEN );
    strncpy( tMember.szJid,  Utf8ToAnsi(item->Attr(QN_DMS_JID).c_str(),  buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_JID_LEN );

    strncpy( tMember.szReserved[0],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD1).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[1],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD2).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[2],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD3).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[3],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD4).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );


    // �����֪ͨ�³�Աid���ǵ�ǰ���member id + 1
    if ( app->m_dwDmsMaxMemberId + 1 != tMember.dwMemberId )
    {
        return;
    }

    vDmsGroup & vGroups = app->m_vDmsGroups;
    vDmsGroup::iterator it_group;

    // ��ѯ������������
    it_group = find_if( vGroups.begin(), vGroups.end(), FindDmsGroupObj( tMember.dwGroupId ) );
    // û�и���
    if ( it_group == vGroups.end() )
    {
        return;
    }


    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = app->m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        for ( it_member = vMembers.begin(); it_member != vMembers.end(); it_member++ )
        {
            LibJingleEx::PTDmsMember p = *it_member;
            // �������ͬ�������ͬ��jid�ĳ�Ա����
            if ( p->dwGroupId == tMember.dwGroupId && 0 == strcmp( p->szJid, tMember.szJid ) )
            {
                return;
            }
        }
    }
    
    app->m_dwDmsMaxMemberId++;

    LibJingleEx::PTDmsMember ptNewMember = new LibJingleEx::TDmsMember;
    memcpy( ptNewMember, &tMember, sizeof(LibJingleEx::TDmsMember) );

    unsigned long dwBlockBits = 0;
    app->AddDmsMember2Block( ptNewMember, dwBlockBits );
   
#if DMS_CONTACTS_SWITCH
    // ֪ͨ������������³�Ա
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_ADD_DMS_MEMBER, app, app->m_pUserData, 
                        (unsigned char *)&tMember, sizeof(tMember) );
    }
#endif
}

void  OnDmsDeleteMember( CLibJinglexApp * app, const XmlElement * item )
{
    unsigned long dwId = 0;
    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &dwId );

    // û���ҵ����Ҫɾ���ĳ�Ա
    if ( -1 == app->DeleteDmsMember( dwId ) )
    {
        return;
    }

#if DMS_CONTACTS_SWITCH
    // ֪ͨ�����ɾ���˳�Ա
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_DEL_DMS_MEMBER, app, app->m_pUserData, 
                         (unsigned char *)&dwId, sizeof(dwId) );
    }
#endif
}

void  OnDmsModifyMember( CLibJinglexApp * app, const XmlElement * item )
{
    char buf[MAX_BUFF_SIZE];
    LibJingleEx::TDmsMember    tMember;
    unsigned long dwId = 0;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &dwId );
    strncpy( tMember.szName, Utf8ToAnsi(item->Attr(QN_DMS_NAME).c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_NAME_LEN );

    strncpy( tMember.szReserved[0],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD1).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[1],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD2).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[2],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD3).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
    strncpy( tMember.szReserved[3],  Utf8ToAnsi(item->Attr(QN_DMS_RESERVERD4).c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );


    LibJingleEx::PTDmsMember p = app->GetDmsMember( dwId );
    
    // ���û���ҵ�����id�ĳ�Ա
    if ( 0 == p )
    {
        return;
    }

    // �޸����о�����ͬjid�ĳ�Ա
    strcpy( tMember.szJid, p->szJid );
    app->ModifyDmsMemberByJid( &tMember );

    
#if DMS_CONTACTS_SWITCH
    // ֪ͨ������޸��˳�Ա
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_MODIFY_DMS_MEMBER, app, app->m_pUserData, 
                        (unsigned char *)p, sizeof(LibJingleEx::TDmsMember) );
    }
#endif
}

void  OnDmsMoveMember( CLibJinglexApp * app, const XmlElement * item )
{
    unsigned long dwId       = 0;
    unsigned long dwParentId = 0;

    sscanf( item->Attr(QN_DMS_ID).c_str(),        " %u", &dwId );
    sscanf( item->Attr(QN_DMS_PARENT_ID).c_str(), " %u", &dwParentId );

    LibJingleEx::PTDmsMember p = app->GetDmsMember( dwId );
    // ���û���ҵ�
    if ( 0 == p )
    {
        return;
    }
    
    // �������Ҫ�ƶ�
    if ( p->dwGroupId == dwParentId )
    {
        return;
    }

    vDmsGroup & vGroup = app->m_vDmsGroups;
    vDmsGroup::iterator it_group;

    // �鿴Ŀ�����Ƿ����
    it_group = find_if( vGroup.begin(), vGroup.end(), FindDmsGroupObj(dwParentId) );
    // ���Ŀ���鲻����
    if ( vGroup.end() == it_group )
    {
        return;
    }

    // ���Ŀ�������Ѿ���ͬ��jid�Ĵ���
    if ( app->IfExistDmsMemberInTheGroup( dwParentId, p->szJid ) )
    {
        return;
    }

    unsigned long  dwOldGroupId = p->dwGroupId;
    p->dwGroupId = dwParentId;

#if DMS_CONTACTS_SWITCH
    // ֪ͨ������޸��˳�Ա
    if ( g_pfnOnMessage )
    {
        //g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_MOVE_DMS_MEMBER, app, app->m_pUserData, 
        //                 (unsigned char *)p, sizeof(LibJingleEx::TDmsMember) );
        
        LibJingleEx::CMsgData  cMsg;
        cMsg.Append( (unsigned char *)&dwOldGroupId,    sizeof(dwOldGroupId) );
        cMsg.Append( (unsigned char *)p,                sizeof(LibJingleEx::TDmsMember) );

        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_MOVE_DMS_MEMBER, app, app->m_pUserData, 
                         cMsg.GetMsgData(), cMsg.GetMsgLength() );
    }
#endif
}

void  OnSetChatroomHistory( CLibJinglexApp * app, const XmlElement * item )
{
    const std::string  & room_id   =  item->Attr( buzz::QN_ID );
    const std::string  & service   =  item->Attr( buzz::QN_CHATROOM_SERVICE );
    const std::string  & msg_id    =  item->BodyText();

    vRoom::iterator it_find = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                       FindRoomByFullId( room_id.c_str(), service.c_str() ) );
    // û���������
    if ( it_find == app->m_Rooms.end() )
    {
        return;
    }

    PTRoom pRoom = *it_find;

    // ���仹û��׼����
    if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
    {
        return;
    }

    // ������ǳ־��Է���
    if ( !pRoom->m_bPersistent )
    {
        return;
    }



    unsigned long dwEndIndex = 0;

    // �ӵ�ǰ�����������Ϣ��ʼ����Ϊ�Ѷ�
    if ( 0 == msg_id.length() )
    {
        dwEndIndex = pRoom->m_LatestMsgIds.GetEndPos();
    }
    // ��szLastReadMessageId֮ǰ����Ϣ�����Ϊ�Ѷ�
    else
    {
        dwEndIndex = pRoom->m_LatestMsgIds.FindIndex( msg_id.c_str() );
        // ����ҵ��ˣ��ƶ�pos������λ��
        if ( -1 != dwEndIndex )
        {
            dwEndIndex++;
        }
        else
        {
            dwEndIndex = pRoom->m_LatestMsgIds.GetEndPos();
        }
    }

    unsigned long  dwCnt = dwEndIndex - pRoom->m_LatestMsgIds.GetBeginPos();
    // ���û�п�����ӵ��Ѷ���ʷ��¼���
    if ( 0 == dwCnt )
    {
        return;
    }


    std::vector< CRoomReadMsgs *>::iterator it_history_room;
    std::vector< CRoomReadMsgs *> & w = app->m_vRoomReadMsgs;

    CRoomReadMsgs * pHistoryRoom = 0;
    it_history_room = find_if( w.begin(), w.end(), FindHistoryRoom(pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );
    // ���û���ҵ�
    if ( it_history_room == w.end() )
    {
        pHistoryRoom = new CRoomReadMsgs( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
        app->m_vRoomReadMsgs.push_back( pHistoryRoom );
    }
    else
    {
        pHistoryRoom = *it_history_room;
    }

    pHistoryRoom->m_room.Clear();


    // �����µ���Ϣid����it_end�������20�������Ƶ��Ѷ���ʷ��¼��
    unsigned long i;
    for ( i = pRoom->m_LatestMsgIds.GetBeginPos(); i != dwEndIndex; i++ )
    {
        pHistoryRoom->m_room.AddMessageId( pRoom->m_LatestMsgIds[i] );
        app->m_bRoomReadMsgsChanged = TRUE;
        app->m_dwChatroomHistoryVersion++;
        app->SaveChatRoomHistory2Local();
    }
    // END �����µ���Ϣ����it_end����20�������Ƶ�m_vLastMsgIds
}

//void  OnAddRosterGroup( CLibJinglexApp * app, const XmlElement * item,  unsigned long dwVersion )
//{
//    char buf[MAX_BUFF_SIZE];
//    const std::string  & group_name   =  item->Attr( buzz::QN_DMS_NAME );
//    
//    BOOL  bAdded = FALSE;
//    LibJingleEx::PTGroup pRosterGroup = app->AddGroup( Utf8ToAnsi( group_name.c_str(), buf, MAX_BUFF_SIZE ), &bAdded );
//
//    // ���ȷʵ����ˣ�֪ͨ�ϲ�Ӧ��
//    if ( bAdded )
//    {
//        app->m_bRosterChanged = TRUE;
//        app->SaveRoster2File();
//
//        if ( g_pfnOnMessage )
//        {
//            LibJingleEx::TDmsGroup  tGroup;
//            strncpy( tGroup.szGroupName, pRosterGroup->szName, sizeof(tGroup.szGroupName) );
//            tGroup.dwGroupId = pRosterGroup->dwGroupId;
//
//            g_pfnOnMessage( LibJingleEx::ON_NOTIFY_ADD_DMS_GROUP, app, app->m_pUserData, 
//                            (unsigned char *)&tGroup, sizeof(LibJingleEx::TDmsGroup) );
//        }
//    }
//}

void  OnChangeRoomName( CLibJinglexApp * app, const XmlElement * item )
{
    char buf[MAX_BUFF_SIZE];

    std::string  room_id      =  Utf8ToAnsi( item->Attr( buzz::QN_ID ).c_str(), buf, MAX_BUFF_SIZE );
    std::string  service      =  Utf8ToAnsi( item->Attr( buzz::QN_CHATROOM_SERVICE ).c_str(), buf, MAX_BUFF_SIZE );
    std::string  room_name    =  Utf8ToAnsi( item->BodyText().c_str(), buf, MAX_BUFF_SIZE );

    vRoom::iterator it_find = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
        FindRoomByFullId( room_id.c_str(), service.c_str() ) );
    // û���������
    if ( it_find == app->m_Rooms.end() )
    {
        return;
    }

    PTRoom pRoom = *it_find;

    if ( g_pfnOnMessage )
    {
        // ֪ͨ�ϲ㷿��������Ϣ
        if ( g_pfnOnMessage )
        {
            char szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN] = { 0 };
            strncpy( szRoomName, room_name.c_str(), LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );

            LibJingleEx::CMsgData msgData;
            msgData.Append( (unsigned char *)&pRoom,     sizeof(pRoom) );
            msgData.Append( (unsigned char *)szRoomName, sizeof(szRoomName) );

            msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId) );
            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );


            g_pfnOnMessage( LibJingleEx::ON_ROOM_CONFIG_MSG, app, app->m_pUserData, 
                msgData.GetMsgData(), msgData.GetMsgLength() );
        }
        // END ֪ͨ�ϲ㷿��������Ϣ
    }
    
}


void  OnChatroomAddMember( CLibJinglexApp * app, const XmlElement * item, LibJingleEx::PTJid ptFromJid )
{   
    char buf[MAX_BUFF_SIZE];
    const std::string  &  member_jid     = item->Attr( buzz::QN_JID );
    const std::string  &  strAffiliation = item->Attr( buzz::QN_MUC_AFFILIATION );

    // ���������������
    vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), FindRoomByFullId(ptFromJid->sNode,ptFromJid->sDomain) );
    if ( app->m_Rooms.end() == it )
    {
        return;
    }

    PTRoom pRoom = *it;

    vRoomRoster & v = pRoom->m_vRoster;
    vRoomRoster::iterator it_r;

    LibJingleEx::TJid    tMemberJid;
    LibJingleEx::StrJid2TJid( Utf8ToAnsi(member_jid.c_str(), buf, MAX_BUFF_SIZE), tMemberJid );
    LibJingleEx::EAffiliation eAffiliation = StrAffi2Int( strAffiliation.c_str() );

    it_r = find_if ( v.begin(), v.end(), FindRoomRosterItem( &tMemberJid ) );
    if ( v.end() == it_r )
    {
        LibJingleEx::PTRoomRosterItem  pItem = new LibJingleEx::TRoomRosterItem;
        memcpy( &pItem->m_tJid, &tMemberJid, sizeof(LibJingleEx::TJid) );
        pItem->m_eAlliliation = eAffiliation;
        v.push_back( pItem );


        // ֪ͨ�����˷����Ա(������)
        if ( g_pfnOnMessage )
        {
            vRoomMember::iterator it_m;
            it_m = find_if( pRoom->m_vMembers.begin(), pRoom->m_vMembers.end(), FindRoomMemberByBareJidObj( &tMemberJid ) );
            // ���û���ҵ�����Ҫ��֪�ϲ�
            if ( it_m == pRoom->m_vMembers.end() )
            {
                
                LibJingleEx::CMsgData   msgData;
                unsigned long dwCnt = 1;

                msgData.Append( (unsigned char *)&pRoom,    sizeof(pRoom) );
                msgData.Append( (unsigned char *)pRoom->m_szRoomId,    sizeof(pRoom->m_szRoomId) );
                msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                msgData.Append( (unsigned char *)&dwCnt,    sizeof(dwCnt) );
                msgData.Append( (unsigned char *)pItem,     sizeof(LibJingleEx::TRoomRosterItem) );

                g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_ADDED, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );

            }
        }
        // END( ֪ͨ�����˷����Ա(������) )


        // ֪ͨ���������ӳ�Ա
        if ( g_pfnOnMessage )
        {
            LibJingleEx::CMsgData   msgData;

            msgData.Append(  (unsigned char *)&pRoom,        sizeof(pRoom)  );
            msgData.Append(  (unsigned char *)&tMemberJid,   sizeof(tMemberJid)  );
            msgData.Append(  (unsigned char *)pRoom->m_szRoomId,   sizeof(pRoom->m_szRoomId)  );
            msgData.Append(  (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName)  );

            g_pfnOnMessage( LibJingleEx::ON_CHATROOM_ADD_MEMBER_NOTIFY, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
        }
        // END
    }
}


void  OnChatroomDeleteMember( CLibJinglexApp * app, const XmlElement * item, LibJingleEx::PTJid ptFromJid )
{
    char buf[MAX_BUFF_SIZE];

    const std::string & member_jid = item->Attr( buzz::QN_JID );
    
    vRoom::iterator  it_find = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), FindRoomByFullId( ptFromJid->sNode, ptFromJid->sDomain ) );
    if ( it_find == app->m_Rooms.end() )
    {
        return;
    }

    PTRoom pRoom = *it_find;

    vRoomRoster & v = pRoom->m_vRoster;
    vRoomRoster::iterator it_r;

    LibJingleEx::TJid    tMemberJid;
    LibJingleEx::StrJid2TJid( Utf8ToAnsi(member_jid.c_str(), buf, MAX_BUFF_SIZE), tMemberJid );   

    it_r = find_if ( v.begin(), v.end(), FindRoomRosterItem( &tMemberJid ) );
    // ��������roster item
    if ( v.end() != it_r )
    {
        LibJingleEx::PTRoomRosterItem pRosterItem = *it_r;

        if ( g_pfnOnMessage )
        {
            LibJingleEx::CMsgData  msgData;

            msgData.Append( (unsigned char * )&pRoom,       sizeof(pRoom)  );
            msgData.Append( (unsigned char * )pRoom->m_szRoomId,       sizeof(pRoom->m_szRoomId)  );
            msgData.Append( (unsigned char * )pRoom->m_szGroupChatServiceName,       sizeof(pRoom->m_szGroupChatServiceName)  );

            unsigned long dwCnt = 1;
            msgData.Append( (unsigned char *)&dwCnt,        sizeof(dwCnt)   );

            msgData.Append( (unsigned char *)pRosterItem,   sizeof(LibJingleEx::TRoomRosterItem)  );
            
            g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_DELETED, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
        }
    
        delete pRosterItem;
        v.erase( it_r );
    }
}


void  OnNotifyInviterDecline( CLibJinglexApp * app, const XmlElement * item, LibJingleEx::PTJid ptFromJid )
{
    char buf[MAX_BUFF_SIZE];
    char szId[MAX_RAND_ID_LENGTH];

    char szRoomId  [LibJingleEx::MAX_CHATROOM_ID_LEN];
    char szService [LibJingleEx::MAX_SERVICE_NAME_LEN];
    char szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];

    Utf8ToAnsi( item->Attr( buzz::QN_ID ).c_str(),               szRoomId,   LibJingleEx::MAX_CHATROOM_ID_LEN );
    Utf8ToAnsi( item->Attr( buzz::QN_CHATROOM_SERVICE ).c_str(), szService,  LibJingleEx::MAX_SERVICE_NAME_LEN );
    Utf8ToAnsi( item->Attr( buzz::QN_NAME ).c_str(),             szRoomName, LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );


    int nReason = LibJingleEx::DECLINE_REASON_UNKNOWN;

    std::string  reason       = item->Attr( buzz::QN_REASON );
    if ( reason == DECLINE_PERSISTENT_ROOM )
    {
        nReason = LibJingleEx::DECLINE_REASON_MAX_PERSISTENT_ROOM_COUNT;
    }
    else if ( reason == DECLINE_TEMPORARY_ROOM )
    {
        nReason = LibJingleEx::DECLINE_REASON_MAX_TEMPORARY_ROOM_COUNT;
    }
    else if ( reason == DECLINE_FROM_WEIBO )
    {
        nReason = LibJingleEx::DECLINE_REASON_FROM_WEIBO;
    }

    
    // ֪ͨ�ϲ㷿��������Ϣ
    if ( g_pfnOnMessage )
    {
        LibJingleEx::CMsgData msgData;
        msgData.Append( (unsigned char *)szRoomId,      sizeof(szRoomId) );
        msgData.Append( (unsigned char *)szService,     sizeof(szService) );
        msgData.Append( (unsigned char *)szRoomName,    sizeof(szRoomName) );
        msgData.Append( (unsigned char *)&nReason,      sizeof(nReason) );
        msgData.Append( (unsigned char *)ptFromJid,     sizeof(LibJingleEx::TJid) );     // �ܾ�����˭

        g_pfnOnMessage( LibJingleEx::ON_DECLINE_INVITING, app, app->m_pUserData, 
            msgData.GetMsgData(), msgData.GetMsgLength() ); 
    }
    // END ֪ͨ�ϲ㷿��������Ϣ


    // �鿴��������Ƿ����
    vRoom::iterator  it_find = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), FindRoomByFullId( szRoomId, szService ) );

    // �������ֱ��ɾ���ó�Ա
    if ( it_find != app->m_Rooms.end() )
    {
        PTRoom pRoom = *it_find;

        DeleteChatroomMember( pRoom, ptFromJid, app->m_pXmppThread->client() );
    }
    // ��������ڣ��Ȼ�������
    else
    {
        PTDeclineItem  pItem = new TDeclineItem;
        strcpy( pItem->szRoomId,     szRoomId );
        strcpy( pItem->szService,    szService );
        memcpy( &pItem->tDeclineJid, ptFromJid,   sizeof(LibJingleEx::TJid) );
        pItem->tDeclineJid.sResource[0] = '\0';

        std::vector<PTDeclineItem>::iterator  it_d;
        it_d = find_if( app->m_vDeclineItems.begin(), app->m_vDeclineItems.end(), FindDeclineItemObj(pItem) );
        // ���û�����decline item�� �Ȼ�������
        if ( it_d == app->m_vDeclineItems.end() )
        {
            app->m_vDeclineItems.push_back( pItem );
        }
        else
        {
            delete pItem;
        }
    }

}

void  OnDeletePersistentRoom( CLibJinglexApp * app, const XmlElement * item )
{
    const std::string & room_id = item->Attr( QN_ID );
    const std::string & service = item->Attr( QN_CHATROOM_SERVICE );

    vPersistentRoom::iterator it = find_if( app->m_vPersistentRooms.begin(), app->m_vPersistentRooms.end(), 
                                            FindPersistentRoom(room_id.c_str(), service.c_str()) );
    if ( it != app->m_vPersistentRooms.end() )
    {
        delete * it;
        app->m_vPersistentRooms.erase( it );
    }
}

void  OnAddPersistentRoom( CLibJinglexApp * app, const XmlElement * item )
{
    const std::string & room_id = item->Attr( QN_ID );
    const std::string & service = item->Attr( QN_CHATROOM_SERVICE );

    vPersistentRoom::iterator it = find_if( app->m_vPersistentRooms.begin(), app->m_vPersistentRooms.end(), 
        FindPersistentRoom(room_id.c_str(), service.c_str()) );
    if ( it == app->m_vPersistentRooms.end() )
    {
        PTPersistentRoom  pPersistent = new TPersistentRoom;
        strncpy( pPersistent->m_szRoomId,                room_id.c_str(),  sizeof(pPersistent->m_szRoomId) );
        strncpy( pPersistent->m_szGroupChatServiceName,  service.c_str(),  sizeof(pPersistent->m_szGroupChatServiceName) );

        app->m_vPersistentRooms.push_back( pPersistent );
    }
}


void  OnQueryGetStatus( CLibJinglexApp * app, const XmlElement * item )
{
    char buf[MAX_BUFF_SIZE];

    const std::string & query_jid = item->Attr( QN_ID );

    LibJingleEx::TJid   tJid;
    LibJingleEx::StrJid2TJid( query_jid.c_str(), tJid );

    std::vector<LibJingleEx::PTJid>::iterator it = find_if( app->m_vQueryStatusContact.begin(), app->m_vQueryStatusContact.end(), 
                   FindBareJidObj( &tJid ) );
    if ( it == app->m_vQueryStatusContact.end() )
    {
        LibJingleEx::PTJid  ptJid = new LibJingleEx::TJid;
        memcpy( ptJid, &tJid, sizeof(LibJingleEx::TJid) );
        app->m_vQueryStatusContact.push_back( ptJid );
    }

    // ����״̬
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
    LibJingleEx::TJid2StrJid( tJid, buf, MAX_BUFF_SIZE );
    message->SetAttr( buzz::QN_TO,   buf );
    message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "info status" );

    buzz::XmlElement * item_new = new buzz::XmlElement( buzz::QN_DMS_ITEM );
    operation->AddElement( item_new );

    const char * szPresence = GetPresShowString( app->m_ePres );
    if ( 0 != szPresence )
    {
        item_new->SetAttr( buzz::QN_SHOW, szPresence );
    }

    std::string   ability;
    if ( GetAbilityString( ability, app ) )
    {
        MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
        item_new->SetAttr( buzz::QN_VER, buf );
        item_new->SetAttr( buzz::QN_EXT, ability );
    }

    app->m_pXmppThread->pump()->client()->SendStanza( message );
    delete message;
}

void  OnInfoStatus( CLibJinglexApp * app, const XmlElement * item, const std::string & jid, const LibJingleEx::TJid * ptJid )
{
    const std::string & show = item->Attr(QN_SHOW);
    const std::string & exts = item->Attr(QN_EXT);
    const std::string & presence_version = item->Attr(QN_VER);

    bool fileshare_ability = false;
    bool audio_ability     = false;
    bool video_ability     = false;
    bool call_ability      = false;

    if ( ListContainsToken(exts, "share-v1") )
    {
        fileshare_ability = true;
    }

    if ( ListContainsToken(exts, "audio") )
    {
        audio_ability = true;
    }

    if ( ListContainsToken(exts, "video") )
    {
        video_ability = true;
    }

    if ( ListContainsToken(exts, "call") )
    {
        call_ability = true;
    }

    buzz::Jid     jid_( jid );
    buzz::Status  status;

    status.set_jid( jid_ );

    LibJingleEx::EPresence eShow = LibJingleEx::PRESENCE_ONLINE;;
    if ( show.length() > 0 )
    {
        eShow = StrShow2Int( show.c_str() );
    }

    status.set_show( (buzz::Status::Show)eShow );
    status.set_fileshare_capability(fileshare_ability);
    status.set_audio_capability( audio_ability );
    status.set_video_capability( video_ability );
    status.set_call_capability( call_ability );
    status.set_version(presence_version);


    std::vector<PTUnscribedContact> & v = app->m_vUnscribedContacts;
    std::vector<PTUnscribedContact>::iterator it = find_if( v.begin(), v.end(),  FindUnscribedContactObj(ptJid) );

    // ���δ��ע��ϵ����û�У�����ӽ���
    if ( eShow != LibJingleEx::PRESENCE_NONE && v.end() == it )
    {
        PTUnscribedContact  p = new TUnscribedContact;
        memcpy( &p->tJid, ptJid, sizeof(LibJingleEx::TJid) );
        v.push_back( p );
    }
    // ��������ߣ���ɾ��֮
    else if ( eShow == LibJingleEx::PRESENCE_NONE && v.end() != it )
    {
        delete *it;
        v.erase( it );
    }
    
    
    app->m_pXmppThread->OnStatusUpdate( status );

    
}

void  OnCustomConfigNotify( CLibJinglexApp * app, const XmlElement * first_item )
{
    char buf[MAX_BUFF_SIZE];

    const XmlElement * child = first_item;
    while( child )
    {
        LibJingleEx::TCustomConfigItem   tItem;

        Utf8ToAnsi( child->Attr(buzz::QN_NAME).c_str(), tItem.szKey,   sizeof(tItem.szKey) );
        Utf8ToAnsi( child->BodyText().c_str(),          tItem.szValue, sizeof(tItem.szValue) );

        // ��ȡ��ֵ
        char  szOldValue[LibJingleEx::MAX_CUSTOM_CONFIG_VALUE_LEN];
        memset( szOldValue, 0, sizeof(szOldValue) );
        strncpy( szOldValue, app->m_cConfig[tItem.szKey], LibJingleEx::MAX_CUSTOM_CONFIG_VALUE_LEN );

        app->m_cConfig.SetConfig( tItem.szKey, tItem.szValue );

        // ֪ͨ�ϲ�
        if ( g_pfnOnMessage )
        {
            LibJingleEx::CMsgData   msgData;

            msgData.Append( (unsigned char *)&tItem,      sizeof(tItem) );
            msgData.Append( (unsigned char *)szOldValue,  sizeof(szOldValue) );

            g_pfnOnMessage( LibJingleEx::ON_CUSTOM_CONFIG_CHANGE, app, app->m_pUserData, 
                            msgData.GetMsgData(), msgData.GetMsgLength()  );
        }

        child = child->NextElement();
    }
}

void  OnSyncChatroomNotify( CLibJinglexApp * app, const XmlElement * item, XmppClient * client )
{
    char buf[MAX_BUFF_SIZE];

    char     szRoomId[MAX_UNIQUE_ROOM_ID_LENGTH];
    char     szService[MAX_GROUPCHAT_SERVICE_NAME_LEN];
    char     szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];
    BOOL     bPersistent = FALSE;

    strncpy( szRoomId,   item->Attr(buzz::QN_ROOM_ID).c_str(),          sizeof(szRoomId) );
    strncpy( szService,  item->Attr(buzz::QN_CHATROOM_SERVICE).c_str(), sizeof(szService) );
    Utf8ToAnsi( item->Attr(buzz::QN_NAME).c_str(), szRoomName, sizeof(szRoomName) );
    bPersistent = ( item->Attr(buzz::QN_TYPE) == "persistent" ) ? TRUE : FALSE ;

    vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                       FindRoomByFullId( szRoomId, szService ) );
    // ������ڸ�������
    if ( it_room != app->m_Rooms.end() )
    {
        return;
    }

    

    


    // ���뷿��
    buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

    std::string sTmp;

    sTmp  = szRoomId;
    sTmp += '@';
    sTmp += szService;
    sTmp += '/';
    sTmp += client->jid().Str();

    presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

    const char * szPresence = GetPresShowString( app->m_ePres );
    if ( szPresence )
    {
        buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
        presence->AddElement( show );
        show->SetBodyText( szPresence );
    }

    buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
    presence->AddElement( priority );
    priority->SetBodyText("1");

    std::string  ability;
    if ( GetAbilityString( ability, app ) )
    {
        presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
        presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
        MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
        presence->AddAttr( buzz::QN_VER,  buf, 1);
        presence->AddAttr( buzz::QN_EXT, ability, 1);
    }

    buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
    presence->AddElement( x );


    // ����ÿ�����ȡ��20��stanza
    buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
    x->AddElement( history );
    history->SetAttr( buzz::QN_MAX_STANZAS, "0" );

    client->SendStanza( presence );
    delete presence;


    // ���TRoom����
    PTRoom pRoom = new TRoom( app );
    strcpy ( pRoom->m_szRoomId,               szRoomId );
    strcpy ( pRoom->m_szGroupChatServiceName, szService );
    strcpy( pRoom->m_szName, szRoomName );              
    pRoom->m_bPersistent = bPersistent;
    pRoom->m_eState      = ROOM_STATE_TRYING_PERSISTENT;
    strncpy( pRoom->m_szNick, client->jid().Str().c_str(), LibJingleEx::MAX_RESOURCE_LEN );
    app->m_Rooms.push_back( pRoom );
    // END ���TRoom����

}

void MyMsgTask::CheckMsgResp()
{
    XmppClient * client =  GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;

    unsigned long dwCurTick = GetCurTick();

    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end();  )
    {
        TIqId * pIqId = *it;

        if ( dwCurTick - pIqId->dwTick >= SEND_CHAT_MSG_FAILURE_TIMEOUT )
        {
            if ( g_pfnOnMessage )
            {
                if ( IQ_TYPE_P2P_MESSAGE == pIqId->dwType )
                {
                    BOOL   bSuccessfull = FALSE;
                    LibJingleEx::CMsgData   msgData;
                    msgData.Append( (unsigned char *)&pIqId->dwContextId,                sizeof(pIqId->dwContextId) );
                    msgData.Append( (unsigned char *)&bSuccessfull,                      sizeof(bSuccessfull) );
                    g_pfnOnMessage( LibJingleEx::ON_P2P_CHAT_MESSAGE_ACK, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
                }
            }
            it = m_vIqIds.erase( it );
            delete pIqId;
        }
        else
        {
            it++;
        }
    }

    vRoom::iterator it_r;
    for ( it_r = app->m_Rooms.begin(); it_r != app->m_Rooms.end(); it_r++ )
    {
        PTRoom pRoom = *it_r;
        
        std::vector<PTRoomChatMsgItem>::iterator it_s;
        for ( it_s = pRoom->m_vSendMsgs.begin(); it_s != pRoom->m_vSendMsgs.end(); )
        {
            PTRoomChatMsgItem  pItem = *it_s;

            if ( dwCurTick - pItem->dwTick >= SEND_CHAT_MSG_FAILURE_TIMEOUT )
            {
                if ( g_pfnOnMessage )
                {
                    BOOL   bSuccessfull = FALSE;
                    LibJingleEx::CMsgData msgData;
                    msgData.Append( (unsigned char *)&pRoom,                          sizeof(pRoom) );
                    msgData.Append( (unsigned char *)&pItem->dwContextId,             sizeof(pItem->dwContextId) );
                    msgData.Append( (unsigned char *)&bSuccessfull,                   sizeof(bSuccessfull) );
                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_GROUP_CHAT_MESSAGE_ACK, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
                }
                it_s = pRoom->m_vSendMsgs.erase( it_s );
                delete pItem;
            }
            else
            {
                it_s++;
            }
        }
    }
}

bool MyMsgTask::HandleStanza(const XmlElement * stanza)
{
    if ( stanza->Name() != QN_MESSAGE )
    {
        return false;
    }

    char   buf[MAX_BUFF_SIZE];
    char   szId[MAX_RAND_ID_LENGTH];

    const std::string & strJid   = stanza->Attr( QN_FROM );
    const std::string & strMsgId = stanza->Attr( QN_ID );
    const std::string & strType  = stanza->Attr( QN_TYPE );


    const XmlElement * muc_user_x = 0;
    const XmlElement * conf_x = 0;
    const XmlElement * body = 0;
    const XmlElement * addresses = 0;
    const XmlElement * delay = 0;
    const XmlElement * delay_x = 0;
    const XmlElement * err = 0;
    const XmlElement * subject = 0;
    const XmlElement * sysnotify = 0;
    const XmlElement * ibb_open = 0;
    const XmlElement * ibb_close = 0;
    const XmlElement * ibb_data = 0;

    const XmlElement * composing = 0;
    const XmlElement * paused = 0;

    const XmlElement * custom_body = 0;
    const XmlElement * receipts_request = 0;

    const XmlElement * received = 0;


    const XmlElement * e = stanza->FirstElement();
    while ( e  )
    {
        // if ( e->Name() == QN_BODY )
        if ( e->Name() == QN_XBODY )
        {
            body = e;
        }
        else if ( e->Name() == QN_MUC_USER_X )
        {
            muc_user_x = e;
        }
        else if ( e->Name() == QName(true, "jabber:x:conference", "x" ) )
        {
            conf_x = e;
        }
        else if ( e->Name() == QN_ADDRESSES )
        {
            addresses = e;
        }
        else if ( e->Name() == QN_DELAY )
        {
            delay = e;
        }
        else if ( e->Name() == QN_DELAY_X )
        {
            delay_x = e;
        }
        else if ( e->Name() == QN_ERROR )
        {
            err = e;
        }
        else if ( e->Name() == QN_SUBJECT )
        {
            subject = e;
        }
        else if ( e->Name() == QN_SYS_NOTIFY)
        {
            sysnotify = e;
        }
        else if ( e->Name() == QN_IBB_OPEN )
        {
            ibb_open = e;
        }
        else if ( e->Name() == QN_IBB_CLOSE )
        {
            ibb_close = e;
        }
        else if ( e->Name() == QN_IBB_DATA )
        {
            ibb_data = e;
        }
        else if ( e->Name() == QN_CHAR_COMPOSING )
        {
            composing = e;
        }
        else if ( e->Name() == QN_CHAR_PAUSED )
        {
            paused = e;
        }
        else if ( e->Name() == QN_COSTOM_BODY )
        {
            custom_body = e;
        }
        else if ( e->Name() ==  QN_RECEIPT_REQUEST  )
        {
            receipts_request = e;
        }
        else if ( e->Name() ==  QN_RECEIPT_RECEIVED  )
        {
            received = e;
        }

        e = e->NextElement();
    }

    XmppClient * client =  GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );


    LibJingleEx::TJid tJid;                               // message��from����ת����jid
    LibJingleEx::StrJid2TJid( strJid.c_str(), tJid );

    // �����error����
    if (  stanza->Attr(QN_TYPE) == "error" )
    {
        // �Դ��жϿ���Ϊ����chat��Ϣ����
        if ( 0 != body )
        {
            // MESSAGE overflow
            if ( err && err->Attr( QN_CODE ) == "500" )
            {
                if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData   msgData;
                    msgData.Append( (unsigned char *)&tJid,  sizeof(tJid) );
                    g_pfnOnMessage( LibJingleEx::ON_OFFLINE_MSG_OVERFLOW, app, app->m_pUserData, 
                        msgData.GetMsgData(), msgData.GetMsgLength()  );
                }
            }
        }

        return true;
    }

    BOOL  bOfflineMsg = (  (( 0 != delay ) || ( 0 != delay_x )) ? TRUE : FALSE  );

    // �վݻظ�
    if ( received )
    {
        TIqId  tIqId;
        // ����ҵ� iq id
        if( IfExistIqId( received->Attr(QN_ID).c_str(), true, &tIqId ) )
        {
            // ֪ͨ�ϲ㷢����Ϣ�ɹ�
            if ( tIqId.dwType == IQ_TYPE_P2P_MESSAGE )
            {
                if (g_pfnOnMessage)
                {
                    BOOL   bSuccessfull = TRUE;
                    LibJingleEx::CMsgData   msgData;
                    msgData.Append( (unsigned char *)&tIqId.dwContextId,                sizeof(tIqId.dwContextId) );
                    msgData.Append( (unsigned char *)&bSuccessfull,                     sizeof(bSuccessfull) );

                    g_pfnOnMessage( LibJingleEx::ON_P2P_CHAT_MESSAGE_ACK, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
                }
            }
        }
        return true;
    }

    // ��muc_user_x
    if ( muc_user_x )
    {
        e = muc_user_x->FirstElement();
        if ( 0 == e )
        {
            return false;
        }

        // �յ�����
        if ( e->Name() == QName(true,buzz::NS_MUC_USER,"invite") )
        {
            LibJingleEx::TJid   tInviterJid;
            LibJingleEx::StrJid2TJid( e->Attr(QN_FROM).c_str(), tInviterJid );

            if ( conf_x )
            {
                LibJingleEx::TJid  tConfJid;
                LibJingleEx::StrJid2TJid( conf_x->Attr(QN_JID).c_str(), tConfJid );

                BOOL               bPersistent = FALSE;
                int                nRoomType   = ROOM_TYPE_NORMAL;
                char               szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];


                const XmlElement * child = e->FirstElement();
                if ( child && child->Name() == QName(true, NS_MUC_USER, "reason") )
                {
                    const std::string & sTmp = child->BodyText();

                    // ������Զ����reason
                    if (      sTmp.length() >= MAGIC_PREFIX_LEN + 1 
                        &&  0 == strncmp( MAGIC_PREFIX, sTmp.c_str(), MAGIC_PREFIX_LEN )  )
                    {
                        unsigned long  dwPos = MAGIC_PREFIX_LEN;

                        // �־���֪ͨ
                        if ( '1' == sTmp[dwPos]  )
                        {
                            bPersistent = TRUE;
                        }
                        dwPos += 1;

                        if ( '1' == sTmp[dwPos] )
                        {
                            nRoomType = ROOM_TYPE_VISUAL;
                        }
                        dwPos += 1;

                        unsigned long dwSize = LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN;
                        LibJingleEx::DecodeBase64( (unsigned char *)szRoomName, &dwSize, sTmp.c_str() + dwPos );
                    }
                }

                // �����û��ȷ���ϴε�¼�ĳ־��������һ�ʣ����������ʱ�������Ϣ
                // ����ϲ�û��׼���ã�Ҳ����
                if ( !app->m_bPrivatePersistentRoomsCntConfirmed || !app->m_bUpperAppReady )
                {
                    PTInvit2ChatRoomMsg   ptInviteMsg = new TInvit2ChatRoomMsg;

                    // �����ߵ�bare jid
                    memcpy( &ptInviteMsg->m_tInviterJid, &tInviterJid, sizeof(LibJingleEx::TJid) );
                    ptInviteMsg->m_tInviterJid.sResource[0] = '\0';

                    ptInviteMsg->m_bPersistent = bPersistent;
                    ptInviteMsg->m_nType       = nRoomType;
                    STRNCPY( ptInviteMsg->m_szRoomId, tConfJid.sNode, MAX_UNIQUE_ROOM_ID_LENGTH );
                    STRNCPY( ptInviteMsg->m_szGroupChatServiceName, tConfJid.sDomain, MAX_GROUPCHAT_SERVICE_NAME_LEN );
                    strcpy( ptInviteMsg->m_szName, szRoomName );

                    app->m_vInviteMsgs.push_back( ptInviteMsg );
                    return true;
                }



                vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                    FindRoomByFullId( tConfJid.sNode, tConfJid.sDomain ) );

                PTRoom pRoom = 0;

                // ��û���������
                if ( it == app->m_Rooms.end() )
                {
                    pRoom = new TRoom(app);
                    strncpy( pRoom->m_szRoomId, tConfJid.sNode, MAX_UNIQUE_ROOM_ID_LENGTH );
                    strncpy( pRoom->m_szGroupChatServiceName, tConfJid.sDomain, MAX_GROUPCHAT_SERVICE_NAME_LEN );

                    pRoom->m_eState = ROOM_STATE_ON_INVITING;
                    
                    pRoom->m_bPersistent = bPersistent;
                    pRoom->m_nType       = nRoomType;
                    strcpy( pRoom->m_szName, szRoomName );

                    app->m_Rooms.push_back( pRoom );
                }
                // ����������䣬��������һ�ε�����
                else
                {
                    pRoom = *it;
                }

                // ����Ǳ�����̬����һ��֪ͨ(Ҳ���ǵ�һ��)
                if ( ROOM_STATE_ON_INVITING == pRoom->m_eState )
                {
                    if ( g_tEvents.pOnInvited )
                    {
                        g_tEvents.pOnInvited( app, pRoom, &tInviterJid, pRoom->m_bPersistent, pRoom->m_szName );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData   msgData;
                        msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&tInviterJid,          sizeof(tInviterJid) );
                        msgData.Append( (unsigned char *)&pRoom->m_bPersistent, sizeof(pRoom->m_bPersistent) );
                        msgData.Append( (unsigned char *)&pRoom->m_nType,       sizeof(pRoom->m_nType) );
                        msgData.Append( (unsigned char *)pRoom->m_szName,       sizeof(pRoom->m_szName) );
                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, 
                            sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_INVITED_TO_CHATROOM, app, app->m_pUserData, 
                            msgData.GetMsgData(),  msgData.GetMsgLength() );
                    }
                }                
            }
        }

        return true;
    }
    // �����subject
    else if ( subject )
    {
        vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
            FindRoomByFullId( tJid.sNode, tJid.sDomain ) );
        // û���������
        if ( it_room == app->m_Rooms.end() )
        {
            return true;
        }

        PTRoom pRoom = *it_room;
        const  char * szFormat = 0; 

        // ֻ����ʱȺ�͹̶�ȺȥȡRoster items
        if ( ROOM_TYPE_NORMAL !=  pRoom->m_nType )
        {
            return true;
        }

        // �����û��ȡ��subject
        if ( !pRoom->m_bGetSubject )
        {
            ClearVector( pRoom->m_vQueryQuitMembers );
            pRoom->ParseSubject( subject->BodyText() );
            strncpy( pRoom->m_szLastSubjectDelayMsgId, strMsgId.c_str(), sizeof(pRoom->m_szLastSubjectDelayMsgId) );


            // ׼��1���Ӻ������subject delay�Ƿ�����delay��Ϣ(��ʱ��Ϊ1���û����delay subject�����Ѿ����������delay subject��Ϣ)
            SubjectDelayMsg * pDelayMsg = new SubjectDelayMsg;
            strncpy( pDelayMsg->szRoomId,                tJid.sNode,           sizeof(pDelayMsg->szRoomId) );
            strncpy( pDelayMsg->szGroupChatServiceName,  tJid.sDomain,         sizeof(pDelayMsg->szGroupChatServiceName) );
            strncpy( pDelayMsg->szSubjectMsgId,          strMsgId.c_str(),     sizeof( pDelayMsg->szSubjectMsgId) );

            // �ӳ�1���Եȴ�subject�ȶ�
            app->m_pXmppThread->PostDelayed( INTERVAL_SUBJECT_DELAY, app->m_pXmppThread, MSG_SUBJECT_DELAY, pDelayMsg ); 
        }
        // �Ѿ�����ȶ���subject��
        else
        {
            // �������˳�(��Ϊ�˳��˵�jid��tJid.sResource)
            if ( subject->BodyText().length() > 0 )
            {
                LibJingleEx::TJid       tQuitMemberJid;
                LibJingleEx::StrJid2TJid( tJid.sResource,  tQuitMemberJid );

                // �Ƿ�ͬһ�˺�
                BOOL bSameAccount = ( (client->jid().node() == tQuitMemberJid.sNode) && (client->jid().domain() == tQuitMemberJid.sDomain ) ? TRUE : FALSE );

                // �Ƿ����Լ�������
                BOOL bSelf = FALSE;
                if ( bSameAccount )
                {
                    if ( client->jid().resource() == tQuitMemberJid.sResource )
                    {
                        bSelf = TRUE;
                    }
                }

                // ������Լ���ʲô������
                if ( bSelf )
                {
                    return true;
                }

                // �����ͬһ�˺�,��Ҳ�˳�������
                if ( bSameAccount )
                {
                    pRoom->m_bQuitOnSelfOwn = TRUE;
                    szFormat= "<presence to='%s@%s' type='unavailable'/>";
                    SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    client->SendRaw( buf );
                }

                // ������ͬһ�˺ŵ���
                // ֪ͨ�ϲ�Ӧ��
                if ( g_pfnOnMessage )
                {
                    vRoomRoster::iterator it_r = find_if ( pRoom->m_vRoster.begin(), pRoom->m_vRoster.end(), FindRoomRosterItem(&tQuitMemberJid) );
                    if ( pRoom->m_vRoster.end() != it_r )
                    {
                        LibJingleEx::PTRoomRosterItem  pRosterItem  =  *it_r;

                        LibJingleEx::CMsgData          msgData;
                        unsigned long                  dwCnt = 1;

                        msgData.Append( (unsigned char *)&pRoom,           sizeof(pRoom) );
                        msgData.Append( (unsigned char * )pRoom->m_szRoomId,       sizeof(pRoom->m_szRoomId)  );
                        msgData.Append( (unsigned char * )pRoom->m_szGroupChatServiceName,       sizeof(pRoom->m_szGroupChatServiceName)  );

                        msgData.Append( (unsigned char *)&dwCnt,           sizeof(dwCnt) );
                        msgData.Append( (unsigned char *)pRosterItem,      sizeof(LibJingleEx::TRoomRosterItem) );

                        g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_DELETED, app, app->m_pUserData, 
                                        msgData.GetMsgData(), msgData.GetMsgLength() );
                    }
                }

                // ����Լ��ǹ���Ա
                // ��Ҫ��ɾ����Ա�������subject
                if ( LibJingleEx::AFFILIATION_OWNER ==  pRoom->m_eAffiliation )
                {
                    assert( 0 == pRoom->m_vQueryQuitMembers.size() );

                    szFormat =              "<iq  id='%s' to='%s@%s' type='set'>"
                        "<query xmlns='http://jabber.org/protocol/muc#admin'>"
                        "<item jid='%s@%s' affiliation='none' />"
                        "</query>"
                        "</iq>";     

                    GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
                    SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, szId, pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, 
                        tQuitMemberJid.sNode, tQuitMemberJid.sDomain );

                    // ���������ɾ����Ա
                    client->SendRaw( buf );

                    // ���subject
                    szFormat = "<message to='%s@%s' type='groupchat'><subject/></message>";
                    SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    client->SendRaw( buf );
                }
                // �ǹ���Ա����˳���Ա��ӵ������˳��Ķ�����ȥ
                else
                {
                    std::vector<LibJingleEx::PTJid>::iterator it_x = find_if( pRoom->m_vQueryQuitMembers.begin(), 
                        pRoom->m_vQueryQuitMembers.end(), 
                        FindBareJidObj( &tQuitMemberJid )  );
                    if ( pRoom->m_vQueryQuitMembers.end() == it_x )
                    {
                        LibJingleEx::PTJid  ptNewJid = new LibJingleEx::TJid;
                        memcpy( ptNewJid, &tQuitMemberJid, sizeof(LibJingleEx::TJid) );
                        ptNewJid->sResource[0] = '\0';

                        pRoom->m_vQueryQuitMembers.push_back( ptNewJid );
                    }
                }
            }
            // ���ǹ���Ա�����subject
            else
            {
                // CAUSION: pRoom->m_vRoster�ж�Ӧ�ĳ�ԱҲҪɾ��

                std::vector<LibJingleEx::PTJid>::iterator it_x;
                for ( it_x = pRoom->m_vQueryQuitMembers.begin(); it_x != pRoom->m_vQueryQuitMembers.end(); it_x++ )
                {
                    LibJingleEx::PTJid  ptJid = *it_x;

                    vRoomRoster::iterator it_r = find_if( pRoom->m_vRoster.begin(), pRoom->m_vRoster.end(), FindRoomRosterItem( ptJid ) );
                    if ( it_r != pRoom->m_vRoster.end() )
                    {
                        delete *it_r;
                        pRoom->m_vRoster.erase( it_r );
                    }
                }

                ClearVector( pRoom->m_vQueryQuitMembers );
            }
        }

        return true;

    }
    // ����Ƕ���������
    else if ( ibb_open || ibb_close || ibb_data )
    {
        // �����Ⱥ�ĵ�ͼƬ����
        if ( strType == "groupchat" )
        {
            vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                               FindRoomByFullId( tJid.sNode, tJid.sDomain ) );

            // û���������
            if ( it_room == app->m_Rooms.end() )
            {
                return true;
            }

            PTRoom pRoom = *it_room;

            // ������ʷ(���20��)
            pRoom->m_LatestMsgIds.AddMessageId( strMsgId.c_str() );

            // ������ʷ����
            std::vector< CRoomReadMsgs *>::iterator   it_find;
            std::vector< CRoomReadMsgs *> &  w = app->m_vRoomReadMsgs;
            CRoomReadMsgs * pHistoryRoom = 0;

            it_find = find_if( w.begin(), w.end(), 
                FindHistoryRoom(pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );
            if ( it_find != w.end() )
            {
                pHistoryRoom = *it_find;

                // �鿴�Ƿ��Ѿ��Ķ�����
                unsigned long dwPos = pHistoryRoom->m_room.FindIndex( strMsgId.c_str() );
                // �Ѿ��Ķ���
                if ( -1 != dwPos )
                {
                    return true;
                }
            }

            // ����ǹ̶�Ⱥ����Ҫ������ϢΪ�Ѷ�
            if ( pRoom->m_bPersistent && pRoom->m_nType != ROOM_TYPE_SPECIAL )
            {
                if ( 0 == pHistoryRoom )
                {
                    pHistoryRoom = new CRoomReadMsgs( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    w.push_back( pHistoryRoom );
                }

                pHistoryRoom->m_room.AddMessageId( strMsgId.c_str() );
                app->m_bRoomReadMsgsChanged = TRUE;
                app->m_dwChatroomHistoryVersion++;
                app->SaveChatRoomHistory2Local();
            }


            // ���������������Ϣ�����Ҹ���Ϣ�����ߵģ�ֱ�ӷ���
            if ( pRoom->m_bNoOfflineMsg && bOfflineMsg )
            {
                return true;
            }

            // ���û������������ͼƬ�ı���·�����Ͳ����� 
            if ( '\0' == app->m_szGroupChatPicSaveDir[0] )
            {
                return true;
            }

            LibJingleEx::TJid   tMemberJid;
            GetMemberJidFromChatRoomResource( pRoom, tJid.sResource, &tMemberJid );


            std::vector< cPicSession * >::iterator it_pic;
            
            // �����open
            if ( ibb_open )
            {
                const std::string & strSid     = ibb_open->Attr( QN_SID );
                const std::string & strSize    = ibb_open->Attr( QN_BLOCK_SIZE );
                const std::string & strPicName = ibb_open->Attr( QN_NAME );

                it_pic = find_if ( pRoom->m_vPicSession.begin(), pRoom->m_vPicSession.end(), FindRoomPicSessionObj( tJid.sResource, strSid.c_str()) );
                // �������
                if ( it_pic != pRoom->m_vPicSession.end() )
                {
                    assert( 0 );
                    return true;
                }

                cPicSession * pSes = new cPicSession;

                STRNCPY( pSes->m_szNick,       tJid.sResource,                                         sizeof(pSes->m_szNick) );
                STRNCPY( pSes->m_szSid,        strSid.c_str(),                                         sizeof(pSes->m_szSid) );
                STRNCPY( pSes->m_szPicName,    Utf8ToAnsi( strPicName.c_str(), buf, MAX_BUFF_SIZE ),   sizeof(pSes->m_szPicName) );
                sscanf( strSize.c_str(), "%u", &pSes->m_dwPicSize );
                
                talk_base::Pathname temp_name;
                temp_name.SetFilename( pSes->m_szPicName );
                // temp_name.SetFolder( app->m_szGroupChatPicSaveDir );

                // ����Ψһ�ļ���
                if ( !talk_base::CreateUniqueFile( temp_name, false ) )
                {
                    LIBJINGLE_LOG_ERROR( "FAILED to talk_base::CreateUniqueFile()!" );
                    delete pSes;
                    return true;
                }

                STRNCPY( pSes->m_szTempPath, temp_name.pathname().c_str(), sizeof(pSes->m_szTempPath)  );

                // ���ļ�д
                if ( !pSes->m_stream.Open( temp_name.pathname().c_str(), "wb" ) )
                {
                    LIBJINGLE_LOG_ERROR( "FAILED to file->Open()!" );
                    delete pSes;
                    return true;
                }

                pRoom->m_vPicSession.push_back( pSes );

                // ֪ͨ�ϲ�׼���ļ�����
                if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData    msgData;
                    
                    msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom)  );
                    msgData.Append( (unsigned char *)&tMemberJid,           sizeof(tMemberJid)  );
                    msgData.Append( (unsigned char *)pSes->m_szSid,         sizeof(pSes->m_szSid) );
                    msgData.Append( (unsigned char *)&pSes->m_dwPicSize,    sizeof(pSes->m_dwPicSize) );
                    msgData.Append( (unsigned char *)pSes->m_szPicName,     LibJingleEx::MAX_FILENAME_SIZE );
                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,     sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_CHATROOM_PIC_OPEN, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                }
                // END( ֪ͨ�ϲ�׼���ļ����� )
            }
            else if ( ibb_data )
            {
                const std::string & strSid     = ibb_data->Attr( QN_SID );

                it_pic = find_if ( pRoom->m_vPicSession.begin(), pRoom->m_vPicSession.end(), FindRoomPicSessionObj( tJid.sResource, strSid.c_str()) );
                // ��������ڣ������data
                if ( it_pic == pRoom->m_vPicSession.end() )
                {
                    return true;
                }

                cPicSession * pSes = *it_pic;

                unsigned char  abyData[MAX_BYTE_PER_TRANSFER];
                unsigned long  dwDataSize = MAX_BYTE_PER_TRANSFER;

                // ��base64
                if ( 0 != LibJingleEx::DecodeBase64( abyData,  &dwDataSize , ibb_data->BodyText().c_str() )  )
                {
                    assert( 0 );
                    delete pSes;
                    pRoom->m_vPicSession.erase( it_pic );
                    return true;
                }

                size_t dwWrite = 0;
                int    err_  = 0;
                if ( talk_base::SR_SUCCESS != pSes->m_stream.Write( abyData, dwDataSize, &dwWrite, &err_ ) )
                {
                    // ���д��ʧ�ܣ�������û�д��̿ռ�
                    LIBJINGLE_LOG_ERROR( "Failed to pSes->m_stream.Write()" );

                    // ֪ͨ�����ļ�ʧ��
                    if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData    msgData;

                        msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom)  );
                        msgData.Append( (unsigned char *)&tMemberJid,           sizeof(tMemberJid)  );
                        msgData.Append( (unsigned char *)pSes->m_szSid,         sizeof(pSes->m_szSid) );

                        unsigned long dwErrorCode  = LibJingleEx::FILE_SHARE_ERR_NO_SPACE;
                        msgData.Append( (unsigned char *)&dwErrorCode,         sizeof(dwErrorCode) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,     sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_CHATROOM_PIC_FAIL, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                    }
                    // END

                    delete pSes;
                    pRoom->m_vPicSession.erase( it_pic );
                    return true;
                }

                pSes->m_dwWiteBytes += dwWrite;

                // ֪ͨ����
                if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData    msgData;

                    msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom)  );
                    msgData.Append( (unsigned char *)&tMemberJid,           sizeof(tMemberJid)  );
                    msgData.Append( (unsigned char *)pSes->m_szSid,         sizeof(pSes->m_szSid) );

                    assert( pSes->m_dwPicSize > 0 );
                    unsigned long dwProgress = ( unsigned long )( pSes->m_dwWiteBytes * 100.0f / pSes->m_dwPicSize );

                    msgData.Append( (unsigned char *)&dwProgress,           sizeof(dwProgress) );

                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,     sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_CHATROOM_PIC_DATA, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                }
                // END ( ֪ͨ���� )
            }
            else if ( ibb_close )
            {
                const std::string & strSid     = ibb_close->Attr( QN_SID );

                it_pic = find_if ( pRoom->m_vPicSession.begin(), pRoom->m_vPicSession.end(), FindRoomPicSessionObj( tJid.sResource, strSid.c_str()) );
                // ��������ڣ������close
                if ( it_pic == pRoom->m_vPicSession.end() )
                {
                    return true;
                }

                cPicSession * pSes = *it_pic;

                pSes->m_stream.Close();


                talk_base::Pathname path_old( pSes->m_szTempPath );

                talk_base::Pathname path_new;
                path_new.SetPathname( pSes->m_szPicName );
                path_new.SetFolder( app->m_szGroupChatPicSaveDir );

                if ( !talk_base::CreateUniqueFile(path_new, false) )
                {
                    assert( 0 );
                    delete pSes;
                    pRoom->m_vPicSession.erase( it_pic );
                    return true;
                }

                if ( !talk_base::Filesystem::MoveFile( path_old, path_new ) )
                {
                    // ֪ͨ�����ļ�ʧ��
                    if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData    msgData;

                        msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom)  );
                        msgData.Append( (unsigned char *)&tMemberJid,           sizeof(tMemberJid)  );
                        msgData.Append( (unsigned char *)pSes->m_szSid,         sizeof(pSes->m_szSid) );

                        unsigned long dwErrorCode  = LibJingleEx::FILE_SHARE_ERR_NO_SPACE;
                        msgData.Append( (unsigned char *)&dwErrorCode,         sizeof(dwErrorCode) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,     sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_CHATROOM_PIC_FAIL, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                    }
                    // END

                    delete pSes;
                    pRoom->m_vPicSession.erase( it_pic );
                    return true;
                }


                
                // ֪ͨ�ļ��������
                if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData    msgData;

                    msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom)  );
                    msgData.Append( (unsigned char *)&tMemberJid,           sizeof(tMemberJid)  );
                    msgData.Append( (unsigned char *)pSes->m_szSid,         sizeof(pSes->m_szSid) );

                    char  szFinalPath[LibJingleEx::MAX_PATH_SIZE];
                    STRNCPY( szFinalPath,     path_new.pathname().c_str(),   sizeof(szFinalPath) );
                    msgData.Append( (unsigned char *)szFinalPath,           sizeof(szFinalPath)  );

                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,     sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_CHATROOM_PIC_CLOSE, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                }
                // END( ֪ͨ�ļ�������� )

                delete pSes;
                pRoom->m_vPicSession.erase( it_pic );
            }

            // ����ͼƬ����һ����
            app->SendRoomPictureNextStep( strMsgId.c_str() );
        }

        return true;
    }
    // û����Ϣ��(xbody)
    else if ( 0 == body )
    {            
        // ����dms����ͬ����Ϣ
        if ( stanza->Attr(QN_TYPE) == "dms" )
        {
            // ��������Լ������Լ�
            if (  strJid != client->jid().Str() )
            {
                const buzz::XmlElement * operation = stanza->FirstElement();
                if ( 0 == operation )
                {
                    return true;
                }

                const std::string & operation_type = operation->Attr( buzz::QN_TYPE );
                const std::string & operation_ver  = operation->Attr( buzz::QN_VER );
                unsigned long dwVersion            = 0;

                if ( operation_ver.length() > 0 )
                {
                    dwVersion = ConvertVersion( operation_ver.c_str() );
                }


                // ��������dms��
                if ( operation_type == "add group" )
                {
                    OnDmsAddGroup( app, operation->FirstElement() );
                }
                else if ( operation_type == "delete group" )
                {
                    OnDmsDeleteGroup( app, operation->FirstElement() );
                }
                else if ( operation_type == "delete group members reserved" )
                {
                    OnDmsDeleteGroup( app, operation->FirstElement(), TRUE );
                }
                else if ( operation_type == "modify group" )
                {
                    OnDmsModifyGroup( app, operation->FirstElement() );
                }
                else if ( operation_type == "move group" )
                {
                    OnDmsMoveGroup( app, operation->FirstElement() );
                }
                else if ( operation_type == "add member" )
                {
                    OnDmsAddMember( app, operation->FirstElement() );
                }
                else if ( operation_type == "delete member" )
                {
                    OnDmsDeleteMember( app, operation->FirstElement() );
                }
                else if ( operation_type == "modify member" )
                {
                    OnDmsModifyMember( app, operation->FirstElement() );
                }
                else if ( operation_type == "move member" )
                {
                    OnDmsMoveMember( app, operation->FirstElement() );
                }
                else if ( operation_type == "set_chatroom_read_messages" )
                {
                    OnSetChatroomHistory( app, operation->FirstElement() );
                }
                //else if ( operation_type == "add roster group" )
                //{
                //    OnAddRosterGroup( app, operation->FirstElement(), dwVersion );
                //}
            }
        }
        else if ( stanza->Attr(QN_TYPE) == "groupchat" )
        {
            const buzz::XmlElement * operation = stanza->FirstElement();
            if ( 0 == operation )
            {
                return true;
            }

            if ( operation->Attr( buzz::QN_TYPE ) == "change room name" )
            {
                // ����delay�ķ���ı�֪ͨ�����
                if ( 0 == delay && 0 == delay_x )
                {
                    OnChangeRoomName( app, operation->FirstElement() );
                }
            }
            else if ( operation->Attr( buzz::QN_TYPE ) == "chatroom add member" )
            {
                // ����delay�ķ���ı�֪ͨ�����
                if ( 0 == delay && 0 == delay_x )
                {
                    OnChatroomAddMember( app, operation->FirstElement(), &tJid );
                }
            }
            else if ( operation->Attr( buzz::QN_TYPE ) == "chatroom delete member" )
            {
                // ����delay�ķ���ı�֪ͨ�����
                if ( 0 == delay && 0 == delay_x )
                {
                    OnChatroomDeleteMember( app, operation->FirstElement(), &tJid );
                }
            }
        }
        else if ( stanza->Attr(QN_TYPE) == "inner" )
        {
            const buzz::XmlElement * operation = stanza->FirstElement();
            if ( 0 == operation )
            {
                return true;
            }

            if ( operation->Attr( buzz::QN_TYPE ) == "notify inviter decline" )
            {
                OnNotifyInviterDecline( app, operation->FirstElement(), &tJid );
            }
            else if ( operation->Attr( buzz::QN_TYPE ) == "delete persistent room" )
            {
                OnDeletePersistentRoom( app, operation->FirstElement() );
            }
            else if ( operation->Attr( buzz::QN_TYPE ) == "add persistent room" )
            {
                OnAddPersistentRoom( app, operation->FirstElement() );
            }
        }
        else if ( sysnotify )
        {
            const XmlElement * msgType = sysnotify->FirstElement();

            if ( 0 == msgType || msgType->Name().LocalPart() != "messageType" )
            {
                MYLOG( "Wrong weibo xml string! No messageType \r\n" );
                return true;
            }


            LibJingleEx::TKedaWeiboMsg     tData;

            // ��ȡtype��Ϣ
            strncpy( tData.szType,   Utf8ToAnsi(msgType->BodyText().c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_KEDA_WEIBO_TYPE_LEN );

            const XmlElement * data = msgType->NextElement();     

            // �����<data> element
            if ( data && data->Name().LocalPart() == "data" )
            {
                const XmlElement * data_item = data->FirstElement();
                while( data_item && tData.dwCnt < LibJingleEx::MAX_KEDA_WEIBO_DATA_ITEM_CNT )
                {
                    strncpy( tData.szKey[tData.dwCnt],  Utf8ToAnsi(data_item->Name().LocalPart().c_str(), buf, MAX_BUFF_SIZE), 
                             LibJingleEx::MAX_KEDA_WEIBO_KEY_LEN );

                    strncpy( tData.szData[tData.dwCnt], Utf8ToAnsi(data_item->BodyText().c_str(), buf, MAX_BUFF_SIZE), 
                             LibJingleEx::MAX_KEDA_WEIBO_DATA_LEN );

                    data_item = data_item->NextElement();
                    tData.dwCnt++;
                }
            }

            if ( g_pfnOnMessage )
            {
                g_pfnOnMessage( LibJingleEx::ON_KEDA_WEIBO_MSG_NOTIFY, app, app->m_pUserData, 
                    (unsigned char *)&tData, sizeof(tData)  );
            }
            // END ��ƴ﹫˾��΢����Ϣ
        }
        else if ( (composing || paused) && !delay && !delay_x )
        {
            LibJingleEx::ECharStatus  eCharState = LibJingleEx::CHAR_STATUS_ACTIVE;

            if ( composing )
            {
                eCharState = LibJingleEx::CHAR_STATUS_COMPOSING;
            }
            else if ( paused )
            {
                eCharState = LibJingleEx::CHAR_STATUS_PAUSED;
            }


            // ���浱ǰ�Լ�����ϵ�˵�״̬
            vMemberGroups & vContacts = app->m_MemberGroups;
            vMemberGroups::iterator it_contact = find_if( vContacts.begin(), vContacts.end(), FindMemberGroupObj( &tJid ) ) ;

            PTMemberGroup pContact = 0;
            // ���û�о����
            if ( it_contact == vContacts.end() )
            {
                pContact = new TMemberGroup;
                memcpy( &pContact->m_tJid, &tJid, sizeof(LibJingleEx::TJid) );
                pContact->m_tJid.sResource[0] = '\0';

                vContacts.push_back( pContact );
            }
            else
            {
                pContact = *it_contact;
            }
            pContact->m_eFromCharStatus = eCharState;
            // 



            LibJingleEx::EPresence contact_pres = GetMaxPresence( pContact->m_Status );

            // �������ϵ���ۺ�״̬Ϊ0����������״̬
            if ( LibJingleEx::PRESENCE_NONE == contact_pres )
            {
                buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                LibJingleEx::TJid2StrJid( pContact->m_tJid, buf, MAX_BUFF_SIZE );
                message->SetAttr( buzz::QN_TO,   buf );
                message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

                buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                message->AddElement( operation );
                operation->SetAttr( buzz::QN_TYPE, "get status" );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM, true );
                operation->AddElement( item );

                item->SetAttr( buzz::QN_ID, client->jid().BareJid().Str()  );

                client->SendStanza( message );
                delete message;
            }
            // END( ������״̬ )



            if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData msgData;

                msgData.Append( (unsigned char *)&tJid,         sizeof(tJid) );
                msgData.Append( (unsigned char *)&eCharState,   sizeof(eCharState) );

                g_pfnOnMessage( LibJingleEx::ON_CHAT_STATE, app, app->m_pUserData, 
                    msgData.GetMsgData(), msgData.GetMsgLength() );
            }
        }
        // ������Զ�����Ϣ
        else if ( custom_body )
        {
            if ( !(delay || delay_x) )
            {
                LibJingleEx::TCustomMsg   tCustomBody;

                ParseCustomMsg( custom_body,  tCustomBody );

                if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData msgData;

                    msgData.Append( (unsigned char *)&tJid,         sizeof(tJid) );
                    msgData.Append( (unsigned char *)&tCustomBody,  sizeof(tCustomBody) );

                    g_pfnOnMessage( LibJingleEx::ON_CUSTOM_MESSAGE, app, app->m_pUserData, 
                        msgData.GetMsgData(), msgData.GetMsgLength() );
                }
            }
        }
        else if ( stanza->Attr(QN_TYPE) == INSTANT_MESSAGE_TYPE )
        {
            // ��������Լ������Լ�
            if (  strJid != client->jid().Str() )
            {
                const buzz::XmlElement * operation = stanza->FirstElement();
                if ( 0 == operation )
                {
                    return true;
                }

                if ( operation->Name().LocalPart() != buzz::QN_OPERATION.LocalPart() )
                {
                    return true;
                }

                if ( operation->Attr( buzz::QN_TYPE ) == "get status" )
                {
                    OnQueryGetStatus( app, operation->FirstElement() );
                }
                else if ( operation->Attr( buzz::QN_TYPE ) == "info status" )
                {
                    OnInfoStatus( app, operation->FirstElement(), strJid, &tJid );
                }
                else if ( operation->Attr( buzz::QN_TYPE ) == "custom config" )
                {
                    OnCustomConfigNotify( app, operation->FirstElement() );
                }
                else if ( OPERATION_TYPE_SYNC_CHATROOM == operation->Attr( buzz::QN_TYPE ) )
                {
                    OnSyncChatroomNotify( app, operation->FirstElement(), client );
                }
            }
        }
        // ����

        return true;
    }
    else if ( 0 != body )
    {
        //const std::string & message = body->BodyText();        
        //char szAnsi[MAX_BUFF_SIZE];
        //Utf8ToAnsi( message.c_str(), szAnsi, MAX_BUFF_SIZE );

        LibJingleEx::TDmsChatContent   tMsg;

        ::ParseChatMessage( body, tMsg );
     

        // Ⱥ��
        if ( stanza->Attr(QN_TYPE) == "groupchat" )
        {
            vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                               FindRoomByFullId( tJid.sNode, tJid.sDomain ) );

            // û���������
            if ( it_room == app->m_Rooms.end() )
            {
                return true;
            }

            PTRoom pRoom = *it_room;

            // ������ʷ(���20��)
            pRoom->m_LatestMsgIds.AddMessageId( strMsgId.c_str() );


            LibJingleEx::TJid   tMemberJid;
            GetMemberJidFromChatRoomResource( pRoom, tJid.sResource, &tMemberJid );

            // ���û�л���Ѷ���ʷ������
            if ( !app->m_bGetRoomReadMsgs )
            {
                PTCachedGroupchatMsg  pDelayedMsg = new TCachedGroupchatMsg;
                pDelayedMsg->bOffline = bOfflineMsg;
                strncpy( pDelayedMsg->szMsgId, strMsgId.c_str(), MAX_RAND_ID_LENGTH );
                memcpy( &pDelayedMsg->tMemberJid , &tMemberJid, sizeof(LibJingleEx::TJid) );
                memcpy( &pDelayedMsg->tMsg, &tMsg, sizeof(LibJingleEx::TDmsChatContent) );
                pRoom->m_vDelayedMsgs.push_back( pDelayedMsg );

                return true;
            }


            // ������ʷ����
            std::vector< CRoomReadMsgs *>::iterator   it_find;
            std::vector< CRoomReadMsgs *> &  w = app->m_vRoomReadMsgs;
            CRoomReadMsgs * pHistoryRoom = 0;

            it_find = find_if( w.begin(), w.end(), 
                               FindHistoryRoom(pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );
            if ( it_find != w.end() )
            {
                pHistoryRoom = *it_find;

                // �鿴�Ƿ��Ѿ��Ķ�����
                unsigned long dwPos = pHistoryRoom->m_room.FindIndex( strMsgId.c_str() );
                // �Ѿ��Ķ���
                if ( -1 != dwPos )
                {
                    // ����������������Ϣ
                    ClearVector( pRoom->m_vDelayedMsgs );
                    return true;
                }
            }


            // ����ǹ̶�Ⱥ����Ҫ������ϢΪ�Ѷ�
            if ( pRoom->m_bPersistent && pRoom->m_nType != ROOM_TYPE_SPECIAL )
            {
                if ( 0 == pHistoryRoom )
                {
                    pHistoryRoom = new CRoomReadMsgs( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    w.push_back( pHistoryRoom );
                }

                pHistoryRoom->m_room.AddMessageId( strMsgId.c_str() );
                app->m_bRoomReadMsgsChanged = TRUE;
                app->m_dwChatroomHistoryVersion++;
                app->SaveChatRoomHistory2Local();
            }


            // ���������������Ϣ
            if ( pRoom->m_bNoOfflineMsg )
            {
               // ���������������Ϣ�����Ҹ���Ϣ�����ߵģ�ֱ�ӷ���
               if ( bOfflineMsg )
               {
                  return true;
               }
            }

            // ֪ͨ�ϲ㻺����Ϣ
            app->NotifyBufferedChatRoomMsgs( pRoom );
            

#if  IM_DEBUG
            pRoom->m_dwDelayMsgCnt++;
#endif

            TRoomChatMsgItem  tItem;

            if ( pRoom->IfExistIqId( strMsgId.c_str(), true, &tItem ) )
            {
                if ( g_pfnOnMessage )
                {
                    BOOL   bSuccessfull = TRUE;
                    LibJingleEx::CMsgData msgData;
                    msgData.Append( (unsigned char *)&pRoom,                          sizeof(pRoom) );
                    msgData.Append( (unsigned char *)&tItem.dwContextId,              sizeof(tItem.dwContextId) );
                    msgData.Append( (unsigned char *)&bSuccessfull,                   sizeof(bSuccessfull) );
                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_GROUP_CHAT_MESSAGE_ACK, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
                }
            }

            if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData msgData;
                char szMsgId[MAX_GROUPCHAT_MESSAGE_ID_LEN] = { 0 };
                strcpy( szMsgId, strMsgId.c_str() );

                msgData.Append( (unsigned char *)&pRoom,           sizeof(pRoom) );
                msgData.Append( (unsigned char *)&tMemberJid,      sizeof(tMemberJid) );
                msgData.Append( (unsigned char *)&bOfflineMsg,     sizeof(bOfflineMsg) );
                msgData.Append( (unsigned char *)szMsgId,          sizeof(szMsgId) );
                msgData.Append( (unsigned char *)&tMsg,            sizeof(tMsg)  );

                msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId)  );
                msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName)  );

                g_pfnOnMessage( LibJingleEx::ON_GROUPCHAT_MESSAGE, app, app->m_pUserData, 
                                msgData.GetMsgData(),  msgData.GetMsgLength() );
            }

        }
        // ����
        else if ( stanza->Attr(QN_TYPE) == "chat" )
        {

            // �Ѹ���ϵ�˵�״̬����Ϊactive
            vMemberGroups & vContacts = app->m_MemberGroups;
            vMemberGroups::iterator it_contact = find_if( vContacts.begin(), vContacts.end(), FindMemberGroupObj( &tJid ) ) ;

            PTMemberGroup pContact = 0;
            // ���û�о����
            if ( it_contact == vContacts.end() )
            {
                pContact = new TMemberGroup;
                memcpy( &pContact->m_tJid, &tJid, sizeof(LibJingleEx::TJid) );
                pContact->m_tJid.sResource[0] = '\0';

                vContacts.push_back( pContact );
            }
            else
            {
                pContact = *it_contact;
            }
            pContact->m_eFromCharStatus = LibJingleEx::CHAR_STATUS_ACTIVE;
            // 



            LibJingleEx::EPresence contact_pres = GetMaxPresence( pContact->m_Status );

            // �������ϵ���ۺ�״̬Ϊ0����������״̬
            if ( LibJingleEx::PRESENCE_NONE == contact_pres )
            {
                buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                LibJingleEx::TJid2StrJid( pContact->m_tJid, buf, MAX_BUFF_SIZE );
                message->SetAttr( buzz::QN_TO,   buf );
                message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

                buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                message->AddElement( operation );
                operation->SetAttr( buzz::QN_TYPE, "get status" );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM, true );
                operation->AddElement( item );

                item->SetAttr( buzz::QN_ID, client->jid().BareJid().Str()  );

                client->SendStanza( message );
                delete message;
            }
            // END( ������״̬ )


            // �ϱ�keybord״̬
            if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData msgData;
                LibJingleEx::ECharStatus  eCharState = LibJingleEx::CHAR_STATUS_ACTIVE;

                msgData.Append( (unsigned char *)&tJid,         sizeof(tJid) );
                msgData.Append( (unsigned char *)&eCharState,   sizeof(eCharState) );

                g_pfnOnMessage( LibJingleEx::ON_CHAT_STATE, app, app->m_pUserData, 
                                msgData.GetMsgData(), msgData.GetMsgLength() );
            }


            if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData msgData;

                msgData.Append( (unsigned char *)&tJid,         sizeof(tJid) );
                msgData.Append( (unsigned char *)&bOfflineMsg,  sizeof(bOfflineMsg) );
                msgData.Append( (unsigned char *)&tMsg,         sizeof(tMsg) );

                g_pfnOnMessage( LibJingleEx::ON_CHAT_MESSAGE, app, app->m_pUserData, 
                                msgData.GetMsgData(), msgData.GetMsgLength() );
            }

            // �����Ҫ�ظ�receipts
            if ( receipts_request )
            {
                XmlElement * message = new XmlElement(QN_MESSAGE);
                message->SetAttr( QN_TO, strJid );
                GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
                message->SetAttr( QN_ID, szId );
                message->SetAttr( QN_TYPE, strType );

                XmlElement * received = new XmlElement(QN_RECEIPT_RECEIVED, true);
                message->AddElement( received );
                received->SetAttr( QN_ID, strMsgId );
                
                client->SendStanza( message );
                delete message;
            }
        }        
        return true;
    }

    return false;
}


void MyDiscoTask::GetInfo()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);

    iq->AddAttr(buzz::QN_TO, client->jid().domain());

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_DISCO_INFO_QUERY, true);
    iq->AddElement( query );

    client->SendStanza( iq );

    delete iq;
}

void MyDiscoTask::GetItems()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);

    iq->AddAttr(buzz::QN_TO, client->jid().domain());

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_DISCO_ITEMS_QUERY, true);
    iq->AddElement( query );

    client->SendStanza( iq );

    delete iq;
}

bool MyDiscoTask::HandleStanza(const XmlElement * stanza)
{
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    XmppClient * client = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );
    
    char buf[MAX_BUFF_SIZE];
    char szId[MAX_RAND_ID_LENGTH];

    if ( stanza->Attr( QN_TYPE ) == STR_RESULT )
    {
        const XmlElement * element = stanza->FirstElement();
        if ( 0 == element )
        {
            return false;
        }

        if ( element->Name() == QN_DISCO_ITEMS_QUERY )
        {
            // ���Զ��������
            if ( stanza->Attr( QN_FROM ) == client->jid().domain() )
            {
                const XmlElement * item = element->FirstElement();
                while( item )
                {
                    // ����disco#info��ѯÿ��item��������conference

                    XmlElement * iq = new XmlElement(QN_IQ);
                    iq->AddAttr(buzz::QN_TYPE,STR_GET);

                    iq->AddAttr(buzz::QN_ID, GenerateRandomId(szId, MAX_RAND_ID_LENGTH) );

                    iq->AddAttr(buzz::QN_TO, item->Attr(QN_JID));

                    XmlElement * query = new XmlElement(buzz::QN_DISCO_INFO_QUERY, true);
                    iq->AddElement( query );

                    client->SendStanza( iq );
                    delete iq;
                    // ����disco#info ����

                    item = item->NextElement();
                }
            }
            // ������������е�������
            else if ( stanza->Attr(QN_FROM) == client->ConferenceJid() )
            {
                ClearVector( app->m_vQueryAllPersistentRoomIds );
                ClearVector( app->m_vTryingPersistents );

                const buzz::XmlElement * item = element->FirstElement();
                while( item )
                {
                    const std::string & strRoomJid = item->Attr( QN_JID );
                    
                    LibJingleEx::TJid  tRoomJid;
                    LibJingleEx::StrJid2TJid( strRoomJid.c_str(),  tRoomJid );

                    vRoom::iterator it_room = find_if ( app->m_Rooms.begin(), app->m_Rooms.end(), FindRoomByFullId( tRoomJid.sNode, tRoomJid.sDomain ) );
                    // ��������ڲż�����ѯdisco#info
                    if ( it_room == app->m_Rooms.end() )
                    {
                        // disco#info
                        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

                        GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
                        iq->SetAttr( buzz::QN_ID, szId );

                        iq->SetAttr( buzz::QN_TO, strRoomJid );
                        iq->SetAttr( buzz::QN_TYPE, "get" );

                        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_DISCO_INFO_QUERY, true );
                        iq->AddElement( query );

                        client->SendStanza( iq );
                        delete iq;


                        char * pNewId = new char[MAX_RAND_ID_LENGTH];
                        strcpy( pNewId, szId );
                        app->m_vQueryAllPersistentRoomIds.push_back( pNewId );
                    }

                    item = item->NextElement();
                }

                if ( 0 == app->m_vQueryAllPersistentRoomIds.size() )
                {
                    app->m_bTryEnterAllPersistentRooms = FALSE;
                }
            }
            return true;
        }
        else if ( element->Name() == QN_DISCO_INFO_QUERY )
        {
            // �����Զ��������
            if ( stanza->Attr( QN_FROM ) != client->jid().domain() )
            {
                const XmlElement * child = element->FirstElement();
                while( child )
                {
                    // ȷ���Ƿ�MUC����
                    if ( child->Name() == QN_DISCO_IDENTITY && child->Attr(buzz::QN_CATEGORY) == "conference" && child->Attr(buzz::QN_TYPE) == "text" )
                    {
                        std::string & conf_jid = client->ConferenceJid();

                        // �����û�м�¼ muc ��jid����ʱ��Ϊ��һ���յ����������ж���������Ⱥ�ĵ�jid��
                        if ( 0 == conf_jid.length() )
                        {
                            conf_jid = stanza->Attr( QN_FROM );

                            if ( g_tEvents.pOnChatRoomService )
                            {
                                g_tEvents.pOnChatRoomService( app,  TRUE );
                            }
                            else if ( g_pfnOnMessage )
                            {
                                BOOL bValue = TRUE;

                                g_pfnOnMessage( LibJingleEx::ON_GROUP_CHAT_ABILITY, app, app->m_pUserData, 
                                    (unsigned char *)&bValue, sizeof(bValue) );
                            }

                            
                            // �������ҵ���֣������״̬����������
                            if ( '\0' != app->m_szEnterpriseName[0] )
                            {
                                // �����android��ipad�Ȳ���Ҫ������״̬����
#ifndef SUPPORT_STATUS_ROOM
                                app->DenyEnterpriseStatusIn();
#endif
                                
                                app->EnterEnterpriseStatusRoom();
                            }
                            // END (�������ҵ���֣������״̬����������)

                        }
                        // Ҳ���ǲ�ѯĳ�������disco#info
                        else
                        {
                            LibJingleEx::TJid tJid;
                            LibJingleEx::StrJid2TJid( stanza->Attr(QN_FROM).c_str(), tJid );

                            // ���������������
                            vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                FindRoomByFullId(tJid.sNode,tJid.sDomain) );

                            // ���������
                            if ( it != app->m_Rooms.end() )
                            {
                                PTRoom pRoom = *it;

                                const std::string & room_name = child->Attr(QN_NAME);
                                
                                if ( g_pfnOnMessage )
                                {
                                    // ֪ͨ�ϲ㷿��������Ϣ
                                    if ( g_pfnOnMessage )
                                    {
                                        char szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN] = { 0 };
                                        strncpy( szRoomName, Utf8ToAnsi(room_name.c_str(), buf, MAX_BUFF_SIZE), sizeof(szRoomName) );

                                        LibJingleEx::CMsgData msgData;
                                        msgData.Append( (unsigned char *)&pRoom,     sizeof(pRoom) );
                                        msgData.Append( (unsigned char *)szRoomName, sizeof(szRoomName) );

                                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId) );
                                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                                        g_pfnOnMessage( LibJingleEx::ON_ROOM_CONFIG_MSG, app, app->m_pUserData, 
                                                        msgData.GetMsgData(), msgData.GetMsgLength() );
                                    }
                                    // END ֪ͨ�ϲ㷿��������Ϣ
                                }
                            }
                            // ����ǵ�¼����־��Է�������info
                            else
                            {
                                vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(),
                                                                          app->m_vPersistentRooms.end(),
                                   FindPersistentRoom( tJid.sNode, tJid.sDomain ) );

                                // ����ڳ־��Է���vector���ҵ�����������뷿��
                                if ( it_p != app->m_vPersistentRooms.end() )
                                {
                                    PTPersistentRoom p = *it_p;

                                    // ���TRoom����
                                    PTRoom pRoom = new TRoom( app );
                                    strcpy ( pRoom->m_szRoomId,               p->m_szRoomId );
                                    strcpy ( pRoom->m_szGroupChatServiceName, p->m_szGroupChatServiceName );
                                    strncpy( pRoom->m_szName, Utf8ToAnsi( child->Attr(QN_NAME).c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );              
                                    pRoom->m_bPersistent = TRUE;
                                    // pRoom->m_eState      = ROOM_STATE_ENTERING_PERSISTENT;
                                    pRoom->m_eState      = ROOM_STATE_HOLD_PERSISTENT;
                                    strncpy( pRoom->m_szNick, client->jid().Str().c_str(), LibJingleEx::MAX_RESOURCE_LEN );

                                    pRoom->m_bFirst = p->m_bFirst;
                                    app->m_Rooms.push_back( pRoom );
                                    // END ���TRoom����

                                    
                                    
                                    vQueryIdsList::iterator  it_id = find_if( app->m_vQueryPersistentRoomIds.begin(), 
                                                                              app->m_vQueryPersistentRoomIds.end(), 
                                                                              FindQueryIdObj(stanza->Attr(QN_ID).c_str()) );

                                    if ( it_id != app->m_vQueryPersistentRoomIds.end() )
                                    {
                                        delete[] *it_id;
                                        app->m_vQueryPersistentRoomIds.erase( it_id );
                                    }

                                    // ����õ����һ��private�־��Է����ѯȷ��
                                    if ( 0 == app->m_vQueryPersistentRoomIds.size() )
                                    {
                                        app->NotifyHoldPersistentRoom();

                                        app->m_bPrivatePersistentRoomsCntConfirmed = TRUE;

                                        app->HandleScreenedChatroomCfg();

                                        app->HandleDelayInvite2ChatRoomMsgs();

                                        if ( app->m_bPersistentRoomsChanged )
                                        {
                                            MYLOG("=====[ Save new chatrooms list, count = %u, unvalaible count = %u ] \r\n", 
                                                app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                                            SavePersistentRoom( app );
                                        }
                                    }
                                }
                                // ���ǲ�ѯ��һ�ε�¼�ĳ־��Է���
                                else
                                {
                                    BOOL bPersistentRoom = FALSE;

                                    const XmlElement * feature = child->NextElement();
                                    while( feature )
                                    {
                                        // �����feature
                                        if ( feature->Name().LocalPart() == "feature" )
                                        {
                                            // ��������ǳ־��Է���
                                            if ( feature->Attr(QN_VAR) == "muc_persistent" )
                                            {
                                                bPersistentRoom  = TRUE;
                                                break;
                                            }
                                        }

                                        feature = feature->NextElement();
                                    }

                                    if ( bPersistentRoom )
                                    {
                                        LibJingleEx::TJid tRoomJid;
                                        LibJingleEx::StrJid2TJid( stanza->Attr(QN_FROM).c_str(), tRoomJid );

                                        // ���뷿��
                                        buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

                                        std::string sTmp;

                                        sTmp  = tRoomJid.sNode;
                                        sTmp += '@';
                                        sTmp += tRoomJid.sDomain;
                                        sTmp += '/';
                                        sTmp += client->jid().Str();

                                        presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

                                        const char * szPresence = GetPresShowString( app->m_ePres );
                                        if ( szPresence )
                                        {
                                            buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
                                            presence->AddElement( show );
                                            show->SetBodyText( szPresence );
                                        }

                                        buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
                                        presence->AddElement( priority );
                                        priority->SetBodyText("1");

                                        std::string  ability;
                                        if ( GetAbilityString( ability, app ) )
                                        {
                                            presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
                                            presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
                                            MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
                                            presence->AddAttr( buzz::QN_VER,  buf, 1);
                                            presence->AddAttr( buzz::QN_EXT, ability, 1);
                                        }

                                        buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
                                        presence->AddElement( x );


                                        // ����ÿ�����ȡ��20��stanza
                                        buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
                                        x->AddElement( history );

                                        // char szMaxStanzas[256] = { 0 };
                                        // SNPRINTF( szMaxStanzas, 256, "%u", MAX_MUC_HISTORY_STANZAS );
                                        history->SetAttr( buzz::QN_MAX_STANZAS, "0" );
                                        // END ����ÿ�����ȡ��20��stanza

                                        client->SendStanza( presence );
                                        delete presence;


                                        // ���TRoom����
                                        PTRoom pRoom = new TRoom( app );
                                        strcpy ( pRoom->m_szRoomId,               tRoomJid.sNode );
                                        strcpy ( pRoom->m_szGroupChatServiceName, tRoomJid.sDomain );
                                        strncpy( pRoom->m_szName, Utf8ToAnsi( child->Attr(QN_NAME).c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );              
                                        pRoom->m_bPersistent = TRUE;
                                        pRoom->m_eState      = ROOM_STATE_TRYING_PERSISTENT;
                                        strncpy( pRoom->m_szNick, client->jid().Str().c_str(), LibJingleEx::MAX_RESOURCE_LEN );
                                        app->m_Rooms.push_back( pRoom );
                                        // END ���TRoom����

                                        PTPersistentRoom  pPersistentRoom = new TPersistentRoom;
                                        strcpy( pPersistentRoom->m_szRoomId,               pRoom->m_szRoomId );
                                        strcpy( pPersistentRoom->m_szGroupChatServiceName, pRoom->m_szGroupChatServiceName );

                                        app->m_vTryingPersistents.push_back( pPersistentRoom );
                                    }

                                    vQueryIdsList::iterator it_iq = find_if( app->m_vQueryAllPersistentRoomIds.begin(), 
                                                                             app->m_vQueryAllPersistentRoomIds.end(), 
                                                                             FindQueryIdObj( stanza->Attr(QN_ID).c_str() )  );

                                    if ( it_iq != app->m_vQueryAllPersistentRoomIds.end() )
                                    {
                                        delete[]  *it_iq;
                                        app->m_vQueryAllPersistentRoomIds.erase( it_iq );
                                    }

                                    // ����Ǵ������һ��query
                                    if ( 0 == app->m_vQueryAllPersistentRoomIds.size() )
                                    {
                                        app->m_bTryEnterAllPersistentRooms = FALSE;
                                    }
                                }
                            }
                        }

                        break;
                    }
                    else if ( child->Name() == QN_DISCO_IDENTITY && child->Attr(buzz::QN_CATEGORY) == "pubsub" && child->Attr(buzz::QN_TYPE) == "service" )
                    {
                        std::string & pubsub_jid = client->PubsubJid();

                        if ( 0 == pubsub_jid.length() )
                        {
                            pubsub_jid = stanza->Attr( QN_FROM );
                        }

                        break;
                    }

                    child = child->NextElement();
                }
            }
            return true;
        }
        else
        {
            return false;
        }

    }
    else if ( stanza->Attr( QN_TYPE ) == STR_ERROR )
    {
        const XmlElement * element = stanza->FirstElement();
        if ( 0 == element )
        {
            return false;
        }

        if ( element->Name() == QN_DISCO_INFO_QUERY )
        {
            // ������ȡerror element
            element = element->NextElement();

            // ���û�� error element
            if ( 0 == element )
            {
                return true;
            }

            // �������error, return
            if ( element->Name() != QN_ERROR )
            {
                return true;
            }

            // item not found
            if ( element->Attr( QN_CODE) == "404" )
            {
                LibJingleEx::TJid tJid;
                LibJingleEx::StrJid2TJid( stanza->Attr(QN_FROM).c_str(), tJid );

                vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(),
                    app->m_vPersistentRooms.end(),
                    FindPersistentRoom( tJid.sNode, tJid.sDomain ) );

                // ����ڳ־��Է���vector���ҵ�����ɾ���Ѿ�����ĳ־��Է���
                if ( it_p != app->m_vPersistentRooms.end() )
                {
                    PTPersistentRoom p = *it_p;

                    MYLOG("=====[ Can't find persistent chatroom: %s@%s ] \r\n", p->m_szRoomId, p->m_szGroupChatServiceName );
                    p->m_bNotFound = TRUE;


                    if ( app->m_vPersistentRooms.size() > MIN_SAVE_PERSISTENT_CHATROOM_COUNT )
                    {
                        // �ҵ���һ����ѯ�����ڵ�chat roomɾ��
                        vPersistentRoom::iterator it_first_not_found;
                        for ( it_first_not_found = app->m_vPersistentRooms.begin(); it_first_not_found != app->m_vPersistentRooms.end(); it_first_not_found++ )
                        {
                            PTPersistentRoom pFirstNotFound = *it_first_not_found;
                            if ( pFirstNotFound->m_bNotFound )
                            {
                                delete pFirstNotFound;
                                app->m_vPersistentRooms.erase( it_first_not_found );
                                app->m_bPersistentRoomsChanged = TRUE;

                                break;
                            }
                        }
                    }
                    

                    vQueryIdsList::iterator  it_id = find_if( app->m_vQueryPersistentRoomIds.begin(), 
                        app->m_vQueryPersistentRoomIds.end(), 
                        FindQueryIdObj(stanza->Attr(QN_ID).c_str()) );

                    if ( it_id != app->m_vQueryPersistentRoomIds.end() )
                    {
                        delete[] *it_id;
                        app->m_vQueryPersistentRoomIds.erase( it_id );
                    }

                    // ����õ����һ��private�־��Է����ѯȷ��
                    if ( 0 == app->m_vQueryPersistentRoomIds.size() )
                    {
                        app->NotifyHoldPersistentRoom();

                        app->m_bPrivatePersistentRoomsCntConfirmed = TRUE;

                        app->HandleScreenedChatroomCfg();

                        app->HandleDelayInvite2ChatRoomMsgs();

                        MYLOG("=====[ Save new chatrooms list, count = %u, unvailable count = %u ] \r\n", 
                            app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                        SavePersistentRoom( app );
                    }
                }
            }
            // service-unavailable
            else if ( element->Attr( QN_CODE) == "503" )
            {

            }

            return true;
        }
        else if ( element->Name() == QN_DISCO_ITEMS_QUERY )
        {
            vQueryIdsList::iterator it_iq = find_if( app->m_vQueryAllPersistentRoomIds.begin(), 
                app->m_vQueryAllPersistentRoomIds.end(), 
                FindQueryIdObj( stanza->Attr(QN_ID).c_str() )  );

            if ( it_iq != app->m_vQueryAllPersistentRoomIds.end() )
            {
                delete[]  *it_iq;
                app->m_vQueryAllPersistentRoomIds.erase( it_iq );
            }

            // ����Ǵ������һ��query
            if ( 0 == app->m_vQueryAllPersistentRoomIds.size() )
            {
                app->m_bTryEnterAllPersistentRooms = FALSE;
            }
        }

        return false;
    }


    return false;
}

bool MyChatRoomTask::Handle_iq( const XmlElement * iq )
{
    char buf[MAX_BUFF_SIZE];

    XmppClient * client = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );


    const XmlElement * unique = 0;
    const XmlElement * muc_owner_query = 0;
    const XmlElement * muc_admin_query = 0;

    LibJingleEx::TJid tJid;
    LibJingleEx::StrJid2TJid( iq->Attr(QN_FROM).c_str(), tJid );

    // TYPE: get
    if ( iq->Attr( QN_TYPE ) == STR_GET )
    {

    }
    // TYPE: set
    else if ( iq->Attr( QN_TYPE ) == STR_SET )
    {

    }
    // TYPE: result
    else
    {
        const XmlElement * child = iq->FirstElement();
        while( child )
        {
            if ( child->Name() == QN_MUC_UNIQUE )
            {
                unique = child;
            }
            else if ( child->Name() == QN_MUC_QUERY_OWNER )
            {
                muc_owner_query = child;
            }
            else if ( child->Name() == QN_MUC_ADMIN_QUERY )
            {
                muc_admin_query = child;
            }

            child = child->NextElement();
        }

        // �����uniqueԪ��
        if ( unique )
        {
            vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                FindRoomByQueryId(iq->Attr(QN_ID).c_str())  );

            // ����query id�ҵ���֮ǰ������Ψһ����
            if ( it != app->m_Rooms.end() )
            {
                PTRoom pRoom = *it;

                assert( pRoom->m_eState == ROOM_STATE_ON_QUERY_ID );

                // ����Ψһid��
                strcpy( pRoom->m_szRoomId, unique->BodyText().c_str() );


                // ��service�����󴴽�����
                XmlElement * presence = new XmlElement( QN_PRESENCE );

                std::string sTmp;
                sTmp  = pRoom->m_szRoomId;
                sTmp += '@';
                sTmp += client->ConferenceJid();
                sTmp += '/';
                sTmp += client->jid().Str();

                presence->SetAttr( QN_TO, sTmp );

                const char * szPresence = GetPresShowString( app->m_ePres );
                if ( szPresence )
                {
                    buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
                    presence->AddElement( show );
                    show->SetBodyText( szPresence );
                }

                buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
                presence->AddElement( priority );
                priority->SetBodyText("1");

                std::string  ability;
                if ( GetAbilityString( ability, app ) )
                {
                    presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
                    presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
                    MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
                    presence->AddAttr( buzz::QN_VER,  buf, 1);
                    presence->AddAttr( buzz::QN_EXT, ability, 1);
                }

                XmlElement * x = new XmlElement( QN_MUC_X, true );
                presence->AddElement( x );

                // ����ÿ�����ȡ��20��stanza
                buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
                x->AddElement( history );

                char szMaxStanzas[256] = { 0 };
                // SNPRINTF( szMaxStanzas, 256, "%u", MAX_MUC_HISTORY_STANZAS );
                SNPRINTF( szMaxStanzas, 256, "%u", 0 );
                history->SetAttr( buzz::QN_MAX_STANZAS, szMaxStanzas );
                // END ����ÿ�����ȡ��20��stanza




                client->SendStanza( presence );
                delete presence;
                // (��service�����󴴽�����)����


                pRoom->m_eState = ROOM_STATE_ON_PRESENCE;
                return true;
            }
        }
        else if ( muc_owner_query )
        {
            vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                FindRoomByQueryId(iq->Attr(QN_ID).c_str())  );
            if ( ( it != app->m_Rooms.end() ) && ( 0 == strcmp( tJid.sNode, (*it)->m_szRoomId ) ) )
            {
                PTRoom pRoom = *it;
                pRoom->m_eState = ROOM_STATE_AVAILABLE;

                // ���������ҵ״̬����
                if ( ROOM_TYPE_SPECIAL != pRoom->m_nType )
                {
                    if ( g_tEvents.pOnCreateUniqueRoom )
                    {
                        g_tEvents.pOnCreateUniqueRoom( app, pRoom, 0 );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData msgData;

                        int nError = 0;
                        msgData.Append( (unsigned char *)&pRoom,  sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&nError, sizeof(nError) );
                        msgData.Append( (unsigned char *)pRoom->m_szRoomId, sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_CREATE_UNIQUE_CHATROOM, app, app->m_pUserData, 
                            msgData.GetMsgData(), msgData.GetMsgLength() );
                    }



                    // ֪ͨ�����������Լ�
                    LibJingleEx::TJid tItemJid;
                    LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), tItemJid );

                    LibJingleEx::EPresence * peShow = &app->m_ePres;
                    char * pStatus = 0;

                    BOOL bSelf = TRUE;
                    int nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_FIRST_ENTER;

                    if ( g_tEvents.pOnParticipantPresenceChange )
                    {
                        g_tEvents.pOnParticipantPresenceChange( pRoom, &tItemJid, peShow, pStatus, 
                                                                pRoom->m_eAffiliation, bSelf, nFlag, app );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&tItemJid, sizeof(tItemJid) );

                        LibJingleEx::EPresence * pTmpShow = peShow;
                        msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                        char * pTmpDescription = pStatus;
                        msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                        msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );

                        msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                        msgData.Append( (unsigned char *)&nFlag,    sizeof(nFlag) );

                        LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &tItemJid );
                        msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                                        msgData.GetMsgData(),  msgData.GetMsgLength()  );
                    }
                    // END



                    // ����ǳ־��Եķ��䱣������
                    if ( pRoom->m_bPersistent )
                    {
                        // ͬ������ͬresource
                        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
                        message->SetAttr( buzz::QN_TYPE, "inner" );

                        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                        message->AddElement( operation );
                        operation->SetAttr( buzz::QN_TYPE, "add persistent room" );

                        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
                        operation->AddElement( item );

                        item->SetAttr( buzz::QN_ROOM_ID,          pRoom->m_szRoomId  );
                        item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );

                        client->SendStanza( message );
                        delete message;
                        // END


                        PTPersistentRoom  p = new TPersistentRoom;
                        strcpy( p->m_szRoomId,               pRoom->m_szRoomId );
                        strcpy( p->m_szGroupChatServiceName, pRoom->m_szGroupChatServiceName );
                        app->m_vPersistentRooms.push_back( p );
                        app->m_bPersistentRoomsChanged = TRUE;

                        MYLOG("=====[ ON_CREATE_UNIQUE_CHATROOM: %s@%s, count = %u, unvailable count = %u ] \r\n", 
                            pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                        SavePersistentRoom(app);
                    }
                    //// 


#if SYNC_CHATROOM_FLAG
                    // ͬ�������ն˽���������
                    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                    message->SetAttr( buzz::QN_TO,   client->jid().BareJid().Str() );
                    message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

                    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                    message->AddElement( operation );
                    operation->SetAttr( buzz::QN_TYPE, OPERATION_TYPE_SYNC_CHATROOM );

                    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM, true );
                    operation->AddElement( item );

                    item->SetAttr( buzz::QN_ROOM_ID,          pRoom->m_szRoomId  );
                    item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );
                    item->SetAttr( buzz::QN_TYPE,             pRoom->m_bPersistent ? "persistent" : "temporary" );
                    item->SetAttr( buzz::QN_NAME,             AnsiToUtf8(pRoom->m_szName, buf, MAX_BUFF_SIZE ) );

                    client->SendStanza( message );
                    delete message;
                    // END
#endif

                }
                return true;
            }
        }
        else if ( muc_admin_query )
        {
            vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), FindRoomByFullId( tJid.sNode, tJid.sDomain )  );
            if ( it != app->m_Rooms.end() )
            {
                PTRoom pRoom = *it;

                // �����û�������Ļ�ȡ������
                if ( !pRoom->m_hasGotMembers )
                {
                    vRoomRoster & v = pRoom->m_vRoster;
                    vRoomRoster::iterator it_r;

                    const XmlElement * item = muc_admin_query->FirstElement();
                    while ( item )
                    {
                        LibJingleEx::TJid    tMemberJid;
                        LibJingleEx::StrJid2TJid( item->Attr(QN_JID).c_str(), tMemberJid );

                        LibJingleEx::EAffiliation eAffiliation = StrAffi2Int( item->Attr( QN_MUC_AFFILIATION ).c_str() );

                        it_r = find_if ( v.begin(), v.end(), FindRoomRosterItem( &tMemberJid) );
                        if ( v.end() == it_r )
                        {
                            LibJingleEx::PTRoomRosterItem  pItem = new LibJingleEx::TRoomRosterItem;
                            memcpy( &pItem->m_tJid, &tMemberJid, sizeof(LibJingleEx::TJid) );
                            pItem->m_eAlliliation = eAffiliation;
                            pItem->m_bSelf = ( client->jid().BareJid().Str() == item->Attr(QN_JID) ? TRUE : FALSE );
                            v.push_back( pItem );
                        }

                        item = item->NextElement();
                    }


                    // �����LastQueryId�������Ƿ������һ�β�ѯroster items��query id
                    if ( '\0' != pRoom->m_szLastRosterQueryId[0] )
                    {
                        // ��������һ�β�ѯroster items��query id
                        if ( iq->Attr( QN_ID ) == pRoom->m_szLastRosterQueryId )
                        {
                            if ( g_pfnOnMessage )
                            {
                                // ��ѯ��Щ�ǲ����Ѿ���ϯ��Ա֮�ڵ�
                                vRoomRoster vTmpRoster;
                                std::copy( v.begin(), v.end(), back_inserter(vTmpRoster) );

                                vRoomMember::iterator it_m;
                                for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
                                {
                                    PTRoomMember p = *it_m;
                                    it_r = find_if( vTmpRoster.begin(), vTmpRoster.end(), FindRoomRosterItem( &p->m_tJid ) );
                                    if ( vTmpRoster.end() != it_r )
                                    {
                                        vTmpRoster.erase( it_r );
                                    }
                                }
                                // END (��ѯ��Щ�ǲ����Ѿ���ϯ��Ա֮�ڵ�)



                                // ��ȥ��Щ���������˳���
                                std::vector<LibJingleEx::PTJid>::iterator  ix;
                                for ( ix = pRoom->m_vQueryQuitMembers.begin(); ix != pRoom->m_vQueryQuitMembers.end(); ix++ )
                                {
                                    LibJingleEx::PTJid ptJid = *ix;
                                    it_r = find_if( vTmpRoster.begin(), vTmpRoster.end(), FindRoomRosterItem( ptJid ) );
                                    if ( vTmpRoster.end() != it_r )
                                    {
                                        vTmpRoster.erase( it_r );
                                    }
                                }
                                // END(��ȥ��Щ���������˳���)



                                // ֪ͨ�ϲ��Ա�б�(�����ߵ�)
                                if ( vTmpRoster.size() > 0 )
                                {
                                    unsigned long  dwTime  =  vTmpRoster.size() / LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT;
                                    unsigned long  dwResue =  vTmpRoster.size() % LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT;

                                    unsigned long i;
                                    unsigned long dwCnt = 0;

                                    for ( i = 0; i < dwTime; i++ )
                                    {
                                        LibJingleEx::CMsgData   msgData;

                                        dwCnt = LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT;

                                        msgData.Append( (unsigned char *)&pRoom,    sizeof(pRoom) );
                                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,    sizeof(pRoom->m_szRoomId) );
                                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                                        msgData.Append( (unsigned char *)&dwCnt,    sizeof(dwCnt) );

                                        unsigned long j;
                                        for ( j = 0; j < LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT; j++ )
                                        {
                                            LibJingleEx::PTRoomRosterItem & p = vTmpRoster[LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT*i + j];
                                            msgData.Append( (unsigned char *)p,     sizeof(LibJingleEx::TRoomRosterItem) );
                                        }

                                        g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_ADDED, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                                    }

                                    if ( dwResue > 0 )
                                    {
                                        LibJingleEx::CMsgData   msgData;

                                        dwCnt = dwResue;

                                        msgData.Append( (unsigned char *)&pRoom,    sizeof(pRoom) );
                                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,    sizeof(pRoom->m_szRoomId) );
                                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                                        msgData.Append( (unsigned char *)&dwCnt,    sizeof(dwCnt) );

                                        unsigned long j;
                                        for ( j = 0; j < dwResue; j++ )
                                        {
                                            LibJingleEx::PTRoomRosterItem & p = vTmpRoster[LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT*dwTime+j];
                                            msgData.Append( (unsigned char *)p,     sizeof(LibJingleEx::TRoomRosterItem) );
                                        }

                                        g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_ADDED, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                                    }
                                }
                                // END( ֪ͨ�ϲ��Ա�б�(�����ߵ�) )
                            }

                            pRoom->m_szLastRosterQueryId[0] = '\0';
                            pRoom->m_hasGotMembers = TRUE;
                        }
                    }
                }
                // �����Ļ�ȡ��һ������(�����ݶԱ�)
                else
                {
                    // �Ȱ�roster���ݴ洢��vTmpRoster��
                    vRoomRoster & v = pRoom->m_vTmpRoster;
                    vRoomRoster::iterator it_r;

                    const XmlElement * item = muc_admin_query->FirstElement();
                    while ( item )
                    {
                        LibJingleEx::TJid    tMemberJid;
                        LibJingleEx::StrJid2TJid( item->Attr(QN_JID).c_str(), tMemberJid );
                        LibJingleEx::EAffiliation eAffiliation = StrAffi2Int( item->Attr( QN_MUC_AFFILIATION ).c_str() );

                        it_r = find_if ( v.begin(), v.end(), FindRoomRosterItem( &tMemberJid) );
                        if ( v.end() == it_r )
                        {
                            LibJingleEx::PTRoomRosterItem  pItem = new LibJingleEx::TRoomRosterItem;
                            memcpy( &pItem->m_tJid, &tMemberJid, sizeof(LibJingleEx::TJid) );
                            pItem->m_eAlliliation = eAffiliation;
                            pItem->m_bSelf = ( client->jid().BareJid().Str() == item->Attr(QN_JID) ? TRUE : FALSE );
                            v.push_back( pItem );
                        }
                        item = item->NextElement();
                    }

                    // ��������һ�β�ѯroster items��query id����Ƚ�vTmpRoster��vRoster
                    if ( iq->Attr( QN_ID ) == pRoom->m_szLastRosterQueryId )
                    {
                        // ��ѯ��Щ�ǲ����Ѿ���ϯ��Ա֮�ڵ�
                        //vRoomRoster vTmpRoster;
                        //std::copy( v.begin(), v.end(), back_inserter(vTmpRoster) );

                        //vRoomMember::iterator it_m;
                        //for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
                        //{
                        //    PTRoomMember p = *it_m;
                        //    it_r = find_if( vTmpRoster.begin(), vTmpRoster.end(), FindRoomRosterItem( &p->m_tJid ) );
                        //    if ( vTmpRoster.end() != it_r )
                        //    {
                        //        vTmpRoster.erase( it_r );
                        //    }
                        //}
                        //// END (��ѯ��Щ�ǲ����Ѿ���ϯ��Ա֮�ڵ�)


                        //// ��ȥ��Щ���������˳���
                        //std::vector<LibJingleEx::PTJid>::iterator  ix;
                        //for ( ix = pRoom->m_vQueryQuitMembers.begin(); ix != pRoom->m_vQueryQuitMembers.end(); ix++ )
                        //{
                        //    LibJingleEx::PTJid ptJid = *ix;
                        //    it_r = find_if( vTmpRoster.begin(), vTmpRoster.end(), FindRoomRosterItem( ptJid ) );
                        //    if ( vTmpRoster.end() != it_r )
                        //    {
                        //        vTmpRoster.erase( it_r );
                        //    }
                        //}
                        //// END(��ȥ��Щ���������˳���)


                        // �Ƚ�
                        vRoomRoster  vAdded;
                        vRoomRoster  vRemoved;

                        for ( it_r = pRoom->m_vRoster.begin(); it_r != pRoom->m_vRoster.end(); it_r++ )
                        {
                            LibJingleEx::PTRoomRosterItem  pItem = *it_r;

                            vRoomRoster::iterator it_find = find_if(  v.begin(), v.end(), 
                                FindRoomRosterItem( &pItem->m_tJid ) );

                            // û�ҵ����Ž��Ƴ�vector��
                            if ( v.end() == it_find )
                            {
                                vRemoved.push_back( pItem );
                            }
                        }

                        for ( it_r = v.begin(); it_r != v.end(); it_r++ )
                        {
                            LibJingleEx::PTRoomRosterItem  pItem = *it_r;

                            vRoomRoster::iterator it_find = find_if(  pRoom->m_vRoster.begin(), pRoom->m_vRoster.end(), 
                                FindRoomRosterItem( &pItem->m_tJid ) );

                            // û�ҵ����Ž�����vector��
                            if ( pRoom->m_vRoster.end() == it_find )
                            {
                                vAdded.push_back( pItem );
                            }
                        }
                        // END(�Ƚ�)


                        unsigned long  dwLeft      = 0;
                        unsigned long  dwIndex     = 0;
                        vRoomRoster *  pVector     = 0;

                        // ֪ͨ��ӵ�roster�ͼ��ٵ�roster
                        pVector = &vAdded;
                        dwLeft  = pVector->size();
                        dwIndex = 0;

                        while( dwLeft > 0 )
                        {
                            unsigned long dwCnt = 0;
                            if ( dwLeft >= LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT )
                            {
                                dwCnt = LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT;
                            }
                            else
                            {
                                dwCnt = dwLeft;
                            }


                            LibJingleEx::CMsgData   msgData;
                            msgData.Append( (unsigned char *)&pRoom,                             sizeof(pRoom) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );
                            msgData.Append( (unsigned char *)&dwCnt,                             sizeof(dwCnt) );

                            unsigned long j;
                            for ( j = 0; j < dwCnt; j++ )
                            {
                                LibJingleEx::PTRoomRosterItem & p = pVector->at(dwIndex + j);
                                msgData.Append( (unsigned char *)p,     sizeof(LibJingleEx::TRoomRosterItem) );
                            }

                            if ( g_pfnOnMessage )
                            {
                                g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_ADDED, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                            }

                            dwLeft      -= dwCnt;
                            dwIndex     += dwCnt;
                        }


                        pVector = &vRemoved;
                        dwLeft  = pVector->size();
                        dwIndex = 0;

                        while( dwLeft > 0 )
                        {
                            unsigned long dwCnt = 0;
                            if ( dwLeft >= LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT )
                            {
                                dwCnt = LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT;
                            }
                            else
                            {
                                dwCnt = dwLeft;
                            }


                            LibJingleEx::CMsgData   msgData;
                            msgData.Append( (unsigned char *)&pRoom,                             sizeof(pRoom) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );
                            msgData.Append( (unsigned char *)&dwCnt,                             sizeof(dwCnt) );

                            unsigned long j;
                            for ( j = 0; j < dwCnt; j++ )
                            {
                                LibJingleEx::PTRoomRosterItem & p = pVector->at(dwIndex + j);
                                msgData.Append( (unsigned char *)p,     sizeof(LibJingleEx::TRoomRosterItem) );
                            }

                            if ( g_pfnOnMessage )
                            {
                                g_pfnOnMessage( LibJingleEx::ON_ROOM_ROSTER_DELETED, app, app->m_pUserData, msgData.GetMsgData(), msgData.GetMsgLength() );
                            }

                            dwLeft      -= dwCnt;
                            dwIndex     += dwCnt;
                        }



                        // ����vTmpRoster��vRoster
                        ClearVector( pRoom->m_vRoster );
                        std::copy( v.begin(), v.end(), back_inserter(pRoom->m_vRoster) );
                        v.clear();
                    }
                }


            }
        }
    }

    return false;
}

void SetChatroomConfigure( PTRoom pRoom, LibJingleEx::TJid & tJid, buzz::XmppClient * client, const XmlElement * item )
{
    char szId[MAX_RAND_ID_LENGTH];
    char buf[MAX_BUFF_SIZE];

    std::string sRoomFullId;

    // ���÷���Ĭ������Ϊpublic to all
    sRoomFullId  = tJid.sNode;
    sRoomFullId += '@';
    sRoomFullId += tJid.sDomain;

    XmlElement * new_iq = new XmlElement( QN_IQ );
    new_iq->SetAttr( QN_TYPE, "set" );
    GenerateRandomId(szId,MAX_RAND_ID_LENGTH);
    new_iq->SetAttr( QN_ID, szId );
    new_iq->SetAttr( QN_TO, sRoomFullId );

    XmlElement * new_query = new XmlElement( QN_MUC_QUERY_OWNER, true );
    new_iq->AddElement( new_query );

    XmlElement * new_x = new XmlElement( QName(true, "jabber:x:data", "x"), true );
    new_query->AddElement( new_x );
    new_x->SetAttr( QN_TYPE, "submit" );

    XmlElement * filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
    new_x->AddElement( filed );
    filed->SetAttr( QName(true, "", "var"), "FORM_TYPE" );

    XmlElement * new_value = new XmlElement( QN_VALUE  );
    filed->AddElement( new_value );
    new_value->SetBodyText("http://jabber.org/protocol/muc#roomconfig");


    // ��ӷ�����
    if ( pRoom->m_szName[0] != '\0' )
    {
        filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( QName(true, "", "var"), "muc#roomconfig_roomname" );
        filed->SetAttr( QN_TYPE,  "text-single" );
        filed->SetAttr( QN_LABEL, "Room title" );

        new_value = new XmlElement( QN_VALUE );
        filed->AddElement( new_value );
        new_value->SetBodyText(AnsiToUtf8(pRoom->m_szName, buf, MAX_BUFF_SIZE));
    }
    // ���� ��ӷ�����


    // ���ó־÷���
    if ( pRoom->m_bPersistent )
    {
        filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( QName(true, "", "var"), "muc#roomconfig_persistentroom" );
        filed->SetAttr( QN_TYPE,  "boolean" );
        filed->SetAttr( QN_LABEL, "Make room persistent" );

        new_value = new XmlElement( QN_VALUE );
        filed->AddElement( new_value );
        new_value->SetBodyText("1");
    }
    // �������ó־÷���


    filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
    new_x->AddElement( filed );
    filed->SetAttr( QName(true, "", "var"), "muc#roomconfig_whois" );

    new_value = new XmlElement( QN_VALUE );
    filed->AddElement( new_value );
    new_value->SetBodyText("anyone");


    // ����ǳ־��Է��䣬ֻ�ܻ�Ա����(����һ�����ʱ������)
    if ( pRoom->m_bPersistent || ( ROOM_TYPE_NORMAL == pRoom->m_nType ) )
    {
        filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( QName(true, "", "var"), "muc#roomconfig_membersonly" );

        new_value = new XmlElement( QN_VALUE );
        filed->AddElement( new_value );
        new_value->SetBodyText("1");
    }

    if ( ROOM_TYPE_SPECIAL == pRoom->m_nType )
    {
        filed = new XmlElement( QName(true, STR_EMPTY, "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( QName(true, "", "var"), "muc#roomconfig_maxusers" );

        new_value = new XmlElement( QN_VALUE );
        filed->AddElement( new_value );

        SNPRINTF( buf, MAX_BUFF_SIZE, "%u", SPECIAL_ROOM_MAX_USERS_CNT );
        new_value->SetBodyText(buf);
    }

    client->SendStanza( new_iq );
    delete new_iq;
    // (���÷���Ĭ������Ϊpublic to all)����

    strcpy( pRoom->m_szQueryId, szId );
    pRoom->m_eAffiliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );
    strcpy( pRoom->m_szNick, tJid.sResource );    //�����Լ���nick
    pRoom->m_eState = ROOM_STATE_ON_CONFIG;


    // ��Ƶ�����״̬�����Ҳ���subject
    if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
    {
        // ������һ��subject
        XmlElement * message = new XmlElement( QN_MESSAGE );
        message->SetAttr( QN_TO, sRoomFullId );
        message->SetAttr( QN_TYPE, "groupchat" );
        GenerateRandomId(szId,MAX_RAND_ID_LENGTH, 0);
        message->SetAttr( QN_ID, szId );

        XmlElement * subject = new XmlElement( QN_SUBJECT );
        message->AddElement( subject );

        client->SendStanza( message );
        delete message;
    }
}



bool MyChatRoomTask::Handle_presence( const XmlElement * presence )
{
    XmppClient * client = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );

    char buf[MAX_BUFF_SIZE];
    char buf_tmp[MAX_BUFF_SIZE];

    const XmlElement * x      = 0;
    // const XmlElement * c      = 0;
    const XmlElement * show   = 0;
    const XmlElement * status = 0;
    // const XmlElement * query  = 0;
    const XmlElement * item = 0;
    const XmlElement * destroy = 0;
    bool abStatus[4] = { false, false, false, false };    // 110, 201, 307, 321
    const XmlElement * xx     = 0;
    const XmlElement * c      = 0;
    bool  fileshare_ability   = false;
    bool  audio_ability   = false;
    bool  video_ability   = false;
    bool  call_ability    = false;
    std::string presence_version;

    const XmlElement * child = presence->FirstElement();
    while( child )
    {
        if ( child->Name() == QN_STATUS )
        {
            status = child;
        }
        else if ( child->Name() == QN_SHOW )
        {
            show = child;         
        }
        else if ( child->Name() == QN_MUC_USER_X )
        {
            x = child;

            const XmlElement * gchild = x->FirstElement();
            while( gchild )
            {
                if ( gchild->Name() == QN_MUC_USER_ITEM )
                {
                    item = gchild;
                }
                else if ( gchild->Name() == QName(true, NS_MUC_USER, "destroy") )
                {
                    destroy = gchild;
                }
                else if ( gchild->Name() == QN_MUC_USER_STATUS )
                {
                    if ( gchild->Attr(QN_CODE) == "110" )
                    {
                        abStatus[0] = true;
                    }
                    else if ( gchild->Attr(QN_CODE) == "201" )
                    {
                        abStatus[1] = true;
                    }
                    else if ( gchild->Attr(QN_CODE) == "307" )
                    {
                        abStatus[2] = true;
                    }
                    else if ( gchild->Attr(QN_CODE) == "321" )
                    {
                        abStatus[3] = true;
                    }
                }
                gchild = gchild->NextElement();
            }
        }
        else if ( child->Name() == QN_MUC_X )
        {
            xx = child;
        }
        else if ( child->Name() == QN_CAPS_C )
        {
            c = child;

            std::string node  = c->Attr(QN_NODE);
            presence_version  = c->Attr(QN_VER);
            std::string exts  = c->Attr(QN_EXT);

            if ( node == GOOGLE_CLIENT_NODE )
            {
                if ( ListContainsToken(exts, "share-v1") )
                {
                    fileshare_ability = true;
                }

                if ( ListContainsToken(exts, "audio") )
                {
                    audio_ability = true;
                }

                if ( ListContainsToken(exts, "video") )
                {
                    video_ability = true;
                }

                if ( ListContainsToken(exts, "call") )
                {
                    call_ability = true;
                }

            }
        }
        child = child->NextElement();
    }

    // û��xԪ�أ���MUC��չ
    if ( 0 == x && 0 == xx )
    {
        return false;
    }

    if ( xx )
    {
        // Ҳ���䴴��ʧ�� 
        if ( presence->Attr( QN_TYPE ) == "error" )
        {
            // ת��jid
            LibJingleEx::TJid tJid;
            LibJingleEx::StrJid2TJid( presence->Attr(QN_FROM).c_str(), tJid );

            // ���������������
            vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                          FindRoomByFullId(tJid.sNode,tJid.sDomain) );
            // ���������
            if ( it != app->m_Rooms.end() )
            {
                PTRoom pRoom = *it;
    
                // try entering persistent room
                if ( ROOM_STATE_TRYING_PERSISTENT == pRoom->m_eState )
                {
                    vPersistentRoom::iterator it_try = find_if( app->m_vTryingPersistents.begin(), app->m_vTryingPersistents.end(),
                                                                FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName )   );
                    if ( it_try != app->m_vTryingPersistents.end() )
                    {
                        delete * it_try;
                        app->m_vTryingPersistents.erase( it_try );
                    }

                    // ��������һ������
                    if (  0 == app->m_vTryingPersistents.size() )
                    {
                        if ( app->m_bPersistentRoomsChanged )
                        {
                            MYLOG("=====[ the last enter persistent room error, state = ROOM_STATE_TRYING_PERSISTENT, count = %u, unvailable count = %u ] \r\n", 
                                app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                            SavePersistentRoom(app);
                        }
                    }
                }
                // entering persistent room entered in the latest login session
                else if (  (ROOM_STATE_ENTERING_PERSISTENT == pRoom->m_eState) || (ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState) )
                {
                    // ɾ��������ʷ��¼(��Ϊÿ�εǳ����¼�����Բ��������浽˽������)
                    app->EraseHistoryRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    //// END ɾ��������ʷ��¼

                    vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(),
                        app->m_vPersistentRooms.end(),
                        FindPersistentRoom( pRoom->m_szRoomId,pRoom->m_szGroupChatServiceName ) );

                    if ( it_p != app->m_vPersistentRooms.end() )
                    {
                        PTPersistentRoom p = *it_p;
                        p->m_bNotFound = TRUE;

                        if ( app->m_vPersistentRooms.size() > MIN_SAVE_PERSISTENT_CHATROOM_COUNT )
                        {
                            // �ҵ���һ����ѯ�����ڵ�chat roomɾ��
                            vPersistentRoom::iterator it_first_not_found;
                            for ( it_first_not_found = app->m_vPersistentRooms.begin(); it_first_not_found != app->m_vPersistentRooms.end(); it_first_not_found++ )
                            {
                                PTPersistentRoom pFirstNotFound = *it_first_not_found;
                                if ( pFirstNotFound->m_bNotFound )
                                {
                                    delete pFirstNotFound;
                                    app->m_vPersistentRooms.erase( it_first_not_found );
                                    app->m_bPersistentRoomsChanged = TRUE;

                                    break;
                                }
                            }
                        }


                        if ( app->m_bPrivatePersistentRoomsCntConfirmed )
                        {
                            MYLOG("=====[ enter persistent room error: %s@%s,  count = %u, unvailable count =%u ] \r\n", 
                                pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                            SavePersistentRoom( app );
                        }

                    }

                    
                    // ֪ͨ�ϲ����̶�Ⱥʧ��
                    if ( ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState )
                    {
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)pRoom->m_szName,                   sizeof(pRoom->m_szName) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,                 sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName) );

                            g_pfnOnMessage( LibJingleEx::ON_ENTER_PERSISTENT_ROOM_FAIL, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }
                    }
                    // END ( ֪ͨ�ϲ����̶�Ⱥʧ�� )

                }
                // һ��ĵ�ǰlogin session����
                else
                {
                    if ( g_tEvents.pOnCreateUniqueRoom )
                    {
                        g_tEvents.pOnCreateUniqueRoom( app, pRoom, -1 );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData msgData;

                        int nError = -1;
                        msgData.Append( (unsigned char *)&pRoom,  sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&nError, sizeof(nError) );
                        msgData.Append( (unsigned char *)pRoom->m_szRoomId, sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_CREATE_UNIQUE_CHATROOM, app, app->m_pUserData, 
                            msgData.GetMsgData(), msgData.GetMsgLength() );
                    }
                }

                // ɾ���ڴ�
                delete  pRoom;
                app->m_Rooms.erase( it );
            }
        }
        return true;
    }

    LibJingleEx::EPresence    eShow = LibJingleEx::PRESENCE_NONE;
    LibJingleEx::EPresence * peShow = 0;

    char szStatus[256] = {0};
    char * pStatus     = 0;

    char szId[MAX_RAND_ID_LENGTH];

    if ( show )
    {
        eShow = StrShow2Int( show->BodyText().c_str() );
        peShow = &eShow;
    }
    else
    {
        eShow = LibJingleEx::PRESENCE_ONLINE;
        peShow = &eShow;
    }

    if ( status )
    {
        Utf8ToAnsi( status->BodyText().c_str(), szStatus, 256 );
        pStatus = szStatus;
    }

    // ת��jid
    LibJingleEx::TJid tJid;
    LibJingleEx::StrJid2TJid( presence->Attr(QN_FROM).c_str(), tJid );

    // ���������������
    vRoom::iterator it = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
        FindRoomByFullId(tJid.sNode,tJid.sDomain) );

    // ���������
    if ( it != app->m_Rooms.end() )
    {
        PTRoom pRoom = *it;

        BOOL   bSelf    = FALSE;
        //BOOL   bEntered = FALSE;               // �Ƿ��״ν��뷿��
        int    nFlag    = 0;                     // 0: �뿪���䣬1: ��һ�ν��뷿�䣬2: ״̬�ı�
                                                 // 3: ������Ա�߳�����

        // ����ǻش����Լ� ( code 110 )
        if ( abStatus[0] )
        {
            bSelf = TRUE;

            // ����Ǵ�������ɹ�( code 201 )
            if ( abStatus[1] )
            {
                // �Ѿ��ջ��Լ���״̬��
                pRoom->m_bReceivedSelfPresence = TRUE;

                // ������뿪(һ�㲻���õ�)
                if ( presence->Attr(QN_TYPE) == "unavailable" )
                {
                    assert( 0 );
                }
                // ���뿪
                else
                {
                    // ��Ϊ�Ѿ���ȡ���г�Ա�б�
                    pRoom->m_bGetMembers = TRUE;              


                    // ״̬���䲻��¼vMembers
                    if ( ROOM_TYPE_SPECIAL != pRoom->m_nType )
                    {
                        // ���Լ�����vMembers��ȥ
                        PTRoomMember  ptMember = new TRoomMember;
                        ptMember->m_bSelf = TRUE;
                        ptMember->m_eAlliliation = LibJingleEx::AFFILIATION_OWNER;
                        LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), ptMember->m_tJid );
                        strcpy( ptMember->m_szNick, tJid.sResource );                    
                        ptMember->m_ePres = eShow;
                        pRoom->m_vMembers.push_back( ptMember );
                        // END(���Լ�����vMembers��ȥ)
                    }
                    // END(״̬���䲻��¼vMembers)



                    // ������յ���������ģ����Ǹ÷����Ѿ���������
                    if ( ROOM_STATE_ON_ACCEPTING == pRoom->m_eState )
                    {
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)&pRoom,   sizeof(pRoom) );

                            BOOL  bResult = FALSE;
                            msgData.Append( (unsigned char *)&bResult, sizeof(bResult) );


                            g_pfnOnMessage( LibJingleEx::ON_ACCEPT_INVITE_RESULT, app, app->m_pUserData,
                                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }

                        std::string sTemplate = "<presence to='%s@%s' type='unavailable'/>";

                        SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                        client->SendRaw( buf );

                        app->m_Rooms.erase( it );
                        delete pRoom;
                    }
                    else if ( (ROOM_STATE_ON_PRESENCE == pRoom->m_eState) && item
                        && (item->Attr(QN_JID) == client->jid().Str())   )
                    {
                        SetChatroomConfigure( pRoom, tJid, client, item );
                    }
                    // ���������̶�Ⱥ��ȴ����Ⱥ�Ѳ���( ��ѯһ�̿��ڣ�����һ��ȴ������)
                    else if ( (ROOM_STATE_ENTERING_PERSISTENT == pRoom->m_eState) ||  (ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState)  )
                    {
                        // �˳�����
                        std::string sTemplate = "<presence to='%s@%s' type='unavailable'/>";
                        SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                        client->SendRaw( buf );
                        // END


                        // ɾ��������ʷ��¼(��Ϊÿ�εǳ����¼�����Բ��������浽˽������)
                        app->EraseHistoryRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                        // END ɾ��������ʷ��¼

                        vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(),
                            app->m_vPersistentRooms.end(),
                            FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName ) );

                        assert( it_p != app->m_vPersistentRooms.end() );
                        PTPersistentRoom p = *it_p;
                        app->m_vPersistentRooms.erase( it_p );
                        delete p;
                        app->m_bPersistentRoomsChanged = TRUE;

                        if ( app->m_bPrivatePersistentRoomsCntConfirmed )
                        {
                            MYLOG("=====[ persistent room does not exist: %s@%s state = ROOM_STATE_ENTERING_PERSISTEN( or ROOM_STATE_HOLD_PERSISTENT ), count = %u, unvailable count = %u ] \r\n", 
                                                                       pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(),
                                                                       GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                            SavePersistentRoom( app );
                        }


                        // ����Ǵ�����Ĺ̶�Ⱥ��֪ͨ�ϲ����ʧ��
                        if ( ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState )
                        {
                            if ( g_pfnOnMessage )
                            {
                                LibJingleEx::CMsgData  msgData;

                                msgData.Append( (unsigned char *)pRoom->m_szName,                   sizeof(pRoom->m_szName) );
                                msgData.Append( (unsigned char *)pRoom->m_szRoomId,                 sizeof(pRoom->m_szRoomId) );
                                msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName) );
                                g_pfnOnMessage( LibJingleEx::ON_ENTER_PERSISTENT_ROOM_FAIL, app, app->m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength()  );
                            }
                        }
                        
                        
                        app->m_Rooms.erase( it );
                        delete pRoom;

                        return true;
                    }
                }
            }
            // ����ǷǴ�������ɹ�( û��code 201 )
            else
            {
                // ���������Ը�뿪��
                if ( presence->Attr(QN_TYPE) == "unavailable" && !pRoom->m_bQuitOnSelfOwn )
                {
                    // ����һ����Ϣȥ���½���
                    ChatRoomMsg * pParam = new ChatRoomMsg;
                    strcpy( pParam->m_szRoomId,               pRoom->m_szRoomId );
                    strcpy( pParam->m_szGroupChatServiceName, pRoom->m_szGroupChatServiceName );
                    strcpy( pParam->m_szName,                 pRoom->m_szName );
                    pParam->m_nType = pRoom->m_nType;
                    pParam->m_bPersistent = pRoom->m_bPersistent;

                    app->m_pXmppThread->Post( app->m_pXmppThread, MSG_RE_ENTER_CHATROOM, pParam  );
                }



                // �����֪�Լ�״̬������
                if ( ROOM_TYPE_SPECIAL == pRoom->m_nType )
                {
                    if ( presence->Attr(QN_TYPE) == "unavailable" )
                    {
                        app->m_Rooms.erase( it );
                        delete pRoom;
                    }
                    else
                    {
                        pRoom->m_eState = ROOM_STATE_AVAILABLE;
                    }
                    return true;
                }

                // ������뿪
                if ( presence->Attr(QN_TYPE) == "unavailable" )
                {
                    LibJingleEx::TJid tSelfJid;
                    LibJingleEx::StrJid2TJid( presence->Attr(QN_TO).c_str(), tSelfJid );

                    // �������Ը�˳�
                    if ( pRoom->m_bPersistent && pRoom->m_bNeverIn )
                    {
                        nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED;
                    }

                    eShow = LibJingleEx::PRESENCE_NONE;
                    if ( g_tEvents.pOnParticipantPresenceChange )
                    {
                        g_tEvents.pOnParticipantPresenceChange( pRoom, &tSelfJid, &eShow, 0, 
                            pRoom->m_eAffiliation, bSelf, nFlag, app );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&tSelfJid, sizeof(tSelfJid) );

                        LibJingleEx::EPresence * pTmpShow = &eShow;
                        msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                        char * pTmpDescription = 0;
                        msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                        msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );

                        msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                        msgData.Append( (unsigned char *)&nFlag, sizeof(nFlag) );

                        pRoom->SetMemberPresence( &tSelfJid, LibJingleEx::PRESENCE_NONE );
                        LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &tSelfJid );
                        msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                    }

                    // ����������ٽ��־��Է�����
                    if ( pRoom->m_bPersistent && pRoom->m_bNeverIn )
                    {
                        vPersistentRoom::iterator it_p = find_if(  app->m_vPersistentRooms.begin(),
                                                                   app->m_vPersistentRooms.end(),
                            FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName ) );

                        if ( it_p != app->m_vPersistentRooms.end() )
                        {

                            // ͬ������ͬresource
                            buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                            message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
                            message->SetAttr( buzz::QN_TYPE, "inner" );

                            buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                            message->AddElement( operation );
                            operation->SetAttr( buzz::QN_TYPE, "delete persistent room" );

                            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
                            operation->AddElement( item );

                            item->SetAttr( buzz::QN_ROOM_ID,          pRoom->m_szRoomId  );
                            item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );

                            client->SendStanza( message );
                            delete message;
                            // END

                
                            delete (*it_p);
                            app->m_vPersistentRooms.erase( it_p );
                            app->m_bPersistentRoomsChanged = TRUE;

                            MYLOG("=====[ quit persistent room: %s@%s,  count = %u, unvailable count = %u ] \r\n", 
                                pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                            SavePersistentRoom( app );
                        }
                    }

                    app->m_Rooms.erase( it );
                    delete pRoom;
                }
                // ���뿪(ͨ����������룬���Զ�������һ�εĹ̶�Ⱥ)
                else
                {
                    nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_CHANGE;

                    if ( item )
                    {
                        pRoom->m_eAffiliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );
                    }

                    strcpy( pRoom->m_szNick, tJid.sResource );


                    // ������״̬���ͷ��䣬����Ƿ���Ƶ���鷿��
                    // if ( ROOM_TYPE_VISUAL != pRoom->m_nType )
                    {
                        // �鿴m_vMembers���Ƿ����Լ�
                        vRoomMember::iterator it_m;
                        for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
                        {
                            PTRoomMember p = *it_m;
                            if ( p->m_bSelf )
                            {
                                break;
                            }
                        }

                        // ���û���Լ������Լ����뵽m_vMembers��ȥ
                        if ( it_m == pRoom->m_vMembers.end() )
                        {
                            // ���Լ�����vMembers��ȥ
                            PTRoomMember  ptMember = new TRoomMember;
                            ptMember->m_bSelf = TRUE;
                            ptMember->m_eAlliliation = pRoom->m_eAffiliation;
                            LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), ptMember->m_tJid );
                            strcpy( ptMember->m_szNick, tJid.sResource );    
                            assert( peShow );
                            ptMember->m_ePres = eShow;
                            pRoom->m_vMembers.push_back( ptMember );
                            // END(���Լ�����vMembers��ȥ

                            nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_FIRST_ENTER;


                            // �����owner������Ƿ���decline��Ϣ
                            if ( LibJingleEx::AFFILIATION_OWNER == ptMember->m_eAlliliation )
                            {
                                std::vector<PTDeclineItem>::iterator it_d;
                                for (  it_d = app->m_vDeclineItems.begin(); it_d != app->m_vDeclineItems.end();  )
                                {
                                    PTDeclineItem  pItem = *it_d;

                                    // ����ҵ�������decline��Ϣ
                                    if (    ( 0 == strcmp(pItem->szRoomId, pRoom->m_szRoomId) )
                                         && ( 0 == strcmp(pItem->szService, pRoom->m_szGroupChatServiceName) ) )
                                    {
                                        DeleteChatroomMember( pRoom, &pItem->tDeclineJid, app->m_pXmppThread->client() );

                                        delete pItem;
                                        it_d = app->m_vDeclineItems.erase( it_d );
                                    }
                                    // ���û���ҵ�, it_d����
                                    else
                                    {
                                        it_d++;
                                    }
                                }
                            }
                        } // END(�鿴m_vMembers���Ƿ����Լ�)
                        else
                        {
                            PTRoomMember p = *it_m;
                            p->m_ePres = eShow;
                        }
                    }

                    RoomState  ePreRoomState = pRoom->m_eState;

                    if ( pRoom->m_eState == ROOM_STATE_ON_ACCEPTING )
                    {
                        //strcpy( pRoom->m_szNick, tJid.sResource );
                        // ֪ͨ��������ɹ�
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)&pRoom,   sizeof(pRoom) );

                            BOOL  bResult = TRUE;
                            msgData.Append( (unsigned char *)&bResult, sizeof(bResult) );

                            g_pfnOnMessage( LibJingleEx::ON_ACCEPT_INVITE_RESULT, app, app->m_pUserData,
                                msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }
                        // END ֪ͨ��������ɹ�
                    }
                    else if ( pRoom->m_eState == ROOM_STATE_ENTERING_PERSISTENT || ROOM_STATE_TRYING_PERSISTENT == pRoom->m_eState || ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState )
                    {
                        // ֪ͨ�����˹̶�Ⱥ
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)&pRoom,                 sizeof(pRoom) );
                            msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );
                            msgData.Append( (unsigned char *)pRoom->m_szName,        sizeof(pRoom->m_szName) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,      sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName) );
                            msgData.Append( (unsigned char *)&pRoom->m_bPersistent,  sizeof(pRoom->m_bPersistent) );


                            g_pfnOnMessage( LibJingleEx::ON_ENTER_PERSISTENT_ROOM, app, app->m_pUserData,
                                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }
                        // END ֪ͨ�����˹̶�Ⱥ
                    }
                    else if ( pRoom->m_eState == ROOM_STATE_ON_PRESENCE )
                    {
                        // ֪ͨ���뷿��ɹ�
                        if ( g_tEvents.pOnCreateUniqueRoom )
                        {
                            g_tEvents.pOnCreateUniqueRoom( app, pRoom, 1 );
                        }
                        else if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData msgData;

                            int nError = 1;
                            msgData.Append( (unsigned char *)&pRoom,  sizeof(pRoom) );
                            msgData.Append( (unsigned char *)&nError, sizeof(nError) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId, sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                            g_pfnOnMessage( LibJingleEx::ON_CREATE_UNIQUE_CHATROOM, app, app->m_pUserData, 
                                msgData.GetMsgData(), msgData.GetMsgLength() );
                        }
                        // END ֪ͨ���뷿��ɹ�
                    }
                    else
                    {
                        // assert( pRoom->m_eState == ROOM_STATE_NOTIFIED_CREATE );
                        // assert( pRoom->m_eState == ROOM_STATE_AVAILABLE );
                    }

                    // ����ǽ�����Ƶ���飬���󷿼�����
                    if (    pRoom->m_eState == ROOM_STATE_NOTIFIED_CREATE 
                         || pRoom->m_eState == ROOM_STATE_ON_PRESENCE )
                    {
                        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

                        char randid[MAX_RAND_ID_LENGTH];
                        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
                        iq->SetAttr( buzz::QN_ID, randid );

                        SNPRINTF( buf, MAX_BUFF_SIZE, "%s@%s", pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                        iq->SetAttr( buzz::QN_TO, AnsiToUtf8( buf, buf_tmp, MAX_BUFF_SIZE ) );

                        iq->SetAttr( buzz::QN_TYPE, "get" );

                        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_DISCO_INFO_QUERY, true );
                        iq->AddElement( query );

                        client->SendStanza( iq );

                        delete iq;
                    }
                    
                    pRoom->m_eState = ROOM_STATE_AVAILABLE;


                    // ��״̬�����Һ���Ƶ�����ң��ǵ�ȥ��ȡ��Ա�б�
                    if ( ROOM_TYPE_SPECIAL != pRoom->m_nType && ROOM_TYPE_VISUAL != pRoom->m_nType )
                    {
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
                        // �����û��ȥ��ȡ��Ա������Ȩ��, �ǵ�ȥ��ȡ��Ա
                        if (     pRoom->m_bGetSubject && !pRoom->m_bGetMembers 
                            && ( LibJingleEx::AFFILIATION_MEMBER == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                        {
                            GetRoomRoster( client, pRoom );
                            pRoom->m_bGetMembers = TRUE;
                        }
                        else if ( !pRoom->m_bGetMembers 
                            && ( LibJingleEx::AFFILIATION_MEMBER == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                        {
                            SubjectDelayMsg * pDelayMsg = new SubjectDelayMsg;
                            strcpy( pDelayMsg->szRoomId, pRoom->m_szRoomId );
                            strcpy( pDelayMsg->szGroupChatServiceName, pRoom->m_szGroupChatServiceName );

                            app->m_pXmppThread->PostDelayed( INTERVAL_GET_CHATROOM_MEMBERS_DELAY, app->m_pXmppThread, MSG_GET_CHATROOM_MEMBERS_DELAY, pDelayMsg  );
                        }
#else
                        // �����û��ȥ��ȡ��Ա������Ȩ��, �ǵ�ȥ��ȡ��Ա
                        if (     pRoom->m_bGetSubject && !pRoom->m_bGetMembers 
                            && ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                        {
                            GetRoomRoster( client, pRoom );
                            pRoom->m_bGetMembers = TRUE;
                        }
                        else if ( !pRoom->m_bGetMembers 
                            && ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                        {
                            SubjectDelayMsg * pDelayMsg = new SubjectDelayMsg;
                            strcpy( pDelayMsg->szRoomId, pRoom->m_szRoomId );
                            strcpy( pDelayMsg->szGroupChatServiceName, pRoom->m_szGroupChatServiceName );

                            app->m_pXmppThread->PostDelayed( INTERVAL_GET_CHATROOM_MEMBERS_DELAY, app->m_pXmppThread, MSG_GET_CHATROOM_MEMBERS_DELAY, pDelayMsg  );
                        }
#endif
                    }


                    // ���֮ǰpRoom->m_bReceivedSelfPresence��FALSE, �����֪�ϲ����ĳ�Ա��״̬
                    if ( !pRoom->m_bReceivedSelfPresence )
                    {
                        vRoomMember::iterator it_member;
                        for ( it_member = pRoom->m_vMembers.begin();  it_member != pRoom->m_vMembers.end(); it_member++ )
                        {
                            PTRoomMember  pMember = *it_member;
                            if ( pMember->m_bSelf )
                            {
                                continue;
                            }
                            
                            if ( g_tEvents.pOnParticipantPresenceChange )
                            {
                                g_tEvents.pOnParticipantPresenceChange( pRoom, &pMember->m_tJid, &pMember->m_ePres, 0, 
                                    pMember->m_eAlliliation, pMember->m_bSelf, LibJingleEx::CHATROOM_PRESENCE_FLAG_FIRST_ENTER, app );
                            }
                            else if ( g_pfnOnMessage )
                            {
                                LibJingleEx::CMsgData  msgData;

                                msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                                msgData.Append( (unsigned char *)&pMember->m_tJid, sizeof(pMember->m_tJid) );

                                LibJingleEx::EPresence * pTmpPres = &pMember->m_ePres;
                                msgData.Append( (unsigned char *)&pTmpPres,        sizeof(pTmpPres) );

                                char * pTmpDescription = 0;
                                msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                                msgData.Append( (unsigned char *)&pMember->m_eAlliliation, sizeof(pMember->m_eAlliliation) );

                                msgData.Append( (unsigned char *)&pMember->m_bSelf,    sizeof(pMember->m_bSelf) );

                                int nTmpFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_FIRST_ENTER;
                                msgData.Append( (unsigned char *)&nTmpFlag,    sizeof(nTmpFlag) );

                                LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &pMember->m_tJid );
                                msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                                msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                                msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                                g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                                                msgData.GetMsgData(),  msgData.GetMsgLength()  );
                            }
                        }
                        
                        pRoom->m_bReceivedSelfPresence = TRUE;
                    }
                    // END


                    if ( item )
                    {
                        LibJingleEx::TJid tItemJid;
                        LibJingleEx::StrJid2TJid( item->Attr( QN_JID ).c_str(), tItemJid );

                        if ( g_tEvents.pOnParticipantPresenceChange )
                        {
                            g_tEvents.pOnParticipantPresenceChange( pRoom, &tItemJid, peShow, pStatus, 
                                pRoom->m_eAffiliation, bSelf, nFlag, app );
                        }
                        else if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                            msgData.Append( (unsigned char *)&tItemJid, sizeof(tItemJid) );

                            LibJingleEx::EPresence * pTmpShow = peShow;
                            msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                            char * pTmpDescription = pStatus;
                            msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                            msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );

                            msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                            msgData.Append( (unsigned char *)&nFlag,    sizeof(nFlag) );

                            LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &tItemJid );
                            msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                            g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                                msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }
                    }

                    // ����ǳ־��Եķ��䱣������
                    if ( pRoom->m_bPersistent )
                    {
                        vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(), 
                                                                  app->m_vPersistentRooms.end(), 
                             FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );

                        // ����־��Է�����û������־÷�������
                        if ( it_p == app->m_vPersistentRooms.end() ) 
                        {
                            PTPersistentRoom  p = new TPersistentRoom;
                            strcpy( p->m_szRoomId,               pRoom->m_szRoomId );
                            strcpy( p->m_szGroupChatServiceName, pRoom->m_szGroupChatServiceName );
                            app->m_vPersistentRooms.push_back( p );
                            app->m_bPersistentRoomsChanged = TRUE;

                            if ( ePreRoomState == ROOM_STATE_TRYING_PERSISTENT )
                            {
                                vPersistentRoom::iterator it_try = find_if( app->m_vTryingPersistents.begin(), app->m_vTryingPersistents.end(),
                                    FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName )   );
                                if ( it_try != app->m_vTryingPersistents.end() )
                                {
                                    delete * it_try;
                                    app->m_vTryingPersistents.erase( it_try );
                                }

                                // ��������һ������
                                if (  0 == app->m_vTryingPersistents.size() )
                                {
                                    MYLOG("=====[ the try entering the last persistent room, count = %u, unvailable count = %u ] \r\n", 
                                        app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                                    SavePersistentRoom(app);
                                }
                            }
                            else
                            {
                                MYLOG("=====[ enter the persistent room: %s@%s count = %u, unvailable count = %u ] \r\n", 
                                    pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                                SavePersistentRoom(app);
                            }
                        }
                    }
                    //// 
                }
            }

            return true;
        }
        // ��������ǻش����Լ� ( û�� code 110 )
        else
        {
            if ( destroy )
            {
                if ( g_tEvents.pOnDestroyRoom )
                {
                    g_tEvents.pOnDestroyRoom( pRoom, app );
                }
                else if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData  msgData;

                    msgData.Append( (unsigned char *)&pRoom,                            sizeof(pRoom) );
                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,                 sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage(  LibJingleEx::ON_DESTROY_CHATROOM, app, app->m_pUserData,
                                     msgData.GetMsgData(), msgData.GetMsgLength()  );
                }

                // ����ǳ־��Եķ��䱣������
                if ( pRoom->m_bPersistent )
                {
                    // ͬ������ͬresource
                    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                    message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
                    message->SetAttr( buzz::QN_TYPE, "inner" );

                    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                    message->AddElement( operation );
                    operation->SetAttr( buzz::QN_TYPE, "delete persistent room" );

                    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
                    operation->AddElement( item );

                    item->SetAttr( buzz::QN_ROOM_ID,          pRoom->m_szRoomId  );
                    item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );

                    client->SendStanza( message );
                    delete message;
                    // END


                    // ɾ��������ʷ��¼(��Ϊÿ�εǳ��ᱣ�棬���Դ˴����������浽˽������)
                    app->EraseHistoryRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                    // END ɾ��������ʷ��¼


                    vPersistentRoom::iterator  it_p = find_if( app->m_vPersistentRooms.begin(), 
                                                               app->m_vPersistentRooms.end(), 
                         FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );

                    assert( it_p != app->m_vPersistentRooms.end() );

                    PTPersistentRoom p = *it_p;
                    app->m_vPersistentRooms.erase( it_p );
                    delete  p;
                    app->m_bPersistentRoomsChanged = TRUE;

                    MYLOG("=====[ persistent room destroyed: %s@%s count = %u, unvailable count = %u ] \r\n", 
                        pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );
                    SavePersistentRoom(app);
                }
                //// 

                app->m_Rooms.erase( it );
                delete pRoom;

                return true;
            }

            // �����˲�������������
            if ( 0 == item )
            {
                return false;
            }

            std::string jid;

            // ����������
            if ( item->Attr(QN_JID).length() > 0 )
            {
                jid = item->Attr(QN_JID);
            }
            // ��������(��ǰ����)
            else
            {
                // һ�㲻�����
                // jid = presence->Attr(QN_FROM).c_str();
                jid = tJid.sResource;
            }

            // ��ȡ״̬
            if ( ROOM_TYPE_SPECIAL == pRoom->m_nType )
            {
                // ������״̬���͹���
#ifdef SUPPORT_STATUS_ROOM
                buzz::Status status;
                buzz::Jid    jid_( jid );

                if ( presence->Attr(QN_TYPE) == "unavailable" )
                {
                    eShow = LibJingleEx::PRESENCE_NONE;
                }

                status.set_jid( jid_ );
                status.set_show( (buzz::Status::Show)eShow );
                status.set_fileshare_capability( fileshare_ability );
                status.set_audio_capability( audio_ability );
                status.set_video_capability( video_ability );
                status.set_call_capability( call_ability );
                status.set_from_status_chatroom(true);

                status.set_ignore_capability( 0 == c ? true : false );
                status.set_version(presence_version);

                app->m_pXmppThread->OnStatusUpdate( status );
#endif

                return true;
            }

            LibJingleEx::TJid tMemberJid;
            LibJingleEx::StrJid2TJid( jid.c_str(), tMemberJid  );

            // item���jid���Լ�
            if ( jid == client->jid().Str() )
            {
                bSelf = TRUE;

                if ( presence->Attr(QN_TYPE) == "unavailable" )
                {
                    pRoom->m_eAffiliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );
                    eShow = LibJingleEx::PRESENCE_NONE;

                    // ����б��߳���status
                    if ( abStatus[2] || abStatus[3] )
                    {
                        // ������Լ������˳�
                        if ( pRoom->m_bNeverIn )
                        {
                            nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_LEAVE;
                        }
                        // ����Ǳ�����Աǿ���˳�
                        else
                        {
                            nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED;
                        }
                        

                        // ����ǳ־��Է���
                        if ( pRoom->m_bPersistent )
                        {
                            // ɾ��������ʷ��¼(��Ϊÿ�εǳ��ᱣ�棬���Դ˴����������浽˽������)
                            app->EraseHistoryRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
                            // END ɾ��������ʷ��¼
                        }
                    }

                    if ( g_tEvents.pOnParticipantPresenceChange )
                    {
                        g_tEvents.pOnParticipantPresenceChange( pRoom, &tMemberJid, &eShow, 0, 
                            pRoom->m_eAffiliation, bSelf, nFlag, app );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&tMemberJid, sizeof(tMemberJid) );

                        LibJingleEx::EPresence * pTmpShow = &eShow;
                        msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                        char * pTmpDescription = 0;
                        msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                        msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );

                        msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                        msgData.Append( (unsigned char *)&nFlag,    sizeof(nFlag) );

                        pRoom->SetMemberPresence( &tMemberJid, LibJingleEx::PRESENCE_NONE );
                        LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &tMemberJid );
                        msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                    }

                    // ������߳�memberȨ��
                    if ( (abStatus[2] || abStatus[3]) && pRoom->m_bPersistent )
                    {
                        // ����־��Է���
                        vPersistentRoom::iterator it_p = find_if( app->m_vPersistentRooms.begin(),
                            app->m_vPersistentRooms.end(),
                            FindPersistentRoom( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName ) );

                        if ( it_p != app->m_vPersistentRooms.end() )
                        {
                            PTPersistentRoom p = *it_p;

                            app->m_vPersistentRooms.erase( it_p );
                            delete p;
                            app->m_bPersistentRoomsChanged = TRUE;

                            MYLOG("=====[ be kicked off the persistent chatroom: %s@%s, count = %u, unvailable count =%u ] \r\n", pRoom->m_szRoomId,  
                                pRoom->m_szGroupChatServiceName, app->m_vPersistentRooms.size(), GetUnavailablePersistentRoomCnt(app->m_vPersistentRooms) );

                            SavePersistentRoom( app );
                        }
                        // END ����־��Է���
                    }

                    app->m_Rooms.erase( it );
                    delete pRoom;
                }
                // ���뿪
                else
                {
                    LibJingleEx::EAffiliation  affiliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );

                    if ( affiliation != pRoom->m_eAffiliation )
                    {
                        // ��״̬�����Һ���Ƶ�����ң��ǵ�ȥ��ȡ��Ա�б�
                        if ( ROOM_TYPE_SPECIAL != pRoom->m_nType && ROOM_TYPE_VISUAL != pRoom->m_nType )
                        {
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
                            // �����û��ȥ��ȡ��Ա������Ȩ��, �ǵ�ȥ��ȡ��Ա
                            if (    pRoom->m_bGetSubject && !pRoom->m_bGetMembers 
                                 && ( LibJingleEx::AFFILIATION_MEMBER == affiliation || LibJingleEx::AFFILIATION_ADMIN == affiliation || LibJingleEx::AFFILIATION_OWNER == affiliation ) )
                            {
                                GetRoomRoster( client, pRoom );
                                pRoom->m_bGetMembers = TRUE;
                            }
                            else if ( !pRoom->m_bGetMembers 
                                && ( LibJingleEx::AFFILIATION_MEMBER == affiliation || LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                            {
                                SubjectDelayMsg * pDelayMsg = new SubjectDelayMsg;
                                strcpy( pDelayMsg->szRoomId, pRoom->m_szRoomId );
                                strcpy( pDelayMsg->szGroupChatServiceName, pRoom->m_szGroupChatServiceName );

                                app->m_pXmppThread->PostDelayed( INTERVAL_GET_CHATROOM_MEMBERS_DELAY, app->m_pXmppThread, MSG_GET_CHATROOM_MEMBERS_DELAY, pDelayMsg  );
                            }
#else
                            // �����û��ȥ��ȡ��Ա������Ȩ��, �ǵ�ȥ��ȡ��Ա
                            if (    pRoom->m_bGetSubject && !pRoom->m_bGetMembers 
                                && ( LibJingleEx::AFFILIATION_ADMIN == affiliation || LibJingleEx::AFFILIATION_OWNER == affiliation ) )
                            {
                                GetRoomRoster( client, pRoom );
                                pRoom->m_bGetMembers = TRUE;
                            }
                            else if ( !pRoom->m_bGetMembers 
                                && ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                            {
                                SubjectDelayMsg * pDelayMsg = new SubjectDelayMsg;
                                strcpy( pDelayMsg->szRoomId, pRoom->m_szRoomId );
                                strcpy( pDelayMsg->szGroupChatServiceName, pRoom->m_szGroupChatServiceName );

                                app->m_pXmppThread->PostDelayed( INTERVAL_GET_CHATROOM_MEMBERS_DELAY, app->m_pXmppThread, MSG_GET_CHATROOM_MEMBERS_DELAY, pDelayMsg  );
                            }
#endif
                        }

                        if ( ROOM_TYPE_VISUAL != pRoom->m_nType && ROOM_TYPE_SPECIAL != pRoom->m_nType )
                        {
                            // �鿴m_vMembers���Ƿ����Լ�
                            vRoomMember::iterator it_m;
                            for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
                            {
                                PTRoomMember p = *it_m;
                                if ( p->m_bSelf )
                                {
                                    p->m_eAlliliation = affiliation;
                                    break;
                                }
                            }

                            // ���û���Լ������Լ����뵽m_vMembers��ȥ
                            if ( it_m == pRoom->m_vMembers.end() )
                            {
                                assert( 0 );

                                // ���Լ�����vMembers��ȥ
                                PTRoomMember  ptMember = new TRoomMember;
                                ptMember->m_bSelf = TRUE;
                                ptMember->m_eAlliliation = affiliation;
                                LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), ptMember->m_tJid );
                                strcpy( ptMember->m_szNick, tJid.sResource );                    
                                pRoom->m_vMembers.push_back( ptMember );
                                // END(���Լ�����vMembers��ȥ
                            }
                            // END(�鿴m_vMembers���Ƿ����Լ�)
                        }


                        // ֪ͨaffiliation�ı�
                        if ( g_tEvents.pOnGrant )
                        {
                            g_tEvents.pOnGrant( app, pRoom, pRoom->m_eAffiliation, affiliation );
                        }
                        else if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData   msgData;

                            msgData.Append( (unsigned char *)&pRoom,                 sizeof(pRoom) );
                            msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );
                            msgData.Append( (unsigned char *)&affiliation,           sizeof(affiliation) );


                            g_pfnOnMessage( LibJingleEx::ON_GRANT, app, app->m_pUserData, 
                                msgData.GetMsgData(),  msgData.GetMsgLength() );
                        }
                        // end ֪ͨaffiliation�ı�

                        pRoom->m_eAffiliation = affiliation;
                    }
                }
                return true;
            }

            bSelf = FALSE;

            vRoomMember::iterator it_m;
            it_m = find_if( pRoom->m_vMembers.begin(), pRoom->m_vMembers.end(), FindRoomMemberObj( &tMemberJid ) );

            // ��Ա�뿪
            if ( presence->Attr(QN_TYPE) == "unavailable" )
            {
                // û���ҵ������Ա
                if ( it_m == pRoom->m_vMembers.end() )
                {
                    return true;
                }

                // �鿴�Ƿ��߳�
                if ( abStatus[2] || abStatus[3] )
                {
                    nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED;
                }


                PTRoomMember pMember = *it_m;
                pMember->m_eAlliliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );
                // �뿪
                eShow = LibJingleEx::PRESENCE_NONE;


                if ( g_tEvents.pOnParticipantPresenceChange )
                {
                    g_tEvents.pOnParticipantPresenceChange( pRoom, &pMember->m_tJid, &eShow, 0, 
                        pMember->m_eAlliliation, bSelf, nFlag, app );
                }
                else if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData  msgData;

                    msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                    msgData.Append( (unsigned char *)&pMember->m_tJid, sizeof(pMember->m_tJid) );

                    LibJingleEx::EPresence * pTmpShow = &eShow;
                    msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                    char * pTmpDescription = 0;
                    msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                    msgData.Append( (unsigned char *)&pMember->m_eAlliliation, sizeof(pMember->m_eAlliliation) );

                    msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                    msgData.Append( (unsigned char *)&nFlag,    sizeof(nFlag) );

                    pRoom->SetMemberPresence( &pMember->m_tJid, LibJingleEx::PRESENCE_NONE );
                    LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &pMember->m_tJid );
                    msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                    msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                    msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                    g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                        msgData.GetMsgData(),  msgData.GetMsgLength()  );
                }

                if ( LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED == nFlag )
                {
                    pRoom->EraseRosterItem( &pMember->m_tJid );
                }


                // ��ʱȺ����Ƿ���ͨ��leave������ǣ�����Ա��Ҫɾ���ó�Ա
                // ����Լ���owner
                if ( LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation )
                {
                    // �������ʱȺ
                    if ( (pRoom->m_nType == ROOM_TYPE_NORMAL) && !pRoom->m_bPersistent )
                    {
                        // ������Ǳ�����Ա�߳�
                        if ( LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED != nFlag )
                        {
                            vRoomMember::iterator it_m_another;
                            for ( it_m_another = pRoom->m_vMembers.begin(); it_m_another !=  pRoom->m_vMembers.end(); it_m_another++ )
                            {
                                // �����ͬһ�ˣ�����
                                if ( it_m_another == it_m )
                                {
                                    continue;
                                }

                                PTRoomMember  ptAnotherMember = *it_m_another;

                                // ���bare jid���
                                if ( IfBareJidEqual(ptAnotherMember->m_tJid, pMember->m_tJid) )
                                {
                                    break;
                                }
                            }

                            // ���û���ҵ�һ����ͬbare jid��member��ɾ����Ա�б������
                            if ( it_m_another == pRoom->m_vMembers.end() )
                            {
                                DeleteChatroomMember( pRoom, &pMember->m_tJid, client );
                            }
                        }
                    }
                }
                // END


                delete pMember;
                pRoom->m_vMembers.erase( it_m );

            }
            // ���뿪
            else
            {
                PTRoomMember pMember = 0;
                if ( it_m == pRoom->m_vMembers.end() )
                {
                    pMember = new TRoomMember;
                    memcpy( &pMember->m_tJid, &tMemberJid, sizeof(LibJingleEx::TJid) );
                    strcpy( pMember->m_szNick, tJid.sResource );  // ��¼�ڷ����е��ǳ�
                    pRoom->m_vMembers.push_back( pMember );

                    nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_FIRST_ENTER;


                    // ��ʱ����״̬(�������ʱʱ�����ȡ��״̬��������)
                    // END
                }
                else
                {
                    pMember = *it_m;
                    nFlag = LibJingleEx::CHATROOM_PRESENCE_FLAG_CHANGE;
                }

                // �����������ģʽ�������Ѿ���������
                // �������Ƶ����ģʽ����Ϊpresence�����֪ͨ�Լ�������Ҫ����һ��˳��
                if ( pRoom->m_eState == ROOM_STATE_ON_PRESENCE )
                {
                    // ֪ͨ���뷿��ɹ�
                    if ( g_tEvents.pOnCreateUniqueRoom )
                    {
                        g_tEvents.pOnCreateUniqueRoom( app, pRoom, 1 );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData msgData;

                        int nError = 1;
                        msgData.Append( (unsigned char *)&pRoom,  sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&nError, sizeof(nError) );
                        msgData.Append( (unsigned char *)pRoom->m_szRoomId, sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_CREATE_UNIQUE_CHATROOM, app, app->m_pUserData, 
                            msgData.GetMsgData(), msgData.GetMsgLength() );
                    }
                    // END ֪ͨ���뷿��ɹ�

                    pRoom->m_eState = ROOM_STATE_NOTIFIED_CREATE;
                }
                // ������ڽ�������
                else if ( pRoom->m_eState == ROOM_STATE_ON_ACCEPTING )
                {
                    // ֪ͨ��������ɹ�
                    if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        msgData.Append( (unsigned char *)&pRoom,   sizeof(pRoom) );

                        BOOL  bResult = TRUE;
                        msgData.Append( (unsigned char *)&bResult, sizeof(bResult) );

                        g_pfnOnMessage( LibJingleEx::ON_ACCEPT_INVITE_RESULT, app, app->m_pUserData,
                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                    }
                    // END ֪ͨ��������ɹ�
                    pRoom->m_eState = ROOM_STATE_AVAILABLE;
                }
                else if ( pRoom->m_eState == ROOM_STATE_ENTERING_PERSISTENT || ROOM_STATE_HOLD_PERSISTENT == pRoom->m_eState )
                {
                    if ( !pRoom->m_bNofityRoomCreated )
                    {
                        // ֪ͨ�����˹̶�Ⱥ
                        if ( g_pfnOnMessage )
                        {
                            LibJingleEx::CMsgData  msgData;

                            msgData.Append( (unsigned char *)&pRoom,                 sizeof(pRoom) );
                            // msgData.Append( (unsigned char *)&pRoom->m_eAffiliation, sizeof(pRoom->m_eAffiliation) );
                            msgData.Append( (unsigned char *)&pRoom->m_szName,       sizeof(pRoom->m_szName) );
                            msgData.Append( (unsigned char *)pRoom->m_szRoomId,      sizeof(pRoom->m_szRoomId) );
                            msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,   sizeof(pRoom->m_szGroupChatServiceName) );

                            g_pfnOnMessage( LibJingleEx::ON_ENTER_PERSISTENT_ROOM_FOR_TT_ANDROID, app, app->m_pUserData,
                                msgData.GetMsgData(),  msgData.GetMsgLength()  );
                        }
                        // END ֪ͨ�����˹̶�Ⱥ

                        pRoom->m_bNofityRoomCreated = TRUE;
                    }
                }

                pMember->m_eAlliliation = StrAffi2Int( item->Attr( QName(true, "", "affiliation") ).c_str() );
                pMember->m_ePres        = *peShow;


                // ֻ�����յ��Լ���״̬���֪ͨ�ϲ�presence
                if ( pRoom->m_bReceivedSelfPresence )
                {
                    if ( g_tEvents.pOnParticipantPresenceChange )
                    {
                        g_tEvents.pOnParticipantPresenceChange( pRoom, &pMember->m_tJid, peShow, pStatus, 
                            pMember->m_eAlliliation, bSelf, nFlag, app );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        msgData.Append( (unsigned char *)&pRoom, sizeof(pRoom) );
                        msgData.Append( (unsigned char *)&pMember->m_tJid, sizeof(pMember->m_tJid) );

                        LibJingleEx::EPresence * pTmpShow = peShow;
                        msgData.Append( (unsigned char *)&pTmpShow, sizeof(pTmpShow) );

                        char * pTmpDescription = pStatus;
                        msgData.Append( (unsigned char *)&pTmpDescription, sizeof(pTmpDescription) );

                        msgData.Append( (unsigned char *)&pMember->m_eAlliliation, sizeof(pMember->m_eAlliliation) );

                        msgData.Append( (unsigned char *)&bSelf,    sizeof(bSelf) );
                        msgData.Append( (unsigned char *)&nFlag,    sizeof(nFlag) );

                        LibJingleEx::EPresence  eMax = pRoom->GetMaxPresence( &pMember->m_tJid );
                        msgData.Append( (unsigned char *)&eMax,    sizeof(eMax) );

                        msgData.Append( (unsigned char *)pRoom->m_szRoomId,                  sizeof(pRoom->m_szRoomId) );
                        msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName,    sizeof(pRoom->m_szGroupChatServiceName) );

                        g_pfnOnMessage( LibJingleEx::ON_PARTICIPANT_PRESENCE, app, app->m_pUserData,
                            msgData.GetMsgData(),  msgData.GetMsgLength()  );
                    }
                }
                // END ( ֻ�����յ��Լ���״̬���֪ͨ�ϲ�presence )

            }
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }

    return false;
}

bool MyChatRoomTask::HandleStanza( const XmlElement * stanza )
{
    // �����IQ
    if ( stanza->Name() == QN_IQ )
    {
        return Handle_iq( stanza );
    }
    // �����presence
    if ( stanza->Name() == QN_PRESENCE )
    {
        return Handle_presence( stanza );
    }
    else
    {
        return false;
    }
}



bool FileShareTask::HandleStanza(const XmlElement * stanza)
{
    // �����IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    char buf_tmp[MAX_BUFF_SIZE];
    const XmlElement * iq = stanza;

    const XmlElement * fileshare_element = iq->FirstElement();

    if ( 0 == fileshare_element )
    {
        return false;
    }

    if ( fileshare_element->Name() != buzz::QN_FILE_SHARE )
    {
        return false;
    }

    const std::string & type       = iq->Attr(QN_TYPE);
    const std::string & from_jid   = iq->Attr( buzz::QN_FROM );
    const std::string & subtype    = fileshare_element->Attr( buzz::QN_FILE_SHARE_SUBTYPE );
    const std::string & session_id = fileshare_element->Attr( buzz::QN_FILE_SHARE_SESSION_ID );

    EFileShareSubType  eSubType = GetFileShareSubType( subtype.c_str() );

    XmppClient *     client     = GetClient();
    CLibJinglexApp * pAppObj    = (CLibJinglexApp *)client->m_hApp;
    XmppPump       * pump       = pAppObj->m_pXmppThread->pump();

    cFuncLock cLock( &pAppObj->m_lock );


    // ����� type == result
    if ( buzz::STR_RESULT == type )
    {
        TFileShareMsg  tMsg;
        tMsg.eSubType = eSubType;
        strncpy( tMsg.szPeerJid, from_jid.c_str(), MAX_FULL_JID_LEN );
        strncpy( tMsg.szSessionId, session_id.c_str(), MAX_SESSION_ID_LEN );

        pump->OnFileShareResult( &tMsg );
        return true;
    }
    // ����� type == get
    else if ( buzz::STR_GET == type )
    {
        assert( 0 );
        return true;
    }
    // ����� type == error
    else if ( buzz::STR_ERROR == type )
    {
        // ֪ͨ�ļ�����ʧ��
        TFileShareMsg  tMsg;
        tMsg.eSubType = FILE_SHARE_SUB_TYPE_FAIL;
        strncpy( tMsg.szPeerJid, from_jid.c_str(),     MAX_FULL_JID_LEN );
        strncpy( tMsg.szSessionId, session_id.c_str(), MAX_SESSION_ID_LEN );
        if ( fileshare_element->Attr( QN_FILE_SHARE_ERROR ).length() > 0 )
        {
            sscanf( fileshare_element->Attr( QN_FILE_SHARE_ERROR ).c_str(), "%d", &tMsg.nError );
        }
        pump->OnFileShareResult( &tMsg );
        return true;
    }

    // ������ type == set

    // ��¼���´�������Ƿ�����
    bool bError = false;
    // ������
    int  nError = 0;

    FileShareClient * p = 0;

    switch( eSubType )
    {
    case FILE_SHARE_SUB_TYPE_START:
        {
            p = pump->FindFileShareObject( from_jid.c_str(), session_id.c_str() );
            // �����Ѿ����û�Cancel����
            if ( 0 == p )
            {
                return true;
            }

            const std::string & name       = fileshare_element->Attr( buzz::QN_FILE_SHARE_NAME );
            const std::string & strMode    = fileshare_element->Attr( buzz::QN_FILE_SHARE_MODE );

            talk_base::Pathname temp_name;
            std::string file_name = Utf8ToAnsi( name.c_str(), buf_tmp, MAX_BUFF_SIZE );
            temp_name.SetFilename( file_name );
            temp_name.SetFolder( "" );

            // ���������ʱ�ļ����ļ���ʧ��
            if ( !talk_base::CreateUniqueFile( temp_name, false ) )
            {
                // ��ʱ������
                LIBJINGLE_LOG_ERROR( "FAILED to talk_base::CreateUniqueFile()!" );
                bError = true;
                break;
            }

            talk_base::StreamInterface * stream = NULL;
            int mode = 0;

            // ������ļ�
            if ( "file" == strMode )
            {
                talk_base::FileStream * file = new talk_base::FileStream;
                // ������ļ�дʧ��
                if ( !file->Open( temp_name.pathname().c_str(), "wb" ) )
                {
                    // ��ʱ������
                    LIBJINGLE_LOG_ERROR( "FAILED to file->Open()!" );
                    bError = true;
                    break;
                }
                stream = file;
                p->OnNextEntry( file_name.c_str(), 0  );
            }
            // ������ļ���
            else
            {
                temp_name.AppendFolder(temp_name.filename());
                temp_name.SetFilename("");

                talk_base::TarStream * tar = new talk_base::TarStream;
                // ������ļ���дʧ��
                if ( !tar->Open( temp_name.pathname(), false ) )
                {
                    LIBJINGLE_LOG_ERROR( "FAILED to tar->Open()!" );
                    bError = true;
                    break;
                }
                stream = tar;
                tar->SignalNextEntry.connect( p, &FileShareClient::OnNextEntry);
            }

            p->m_stream = stream;
            p->m_counter = new StreamCounter(stream);
            p->m_counter->SignalUpdateByteCount.connect( p, &FileShareClient::OnUpdateBytes);

            strncpy( p->m_szTempFilePath, temp_name.pathname().c_str(), LibJingleEx::MAX_PATH_SIZE );
        }
        break;
    case FILE_SHARE_SUB_TYPE_END:
        {
            p = pump->FindFileShareObject( from_jid.c_str(), session_id.c_str() );
            // ���ܱ��û�Cancel����
            if ( 0 == p )
            {
                return true;
            }

            p->m_stream->Close();
            p->m_stream = 0;

            // �ļ��������
            if ( !pump->OnFileShareComplete( from_jid.c_str(), p->m_szSessionId, p->m_szTempFilePath ) )
            {
                bError = true;
                break;
            }
            pump->RemoveFileShareClient( p );
        }
        break;
    case FILE_SHARE_SUB_TYPE_CANCEL:
        {
            p = pump->FindFileShareObject( from_jid.c_str(), session_id.c_str() );
            // ���ܱ��û�Cancel����
            if ( 0 == p )
            {
                return true;
            }
            pump->CancelFiles( p, FALSE, FALSE );
        }
        break;
    case FILE_SHARE_SUB_TYPE_FAIL:
        {
            assert( 0 );
            return true;
        }
        break;
    case FILE_SHARE_SUB_TYPE_TRANSFER:
        {
            p = pump->FindFileShareObject( from_jid.c_str(), session_id.c_str() );
            // ���ܱ��û�Cancel����
            if ( 0 == p )
            {
                return true;
            }

            // base64���������
            const std::string & data = fileshare_element->BodyText();

            unsigned char buf[MAX_BYTE_PER_TRANSFER];
            unsigned long dwSize = MAX_BYTE_PER_TRANSFER;

            // �������ʧ��
            if ( 0 != LibJingleEx::DecodeBase64( buf, &dwSize, data.c_str() ) )
            {
                // ��ʱ�����ǽ���ʧ��
                assert( 0 );
                LIBJINGLE_LOG_ERROR( "Failed to DecodeBase64()" );
                bError = true;
                break;
            }

            assert( p->m_stream );

            size_t dwWrite = 0;
            int    err_  = 0;
            if ( talk_base::SR_SUCCESS != p->m_counter->Write( buf, dwSize, &dwWrite, &err_ ) )
            {
                // ��ʱ������д�ļ�(���ļ���)ʧ��
                LIBJINGLE_LOG_ERROR( "Failed to p->m_counter->Write()" );
                bError = true;
                nError = talk_base::HE_STREAM;
                break;
            }

            // ֪ͨ����
            unsigned long dwTotalSize = p->m_Manifest.GetTotalSize();
            p->OnUpdateProgress( dwTotalSize, p->m_szTransferName );
        }
        break;
    default:
        break;
    }

    // response
    XmlElement * iq_rsp = new XmlElement( QN_IQ );
    iq_rsp->SetAttr( QN_TO, iq->Attr(QN_FROM) );
    if ( bError )
    {
        iq_rsp->SetAttr( QN_TYPE, STR_ERROR );
    }
    else
    {
        iq_rsp->SetAttr( QN_TYPE, STR_RESULT );
    }
    iq_rsp->SetAttr( QN_ID, iq->Attr(QN_ID) );

    XmlElement * fileshare_rsp = new XmlElement( QN_FILE_SHARE, true );
    iq_rsp->AddElement( fileshare_rsp );

    fileshare_rsp->SetAttr( QN_FILE_SHARE_SUBTYPE,    subtype );
    fileshare_rsp->SetAttr( QN_FILE_SHARE_SESSION_ID, session_id );

    if ( bError )
    {
        char  szError[256];
        SNPRINTF( szError, 256, "%u", nError );
        fileshare_rsp->SetAttr( QN_FILE_SHARE_ERROR, szError );
    }

    client->SendStanza( iq_rsp );
    delete iq_rsp;
    // end of response



    if ( bError )
    {
        assert( 0 != p );

        // ��֪�ϲ��ļ�����ʧ��(Ҫô��cancel֪ͨ��Ҫô��failure֪ͨ)
        pump->CancelFiles( p, TRUE, FALSE, nError );
    }


    return true;
}





void MyPrivateTask::GetDmsGroupInfo()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);


    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * group = new buzz::XmlElement(buzz::QN_DMS_GROUP, true);
    query->AddElement( group );

    client->SendStanza( iq );

    delete iq;
}

void MyPrivateTask::GetDmsMemberInfo()
{
    XmppClient * client = GetClient();

    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

        // iq����type����
        iq->AddAttr(buzz::QN_TYPE,STR_GET);

        // iq����id����
        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->AddAttr(buzz::QN_ID, randid);


        buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
        iq->AddElement( query );

        char  szBlockName[256];
        GetDmsMembersBlockName( szBlockName, 256, i );

        buzz::XmlElement * member = new buzz::XmlElement( buzz::QName( true, szBlockName, "member" ), true );
        query->AddElement( member );

        client->SendStanza( iq );

        delete iq;
    }
}

void MyPrivateTask::GetPersistentRoomsInfo()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);


    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * room = new buzz::XmlElement(buzz::QN_PERSISTENT_ROOM, true);
    query->AddElement( room );

    client->SendStanza( iq );

    delete iq;
}


void MyPrivateTask::GetGroupchatHistory()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);


    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * history = new buzz::XmlElement(buzz::QN_CHATROOM_HISTORY, true);
    query->AddElement( history );

    client->SendStanza( iq );

    delete iq;
}

void MyPrivateTask::GetCustomConfigInfo()
{
    XmppClient * client = GetClient();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);


    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * config = new buzz::XmlElement(buzz::QN_CUSTOM_CONFIG, true);
    query->AddElement( config );

    client->SendStanza( iq );

    delete iq;
}

bool MyPrivateTask::HandleStanza(const XmlElement * stanza)
{
    // �����IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    char buf[MAX_BUFF_SIZE];
    char buf_tmp[MAX_BUFF_SIZE];
    const XmlElement * iq = stanza;

    const XmlElement * query = iq->FirstElement();
    if ( 0 == query || query->Name() != buzz::QN_PRIVATE_QUERY )
    {
        return false;
    }


    const std::string & type       = iq->Attr(QN_TYPE);
    // �����"result"
    if ( buzz::STR_RESULT != type )
    {
        return true;
    }


    // ����
    XmppClient * client  = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );


    const  XmlElement  *  child = query->FirstElement();
    if ( 0 == child )
    {
        std::vector< TIqId * >::iterator it;
        for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
        {
            TIqId * p = *it;

            // ����ҵ�������󱣴��iq id
            if ( stanza->Attr(QN_ID) == p->szIqId  )
            {
                delete p;
                m_vIqIds.erase( it );

                MYLOG("==== [received last save iq result] \r\n");
                app->m_pXmppThread->Post(app->m_pXmppThread, MSG_DISCONNECT2 );
                // �ر��̱߳�
                // app->m_pXmppThread->Post(app->m_pXmppThread, talk_base::Thread::MSG_TERMINATE_THREAD );
                app->m_pXmppThread->PostDelayed( DELAYED_TERMINATE_TIME, app->m_pXmppThread, talk_base::Thread::MSG_TERMINATE_THREAD );
                break;
            }
        }

        return true;
    }
    

    
    if ( child->Name() == buzz::QN_DMS_GROUP )
    {        
        const XmlElement * item = child->FirstElement();

        while( item )
        {
            const std::string & id        = item->Attr( buzz::QN_DMS_ID );
            const std::string & parent_id = item->Attr( buzz::QN_DMS_PARENT_ID );
            const std::string & name      = item->Attr( buzz::QN_DMS_NAME );

            unsigned long  dwId, dwParentId;

            sscanf( id.c_str(),        " %u", &dwId );
            sscanf( parent_id.c_str(), " %u", &dwParentId );

            if ( dwId > app->m_dwDmsMaxGroupId )
            {
                app->m_dwDmsMaxGroupId = dwId;
            }

            LibJingleEx::PTDmsGroup pGroup = new LibJingleEx::TDmsGroup;
            pGroup->dwGroupId  = dwId;
            pGroup->dwParentId = dwParentId;
            strncpy( pGroup->szGroupName, Utf8ToAnsi(name.c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_GROUP_NAME_LEN );

            app->m_vDmsGroups.push_back( pGroup );

            item = item->NextElement();
        }

        // �鿴����������顰δ���顱
        vDmsGroup::iterator  it_find;
        it_find = find_if(  app->m_vDmsGroups.begin(), app->m_vDmsGroups.end(), 
                            FindDmsGroupObj( SPECIAL_GROUP_SN ) );
        // ���û���������
        if (  app->m_vDmsGroups.end() == it_find )
        {
            LibJingleEx::PTDmsGroup pGroup = new LibJingleEx::TDmsGroup;
            pGroup->dwGroupId  = SPECIAL_GROUP_SN;
            pGroup->dwParentId = 0;
            strncpy( pGroup->szGroupName, SPECIAL_GROUP_NAME, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );
            app->m_vDmsGroups.push_back( pGroup );
            app->m_dwDmsMaxGroupId = SPECIAL_GROUP_SN;
        }
        
        app->m_bGetDmsGroupResult = TRUE;
    }
    // else if ( child->Name() == buzz::QN_DMS_MEMBER )
    else if (      child->Name().LocalPart() == "member" 
               &&  0 == strncmp( child->Name().Namespace().c_str(), buzz::NS_KEDACOM_DMS_MEMBER.c_str(), 
                                 buzz::NS_KEDACOM_DMS_MEMBER.length() )       )
    {
        // ˽�п�����
        unsigned long dwBlockIndex = GetDmsMembersBlockIndex( child->Name().Namespace().c_str() );


        const XmlElement * item = child->FirstElement();
        while( item )
        {
            const std::string & id        = item->Attr( buzz::QN_DMS_ID );
            const std::string & parent_id = item->Attr( buzz::QN_DMS_PARENT_ID );
            const std::string & name      = item->Attr( buzz::QN_DMS_NAME );
            const std::string & jid       = item->Attr( buzz::QN_DMS_JID );
            const std::string & mtype     = item->Attr( buzz::QN_DMS_TYPE );

            const std::string & r1        = item->Attr( buzz::QN_DMS_RESERVERD1 );
            const std::string & r2        = item->Attr( buzz::QN_DMS_RESERVERD2 );
            const std::string & r3        = item->Attr( buzz::QN_DMS_RESERVERD3 );
            const std::string & r4        = item->Attr( buzz::QN_DMS_RESERVERD4 );

            unsigned long  dwId, dwParentId, dwType;

            sscanf( id.c_str(),        " %u", &dwId );
            sscanf( parent_id.c_str(), " %u", &dwParentId );
            sscanf( mtype.c_str(),     " %u", &dwType );

            if ( dwId > app->m_dwDmsMaxMemberId )
            {
                app->m_dwDmsMaxMemberId = dwId;
            }

            LibJingleEx::PTDmsMember pMember = new LibJingleEx::TDmsMember;
            pMember->dwMemberId = dwId;
            pMember->dwGroupId  = dwParentId;
            strncpy( pMember->szName, Utf8ToAnsi(name.c_str(), buf, MAX_BUFF_SIZE), LibJingleEx::MAX_DMS_MEMBER_NAME_LEN );
            strncpy( pMember->szJid,  Utf8ToAnsi(jid.c_str(),  buf, MAX_BUFF_SIZE), LibJingleEx::MAX_JID_LEN );
            pMember->eType = (LibJingleEx::EDmsMemberType)dwType;

            strncpy( pMember->szReserved[0], Utf8ToAnsi(r1.c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
            strncpy( pMember->szReserved[1], Utf8ToAnsi(r2.c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
            strncpy( pMember->szReserved[2], Utf8ToAnsi(r3.c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
            strncpy( pMember->szReserved[3], Utf8ToAnsi(r4.c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );

            app->m_vDmsMembers[dwBlockIndex].push_back( pMember );

            item = item->NextElement();
        }

        // ����õ����һ��˽�п�
        if ( dwBlockIndex == DMS_MEMBERS_BLOCK_CNT - 1 )
        {
            app->m_bGetDmsMemberResult = TRUE;

#if DMS_CONTACTS_SWITCH
            if ( g_pfnOnMessage )
            {
                g_pfnOnMessage( LibJingleEx::ON_MEMBERS_DATA_READY, app, app->m_pUserData, 0, 0  );
            }
#endif

        }
    }
    else if ( child->Name() == buzz::QN_PERSISTENT_ROOM )
    {
        // ��������ˣ�ֱ�ӷ���
        if ( app->m_bPersistentRoomsResponsed )
        {
            return true;
        }

        app->m_bPersistentRoomsResponsed = TRUE;

        ClearVector( app->m_vQueryPersistentRoomIds );

        const XmlElement * item = child->FirstElement();

        while( item )
        {
            const std::string & room_id        = item->Attr( buzz::QN_ROOM_ID );
            const std::string & muc_service    = item->Attr( buzz::QN_ROOM_MUC_SERVICE );
            const std::string & first          = item->Attr( buzz::QN_FIRST );

            UniqueRoomId    szRoomId;
            char            szMucId[MAX_GROUPCHAT_SERVICE_NAME_LEN];

            Utf8ToAnsi( room_id.c_str(),      szRoomId, MAX_UNIQUE_ROOM_ID_LENGTH );
            Utf8ToAnsi( muc_service.c_str(),  szMucId,  MAX_GROUPCHAT_SERVICE_NAME_LEN );

            vPersistentRoom::iterator it_p = find_if ( app->m_vPersistentRooms.begin(), app->m_vPersistentRooms.end(), FindPersistentRoom( szRoomId, szMucId ) );
            // ��������ظ���, ֱ������
            if ( app->m_vPersistentRooms.end() != it_p )
            {
                item = item->NextElement();
                continue;
            }
            
            PTPersistentRoom p = new TPersistentRoom;
            strncpy( p->m_szRoomId,               szRoomId,     sizeof(p->m_szRoomId) );
            strncpy( p->m_szGroupChatServiceName, szMucId,      sizeof(p->m_szGroupChatServiceName) );

            if ( first == "true" )
            {
                MYLOG("=====[ web save invite item, %s@%s ] \r\n", szRoomId,  szMucId );

                p->m_bFirst = TRUE;

                // ������״̬�иı�
                app->m_bPersistentRoomsChanged = TRUE;
            }

            app->m_vPersistentRooms.push_back( p );


            // ��ѯ�ó־��Է���治����
            buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

            char randid[MAX_RAND_ID_LENGTH];
            GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
            iq->SetAttr( buzz::QN_ID, randid );

            SNPRINTF( buf, MAX_BUFF_SIZE, "%s@%s", p->m_szRoomId, p->m_szGroupChatServiceName );
            iq->SetAttr( buzz::QN_TO, AnsiToUtf8( buf, buf_tmp, MAX_BUFF_SIZE ) );

            iq->SetAttr( buzz::QN_TYPE, "get" );

            buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_DISCO_INFO_QUERY, true );
            iq->AddElement( query );

            client->SendStanza( iq );
            delete iq;
            // END ��ѯ�ó־��Է���治����

            char * pNewQueryId = new char[MAX_RAND_ID_LENGTH];
            strcpy( pNewQueryId, randid );
            app->m_vQueryPersistentRoomIds.push_back( pNewQueryId );
            
            item = item->NextElement();
        }

        
        MYLOG("==== [ Got chatroom private data, count = %u ] \n", app->m_vQueryPersistentRoomIds.size() );

        // �����һ�ε�¼������û�м���̶�Ⱥ
        if ( 0 == app->m_vQueryPersistentRoomIds.size() )
        {
            app->NotifyHoldPersistentRoom();

            app->m_bPrivatePersistentRoomsCntConfirmed = TRUE;

            app->HandleScreenedChatroomCfg();

            // ������������invirt��Ϣ
            app->HandleDelayInvite2ChatRoomMsgs();
            // END (������������invirt��Ϣ)
        }
    }
    // ��ѯ��muc������ʷ
    else if ( child->Name() == buzz::QN_CHATROOM_HISTORY )
    {
        unsigned long  dwLocalVersion  = 0;
        unsigned long  dwServerVersion = 0;

        if ( child->Attr(QN_VERSION).length() > 0 )
        {
            sscanf( child->Attr(QN_VERSION).c_str(), "%lu", &dwServerVersion );

            if ( app->GetChatRoomHistoryFromLocal( dwLocalVersion ) )
            {
                if ( dwLocalVersion > dwServerVersion )
                {
                    app->ParseChatRoomHistory( app->m_chatroom_history );
                    app->m_dwChatroomHistoryVersion = dwLocalVersion;

                    // �����Ѷ�����������ʷ��������
                    char szLastQueryId[MAX_RAND_ID_LENGTH];
                    app->SavePrivateData( szLastQueryId, MAX_RAND_ID_LENGTH );
                }
                else
                {
                    app->ParseChatRoomHistory( child );
                    app->m_dwChatroomHistoryVersion = dwServerVersion;
                    app->SaveChatRoomHistory2Local();
                }   
            }
            else
            {
                app->ParseChatRoomHistory( child );
                app->m_dwChatroomHistoryVersion = dwServerVersion;
                app->SaveChatRoomHistory2Local();
            }
        }
        else
        {
            app->ParseChatRoomHistory( child );
            app->m_dwChatroomHistoryVersion = 1;
            app->SaveChatRoomHistory2Local();

            // �����Ѷ�����������ʷ��������(��Ҫ��Ϊ�˱���汾��"1")
            char szLastQueryId[MAX_RAND_ID_LENGTH];
            app->SavePrivateData( szLastQueryId, MAX_RAND_ID_LENGTH );
        }

        // ���������ʷ
        app->m_bGetRoomReadMsgs = TRUE;
        app->NotifyAllBufferedChatroomMsgs();
    }
    // ��ȡ�Զ�������
    else if ( child->Name() == buzz::QN_CUSTOM_CONFIG )
    {
        const XmlElement * item = child->FirstElement();
        while( item )
        {
            const std::string &   key_      = item->Attr( buzz::QN_NAME );
            const std::string &   value_    = item->BodyText();

            char buf1[8192];
            char buf2[8192];

            if ( value_.length() > 0 )
            {
                app->m_cConfig.SetConfig( Utf8ToAnsi(key_.c_str(),   buf1, sizeof(buf1)), 
                         Utf8ToAnsi(value_.c_str(), buf2, sizeof(buf2)) );
            }

            item = item->NextElement();
        }

        app->m_bConfigAvailable = TRUE;

        app->HandleScreenedChatroomCfg();
    }

    return true;
}


bool MyVcardTask::HandleStanza(const XmlElement * stanza)
{
    // �����IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    const XmlElement * iq = stanza;
    const std::string & type = iq->Attr(QN_TYPE);
    const std::string & from = iq->Attr(QN_FROM);
    LibJingleEx::TVCard   tVCard;
    char buf[MAX_BUFF_SIZE];


    // ����
    XmppClient * client  = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );


    const XmlElement * vcard = iq->FirstElement();
    // ����Ƿ�յ�result(������"vcard"��iq-result)
    if ( 0 == vcard )
    {
        if ( type == STR_RESULT )    
        {
            const std::string & id = iq->Attr(QN_ID);

            std::vector< char * >::iterator it;
            for ( it = m_vIds.begin(); it != m_vIds.end(); it++ )
            {
                char * pchId = *it;
                // ����ҵ�������ǿյ�result
                if ( id == pchId )
                {
                    delete[] pchId;
                    m_vIds.erase( it );

                    LibJingleEx::TJid tJid;
                    LibJingleEx::StrJid2TJid( from.c_str(), tJid );

                    LibJingleEx::TJid tSelfJid;
                    LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), tSelfJid );
                    tSelfJid.sResource[0] = '\0';

                    BOOL bSelf = ::IfJidEqual( tJid, tSelfJid );

                    if ( g_tEvents.pOnVCardInfo )
                    {
                        g_tEvents.pOnVCardInfo( app, &tJid, &tVCard, bSelf );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;
                        msgData.Append( (unsigned char *)&tJid,   sizeof(tJid)  );
                        msgData.Append( (unsigned char *)&tVCard, sizeof(tVCard) );
                        msgData.Append( (unsigned char *)&bSelf,  sizeof(bSelf) );

                        g_pfnOnMessage( LibJingleEx::ON_VCARD_INFO, app, app->m_pUserData,
                            msgData.GetMsgData(), msgData.GetMsgLength()  );
                    }

                    return true;
                }
            }
        }
        return false;
    }

    if ( vcard->Name() != buzz::QN_VCARD )
    {
        return false;
    }

    if ( type == "result" )
    {
        const XmlElement * fn     = 0;
        const XmlElement * cat    = 0;
        const XmlElement * photo  = 0;

        const XmlElement * child = vcard->FirstElement();
        while( child )
        {
            if ( child->Name() == QN_VCARD_FN )
            {
                fn = child;
            }
            else if ( child->Name() == QN_VCARD_CATEGORIES )
            {
                cat = child;
            }
            else if ( child->Name() == QN_VCARD_PHOTO )
            {
                photo = child;
            }
            child = child->NextElement();
        }

        if ( fn )
        {
            strncpy( tVCard.m_achNickName, Utf8ToAnsi( fn->BodyText().c_str(), buf, MAX_BUFF_SIZE ), LibJingleEx::VCARD_MAX_NICK_LEN );
        }

        if ( cat )
        {
            child = cat->FirstElement();
            while( child )
            {
                if ( child->Attr(QN_NAME) == VCARD_KEYWORD_NAME_CUSTOM_PORTRAIT_FLAG )
                {
                    if ( child->BodyText() == "true" )
                    {
                        tVCard.m_bCustomHeadPortrait = TRUE;
                    }
                    else
                    {
                        tVCard.m_bCustomHeadPortrait = FALSE;
                    }
                }
                else if ( child->Attr(QN_NAME) == VCARD_KEYWORD_NAME_SYS_PORTRAIT_INDEX )
                {
                    sscanf( child->BodyText().c_str(), " %d", &tVCard.m_nSysHeadPortraitIndex );
                }
                child = child->NextElement();
            }
        }

        // ���ȡ�Զ���ͼ��
        if ( tVCard.m_bCustomHeadPortrait )
        {
            if ( photo )
            {
                const XmlElement * image_type = 0;
                const XmlElement * image_data = 0;

                child = photo->FirstElement();
                while( child )
                {
                    if ( child->Name() == QN_VCARD_PHOTO_TYPE )
                    {
                        image_type = child;
                    }
                    else if ( child->Name() == QN_VCARD_PHOTO_BINVAL )
                    {
                        image_data = child;
                    }
                    child = child->NextElement();
                }

                if ( image_type )
                {
                    tVCard.m_eCustomHeadPortraitType = LibJingleEx::IMAGE_TYPE_UNKNOWN;

                    if ( image_type->BodyText() == IMAGE_TYPE_BITMAP_STR )
                    {
                        tVCard.m_eCustomHeadPortraitType = LibJingleEx::IMAGE_TYPE_BITMAP;
                    }
                    else if ( image_type->BodyText() == IMAGE_TYPE_JPG_STR )
                    {
                        tVCard.m_eCustomHeadPortraitType =LibJingleEx:: IMAGE_TYPE_JPG;
                    }
                    if ( image_type->BodyText() == IMAGE_TYPE_GIF_STR )
                    {
                        tVCard.m_eCustomHeadPortraitType = LibJingleEx::IMAGE_TYPE_GIF;
                    }
                    if ( image_type->BodyText() == IMAGE_TYPE_PNG_STR )
                    {
                        tVCard.m_eCustomHeadPortraitType = LibJingleEx::IMAGE_TYPE_PNG;
                    }
                }

                if ( image_data )
                {
                    unsigned long dwSize = LibJingleEx::VCARD_MAX_HEAD_PORTRAIT_SIZE;
                    int nRet = LibJingleEx::DecodeBase64( tVCard.m_abyCumtomHeadPortrait, &dwSize, image_data->BodyText().c_str() );
                    // ������������xmpp�ͻ��������˽ϴ��ͷ��decode base64ʧ��
                    if ( 0 == nRet )
                    {
                        tVCard.m_dwCustomHeadPortraitSize = dwSize;
                    }
                }

            }
        }
       
        LibJingleEx::TJid tJid;
        LibJingleEx::StrJid2TJid( from.c_str(), tJid );

        LibJingleEx::TJid tSelfJid;
        LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), tSelfJid );
        tSelfJid.sResource[0] = '\0';

        BOOL bSelf = ::IfJidEqual( tJid, tSelfJid );

        if ( g_tEvents.pOnVCardInfo )
        {
            g_tEvents.pOnVCardInfo( app, &tJid, &tVCard, bSelf );
        }
        else if ( g_pfnOnMessage )
        {
            LibJingleEx::CMsgData  msgData;
            msgData.Append( (unsigned char *)&tJid,   sizeof(tJid)  );
            msgData.Append( (unsigned char *)&tVCard, sizeof(tVCard) );
            msgData.Append( (unsigned char *)&bSelf,  sizeof(bSelf) );

            g_pfnOnMessage( LibJingleEx::ON_VCARD_INFO, app, app->m_pUserData,
                            msgData.GetMsgData(), msgData.GetMsgLength()  );
        }

    }

    return true;
}


MyVcardTask::~MyVcardTask()
{
    std::vector< char * >::iterator it;
    for ( it = m_vIds.begin();  it != m_vIds.end(); it++ )
    {
        char * pchId = *it;
        delete[] pchId;
    }
    m_vIds.clear();
}


MyPubsubTask::~MyPubsubTask()
{
    
}

bool MyPubsubTask::HandleStanza(const XmlElement * stanza)
{
    // �������IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }
    
    const std::string & type = stanza->Attr( QN_TYPE );

    bool bResult = false;
    bool bError  = false;

    if ( STR_RESULT == type )
    {
        bResult = true;
    }
    else if ( STR_ERROR == type )
    {
        bError = true;
    }

    if ( !bResult && !bError )
    {
        return false;
    }

    bool bFind = false;
    const std::string & id = stanza->Attr( QN_ID );

    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        TIqId * p = *it;
        if ( 0 == strcmp( p->szIqId, id.c_str() ) )
        {
            bFind = true;
            break;
        }
    }

    if ( !bFind )
    {
        return false;
    }

    const TIqId * ptIqId = *it;
    BOOL  bSuccess = ( bResult ? TRUE: FALSE );

    XmppClient * client  = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );

    // �ص�
    if ( g_pfnOnMessage )
    {
        if ( ptIqId->dwSubType == PUBSUB_SUBTYPE_CREATE_NODE )
        {
            LibJingleEx::CMsgData  msgData;
            msgData.Append( (unsigned char *)&ptIqId->dwContextId,   sizeof(ptIqId->dwContextId)  );
            msgData.Append( (unsigned char *)&bSuccess,              sizeof(bSuccess) );

            g_pfnOnMessage( LibJingleEx::ON_PUBSUB_CREATE_NODE_RESULT, app, app->m_pUserData,
                            msgData.GetMsgData(), msgData.GetMsgLength()  );
        }
        else if ( ptIqId->dwSubType == PUBSUB_SUBTYPE_DELETE_NODE )
        {
            LibJingleEx::CMsgData  msgData;
            msgData.Append( (unsigned char *)&ptIqId->dwContextId,   sizeof(ptIqId->dwContextId)  );
            msgData.Append( (unsigned char *)&bSuccess,              sizeof(bSuccess) );

            g_pfnOnMessage( LibJingleEx::ON_PUBSUB_DELETE_NODE_RESULT, app, app->m_pUserData,
                msgData.GetMsgData(), msgData.GetMsgLength()  );
        }
        else if ( ptIqId->dwSubType == PUBSUB_SUBTYPE_SUBSCRIBE_NODE )
        {
            LibJingleEx::CMsgData  msgData;
            msgData.Append( (unsigned char *)&ptIqId->dwContextId,   sizeof(ptIqId->dwContextId)  );
            msgData.Append( (unsigned char *)&bSuccess,              sizeof(bSuccess) );

            g_pfnOnMessage( LibJingleEx::ON_PUBSUB_SUBSCRIBE_NODE_RESULT, app, app->m_pUserData,
                msgData.GetMsgData(), msgData.GetMsgLength()  );
        }
    }
    // end �ص�

    delete ptIqId;
    m_vIqIds.erase( it );

    return true;
}



bool PingTask::HandleStanza(const XmlElement * stanza)
{
    // �������IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    const std::string & type = stanza->Attr( QN_TYPE );
    // ������result��error����
    if ( STR_RESULT != type && STR_ERROR != type )
    {
        return false;
    }

    const XmlElement * err = 0;

    const XmlElement * child = stanza->FirstElement();
    while( child )
    {
        if ( child->Name() == buzz::QN_ERROR )
        {
            err = child;
        }
        child = child->NextElement();
    }


    const std::string & id = stanza->Attr( QN_ID );
    const std::string & from = stanza->Attr( QN_FROM );


    // ���ǰ׺����ping
    if ( id.substr( 0, strlen(PREFIX_PING) ) != PREFIX_PING )
    {
        return false;
    }
    

    // ����
    XmppClient * client  = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );


    // �������503���󣬾ͼ��unscribed��ϵ��
    if ( !(err && err->Attr(buzz::QN_CODE) == "503") )
    {
        LibJingleEx::TJid  tFromJid;
        LibJingleEx::StrJid2TJid( from.c_str(),  tFromJid );


        // ���unscribed ��ϵ�˵�ping
        std::vector<PTUnscribedContact> & v = app->m_vUnscribedContacts;
        std::vector<PTUnscribedContact>::iterator it_u;

        for ( it_u = v.begin(); it_u != v.end(); it_u++ )
        {
            PTUnscribedContact  p = *it_u;

            // �ҵ���
            if ( IfJidEqual( tFromJid, p->tJid ) )
            {
                p->dwFailCnt = 0;
                return true;
            }
        }
    }

    

    std::vector< TIqId * >::iterator it;
    for ( it = m_vIqIds.begin(); it != m_vIqIds.end(); it++ )
    {
        TIqId * p = *it;

        // �ҵ���
        if ( id == p->szIqId )
        {
            delete p;
            m_vIqIds.erase( it );

            // ��ռ�����
            app->m_dwPingTimeoutCnt = 0;
            return true;
        }
    }

    return false;
}




bool MyPrivacyTask::HandleStanza(const XmlElement * stanza)
{
    // �������IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    const XmlElement * query = stanza->FirstElement();
    if ( 0 == query )
    {
        return false;
    }

    if ( query->Name() != QN_PRIVACY_QUERY )
    {
        return false;
    }

    char buf[MAX_BUFF_SIZE];

    // ����
    XmppClient * client  = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock cLock( &app->m_lock );

    const std::string & type = stanza->Attr( QN_TYPE );
    if ( STR_SET == type )
    {
        const char * szFormat = "<iq type='result' id='%s' />";
        SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, stanza->Attr(QN_ID).c_str() );
        client->SendRaw( buf );
    }

    return true;
}


void MyTimeTask::QueryServerTime()
{
    XmppClient * client = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock  cLock( &app->m_lock );

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->AddAttr(buzz::QN_TYPE,STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr(buzz::QN_ID, randid);

    iq->SetAttr(buzz::QN_TO, client->jid().domain() );

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_TIME, true);
    iq->AddElement( query );

    client->SendStanza( iq );
    delete iq;

    app->m_dwQueryTick  = GetCurTick();
    strcpy( app->m_szQueryTimeId, randid );
}

bool  MyTimeTask::HandleStanza(const XmlElement * stanza)
{
    // �������IQ
    if ( stanza->Name() != QN_IQ )
    {
        return false;
    }

    const XmlElement * query = stanza->FirstElement();
    if ( 0 == query )
    {
        return false;
    }

    if ( query->Name() != QN_TIME_QUERY )
    {
        return false;
    }

    if ( stanza->Attr(QN_TYPE) != buzz::STR_RESULT )
    {
        return false;
    }

    XmppClient * client = GetClient();
    CLibJinglexApp * app = (CLibJinglexApp *)client->m_hApp;
    cFuncLock  cLock( &app->m_lock );

    // �����id��ƥ��
    if ( stanza->Attr(QN_ID) != app->m_szQueryTimeId )
    {
        return false;
    }

    const XmlElement * utc = query->FirstElement();
    if ( utc->Name().LocalPart() != "utc" )
    {
        return false;
    }
    
    // ����utcʱ���ʽ������ 20130111T18:03:49
    if ( utc->BodyText().length() != 17 )
    {
        return false;
    }

    struct tm  tTime;
    memset( &tTime, 0, sizeof(tTime) );

    char year[64] = { 0 };
    char month[64] = { 0 };
    char day[64] = { 0 };
    char hour[64] = { 0 };
    char minute[64] = { 0 };
    char second[64] = { 0 };

    const char * pBody = utc->BodyText().c_str();

    memcpy( year, pBody, 4 );
    pBody += 4;

    memcpy( month, pBody, 2 );
    pBody += 2;

    memcpy( day, pBody, 2 );
    pBody += 3;

    memcpy( hour, pBody, 2 );
    pBody += 3;

    memcpy( minute, pBody, 2 );
    pBody += 3;

    memcpy( second, pBody, 2 );
    pBody += 2;

    sscanf( year,  "%d", &tTime.tm_year );
    sscanf( month, "%d", &tTime.tm_mon );
    sscanf( day,   "%d", &tTime.tm_mday );

    sscanf( hour,   "%d", &tTime.tm_hour );
    sscanf( minute, "%d", &tTime.tm_min );
    sscanf( second, "%d", &tTime.tm_sec );

    tTime.tm_year -= 1900;
    tTime.tm_mon--;

    time_t tServerTime = mktime( &tTime );
    if ( -1 == tServerTime )
    {
        tServerTime = 0;
    }
    else
    {
        tServerTime -= CGlobalData::GetInstance()->m_nBias;
    }

    DWORD dwCurTick = GetCurTick();
    DWORD dwElapsedTick = dwCurTick - app->m_dwQueryTick;

    time_t   cur_time;
    time( &cur_time );

    cur_time -= dwElapsedTick / ( 1000 * 2 );
    app->m_dwTimeDiff = tServerTime - cur_time;

    app->m_bGetServerTime = TRUE;   
    return true;
}







}  // end namespace buzz

