#ifndef __lo_HTTP_CURL_H__
#define __lo_HTTP_CURL_H__

#include <string>
#include "loHttpStruct.h"
#include "libhttp/loHttpDef.h"

namespace lohttp
{
	/** �ص����� */
	struct loNovtableM IloTaskCallBack
	{
		virtual void OnTaskState(long lState) = 0;  		                 /** ����״̬ */
		virtual int  OnTaskProgress(long lCurBytes, long lTotalBytes) = 0;   /** ���ؽ��� */
		virtual int  OnTaskBuffer(unsigned char* pBuffer, long lBuffer) = 0; /** ���ػ�����(���㿪�������ļ� @return !=0 ��ʾ�������buffer���ǿ�����Ҫ��)*/
		virtual void OnTaskCode(long code, const char* errString) = 0;       /** ������ @param code = enHTTPCode */
		virtual void OnTaskTime(const THttpTime* pDownloadTime) = 0;         /** ��������ִ��ʱ�� */
	};

	/** ǰ���������� */
	class CloHttpCurlAssoc;

	/** ���� libcurl */
	class CloHttpCurl 
	{
	public:
		CloHttpCurl(IloTaskCallBack* pTaskCallback);
		~CloHttpCurl(void);

	public:
		long GetTaskID() const;
		BOOL IsTaskValid();

		int	 Request();
		void CancelRequest();
		int	Step();		// Managerר�ú�������

		void SetProxyInfo(THttpProxyInfo &ProxyInfo);
		int	SetTransferParam(int nType, LPVOID lpParam);
		void SetUserAgent(const char* agent);

		int	GetType() const;
		int	GetState() const;
		int	GetErrCode() const;
		long GetHttpCode() const;

		int	GetResponedHeader(THttpMemoryBuffer * pHeaderBuf);
		int	GetResponedBody(THttpMemoryBuffer * pBodyBuf);

	protected:
		BOOL IsUserCancel() const;

		int	 TransferProc();

		int	 TransferProcGET();
		int	 TransferProcPOST();
		int	 TransferProcPOSTFormData();
//		int  TransferProcPOSTFormData2();
		int	 TransferProcDELETE();
		int	 TransferProcPUT();
//		int	 TransferProcUPLOAD();

		int	 TransferProcDOWNLOAD();           // ���ص��ļ�
		int	 TransferProcDOWNLOAD_TOBUFFER();  // ���ص�BUFFER

		void SetTransferCallback( LPVOID headerData, LPVOID bodyData );

	private:
		void SetState(int nNewState);

		void TaskState(int nState);
		int TaskProgress(long lCurBytes, long lTotalBytes);
		int TaskBuffer(unsigned char* pBuffer, long lLength);
        void TaskCode(void);

	private:
		void ClearBuffer();
		void ClearParam();

		int	 StartTransfer();
		void StopTransfer();

		void UpdateProxy(char* lpszRequestURL);
		const char* GetUserAgent() const;

	private:
		static size_t ToBodyCallback  ( void *ptr, size_t size, size_t nmemb, void *data );
		static size_t ToHeaderCallback( void *ptr, size_t size, size_t nmemb, void *data );
		static size_t ToFileCallback  ( void *ptr, size_t size, size_t nmemb, void *data );
		static size_t ToBufferCallback( void *ptr, size_t size, size_t nmemb, void *data );
		static size_t ProgressCallback( void *clientp, double dltotal, double dlnow, double ultotal, double ulnow );

	protected:
		friend class CloHttpCurlAssoc;
		CloHttpCurlAssoc* m_pCurlAssoc;

    public:
        void *            m_pContext;
	};

}

#endif /*__lo_HTTP_CURL_H__ */
