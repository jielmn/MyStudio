
#if (defined(WIN32) || defined(_WIN32) ) && !defined(TINYXML_STATICLIB)

	#ifdef TINYXML_VC90_EXPORTS
		#define TINYXML_VC90_API __declspec(dllexport)
	#else
		#define TINYXML_VC90_API __declspec(dllimport)
	#endif

#else  // ����ƽ̨�ģ������Ǿ�̬����

	#define TINYXML_VC90_API 

#endif

