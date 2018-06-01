
#include <assert.h>
#include <time.h>
#include <algorithm>
#include "expat.h"

#if VLD_ON
#ifdef WIN32
#include "vld.h"
#endif
#endif

#include "XmlElement.h"


#define MAX_BUFF_SIZE      8192
#define XML_NS             "http://www.w3.org/XML/1998/namespace"





namespace LmnCommon
{
    CPrefix GetPrefixFromStr( CPrefix & cPrefix, const char * szStr );
    void XMLCALL startElement_(void *data, const char *el, const char **attr);
    void XMLCALL endElement_(void *data, const char *el);
    void XMLCALL getBodyText_(void *userData, const XML_Char *s, int len);
    std::string  GenerateRandPrefix( const std::string & sNamespace );
    void StringReplace( std::string & strBase, const std::string & strSrc, const std::string & strDes );
    std::string XmlSpecialChatReplace( const std::string & strText );

    static std::string  s_sEmptyStr = "";
    static CPrefix  s_XmlPrefix( "xml",XML_NS );
    


    CPrefix::CPrefix( const std::string & sPrefix /* = "" */, const std::string & sFullName /* = "" */ )
    {
        m_sPrefix = sPrefix;
        m_sFullName = sFullName;
    }

    CPrefix::~CPrefix()
    {
        std::vector< CQName * >::iterator it;
        for ( it = m_vReferers.begin(); it != m_vReferers.end(); it++ )
        {
            CQName * pName = *it;
            pName->ClearPrefix();
        }
        m_vReferers.clear();
    }

    bool CPrefix::operator ==(const CPrefix & c ) const
    {
        if ( m_sFullName == c.m_sFullName && m_sPrefix == c.m_sPrefix )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    CPrefix & CPrefix::operator = ( const CPrefix & c )
    {
        m_sPrefix = c.m_sPrefix;
        m_sFullName = c.m_sFullName;
        return *this;
    }

    const std::string & CPrefix::GetPrefix() const
    {
        return m_sPrefix;
    }

    const std::string & CPrefix::GetFullName() const
    {
        return m_sFullName;
    }

    void CPrefix::SetPrefix( const std::string & sPrefix )
    {
        m_sPrefix = sPrefix;
    }

    void CPrefix::SetFullName( const std::string & sFullName )
    {
        m_sFullName = sFullName;
    }

    void CPrefix::AddRef( CQName * p )
    {
        std::vector< CQName * >::iterator it = std::find( m_vReferers.begin(), m_vReferers.end(), p );
        assert( it == m_vReferers.end() );
        if ( it == m_vReferers.end() )
        {
            m_vReferers.push_back( p );
        }
    }

    void CPrefix::DelRef( CQName * p )
    {
        std::vector< CQName * >::iterator it = std::find( m_vReferers.begin(), m_vReferers.end(), p );
        assert( it != m_vReferers.end() );

        if ( it != m_vReferers.end() )
        {
            m_vReferers.erase( it );
        }
    }




    CQName::CQName( const std::string & sLocalPart /*= "" */, const std::string & sNamespace /* = "" */, CPrefix * pPrefix /* = 0 */ )
    {
        m_sLocalPart = sLocalPart;
        m_sNamespace = sNamespace;
        m_pPrefix = 0;

        SetPrefix( pPrefix );
    }

    CQName::~CQName()
    {
        if ( m_pPrefix )
        {
            m_pPrefix->DelRef( this );
            m_pPrefix = 0;
        }
    }

    bool CQName::operator == (const CQName & c ) const
    {
        if ( c.m_sLocalPart == m_sLocalPart && c.GetNamespace() == GetNamespace() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    CQName & CQName::operator = ( const CQName & c )
    {
        if ( m_pPrefix )
        {
            m_pPrefix->DelRef( this );
        }

        m_sNamespace = c.m_sNamespace;
        m_sLocalPart = c.m_sLocalPart;
        m_pPrefix    = c.m_pPrefix;
        
        if ( m_pPrefix )
        {
            m_pPrefix->AddRef( this );
        }
        return *this;
    }

    const std::string & CQName::GetNamespace() const
    {
        if ( 0 == m_pPrefix )
        {
            return m_sNamespace;
        }
        else
        {
            return m_pPrefix->GetFullName();
        }
    }

    const std::string & CQName::GetLocalPart() const
    {
        return m_sLocalPart;
    }

    bool CQName::HasPrefix() const
    {
        return 0 == m_pPrefix ? false : true;
    }

    bool CQName::GetPrefix( std::string & sPrefix ) const
    {
        if ( 0 == m_pPrefix )
        {
            return false;
        }

        sPrefix = m_pPrefix->GetPrefix();
        return true;
    }

    bool CQName::GetPrefix( CPrefix * & pPrefix ) const
    {
        if ( 0 == m_pPrefix )
        {
            return false;
        }

        pPrefix = m_pPrefix;
        return true;
    }


    void CQName::SetNamespace( const std::string & sNamespace )
    {
        if ( m_pPrefix )
        {
            m_pPrefix->DelRef( this );
            m_pPrefix = 0;
        }

        m_sNamespace = sNamespace;
    }

    void CQName::SetLocalPart( const std::string & sLocalPart )
    {
        m_sLocalPart = sLocalPart;
    }

    void CQName::SetPrefix( CPrefix * pPrefix )
    {
        if ( 0 == pPrefix )
        {
            if ( m_pPrefix )
            {
                m_pPrefix->DelRef( this );
                m_pPrefix = 0;
            }
        }
        else
        {
            if ( m_pPrefix )
            {
                m_pPrefix->DelRef( this );
                m_pPrefix = 0;
            }

            m_pPrefix = pPrefix;
            m_pPrefix->AddRef( this );
            m_sNamespace = "";
        }
    }

    void CQName::ClearPrefix( )
    {
        m_pPrefix = 0;
    }



    
    CXmlAttr::CXmlAttr( const CQName & cName, const std::string sValue /* = "" */ )
    {
        m_cName  = cName;
        m_sValue = sValue;
    }

    CXmlAttr::~CXmlAttr()
    {

    }

    CXmlAttr & CXmlAttr::operator = ( const CXmlAttr & src )
    {
        m_cName = src.m_cName;
        m_sValue = src.m_sValue;
        return *this;
    }

    CQName & CXmlAttr::GetName()
    {
        return m_cName;
    }

    const std::string & CXmlAttr::GetValue() const
    {
        return m_sValue;
    }

    void CXmlAttr::SetValue( const std::string & sValue )
    {
        m_sValue = sValue;
    }





    CXmlElement::CXmlElement( const CQName & cName, BOOL bNew /*= TRUE*/, BOOL bUseDefaultNs /* = FALSE */ )
    {
        m_cName = cName;

        m_pParent = 0;
        m_pNestSibling = 0;
        m_pPrevSibling = 0;
        m_pFirstChild = 0;
        
        m_bNew = bNew;
        m_bUseDefaultNs = bUseDefaultNs;
    }


    CXmlElement::~CXmlElement()
    {
        Clear();
    };

    void CXmlElement::Clear()
    {
        m_cName.SetPrefix( 0 );

        std::vector<CXmlAttr *>::iterator it;
        for ( it = m_vAttributes.begin(); it != m_vAttributes.end(); it ++ )
        {
            delete *it;
        }
        m_vAttributes.clear();


        CXmlElement * pChild = m_pFirstChild;
        while( pChild )
        {
            CXmlElement * pTmp = pChild;
            pChild = pChild->m_pNestSibling;

            if ( pTmp )
            {
                delete pTmp;
            }
            else
            {
                pTmp->Clear();
            }
        }

        if ( m_pParent )
        {
            if ( m_pParent->m_pFirstChild == this )
            {
                m_pParent->m_pFirstChild = m_pNestSibling;
            }
        }

        if ( m_pPrevSibling )
        {
            m_pPrevSibling->m_pNestSibling = m_pNestSibling;
        }

        if ( m_pNestSibling )
        {
            m_pNestSibling->m_pPrevSibling = m_pPrevSibling;
        }

        std::vector<CPrefix * > ::iterator it_q;
        for ( it_q = m_vNamespacePrefixs.begin(); it_q != m_vNamespacePrefixs.end(); it_q++ )
        {
            delete *it_q;
        }
        m_vNamespacePrefixs.clear();
    }

    BOOL CXmlElement::AddElement( CXmlElement * pChild )
    {
        if ( 0 == pChild )
        {
            return FALSE;
        }

        if ( pChild->m_pParent )
        {
            return FALSE;
        }

        CXmlElement * pLastChild = GetLastChildElement();

        pChild->m_pParent = this;
        pChild->m_pNestSibling = 0;
        pChild->m_pPrevSibling = pLastChild;

        if ( pLastChild )
        {
            pLastChild->m_pNestSibling = pChild;
        }

        if ( 0 == m_pFirstChild )
        {
            m_pFirstChild = pChild;
        }

        return TRUE;
    }

    CXmlElement * CXmlElement::GetFirstChildElement()
    {
        return m_pFirstChild;
    }

    CXmlElement * CXmlElement::GetParentElement()
    {
        return m_pParent;
    }

    CXmlElement * CXmlElement::GetNextSiblingElement()
    {
        return m_pNestSibling;
    }

    CXmlElement * CXmlElement::GetPrevSiblingElement()
    {
        return m_pPrevSibling;
    }

    CXmlElement * CXmlElement::GetLastChildElement()
    {
        CXmlElement * pChild = m_pFirstChild;
        CXmlElement * pTmp   = 0;

        while( pChild )
        {
            pTmp = pChild;
            pChild = pChild->m_pNestSibling;
        }

        return pTmp;
    }

    CQName & CXmlElement::GetName()
    {
        return m_cName;
    }   

    const std::string & CXmlElement::GetBody() const
    {
        return m_sBody;
    }

    void CXmlElement::SetBody( const std::string & sBody )
    {
        m_sBody = sBody;
    }


    DWORD CXmlElement::GetAttributesCnt() const
    {
        return m_vAttributes.size();
    }

    CXmlAttr * CXmlElement::GetAttr( DWORD dwIndex )
    {
        if ( dwIndex >= m_vAttributes.size() )
        {
            return 0;
        }

        return m_vAttributes[dwIndex];
    }

    DWORD CXmlElement::GetPrefixCnt() const
    {
        return m_vNamespacePrefixs.size();
    }

    CPrefix * CXmlElement::GetPrefix( DWORD dwIndex )
    {
        if ( dwIndex >= m_vNamespacePrefixs.size() )
        {
            return 0;
        }

        return m_vNamespacePrefixs[dwIndex];
    }

    const std::string & CXmlElement::GetAttr( const CQName & cAttrName ) const
    {
        std::vector<CXmlAttr *>::const_iterator it;
        for ( it = m_vAttributes.begin(); it != m_vAttributes.end(); it ++ )
        {
            CXmlAttr * pAttr = *it;

            if ( pAttr->GetName().operator == ( cAttrName ) )
            {
                return pAttr->GetValue();
            }
        }
        return s_sEmptyStr;
    }

    void  CXmlElement::SetAttr( const CQName & cAttrName, const std::string & sValue )
    {
        std::vector<CXmlAttr *>::iterator it;
        for ( it = m_vAttributes.begin(); it != m_vAttributes.end(); it ++ )
        {
            CXmlAttr * pAttr = *it;

            if ( pAttr->GetName().operator == (cAttrName) )
            {
                pAttr->SetValue( sValue );
                return;
            }
        }

        if ( sValue.length() > 0 )
        {
            CXmlAttr * pNewAttr = new CXmlAttr( cAttrName, sValue );
            m_vAttributes.push_back( pNewAttr );
        }
    }

    // 从父element找起
    CPrefix * CXmlElement::FindPrefix( const std::string & sPrefix, int & nDepth, VPrefix * pvPrefix /* = 0 */ )
    {
        std::vector<CPrefix *>::iterator it;
        for ( it = m_vNamespacePrefixs.begin(); it != m_vNamespacePrefixs.end(); it ++ )
        {
            CPrefix * pPrefix = *it;
            if ( pPrefix->GetPrefix() == sPrefix )
            {
                return pPrefix;
            }
        }

        CXmlElement * pParent = GetParentElement();
        if ( pParent )
        {
            nDepth++;
            return pParent->FindPrefix( sPrefix, nDepth, pvPrefix );
        }
        else
        {
            nDepth++;
            if ( sPrefix == "xml" )
            {
                return &s_XmlPrefix;
            }

            if ( pvPrefix )
            {
                for ( it = pvPrefix->begin(); it != pvPrefix->end(); it ++ )
                {
                    CPrefix * pPrefix = *it;
                    if ( pPrefix->GetPrefix() == sPrefix )
                    {
                        return pPrefix;
                    }
                }
            }
            

            return 0;
        }
    }

    // 在local element修改
    CPrefix * CXmlElement::SetNamespacePrefix( const std::string & sPrefix, const std::string & sNamespace )
    {
        std::vector<CPrefix *>::iterator it;
        for ( it = m_vNamespacePrefixs.begin(); it != m_vNamespacePrefixs.end(); it ++ )
        {
            CPrefix * pPrefix = *it;
            if ( pPrefix->GetPrefix() == sPrefix )
            {
                pPrefix->SetFullName( sNamespace );
                return pPrefix;
            }
        }

        CPrefix * pNewPrefix = new CPrefix( sPrefix, sNamespace );
        m_vNamespacePrefixs.push_back( pNewPrefix );
        return pNewPrefix;
    }

    // 根据命名空间找缩写形式
    CPrefix * CXmlElement::FindPrefixByNs( const std::string & sNamespace, int & nDepth, VPrefix * pvPrefix /* = 0 */ )
    {
        std::vector<CPrefix *>::iterator it;
        for ( it = m_vNamespacePrefixs.begin(); it != m_vNamespacePrefixs.end(); it ++ )
        {
            CPrefix * pPrefix = *it;
            if ( pPrefix->GetFullName() == sNamespace )
            {
                return pPrefix;
            }
        }

        CXmlElement * pParent = GetParentElement();
        if ( pParent )
        {
            nDepth++;
            return pParent->FindPrefixByNs( sNamespace, nDepth, pvPrefix );
        }
        else
        {
            nDepth++;
            if ( sNamespace == XML_NS )
            {
                return &s_XmlPrefix;
            }

            if ( pvPrefix )
            {
                for ( it = pvPrefix->begin(); it != pvPrefix->end(); it++ )
                {
                    CPrefix * pPrefix = *it;
                    if ( pPrefix->GetFullName() == sNamespace )
                    {
                        return pPrefix;
                    }
                }
            }
            return 0;
        }
    }

    CPrefix * CXmlElement::GenerateUniqueNsPrefix( CQName & cName, VPrefix * pvPrefix /* = 0 */ )
    {
        // 生成一个不存在的前缀
        std::string sPrefix = GenerateRandPrefix( cName.GetNamespace() );
        int nDepth = 0;
        CPrefix * pPrefix = FindPrefix( sPrefix, nDepth, pvPrefix );

        while( pPrefix )
        {
            sPrefix = GenerateRandPrefix( cName.GetNamespace() );
            nDepth = 0;
            pPrefix = FindPrefix( sPrefix, nDepth, pvPrefix );
        }

        pPrefix = SetNamespacePrefix( sPrefix, cName.GetNamespace() );
        return pPrefix;
    }

    // 尽量把命名空间用缩写的前缀方式，用于把element转化为字节流时用
    void  CXmlElement::Normalize( VPrefix * pvPrefix /* = 0 */ )
    {
        CPrefix * pPrefix = 0;
        CPrefix * pTmp = 0;
        int nDepth = 0;
        int nTmp = 0;

        // 如果没有命名空间的缩写形式
        if ( !m_cName.GetPrefix( pPrefix ) )
        {
            // 而且有命名空间
            if ( m_cName.GetNamespace().length() > 0 )
            {
                nDepth = 0;
                pPrefix = FindPrefixByNs( m_cName.GetNamespace(), nDepth, pvPrefix );
                if ( pPrefix )
                {
                    nTmp = 0;
                    pTmp = FindPrefix( pPrefix->GetPrefix(), nTmp, pvPrefix );
                    assert( pTmp );
                    
                    // 同缩写的不同命名空间
                    if ( nTmp != nDepth )
                    {
                        if ( m_bUseDefaultNs )
                        {
                            pPrefix = SetNamespacePrefix( "", m_cName.GetNamespace() );
                        }
                        else
                        {
                            pPrefix = GenerateUniqueNsPrefix( m_cName, pvPrefix );
                        }
                    }
                }
                else
                {
                    if ( m_bUseDefaultNs )
                    {
                        pPrefix = SetNamespacePrefix( "", m_cName.GetNamespace() );
                    }
                    else
                    {
                        pPrefix = GenerateUniqueNsPrefix( m_cName, pvPrefix );
                    }
                }

                m_cName.SetPrefix( pPrefix );
            }
        }

        std::vector<CXmlAttr *>::iterator it;
        for ( it = m_vAttributes.begin(); it != m_vAttributes.end(); it ++ )
        {
            CXmlAttr * pAttr = *it;
            
            // 如果没有命名空间的缩写形式
            if ( !pAttr->GetName().GetPrefix( pPrefix ) )
            {
                // 而且有命名空间
                if ( pAttr->GetName().GetNamespace().length() > 0 )
                {
                    nDepth = 0;
                    pPrefix = FindPrefixByNs( pAttr->GetName().GetNamespace(), nDepth, pvPrefix );
                    if ( pPrefix )
                    {
                        nTmp = 0;
                        pTmp = FindPrefix( pPrefix->GetPrefix(), nTmp, pvPrefix );
                        assert( pTmp );

                        // 同缩写的不同命名空间
                        if ( nTmp != nDepth )
                        {
                            pPrefix = GenerateUniqueNsPrefix( pAttr->GetName(), pvPrefix );
                        }
                    }
                    else
                    {
                        pPrefix = GenerateUniqueNsPrefix( pAttr->GetName(), pvPrefix );
                    }

                    pAttr->GetName().SetPrefix( pPrefix );
                }
            }
        }

        CXmlElement * pChild = m_pFirstChild;
        while( pChild )
        {
            pChild->Normalize( pvPrefix );
            pChild = pChild->m_pNestSibling;
        }
    }





    CXmlElement * GetXmlElementFromStream( const char * pStream, DWORD dwSize )
    {
        XML_Parser parser = XML_ParserCreate(NULL);

        if ( !parser ) 
        {
            return 0;
        }

        CXmlElement * current_node_ = 0;

        XML_SetElementHandler       ( parser, startElement_, endElement_ );
        XML_SetCharacterDataHandler ( parser, getBodyText_ );
        XML_SetUserData             ( parser, &current_node_ );


        if ( XML_Parse( parser, pStream, dwSize, TRUE ) == XML_STATUS_ERROR )
        {
            fprintf(stderr, "Parse error at line %lu:\n%s\n", XML_GetCurrentLineNumber(parser), XML_ErrorString(XML_GetErrorCode(parser)));
            XML_ParserFree(parser);

            // 销毁root xml element
            CXmlElement * tmp_node_ = 0;
            while( current_node_ )
            {
                tmp_node_ = current_node_;
                current_node_ = current_node_->GetParentElement();
            }

            if ( tmp_node_ )
            {
                delete tmp_node_;
            }
            return 0;
        }

        XML_ParserFree(parser);

        CXmlElement * tmp_node_ = 0;
        while( current_node_ )
        {
            tmp_node_ = current_node_;
            current_node_ = current_node_->GetParentElement();
        }

        return tmp_node_;
    }

    std::string & GetStrFromXmlElement_( std::string & sStr, CXmlElement * ele, VPrefix * pvPrefix, BOOL bGeneratePrefix )
    {
        DWORD i;
        std::string s;
        CPrefix * pElePrefix = 0;
        CPrefix * pPrefix = 0;

        if ( bGeneratePrefix )
        {
            ele->Normalize( pvPrefix );
        }
        
        sStr  += "<";

        if ( ele->GetName().GetPrefix( pElePrefix ) )
        {
            if ( pElePrefix->GetPrefix().length() > 0 )
            {
                sStr += pElePrefix->GetPrefix();
                sStr += ":";
            }
        }
        sStr += ele->GetName().GetLocalPart();
        

        // 写属性
        DWORD dwAttrCnt = ele->GetAttributesCnt();
        for ( i = 0; i < dwAttrCnt; i++ )
        {
            CXmlAttr * pAttr = ele->GetAttr(i);

            sStr    += " ";

            if ( pAttr->GetName().GetPrefix( pPrefix ) )
            {
                if ( pPrefix->GetPrefix().length() > 0 )
                {
                    sStr += pPrefix->GetPrefix();
                    sStr += ":";
                }
            }

            sStr    += pAttr->GetName().GetLocalPart();
            sStr    += "=\"";
            sStr    += XmlSpecialChatReplace( pAttr->GetValue() );
            sStr    += "\"";
        }

        // 写namespace
        DWORD dwPrefixCnt = ele->GetPrefixCnt();
        for ( i = 0; i < dwPrefixCnt; i++ )
        {
            CPrefix * pPrefixItem = ele->GetPrefix( i );

            if ( pPrefixItem->GetPrefix().length() > 0 )
            {
                sStr += " ";
                sStr += "xmlns:";
                sStr += pPrefixItem->GetPrefix();
                sStr += "=\"";
            }
            else
            {
                sStr += " ";
                sStr += "xmlns=\"";
            }

            sStr += XmlSpecialChatReplace( pPrefixItem->GetFullName() );
            sStr += "\"";
        }



        if ( 0 == ele->GetFirstChildElement() && 0 == ele->GetBody().length()  )
        {
            sStr  += "/>";
        }
        else
        {
            sStr  += ">";

            // 没有子节点和BodyText，可以缩写成<element/>形式
            CXmlElement * child = ele->GetFirstChildElement();
            while ( child )
            {
                GetStrFromXmlElement_( sStr, child, pvPrefix, FALSE );
                child = child->GetNextSiblingElement();
            }

            if ( ele->GetBody().length() > 0 )
            {
                sStr += XmlSpecialChatReplace( ele->GetBody() );
            }

            sStr  += "</";

            if ( pElePrefix && pElePrefix->GetPrefix().length() > 0 )
            {
                sStr += pElePrefix->GetPrefix();
                sStr += ":";
            }
            sStr  += ele->GetName().GetLocalPart();
            sStr  += ">";
        }

        return sStr;
    }

    std::string & GetStrFromXmlElement( std::string & sStr, CXmlElement * ele, VPrefix * pvPrefix /* = 0 */ )
    {
        return GetStrFromXmlElement_( sStr, ele, pvPrefix, TRUE );
    }

    void FreeXmlElement( CXmlElement * e )
    {
        if ( e )
        {
            delete e;
        }
    }



    static CPrefix GetPrefixFromStr( CPrefix & cPrefix, const char * szStr )
    {
        const char * pColon = strchr( szStr, ':');
        if ( pColon )
        {
            DWORD dwLen = pColon-szStr;
            char * p = new char[dwLen+1];
            memcpy( p, szStr, dwLen );
            p[dwLen] = '\0';

            cPrefix.SetPrefix( p );
            delete[] p;

            cPrefix.SetFullName( pColon + 1 );
        }
        else
        {
            cPrefix.SetPrefix( "" );
            cPrefix.SetFullName( szStr );
        }
        return cPrefix;
    }

    static std::string  GenerateRandPrefix( const std::string & sNamespace )
    {
        // 随机数字的个数
        const int RAND_DIGITAL_CNT = 3;
        char buf[RAND_DIGITAL_CNT+1] = { 0 };

        srand( LmnGetTickCount() );

        while( true )
        {
            int i;
            for ( i = 0; i < RAND_DIGITAL_CNT; i++ )
            {
                int r = rand() % 26;
                buf[i] = 'a' + r;
            }

            if ( 0 != strcmp(buf, "xml") )
            {
                break;
            }
        }
        
        return buf;
    }

    static void XMLCALL startElement_(void *data, const char *el, const char **attr)
    {
        CXmlElement ** ppCurrent = (CXmlElement **)data;
        CXmlElement *  child = 0;
        CPrefix        cElePrefix;
        CPrefix        cPrefix;
        CPrefix *      pPrefix = 0;
        int            nDepth = 0;

        GetPrefixFromStr( cElePrefix, el );
        // root元素
        if ( 0 == *ppCurrent )
        {
            child = new CXmlElement( CQName( cElePrefix.GetFullName() ), TRUE );
        }
        else
        {
            child = new CXmlElement( CQName( cElePrefix.GetFullName() ) );
        }


        int i;
        // 先保存prefix
        for (i = 0; attr[i]; i += 2)
        {
            GetPrefixFromStr( cPrefix, attr[i] );

            if ( "xmlns" == cPrefix.GetPrefix() )
            {
                child->SetNamespacePrefix( cPrefix.GetFullName(), attr[i+1] );
            }
            else if ( 0 == cPrefix.GetPrefix().length() && "xmlns" == cPrefix.GetFullName() )
            {
                child->SetNamespacePrefix( "", attr[i+1] );
            }
        }

        if ( *ppCurrent )
        {
            (*ppCurrent)->AddElement( child );
        }

        nDepth = 0;
        pPrefix = child->FindPrefix( cElePrefix.GetPrefix(), nDepth );
        child->GetName().SetPrefix( pPrefix );


        for (i = 0; attr[i]; i += 2)
        {
            GetPrefixFromStr( cPrefix, attr[i] );

            if ( !(("xmlns" == cPrefix.GetPrefix()) || (0 == cPrefix.GetPrefix().length() && "xmlns" == cPrefix.GetFullName())) )
            {
                nDepth = 0;
                pPrefix = child->FindPrefix( cPrefix.GetPrefix(), nDepth );
                child->SetAttr( CQName( cPrefix.GetFullName(), "", pPrefix ), attr[i+1] );
            }
        }

        *ppCurrent = child;
    }


    static void XMLCALL endElement_(void *data, const char *el)
    {
        CXmlElement ** ppCurrent = (CXmlElement **)data;
        assert( *ppCurrent );

        if ( (*ppCurrent)->GetParentElement() )
            *ppCurrent = (*ppCurrent)->GetParentElement();
    }

    static void XMLCALL getBodyText_(void *userData, const XML_Char *s, int len)
    {
        CXmlElement ** ppCurrent = (CXmlElement **)userData;
        assert( *ppCurrent );

        char buf[MAX_BUFF_SIZE] = { 0 };
        char * pBuf = 0;

        if ( len >= MAX_BUFF_SIZE )
        {
            pBuf = new char[len+1];
        }
        else
        {
            pBuf = buf;
        }

        memcpy( pBuf, s, len );
        pBuf[len] = '\0';

        StrTrim( pBuf );

        // CAUSION: 只添加非空白字符串为BodyText
        if ( '\0' != pBuf[0] )
        {
            std::string sTmp = (*ppCurrent)->GetBody();
            sTmp += pBuf;
            (*ppCurrent)->SetBody( sTmp.c_str() );
        }

        if ( pBuf != buf )
        {
            delete[] pBuf;
        }
    }

    static void StringReplace( std::string & strBase, const std::string & strSrc, const std::string & strDes )  
    {  
        std::string::size_type pos = 0;  
        std::string::size_type srcLen = strSrc.size();  
        std::string::size_type desLen = strDes.size();  

        pos=strBase.find(strSrc, pos);   
        while ((pos != std::string::npos))  
        {  
            strBase.replace(pos, srcLen, strDes);  
            pos=strBase.find(strSrc, (pos+desLen));  
        }  
    }

    static std::string XmlSpecialChatReplace( const std::string & strText )
    {
        std::string sRet = strText;

        StringReplace( sRet, "&",  "&amp;" );
        StringReplace( sRet, ">",  "&gt;" );
        StringReplace( sRet, "<",  "&lt;" );
        StringReplace( sRet, "\"", "&quot;" );
        StringReplace( sRet, "'",  "&apos;" );

        return sRet;
    }

} // namespace LmnCommon






