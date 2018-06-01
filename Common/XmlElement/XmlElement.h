#ifndef _XML_ELEMENT_2014_01_15_
#define _XML_ELEMENT_2014_01_15_

#include <string>
#include <vector>
#include "LmnCommon.h"

namespace LmnCommon
{
    class CQName;

    class CPrefix
    {
    public:
        CPrefix( const std::string & sPrefix = "", const std::string & sFullName = "" );
        ~CPrefix();
        bool operator ==(const CPrefix & c ) const;
        CPrefix & operator = ( const CPrefix & c );
        
        const std::string & GetPrefix() const;
        const std::string & GetFullName() const;
        void SetPrefix( const std::string & sPrefix );
        void SetFullName( const std::string & sFullName );

        void AddRef( CQName * p );
        void DelRef( CQName * p );

    private:
        std::string   m_sPrefix;
        std::string   m_sFullName;
        std::vector< CQName * > m_vReferers;
    };

    typedef std::vector<CPrefix *> VPrefix;

    class CQName
    {
    public:
        CQName( const std::string & sLocalPart = "", const std::string & sNamespace = "", CPrefix * pPrefix = 0 );
        ~CQName();

        bool operator ==(const CQName & c ) const;
        CQName & operator = ( const CQName & c );

        const std::string & GetNamespace() const;
        const std::string & GetLocalPart() const;
        bool HasPrefix() const;
        bool GetPrefix( std::string & sPrefix ) const;
        bool GetPrefix( CPrefix * & pPrefix ) const;

        void SetNamespace( const std::string & sNamespace );
        void SetLocalPart( const std::string & sLocalPart );
        void SetPrefix   ( CPrefix * pPrefix );
        void ClearPrefix   ( );

    private:
        std::string    m_sNamespace;
        std::string    m_sLocalPart;                // ���������ռ���д���õĻ����ó�Ա��Ч
        CPrefix *      m_pPrefix;                   // ���õ���д�������ռ�
    };

    class CXmlAttr
    {
    public:
        CXmlAttr( const CQName & cName, const std::string sValue = "" );
        ~CXmlAttr();

        CXmlAttr & operator = ( const CXmlAttr & src );

        CQName & GetName();
        const std::string & GetValue() const;
        void SetValue( const std::string & sValue );

    private:
        CQName        m_cName;
        std::string   m_sValue;
    };

    

    class  CXmlElement
    {
    public:
        // bNew:�Ƿ�new������
        // bUseDefaultNs�Ƿ�ʹ��ȱʡ�������ռ䣬��element�����ַ���ʱʹ��
        CXmlElement( const CQName & cName, BOOL bNew = TRUE, BOOL bUseDefaultNs = FALSE );
        ~CXmlElement();
        void Clear();

        BOOL AddElement( CXmlElement * pChild );
        CXmlElement * GetFirstChildElement();
        CXmlElement * GetParentElement();
        CXmlElement * GetNextSiblingElement();
        CXmlElement * GetPrevSiblingElement();
        CXmlElement * GetLastChildElement();

        CQName &            GetName();
        const std::string & GetBody() const;
        void                SetBody( const std::string & sBody );

        DWORD GetAttributesCnt() const;
        CXmlAttr * GetAttr( DWORD dwIndex );
        const std::string & GetAttr( const CQName & cAttrName ) const;
        void  SetAttr( const CQName & cAttrName, const std::string & sValue );

        // �Ӹ�element����
        CPrefix * FindPrefix( const std::string & sPrefix, int & nDepth, VPrefix * pvPrefix = 0 );        
        // ��local element�޸�
        CPrefix * SetNamespacePrefix( const std::string & sPrefix, const std::string & sNamespace );
        // ���������ռ�����д��ʽ
        CPrefix * FindPrefixByNs( const std::string & sNamespace, int & nDepth, VPrefix * pvPrefix = 0 );        
        // �����������ռ�����д��ǰ׺��ʽ�����ڰ�elementת��Ϊ�ֽ���ʱ��
        void Normalize( VPrefix * pvPrefix = 0 );

        DWORD GetPrefixCnt() const;
        CPrefix * GetPrefix( DWORD dwIndex );

    private:
        CPrefix * GenerateUniqueNsPrefix( CQName & cName, VPrefix * pvPrefix = 0 );

        std::vector<CXmlAttr *>      m_vAttributes;
        std::vector<CPrefix * >      m_vNamespacePrefixs;                    // ��¼�����ռ��ǰ׺
        CQName                       m_cName;
        std::string                  m_sBody;

        CXmlElement *                m_pParent;
        CXmlElement *                m_pNestSibling;
        CXmlElement *                m_pPrevSibling;
        CXmlElement *                m_pFirstChild;

        BOOL                         m_bNew;
        BOOL                         m_bUseDefaultNs;
    };


    CXmlElement * GetXmlElementFromStream( const char * pStream, DWORD dwSize );
    void FreeXmlElement( CXmlElement * e );

    std::string & GetStrFromXmlElement( std::string & sStr, CXmlElement * ele, VPrefix * pvPrefix = 0 );


} // namespace LmnCommon



#endif