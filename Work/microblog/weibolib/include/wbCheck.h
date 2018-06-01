/**
*  @brief Check the option is validate
*  @file  wbCheck.h
*/

#ifndef __wb_CHECK_H__
#define __wb_CHECK_H__


typedef enum
{
	Err_OK =0,
	Err_WB_ID,
	Err_USE_ID,
	Err_TEXT_ID,
	Err_IMAGE_ID,
	Err_WBCOM_ID,
	Err_PWD_ID,
	Err_OAUTHVERIFY_ID,
	Err_UIDTYPE_ID,

	// 
	Err_KEYWORD, // keyword null
	Err_TREND_ID,
	Err_TAGS_ID,

	// register
	Err_REG_NICK,	// nick name
	Err_REG_GENDER, // gender
	Err_REG_MAIL,	// email
	Err_REG_PWD,	// password
	Err_REG_IP,		// city id

	// tgt
	Err_UPDATETGT_SUE,
	Err_UPDATETGT_SUP,
	Err_UPDATETGT_VERIFY_TGT,

	// invite mail
	Err_MAIL_ID, // mail ID
	Err_MAIL_PWD,// mail password
	Err_MAIL_TYPE,// mail type
	Err_MAIL_LIST,// mail list
	Err_MAIL_NICKNAME,// mail nick name

	Err_AUTH_MODE,

}WBCheckerror;

/**
 * @brief �������ṹ�б�������ֵ�Ƿ�Ϸ���Ч
 * 
 * @param option �ο� enum WEIBOoption in weibo.h
 * @param *t_wb ���Ӧ option�Ľṹֵ����  WEIBO_OPTION(GETSTATUSES_PUBLIC_TIMELINE)---> t_wb_get_statuses_public_timeline
 *
 * @return WBCheckerrorֵ ��Err_OK ��ʾ�ɹ�������ֵ ʧ��
*/
int Weibo_check_valid(int option , const void* t_wb);

#endif
