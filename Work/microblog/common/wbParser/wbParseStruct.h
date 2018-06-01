#ifndef __WB_PARSE_STRUCT_H__
#define __WB_PARSE_STRUCT_H__

#include <string.h>
#include <stdlib.h>

#define WTRUE  true
#define WFALSE false

#ifndef BUILD_CHARACTER_SET
typedef char WBParseCHAR;
#	define _WBC(c) c
#	define WBPARSER_COPY(dst,size,source,srcsize) strncpy(dst,source,size)
#else

typedef TCHAR WBParseCHAR;

#	define _WBC(c) _T(c)
#	define WBPARSER_COPY(dst,size,source,srcsize) _tcsncpy_s(dst,size,source,srcsize)

#endif // BUILD_CHARACTER_SET

typedef int   WBParseINT;
typedef long  WBParseLONG;
typedef bool  WBParseBOOL;


namespace wbParserNS
{

#define WBPARSER_UINT_DEFINE(name,val)     const unsigned int WBPARSER_##name##_LEN = val
#define WBPARSER_USE_LEN(name)             WBPARSER_##name##_LEN
#define WBPARSER_REAL_LEN(name)            WBPARSER_##name##_LEN + 10 ///< ʵ�ʵĳ���
#define WBPARSER_OPTION_TYPE(name)         WBPT_OPTION_TYPE_##name    ///< ����������
#define WBPARSER_INFOR_TYPE(name)		   WBPT_INFOR_TYPE_##name     ///< �ṹ������


	// ������Ϣ
	WBPARSER_UINT_DEFINE(text,2048);             ///< ΢����Ϣ����

	WBPARSER_UINT_DEFINE(messageType,30);        ///< ����Ϣ����

	WBPARSER_UINT_DEFINE(departmentName,200);    ///< ��֯�ܹ�-��������
	WBPARSER_UINT_DEFINE(fullPath,1000);         ///< ��֯�ܹ�-����ȫ·��
	WBPARSER_UINT_DEFINE(deptPosition,100);      ///< ��֯�ܹ�-����ְλ

	WBPARSER_UINT_DEFINE(wbOnline,10);           ///< ΢������״̬
	WBPARSER_UINT_DEFINE(tlOnline,10);           ///< TrueLink����״̬
	WBPARSER_UINT_DEFINE(tdOnline,10);           ///< pad������״̬(ios/android)
	WBPARSER_UINT_DEFINE(tpOnline,10);           ///< phone������״̬(ios/android)
	WBPARSER_UINT_DEFINE(tsOnline,10);           ///< TrueSens����״̬

	WBPARSER_UINT_DEFINE(entName,100);            ///< ��ҵĿ¼����
	WBPARSER_UINT_DEFINE(entRealName,100);        ///< ��ҵ����
	WBPARSER_UINT_DEFINE(cn,50);                  ///< RDN,Ψһ����
	WBPARSER_UINT_DEFINE(sn,50);                  ///< ��ʵ����,������
	WBPARSER_UINT_DEFINE(enable,5);               ///< ����/���ã�0/1
	WBPARSER_UINT_DEFINE(privileged,5);           ///< ��Ȩ,��ʶ����Ա��0/1
	WBPARSER_UINT_DEFINE(entMail,100);            ///< ��ҵ����,΢����¼�˺�
	WBPARSER_UINT_DEFINE(subMail,100);            ///< ���������ַ,�ʼ�����
	WBPARSER_UINT_DEFINE(e164,16);                ///< E164��
	WBPARSER_UINT_DEFINE(jid,64);                 ///< xmpp�˺�
	WBPARSER_UINT_DEFINE(mobileNum,30);           ///< ��ϵ�绰
	WBPARSER_UINT_DEFINE(extNum,30);              ///< �ֻ�
	WBPARSER_UINT_DEFINE(seat,30);                ///< ��λ��
	WBPARSER_UINT_DEFINE(brief,220);              ///< һ�仰����(70������)
	WBPARSER_UINT_DEFINE(position,30);            ///< ְλ
	WBPARSER_UINT_DEFINE(portrait32,256);         ///< 32λͷ��
	WBPARSER_UINT_DEFINE(portrait40,256);         ///< 40λͷ��
	WBPARSER_UINT_DEFINE(portrait64,256);         ///< 64λͷ��
	WBPARSER_UINT_DEFINE(portrait128,256);        ///< 128λͷ��
	WBPARSER_UINT_DEFINE(portrait256,256);        ///< 256λͷ��
	WBPARSER_UINT_DEFINE(password,40);            ///< XMPP��½����

	WBPARSER_UINT_DEFINE(enableWeibo,5);          ///< ΢��Ȩ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(enableMeeting,5);        ///< �������Ȩ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(enableHD,5);             ///< ����Ȩ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(enableCall,5);           ///< �绰����Ȩ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(enableRoam,5);           ///< ����Ȩ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(weiboAdmin,5);           ///< ΢������ԱȨ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(meetingAdmin,5);         ///< �������ԱȨ��(0/1,û��/��)
	WBPARSER_UINT_DEFINE(isLikeQuery,5);          ///< �Ƿ�֧��ģ������(0:�����ԣ�1:����)
	WBPARSER_UINT_DEFINE(authMode,5);             ///< �����ϵ����֤ģʽ(0:������֤��1:����֤��2:�ܾ�����)
	WBPARSER_UINT_DEFINE(sex,5);                  ///< �Ա�(1/0,��/Ů)

	WBPARSER_UINT_DEFINE(xmppDomain,30);          ///< xmpp����
	WBPARSER_UINT_DEFINE(apiUrl,100);             ///< ƽ̨API��ַ
	WBPARSER_UINT_DEFINE(weiboUrl,100);           ///< ΢��API��ַ
	WBPARSER_UINT_DEFINE(webImUrl,100);           ///< web�ͻ�������XMPP��web�����ַ(�ն˲���Ҫ)
	WBPARSER_UINT_DEFINE(tcpImUrl,100);           ///< XMPP�����ַ
	WBPARSER_UINT_DEFINE(tcpPort,10);             ///< XMPP����˿�
	WBPARSER_UINT_DEFINE(ulsUrl,100);             ///< ULS��ַ
	WBPARSER_UINT_DEFINE(ulsPort,10);             ///< ULS�˿�

	WBPARSER_UINT_DEFINE(content,900);                   ///< ˽������(300������)

	WBPARSER_UINT_DEFINE(feedCount,10);                  ///< �������Ϣ����
	WBPARSER_UINT_DEFINE(userName,60);                   ///< �û�����
	WBPARSER_UINT_DEFINE(feedId,10);                     ///< �޵���ϢID
	WBPARSER_UINT_DEFINE(groupUid,20);                   ///< ��uid
	WBPARSER_UINT_DEFINE(groupBriefly,200);              ///< ����

	WBPARSER_UINT_DEFINE(leaf,5);                        ///< �Ƿ����ӽڵ㣺ture��û�У�false����

	WBPARSER_UINT_DEFINE(mugshot128,256);                     ///< �û�ͷ���ַ
	WBPARSER_UINT_DEFINE(mugshot64,256);                      ///< �û�ͷ���ַ
	WBPARSER_UINT_DEFINE(mugshot40,256);                      ///< �û�ͷ���ַ
	WBPARSER_UINT_DEFINE(personalBriefly,210);               ///< һ�仰����(70������)
	WBPARSER_UINT_DEFINE(uid,30);                            ///< �û�UID/��UID
	WBPARSER_UINT_DEFINE(name,60);                           ///< �û�����/������/��������
	WBPARSER_UINT_DEFINE(email,50);                          ///< �û�email(�˺�)
	WBPARSER_UINT_DEFINE(defaultUid,30);                     ///< ϵͳĬ�ϸ�����û�Ψһ��ʶ����û��ʹ�ã�
	WBPARSER_UINT_DEFINE(online,5);                          ///< �Ƿ�����
	WBPARSER_UINT_DEFINE(beenFollowBoth,5);                  ///< �Ƿ���ع�ע

	WBPARSER_UINT_DEFINE(userId,64);                         ///< �û�ID
	WBPARSER_UINT_DEFINE(jobTitle,60);                       ///< ְλ
	WBPARSER_UINT_DEFINE(location,20);                       ///< ��λ
	WBPARSER_UINT_DEFINE(workPhone,30);                      ///< �����绰
	WBPARSER_UINT_DEFINE(mobilePhone,20);                    ///< �ֻ�

	WBPARSER_UINT_DEFINE(id,64);				    	     ///< ��ϢID/�û�ID/��ID/����ID
	WBPARSER_UINT_DEFINE(body,1500);                         ///< ���������(���280���ַ� 1����=2�ַ�)
	WBPARSER_UINT_DEFINE(threadId,10);                       ///< ����ID
	WBPARSER_UINT_DEFINE(directMessage,5);                   ///< �Ƿ���˽�е���Ϣ
	WBPARSER_UINT_DEFINE(broadcast,5);                       ///< �Ƿ�㲥
	WBPARSER_UINT_DEFINE(systemMessage,5);                   ///< �Ƿ���ϵͳ��Ϣ
	WBPARSER_UINT_DEFINE(feedType,10);                       ///< ��Ϣ����
	WBPARSER_UINT_DEFINE(createdBy,64);                      ///< ������
	WBPARSER_UINT_DEFINE(createdAt,20);                      ///< ����ʱ��
	WBPARSER_UINT_DEFINE(clientType,30);                     ///< ��Ϣ��Դ�����ֿͻ���
	WBPARSER_UINT_DEFINE(type,20);                           ///< ��������(create/destroy)

	WBPARSER_UINT_DEFINE(isBookMarked,5);                    ///< �����Ƿ��ղ�
	WBPARSER_UINT_DEFINE(hot,10);                            ///< �ȶ�
	WBPARSER_UINT_DEFINE(title,150);                         ///< ��ƪ���۱���
	WBPARSER_UINT_DEFINE(isDelete,5);                        ///< �Ƿ�ɾ��

	WBPARSER_UINT_DEFINE(repliedToId,10);                    ///< �ظ���������Ϣ��ID
	WBPARSER_UINT_DEFINE(repliedToBody,1500);                ///< �ظ���������Ϣ������(���280���ַ� 1����=2�ַ�)
	WBPARSER_UINT_DEFINE(repliedToUserId,64);                ///< ��Ϣ�ظ����Ǹ���Ա��ID

	WBPARSER_UINT_DEFINE(lastFeedId,10);                     ///< ��ǰ���������Ϣ�������ϢID

	WBPARSER_UINT_DEFINE(readFeedId,10);                     ///< ���ص���Ϣ�����µ���ϢID
	WBPARSER_UINT_DEFINE(oldestFeedId,10);                   ///< ���ص���Ϣ�������������ظ�ID
	WBPARSER_UINT_DEFINE(totalCount,10);                     ///< �ܵ�����

	WBPARSER_UINT_DEFINE(groupId,10);                        ///< ��ID
	WBPARSER_UINT_DEFINE(topicId,10);                        ///< ����ID

	WBPARSER_UINT_DEFINE(extensionName,10);                  ///< ͼƬ/�����ĺ�׺��
	WBPARSER_UINT_DEFINE(picturePreviewUrl,256);             ///< ��ͼ��ı�����Ե�ַ
	WBPARSER_UINT_DEFINE(pictureTinyUrl,256);                ///< Сͼ��ı�����Ե�ַ
	WBPARSER_UINT_DEFINE(pictureMobileUrl,256);              ///< Mobile��ͼ��ı�����Ե�ַ
	WBPARSER_UINT_DEFINE(pictureName,256);                   ///< ͼƬ����
	WBPARSER_UINT_DEFINE(pictureUrl,256);                    ///< ԭͼƬ��ַ

	WBPARSER_UINT_DEFINE(fileName,256);                      ///< ��������
	WBPARSER_UINT_DEFINE(fileUrl,256);                       ///< �����������Ե�ַ

	WBPARSER_UINT_DEFINE(replyNum,5);                        ///< �ظ��ĸ���
	WBPARSER_UINT_DEFINE(replyNoLoadNum,5);                  ///< δ���صĸ���
	WBPARSER_UINT_DEFINE(beenFollowed,5);                    ///< �Ƿ��ע
	// add end

    WBPARSER_UINT_DEFINE(createLikeTime, 32);


	WBPARSER_UINT_DEFINE(url,1024);
	WBPARSER_UINT_DEFINE(created_time,255);

	WBPARSER_UINT_DEFINE(request,255);
	WBPARSER_UINT_DEFINE(error,32);            ///< ��������
	WBPARSER_UINT_DEFINE(error_code,16);       ///< ������
	WBPARSER_UINT_DEFINE(TGT,255);



    WBPARSER_UINT_DEFINE(subject,   100);
    WBPARSER_UINT_DEFINE(datetime,  20);
    WBPARSER_UINT_DEFINE(organizer, 30);
    WBPARSER_UINT_DEFINE(organizerMail, 50);
    WBPARSER_UINT_DEFINE(phone,   30);
    WBPARSER_UINT_DEFINE(m_brief, 1500);
    WBPARSER_UINT_DEFINE(mobile,  20);
    WBPARSER_UINT_DEFINE(template_id,  20);
    WBPARSER_UINT_DEFINE(template_name,  64);
    WBPARSER_UINT_DEFINE(participantNo,  20);
    WBPARSER_UINT_DEFINE(remark,  100);
    WBPARSER_UINT_DEFINE(common,  64);
    WBPARSER_UINT_DEFINE(moId,    64);
    WBPARSER_UINT_DEFINE(regionName,    50);
    WBPARSER_UINT_DEFINE(companyMoId,    64);

	

	struct t_wbParse_Error;

	typedef enum 
	{
		EPS_OPENAPI ,  ///< from OpenAPI Http 
		EPS_STREAM  ,  ///< from STREAM
		EPS_XMPP    ,  ///< from xmpp

	} eParseSource;

	/** ������Ϣͷ��ÿ���ṹ�����Ҫ�е� by welbon,2011-01-08*/
	struct t_wbParse_Header
	{
		int eSource;
	};
	

	/// ��֤������Ϣ
	struct t_wbParse_Oauth
	{
		t_wbParse_Header ph_;

		char oauth_token[ 257 ];
		char oauth_token_secret[ 257 ];

		char uid_[128];

		char oauth_verifier[ 65 ];
	};

	/** cookie ��Ϣ */
	struct t_wbParse_Cookie
	{
		t_wbParse_Header ph_; ///< ����ͷ

		// cookie
		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];
		//
		char ticket[256];
		char publickey[512];
		char keyversion[64];
		char uid[128];

#ifdef _MSC_VER
		__int64 lservertime;//��ȡTGTʱ������ʱ��
		__int64 llocaltime;//TGT����ʱ����ʱ��
#else
		long long lservertime;//��ȡTGTʱ������ʱ��
		long long llocaltime;//TGT����ʱ����ʱ��
#endif
	};


	/** ����TGT��� */
	struct t_wbParse_updateTGT
	{
		t_wbParse_Header ph_; ///< ����ͷ

		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];///< TGT	
	};

	struct t_wbParse_Cursor
	{
		t_wbParse_Header ph_; ///< ����ͷ

#ifdef _MSC_VER
		__int64 nextcur;
		__int64 prevcur;
#else
		long long nextcur;
		long long prevcur;
#endif

	};


// -------------------------------------

// 2012-03-07 update by sunfei <result> ʹ�õ����ݽṹ

	struct t_wbParse_Keda_Depts;

	struct t_wbParse_Keda_EntUser
	{
        WBParseCHAR moId[WBPARSER_REAL_LEN(moId)];                        ///< 
        WBParseCHAR companyMoId[WBPARSER_REAL_LEN(companyMoId)];          ///< 

		WBParseCHAR entName[WBPARSER_REAL_LEN(entName)];          ///< ��ҵĿ¼��
		WBParseCHAR entRealName[WBPARSER_REAL_LEN(entRealName)];  ///< ��ҵ��
		WBParseCHAR cn[WBPARSER_REAL_LEN(cn)];                    ///< RDN,Ψһ����
		WBParseCHAR sn[WBPARSER_REAL_LEN(sn)];                    ///< ��ʵ����,������
		WBParseCHAR enable[WBPARSER_REAL_LEN(enable)];            ///< ����/���ã�0/1
		WBParseCHAR privileged[WBPARSER_REAL_LEN(privileged)];    ///< ��Ȩ,��ʶ����Ա��0/1
		WBParseCHAR entMail[WBPARSER_REAL_LEN(entMail)];          ///< ��ҵ����,΢����¼�˺�
		WBParseCHAR subMail[WBPARSER_REAL_LEN(subMail)];          ///< ���������ַ,�ʼ�����
		WBParseCHAR e164[WBPARSER_REAL_LEN(e164)];                ///< E164��
		WBParseCHAR jid[WBPARSER_REAL_LEN(jid)];                  ///< xmpp�˺�
		WBParseCHAR mobileNum[WBPARSER_REAL_LEN(mobileNum)];      ///< ��ϵ�绰
		WBParseCHAR extNum[WBPARSER_REAL_LEN(extNum)];            ///< �ֻ�
		WBParseCHAR seat[WBPARSER_REAL_LEN(seat)];                ///< ��λ��
		WBParseCHAR brief[WBPARSER_REAL_LEN(brief)];              ///< һ�仰����
		WBParseCHAR position[WBPARSER_REAL_LEN(position)];        ///< ְλ
		WBParseCHAR portrait32[WBPARSER_REAL_LEN(portrait32)];    ///< 32λͷ��
		WBParseCHAR portrait40[WBPARSER_REAL_LEN(portrait40)];    ///< 40λͷ��
		WBParseCHAR portrait64[WBPARSER_REAL_LEN(portrait64)];    ///< 64λͷ��
		WBParseCHAR portrait128[WBPARSER_REAL_LEN(portrait128)];  ///< 128λͷ��
		WBParseCHAR portrait256[WBPARSER_REAL_LEN(portrait256)];  ///< 256λͷ��
		WBParseCHAR password[WBPARSER_REAL_LEN(password)];        ///< XMPP��½����

		WBParseCHAR enableWeibo[WBPARSER_REAL_LEN(enableWeibo)];     ///< ΢��Ȩ��(0/1,û��/��)
		WBParseCHAR enableMeeting[WBPARSER_REAL_LEN(enableMeeting)]; ///< �������Ȩ��(0/1,û��/��)
		WBParseCHAR enableHD[WBPARSER_REAL_LEN(enableHD)];           ///< ����Ȩ��(0/1,û��/��)
		WBParseCHAR enableCall[WBPARSER_REAL_LEN(enableCall)];       ///< �绰����Ȩ��(0/1,û��/��)
		WBParseCHAR enableRoam[WBPARSER_REAL_LEN(enableRoam)];       ///< ����Ȩ��(0/1,û��/��)
		WBParseCHAR weiboAdmin[WBPARSER_REAL_LEN(weiboAdmin)];       ///< ΢������ԱȨ��(0/1,û��/��)
		WBParseCHAR meetingAdmin[WBPARSER_REAL_LEN(meetingAdmin)];   ///< �������ԱȨ��(0/1,û��/��)
		WBParseCHAR isLikeQuery[WBPARSER_REAL_LEN(isLikeQuery)];     ///< �Ƿ�֧��ģ������(0:�����ԣ�1:����)
		WBParseCHAR authMode[WBPARSER_REAL_LEN(authMode)];           ///< �����ϵ����֤ģʽ(0:������֤��1:����֤��2:�ܾ�����)
		WBParseCHAR sex[WBPARSER_REAL_LEN(sex)];                     ///< �Ա�(1/0,��/Ů)

		t_wbParse_Keda_Depts *depts;               ///< ������Ϣ
	};

	struct t_wbParse_Keda_EntConfig
	{
		WBParseCHAR xmppDomain[WBPARSER_REAL_LEN(xmppDomain)];  ///< xmpp����
		WBParseCHAR apiUrl[WBPARSER_REAL_LEN(apiUrl)];          ///< ƽ̨API��ַ
		WBParseCHAR weiboUrl[WBPARSER_REAL_LEN(weiboUrl)];      ///< ΢��API��ַ
		WBParseCHAR webImUrl[WBPARSER_REAL_LEN(webImUrl)];      ///< web�ͻ������ӵ�Im��ַ
		WBParseCHAR tcpImUrl[WBPARSER_REAL_LEN(tcpImUrl)];      ///< �ն����ӵ�Im��ַ
		WBParseCHAR tcpPort[WBPARSER_REAL_LEN(tcpPort)];        ///< �ն����ӵ�Im�˿�
		WBParseCHAR ulsUrl[WBPARSER_REAL_LEN(ulsUrl)];          ///< ULS��ַ
		WBParseCHAR ulsPort[WBPARSER_REAL_LEN(ulsPort)];        ///< ULS�˿�
	};

	struct t_wbParse_Keda_Result
	{
		t_wbParse_Keda_EntUser *entUser;     ///< 
		t_wbParse_Keda_EntConfig *entConfig; ///< 
	};

	struct t_wbParse_Keda_EntUsers
	{
		WBParseINT entUserNum;
		t_wbParse_Keda_EntUser *entUser;     ///< 
	};

	struct t_wbParse_Keda_Page
	{
		WBParseINT totalCount;
		t_wbParse_Keda_EntUsers *entUsers;   ///< 
	};


// add end


// -------------------------------------

	// 2012-03-01 update by sunfei <users> ʹ�õ����ݽṹ

	struct t_wbParse_Keda_UserProfile
	{
		WBParseCHAR userId[WBPARSER_REAL_LEN(userId)];					///< �û�����id���硰124��
		WBParseCHAR jobTitle[WBPARSER_REAL_LEN(jobTitle)];	            ///< ְλ���硰��Эʦ��
		WBParseCHAR location[WBPARSER_REAL_LEN(location)];		        ///< λ�ã��硰3F-10L-04m��
		WBParseCHAR workPhone[WBPARSER_REAL_LEN(workPhone)];			///< �����绰���硰8484��
		WBParseCHAR mobilePhone[WBPARSER_REAL_LEN(mobilePhone)];		///< �ƶ��绰���硰130xxxx8137��

		WBParseINT birthYear;    ///< ��
		WBParseINT birthMonth;   ///< ��
		WBParseINT birthDay;     ///< ��
	};

	struct t_wbParse_Keda_OnlineState
	{
		WBParseCHAR wbOnline[WBPARSER_REAL_LEN(wbOnline)];		///< ΢������״̬
		WBParseCHAR tlOnline[WBPARSER_REAL_LEN(tlOnline)];	    ///< TrueLink����״̬
		WBParseCHAR tdOnline[WBPARSER_REAL_LEN(tdOnline)];		///< pad������״̬(ios/android)
		WBParseCHAR tpOnline[WBPARSER_REAL_LEN(tpOnline)];		///< phone������״̬(ios/android)
		WBParseCHAR tsOnline[WBPARSER_REAL_LEN(tsOnline)];		///< TrueSens����״̬
	};

	struct t_wbParse_Keda_Dept
	{
		WBParseINT departmentId;                                          ///< �硰51��
		WBParseCHAR departmentName[WBPARSER_REAL_LEN(departmentName)];    ///< ���������硰���������
		WBParseCHAR fullPath[WBPARSER_REAL_LEN(fullPath)];	              ///< �硰�ƴ�Ƽ�/�з�����/���������
		WBParseCHAR deptPosition[WBPARSER_REAL_LEN(deptPosition)];	      ///< �硰�������ʦ��
	};

	struct t_wbParse_Keda_Depts
	{
		WBParseINT deptNum;            ///< ������Ŀ��һ���˿���ͬʱ���ڶ�����ţ�
		t_wbParse_Keda_Dept *dept;     ///< ������ϸ��Ϣ
	};

	struct t_wbParse_Keda_User 
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< �û�����id���硰124��
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                       ///< ΢���ǳƣ��ƴﶼΪʵ�����硰��ɡ���
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];			           ///< �����ʼ����硰sunfei@kedacom.com��
		WBParseCHAR mugshot128[WBPARSER_REAL_LEN(mugshot128)];			   ///< ͷ��128���أ��硰photos/portrait/124/head_128.jpg��
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];			   ///< ͷ��64����
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];			   ///< ͷ��40����
		WBParseCHAR personalBriefly[WBPARSER_REAL_LEN(personalBriefly)];   ///< ����ǩ��

		WBParseINT followersNum;    ///< ��ע��Ŀ
		WBParseINT fansNum;         ///< ��˿��Ŀ
		WBParseINT friendsNum;      ///< ������Ŀ���������ע��Ŀ��
		WBParseINT threadNum;       ///< ������΢����Ŀ

		t_wbParse_Keda_UserProfile *profile; ///< �û��ĸ�����Ϣ

		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];            ///< �Ƿ񱻹�ע
		WBParseCHAR beenFollowBoth[WBPARSER_REAL_LEN(beenFollowBoth)];          ///< �Ƿ���ع�ע
		WBParseCHAR enableWeibo[WBPARSER_REAL_LEN(enableWeibo)];             ///< ΢���Ƿ����

		WBParseCHAR online[WBPARSER_REAL_LEN(online)];                     ///< �Ƿ�����
		WBParseCHAR defaultUid[WBPARSER_REAL_LEN(defaultUid)];	           ///< Ĭ���û�id��ͬuid
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                       ///< �û�ƴ��id���硰sunfei��
		WBParseCHAR e164[WBPARSER_REAL_LEN(e164)];	                       ///< E164
		WBParseCHAR jid[WBPARSER_REAL_LEN(jid)];	                       ///< jid

		t_wbParse_Keda_OnlineState *onlineState;   ///< ���ֵ�½�豸������״̬
		t_wbParse_Keda_Depts *depts;               ///< ������Ϣ
	};

	struct t_wbParse_Keda_Users_Data
	{
		WBParseINT userNum;
		t_wbParse_Keda_User *user; ///< ÿ���û�������
	};

	struct t_wbParse_Keda_Users 
	{
		WBParseINT pageSize;       ///< ÿҳ�ļ�¼��
		WBParseINT totalCount;     ///< �ܼ�¼��
		WBParseINT currentPage;    ///< ��ǰҳ��
		WBParseINT totalPage;      ///< ��ҳ��
		WBParseINT avaCount;       ///< ��ǰҳ����������

		t_wbParse_Keda_Users_Data *data; ///< �û��ľ�������
	};

	struct t_wbParse_Keda_Topic
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];                    ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	      ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	              ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	  ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	  ///< 
		WBParseCHAR feedCount[WBPARSER_REAL_LEN(feedCount)];      ///< 
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];                ///< 
	};

	struct t_wbParse_Keda_Like
	{
		WBParseCHAR userName[WBPARSER_REAL_LEN(userName)];       ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	             ///< 
		WBParseCHAR feedId[WBPARSER_REAL_LEN(feedId)];	         ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	     ///< 
	};

	struct t_wbParse_Keda_GroupStatistics
	{
		WBParseCHAR groupId[WBPARSER_REAL_LEN(groupId)];          ///< 
		WBParseINT feedNum;	                                      ///< 
		WBParseINT memberNum;	                                  ///< 
		WBParseCHAR groupUid[WBPARSER_REAL_LEN(groupUid)];        ///< 
		WBParseINT topicNum;	                                  ///< 
	};

	struct t_wbParse_Keda_Group
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];                      ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                ///< 
		WBParseCHAR groupBriefly[WBPARSER_REAL_LEN(groupBriefly)];	///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];        ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];        ///< 

		t_wbParse_Keda_GroupStatistics *groupStatistics;            ///< ������
	};

	struct t_wbParse_Keda_Groups_Data
	{
		WBParseINT groupNum;
		t_wbParse_Keda_Group *group; ///< ÿ���û���������
	};

	struct t_wbParse_Keda_Groups
	{
		WBParseINT pageSize;       ///< ÿҳ�ļ�¼��
		WBParseINT totalCount;     ///< �ܼ�¼��
		WBParseINT currentPage;    ///< ��ǰҳ��
		WBParseINT totalPage;      ///< ��ҳ��
		WBParseINT avaCount;       ///< ��ǰҳ����������

		t_wbParse_Keda_Groups_Data *data; ///< �û��ľ���������
	};

	struct t_wbParse_Keda_GroupCreate
	{
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                ///< 
		WBParseCHAR groupBriefly[WBPARSER_REAL_LEN(groupBriefly)];	///< 
	};

// add end



// 2012-03-02 add by sunfei ����˽���������

	struct t_wbParse_Keda_Sender
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	        ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	    ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	    ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];	///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_Receiver
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	        ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	    ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	    ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];	///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_PrivateFeed
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	                ///< 
		WBParseCHAR content[WBPARSER_REAL_LEN(content)];	    ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	///< 

		t_wbParse_Keda_Sender *sender;
		t_wbParse_Keda_Receiver *receiver;
	};

	struct t_wbParse_Keda_PrivateContacter
	{
		t_wbParse_Keda_PrivateFeed *privateFeed;    ///< ˽�ŵľ�������
		WBParseINT count;                           ///< ˽�ŵ�����
	};

	struct t_wbParse_Keda_PrivCont_Data
	{
		WBParseINT privContNum;                            ///< ˽�ŻỰ��Ŀ
		t_wbParse_Keda_PrivateContacter *privateContacter; ///< ����һ��˽�ŻỰ
	};

	struct t_wbParse_Keda_PrivateContacters
	{
		WBParseINT pageSize;       ///< ÿҳ�ļ�¼��
		WBParseINT totalCount;     ///< �ܼ�¼��
		WBParseINT currentPage;    ///< ��ǰҳ��
		WBParseINT totalPage;      ///< ��ҳ��
		WBParseINT avaCount;       ///< ��ǰҳ����������

		t_wbParse_Keda_PrivCont_Data *data; ///< �û��ľ���������
	};

	struct t_wbParse_Keda_PrivFeeds_Data
	{
		WBParseINT privateFeedNum;               ///< 
		t_wbParse_Keda_PrivateFeed *privateFeed; ///< 
	};

	struct t_wbParse_Keda_PrivateFeeds
	{
		WBParseINT pageSize;       ///< ÿҳ�ļ�¼��
		WBParseINT totalCount;     ///< �ܼ�¼��
		WBParseINT currentPage;    ///< ��ǰҳ��
		WBParseINT totalPage;      ///< ��ҳ��
		WBParseINT avaCount;       ///< ��ǰҳ����������

		t_wbParse_Keda_PrivFeeds_Data *data; ///< �û��ľ���������
	};


// add end


// 2012-03-16 update by sunfei <departments> ʹ�õ����ݽṹ

	struct t_wbParse_Keda_departmentUsers
	{
		WBParseINT userNum;
		t_wbParse_Keda_User *user; ///< 
	};

	struct t_wbParse_Keda_Department
	{
		WBParseINT  departmentId;                                       ///< 
		WBParseCHAR departmentName[WBPARSER_REAL_LEN(departmentName)];  ///< 
		WBParseCHAR fullPath[WBPARSER_REAL_LEN(fullPath)];              ///< 
		WBParseINT  parentId;                                           ///< 
		WBParseCHAR leaf[WBPARSER_REAL_LEN(leaf)];                      ///< 

		t_wbParse_Keda_departmentUsers *users;                ///< 
	};

	struct t_wbParse_Keda_Departments
	{
		WBParseINT departmentNum;              ///< 
		t_wbParse_Keda_Department *department; ///< 
	};

	struct t_wbParse_Keda_searchDepartment
	{
		WBParseINT totalNum;               ///< 
		t_wbParse_Keda_Departments *data;  ///< 
	};

	struct t_wbParse_Keda_searchUser
	{
		WBParseINT totalNum;              ///< 
		t_wbParse_Keda_Users_Data *data;  ///< 
	};


// add end





// 2012-01-16 add by sunfei    <ThreadMessage> ʹ�õ����ݽṹ

	// @@ <subject> �ṹ����

	struct t_wbParse_Keda_Id
	{
// 		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	                   ///< 2012-02-07 ��
		WBParseLONG id;	                                           ///< 
	};

	struct t_wbParse_Keda_ReferencedUserIds
	{
		WBParseINT referencedUserIdNum;
		t_wbParse_Keda_Id *referencedUserId;
	};

	struct t_wbParse_Keda_TopicIds
	{
		WBParseINT topicIdNum;
		t_wbParse_Keda_Id *topicId;
	};

	struct t_wbParse_Keda_LikeByIds
	{
		WBParseINT likeByIdNum;
		t_wbParse_Keda_Id *likeById;
	};

	struct t_wbParse_Keda_Picture
	{
		WBParseCHAR extensionName[WBPARSER_REAL_LEN(extensionName)];	         ///< 
		WBParseCHAR picturePreviewUrl[WBPARSER_REAL_LEN(picturePreviewUrl)];	 ///< 
		WBParseCHAR pictureTinyUrl[WBPARSER_REAL_LEN(pictureTinyUrl)];	         ///< 
		WBParseCHAR pictureMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///<
		WBParseCHAR pictureName[WBPARSER_REAL_LEN(pictureName)];	             ///< 
		WBParseCHAR pictureUrl[WBPARSER_REAL_LEN(pictureUrl)];	                 ///< 
        WBParseCHAR pictureTinyMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///<
        WBParseCHAR pictureLargeMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///< 
	};

	struct t_wbParse_Keda_Pictures
	{
		WBParseINT pictureNum;
		t_wbParse_Keda_Picture *picture;
	};


	struct t_wbParse_Keda_Attachment
	{
		WBParseCHAR extensionName[WBPARSER_REAL_LEN(extensionName)];	         ///< 
		WBParseCHAR fileName[WBPARSER_REAL_LEN(fileName)];	                     ///< 
		WBParseCHAR fileUrl[WBPARSER_REAL_LEN(fileUrl)];	                     ///< 
	};

	struct t_wbParse_Keda_Attachments
	{
		WBParseINT attachmentNum;
		t_wbParse_Keda_Attachment *attachment;
	};


	struct t_wbParse_Keda_Subject
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];	       ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR title[WBPARSER_REAL_LEN(title)];	                       ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
		WBParseCHAR replyNum[WBPARSER_REAL_LEN(replyNum)];	                   ///< 
		WBParseCHAR replyNoLoadNum[WBPARSER_REAL_LEN(replyNoLoadNum)];	       ///< 

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(createLikeTime)];	       ///< 
	};

	// @@ <subject> end


	// @@ <replies> �ṹ����

	struct t_wbParse_Keda_FeedMessage
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR repliedToUserId[WBPARSER_REAL_LEN(repliedToUserId)];	   ///<
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(createLikeTime)];	                   ///< 
	};

	struct t_wbParse_Keda_Replies
	{
		WBParseINT feedMessNum;
		t_wbParse_Keda_FeedMessage *feedMessage;
	};

	// @@ <replies> end



	// 2012-02-08 add by sunfei

	struct t_wbParse_Keda_RefReply
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
// 		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
	};

	struct t_wbParse_Keda_Reply
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
// 		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
	};

	// add end


	struct t_wbParse_Keda_SubjectStyleThread
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< 
		WBParseCHAR lastFeedId[WBPARSER_REAL_LEN(lastFeedId)];	           ///< 

		t_wbParse_Keda_Subject *subject;
		t_wbParse_Keda_Replies *replies;
	};

	struct t_wbParse_Keda_ReplyStyleThread
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< 
		WBParseCHAR lastFeedId[WBPARSER_REAL_LEN(lastFeedId)];	           ///< 

		t_wbParse_Keda_Subject *subject;
		t_wbParse_Keda_RefReply *refReply;
		t_wbParse_Keda_Reply *reply;

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(datetime)];
	};

	struct t_wbParse_Keda_ThreadMessage
	{
		WBParseINT subjectStyleThreadNum;
		t_wbParse_Keda_SubjectStyleThread *subjectStyleThread;

		WBParseINT replyStyleThreadNum;
		t_wbParse_Keda_ReplyStyleThread *replyStyleThread;
	};

	// @@ <ThreadMessage> end



	struct t_wbParse_Keda_Meta
	{
		WBParseCHAR readFeedId[WBPARSER_REAL_LEN(readFeedId)];			///< ���ص���Ϣ�����µ���ϢID
		WBParseCHAR oldestFeedId[WBPARSER_REAL_LEN(oldestFeedId)];	    ///< ���ص���Ϣ�������������ظ�ID
		WBParseCHAR totalCount[WBPARSER_REAL_LEN(totalCount)];		    ///< �ܵ�����
	};


	// 2012-01-16 add by sunfei    <relatedData> ʹ�õ����ݽṹ

	struct t_wbParse_Keda_UserData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];			        ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];			    ///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_GroupData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];			        ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
	};

	struct t_wbParse_Keda_TopicData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
	};

	struct t_wbParse_Keda_UserEntry
	{
		WBParseCHAR userId[WBPARSER_REAL_LEN(userId)];			    ///< 
		t_wbParse_Keda_UserData *userData;
	};

	struct t_wbParse_Keda_GroupEntry
	{
		WBParseCHAR groupId[WBPARSER_REAL_LEN(groupId)];			///< 
		t_wbParse_Keda_GroupData *groupData;
	};

	struct t_wbParse_Keda_TopicEntry
	{
		WBParseCHAR topicId[WBPARSER_REAL_LEN(topicId)];			///< 
		t_wbParse_Keda_TopicData *topicData;
	};

	struct t_wbParse_Keda_RelatedUsers
	{
		WBParseINT userEntryNum;
		t_wbParse_Keda_UserEntry *userEntry;
	};

	struct t_wbParse_Keda_RelatedGroups
	{
		WBParseINT groupEntryNum;
		t_wbParse_Keda_GroupEntry *groupEntry;

	};

	struct t_wbParse_Keda_RelatedTopics
	{
		WBParseINT topicEntryNum;
		t_wbParse_Keda_TopicEntry *topicEntry;

	};

	struct t_wbParse_Keda_RelatedData
	{
		t_wbParse_Keda_RelatedUsers *relatedUsers;		///< 
		t_wbParse_Keda_RelatedGroups *relatedGroups;	///< 
		t_wbParse_Keda_RelatedTopics *relatedTopics;	///< 
	};

	// <relatedData> end


	struct t_wbParse_Keda_Messages
	{
		t_wbParse_Keda_Meta *meta;				                        ///< 
		t_wbParse_Keda_ThreadMessage *threadMessage;	                ///< 
		t_wbParse_Keda_RelatedData *relatedData;		                ///< 
	};



	// 2012-05-08 add by sunfei ���ڡ���ȡ@���ޡ���˿�ȳ�ʼ��Ϣ��API

	struct t_wbParse_Keda_Notify
	{
		WBParseINT amount;
		WBParseCHAR messageType[WBPARSER_REAL_LEN(messageType)];			///< 
	};

	struct t_wbParse_Keda_Notifies
	{
		WBParseINT notifyNum;
		t_wbParse_Keda_Notify *notify;				                        ///< 
	};


// 2012-09-18 add by sunfei ��ȡ�޹��ҵ�΢������

	struct t_wbParse_Keda_Messages_Data
	{
		t_wbParse_Keda_Messages *messages;
	};

	struct t_wbParse_Keda_Like_Page
	{
		WBParseINT pageSize;       ///< ÿҳ�ļ�¼��
		WBParseINT totalCount;     ///< �ܼ�¼��
		WBParseINT currentPage;    ///< ��ǰҳ��
		WBParseINT totalPage;      ///< ��ҳ��
		WBParseINT avaCount;       ///< ��ǰҳ����������

		t_wbParse_Keda_Messages_Data *data; ///< ��������
	};

// add end


// -------------------------------------


	/** ���� */
	struct t_wbParse_Error 
	{
		t_wbParse_Header ph_; ///< ����ͷ

		WBParseCHAR request[WBPARSER_REAL_LEN(request)];
		WBParseCHAR error[WBPARSER_REAL_LEN(error)];            ///< ��������
		WBParseCHAR error_code[WBPARSER_REAL_LEN(error_code)];  ///< ������
		unsigned int error_sub_code; ///< �Ӵ�����Ϣ

        unsigned int params[4];                                // ��������Ϣ
	};

	/** Ϊ��ý����� */
	struct t_wbParse_media_item
	{
		t_wbParse_Header ph_; ///< ����ͷ

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			 ///<mediaid
		WBParseCHAR url[WBPARSER_REAL_LEN(url)];		 ///<media- url
		WBParseCHAR screen[WBPARSER_REAL_LEN(url)];		 ///<media- screen
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];		 ///<media- type:webpage,video,audio,event,magic,vote
		WBParseCHAR title[WBPARSER_REAL_LEN(name)];		 ///<media- title
		WBParseCHAR flash[WBPARSER_REAL_LEN(url)];		 ///<media- flash
		//
		struct MediaExternItem
		{
			// extern information
			WBParseCHAR ext_from[32]; 
			WBParseCHAR ext_shorturl[WBPARSER_REAL_LEN(url)];	 ///<media- shorturl
			WBParseCHAR ext_description[WBPARSER_REAL_LEN(text)];///<media- description
			WBParseCHAR ext_lastmodified[ WBPARSER_REAL_LEN(created_time) ];///<media- lastmodify

			// extern - music 
			WBParseCHAR ext_audio_artist[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_album[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_appkey[256];

			// extern - event
			WBParseCHAR ext_event_id[WBPARSER_REAL_LEN(id)];

			// extern - vote
			WBParseCHAR ext_vote_uid[WBPARSER_REAL_LEN(id)];

		} media_ext;
	};

	struct t_wbParse_Media
	{
		t_wbParse_Header ph_; ///< ����ͷ

		int cmd;
		WBParseCHAR parentid[WBPARSER_REAL_LEN(id)];///<��΢��ID����������΢���ĸ�΢��id)
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///< ��ǰ΢��ID

		t_wbParse_media_item mitem;
	};


#ifdef _MSC_VER
	const __int64 llMAX = 0xFFFFFFFFFFFFFFFFi64;
#else
	const long long  llMAX = 0xFFFFFFFFFFFFFFFFLL;
#endif

	/** �򵥵� */
	struct t_wbParser_MMId
	{
		enum eType
		{
			EType_Unknow = -1,

			/** �ҵ����� */
			EType_CommentsToMe = 0,

			/** �ҷ���ȥ�� */
			EType_CommentsByMe,

			/** �ҷ����յ� */
			EType_CommentsTimeline,

			/** @�ҵ� */
			EType_AtMe,

			/** ��ҳ */
			EType_Homes,

			/** ˽�� */
			EType_DirectMsgs,

			/** ��˿*/
			EType_Follower,

			EType_End,
		};
		struct IdValue
		{

#ifdef _MSC_VER
			__int64 maxId;
			__int64 minId;
#else
			long long maxId;
			long long minId;
#endif

		};

		struct IdValue _idVal[EType_End];
	};



    struct t_wbParse_m_short_room
    {
        WBParseINT     id;
        WBParseCHAR    name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR    regionName[WBPARSER_REAL_LEN(regionName)];
    };

    struct t_wbParse_m_short_rooms
    {
        WBParseINT                          count;
        struct t_wbParse_m_short_room *     items;
    };

    struct t_wbParse_m_room_ids
    {
        WBParseINT                       count;
        WBParseINT *                     items;
    };

    struct t_wbParse_m_short_meeting
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  organizerMoid[WBPARSER_REAL_LEN(moId)];
        WBParseINT   status;
        WBParseINT   isVideoMeeting;
        WBParseINT   feedbackStatus;
        WBParseINT   regularId;

        struct t_wbParse_m_room_ids      room_ids;
    };

    struct t_wbParse_m_short_meetings
    {
        WBParseINT                           count;
        struct t_wbParse_m_short_meeting *   items;
    };


    struct t_wbParse_m_meeting_list
    {
        struct  t_wbParse_m_short_rooms      related_rooms;
        struct  t_wbParse_m_short_meetings   meetings;
    };



    struct t_wbParse_m_person
    {
        WBParseCHAR            moId[WBPARSER_REAL_LEN(moId)];
        WBParseCHAR            email[WBPARSER_REAL_LEN(email)];
        WBParseCHAR            name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR            participantNo[WBPARSER_REAL_LEN(participantNo)];

        WBParseINT             isParticipant;
        WBParseINT             participantType;
    };

    struct t_wbParse_m_persons
    {
        WBParseINT                           count;
        struct t_wbParse_m_person *          items;
    };


    struct  t_wbParse_m_vMeeting
    {
        WBParseCHAR         e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR         templateId[WBPARSER_REAL_LEN(template_id)];
        WBParseCHAR         templateName[WBPARSER_REAL_LEN(template_name)];
        WBParseCHAR         password[WBPARSER_REAL_LEN(common)];
        WBParseINT          shortNO;
        WBParseINT          state;
        WBParseINT          isAutoCall;
    };



    struct t_wbParse_m_meeting_info
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   status;


        WBParseCHAR  organizerMoid[WBPARSER_REAL_LEN(moId)];
        WBParseCHAR  organizer[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organizerMail[WBPARSER_REAL_LEN(organizerMail)];
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobile[WBPARSER_REAL_LEN(mobile)];
        WBParseCHAR  brief[WBPARSER_REAL_LEN(m_brief)];

        WBParseINT   isVideo;
        // WBParseCHAR  template_id[WBPARSER_REAL_LEN(template_id)];
        // WBParseCHAR  template_name[WBPARSER_REAL_LEN(template_name)];
        // WBParseINT   isAutoCall;
        // WBParseCHAR  e164[WBPARSER_REAL_LEN(e164)];

        t_wbParse_m_vMeeting    vMeeting;

        struct t_wbParse_m_short_rooms  rooms;
        struct t_wbParse_m_persons      participants;
    };

    struct t_wbParse_m_meeting_ids
    {
        WBParseINT                       count;
        WBParseINT *                     items;
    };

    struct t_wbParse_m_regular_info
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   status;


        WBParseCHAR  organizer[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organizerMail[WBPARSER_REAL_LEN(organizerMail)];
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobile[WBPARSER_REAL_LEN(mobile)];
        WBParseCHAR  brief[WBPARSER_REAL_LEN(m_brief)];

        WBParseINT   isVideo;
        // WBParseCHAR  template_id[WBPARSER_REAL_LEN(template_id)];
        // WBParseCHAR  template_name[WBPARSER_REAL_LEN(template_name)];
        // WBParseINT   isAutoCall;

        // struct t_wbParse_m_short_rooms  rooms;
        // struct t_wbParse_m_persons      participants;

        WBParseCHAR  startDate[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endDate[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   repeatType;
        WBParseINT   frequency;
        WBParseCHAR  weekDays[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_m_meeting_ids      meeting_ids;
    };



    struct t_wbParse_m_room
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  regionName[WBPARSER_REAL_LEN(regionName)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   peopleAdmit;
        WBParseINT   deviceMask;
    };

    struct t_wbParse_m_rooms
    {
        WBParseINT                    count;
        struct t_wbParse_m_room *     items;
    };

    struct t_wbParse_m_room_list
    {
        t_wbParse_m_rooms             rooms;
        WBParseINT                    total;
    };

    struct t_wbParse_m_normal_room_list
    {
        int                                 start;
        int                                 count;

        struct t_wbParse_m_short_rooms      rooms;
    };

    struct t_wbParse_m_room_info
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseINT   peopleAdmit;
        WBParseINT   area;
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseINT   hasVideo;
        WBParseCHAR  e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR  deviceAdmin[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  adminPhone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  remark[WBPARSER_REAL_LEN(remark)];
        WBParseINT   deviceMask;
    };

    struct t_wbParse_m_room_status
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseINT   status;
    };


    //struct t_wbParse_m_common_item
    //{
    //    WBParseCHAR   id[WBPARSER_REAL_LEN(id)];
    //    WBParseCHAR   name[WBPARSER_REAL_LEN(name)];
    //};

    //struct t_wbParse_m_common_items
    //{
    //    WBParseINT                        count;
    //    struct t_wbParse_m_common_item *  items;
    //};


    struct t_wbParse_m_template_info
    {
        WBParseCHAR  id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  resolution[WBPARSER_REAL_LEN(common)];
        WBParseCHAR  bandwith[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_m_persons    persons;
    };

    struct t_wbParse_m_template
    {
        WBParseCHAR   id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR   name[WBPARSER_REAL_LEN(name)];
    };

    struct t_wbParse_m_templates
    {
        WBParseINT                        count;
        struct t_wbParse_m_template *     items;
    };

    struct t_wbParse_m_templateList
    {
        t_wbParse_m_templates             templates;
    };


    // ע�⣺ content�����ݱȽϸ��ӣ� ����messgaeType���仯
    struct t_wbParse_m_notify
    {
        WBParseINT     id;
        WBParseCHAR    receiverMoId[WBPARSER_REAL_LEN(moId)];
        WBParseINT     messgaeType;
        WBParseCHAR    companyMoId[WBPARSER_REAL_LEN(companyMoId)];
        WBParseCHAR    createTime[WBPARSER_REAL_LEN(datetime)];
        void *         content;
    };

    struct t_wbParse_m_notifys
    {
        WBParseINT                         count;
        struct t_wbParse_m_notify *        items;
    };




    // content
    // ������messgaeTypeΪ102, 103
    struct t_wbParse_m_notify_content_meeting
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   isVideoMeeting;
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobilePhone[WBPARSER_REAL_LEN(mobile)];

        WBParseCHAR  organiger[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organigerMoid[WBPARSER_REAL_LEN(moId)];

        WBParseCHAR  beforeSubject[WBPARSER_REAL_LEN(subject)];

        struct t_wbParse_m_short_rooms      rooms;
    };


    // ������messgaeTypeΪ104��108��109��115��116��117��118��119
    struct t_wbParse_m_notify_content_meeting_id
    {
        WBParseINT                      meetingId;

        WBParseCHAR                     subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR                     startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     endTime[WBPARSER_REAL_LEN(datetime)];

        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];
    };

    // ������messageTypeΪ105��106,107
    struct t_wbParse_m_notify_content_regular_id
    {
        WBParseINT                      regularId;

        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];
    };

    // ������messageTypeΪ110
    struct t_wbParse_m_notify_content_cancel_room
    {
        WBParseINT                       meetingId;

        WBParseCHAR                     subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR                     startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];

        struct t_wbParse_m_short_rooms   rooms;
    };

    // ������messageTypeΪ111
    struct t_wbParse_m_notify_content_deadline_changed
    {
        WBParseINT                       headline;
    };

    // ������messageTypeΪ112
    struct t_wbParse_m_notify_content_user_feedback
    {
        WBParseINT                       meetingId;
        struct t_wbParse_m_person        participant;

        WBParseINT                       reasonId;
        WBParseINT                       isVideoMeeting;
    };

    // ������messageTypeΪ113, 114
    struct t_wbParse_m_notify_content_room_changed
    {
        WBParseINT                       roomId;
    };


    struct t_wbParse_app_notify
    {
        WBParseINT                       amount;
        WBParseCHAR                      messageType[WBPARSER_REAL_LEN(common)];
    };


    struct t_wbParse_app_notifys
    {
        WBParseINT                          count;
        struct t_wbParse_app_notify *       items;
    };



    struct t_wbParse_monitor_device
    {
        WBParseCHAR                         id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR                         deviceId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         deviceGUID[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR                         deviceType[WBPARSER_REAL_LEN(common)];
        WBParseCHAR                         description[WBPARSER_REAL_LEN(remark)];
    };

    struct t_wbParse_monitor_devices
    {
        WBParseINT                          count;
        struct t_wbParse_monitor_device *   items;
    };

    struct t_wbParse_monitor_group
    {
        WBParseCHAR                         id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         groupId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         groupName[WBPARSER_REAL_LEN(name)];
        WBParseCHAR                         parentId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         newGroup[WBPARSER_REAL_LEN(common)];
        WBParseCHAR                         leaf[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_monitor_devices    devices;
    };

    struct t_wbParse_monitor_groups
    {
        WBParseINT                          count;
        struct t_wbParse_monitor_group *   items;
    };
}

#endif //__WB_PARSE_STRUCT_H__
