#include <stdlib.h>

#include "wbParser/wbParseStruct.h"
#include "wbParser/wbParseMalloc.h"


#define PARSE_STRUCT_MALLOC(p,size) \
	p = malloc(size);\
	if ( p != NULL )\
	{\
		memset(p,0,size);\
		((t_wbParse_Header*)p)->eSource = wbParserNS::EPS_OPENAPI;\
	}

#define PARSE_STRUCT_MALLOC_NORMAL(p,size)\
	p = malloc(size);\
	if ( p != NULL )\
	{\
		memset(p,0,size);\
	}

#define PARSE_STRUCT_FREE(p)\
	if ( p != NULL )\
	{\
		free(p);\
	}

#define BEGIN_STRCUT_FREE(type,p,size)\
		type *pstruct = (type*)p;\
		if( !p ) return;\
		for( int i = 0; i < size; i ++ ){\
			type *pItem = (pstruct + i);


#define END_STRUCT_FREE(p) } PARSE_STRUCT_FREE(p);


namespace wbParserNS
{
	// cursor的接口Android需要使用
	EXTERN void *wbParse_Malloc_Cursor(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Cursor)*size);
		return p;
	}
	EXTERN void wbParse_Free_Cursor(void *pvoid,int size)
	{
		PARSE_STRUCT_FREE(pvoid);
	}

	EXTERN void *wbParse_Malloc_Keda_Notify(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Notify)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Notify(void *pvoid,int size)
	{
		t_wbParse_Keda_Notify *p = (t_wbParse_Keda_Notify*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Notify *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Notifies(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Notifies)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Notifies(void *pvoid,int size)
	{
		t_wbParse_Keda_Notifies *p = (t_wbParse_Keda_Notifies*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Notifies *item = (p+i);
				if( item ->notify ) {
					wbParse_Free_Keda_Notify(item ->notify,item ->notifyNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



//--------------------  2012-03-16 add by sunfei v1.0接口 ----------------------------


	EXTERN void *wbParse_Malloc_Keda_DepartmentUsers(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_departmentUsers)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_DepartmentUsers(void *pvoid,int size)
	{
		t_wbParse_Keda_departmentUsers *p = (t_wbParse_Keda_departmentUsers*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_departmentUsers *item = (p+i);
				if( item ->user ) {
					wbParse_Free_Keda_User(item ->user,item ->userNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Department(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Department)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Department(void *pvoid,int size)
	{
		t_wbParse_Keda_Department *p = (t_wbParse_Keda_Department*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Department *item = (p+i);
				if( item ->users ) {
					wbParse_Free_Keda_DepartmentUsers(item ->users,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Departments(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Departments)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Departments(void *pvoid,int size)
	{
		t_wbParse_Keda_Departments *p = (t_wbParse_Keda_Departments*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Departments *item = (p+i);
				if( item ->department ) {
					wbParse_Free_Keda_Department(item ->department,item ->departmentNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_SearchDepartment(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_searchDepartment)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_SearchDepartment(void *pvoid,int size)
	{
		t_wbParse_Keda_searchDepartment *p = (t_wbParse_Keda_searchDepartment*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_searchDepartment *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_Departments(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_SearchUser(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_searchUser)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_SearchUser(void *pvoid,int size)
	{
		t_wbParse_Keda_searchUser *p = (t_wbParse_Keda_searchUser*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_searchUser *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_Users_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

//--------------------  2012-09-18 add by sunfei v2.5.1接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_Messages_Data(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Messages_Data)*size);
		return p;
	}
	EXTERN void wbParse_Free_Keda_Messages_Data(void *pvoid,int size)
	{
		t_wbParse_Keda_Messages_Data *p = (t_wbParse_Keda_Messages_Data*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Messages_Data *item = (p+i);
				if( item ->messages ) {
					wbParse_Free_Keda_Messages(item ->messages,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_LikePage(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Like_Page)*size);
		return p;
	}
	EXTERN void wbParse_Free_Keda_LikePage(void *pvoid,int size)
	{
		t_wbParse_Keda_Like_Page *p = (t_wbParse_Keda_Like_Page*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Like_Page *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_Messages_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


//--------------------  2012-03-07 add by sunfei v1.0接口 ----------------------------


	EXTERN void *wbParse_Malloc_Keda_EntUser(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_EntUser)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_EntUser(void *pvoid,int size)
	{
		t_wbParse_Keda_EntUser *p = (t_wbParse_Keda_EntUser*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_EntUser *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_EntConfig(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_EntConfig)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_EntConfig(void *pvoid,int size)
	{
		t_wbParse_Keda_EntConfig *p = (t_wbParse_Keda_EntConfig*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_EntConfig *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_Result(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Result)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Result(void *pvoid,int size)
	{
		t_wbParse_Keda_Result *p = (t_wbParse_Keda_Result*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Result *item = (p+i);
				if( item ->entUser ) {
					wbParse_Free_Keda_EntUser(item ->entUser,1);
				}
				if( item ->entConfig ) {
					wbParse_Free_Keda_EntConfig(item ->entConfig,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_EntUsers(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_EntUsers)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_EntUsers(void *pvoid,int size)
	{
		t_wbParse_Keda_EntUsers *p = (t_wbParse_Keda_EntUsers*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_EntUsers *item = (p+i);
				if( item ->entUser ) {
					wbParse_Free_Keda_EntUser(item ->entUser,item ->entUserNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_Page(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Page)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Page(void *pvoid,int size)
	{
		t_wbParse_Keda_Page *p = (t_wbParse_Keda_Page*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Page *item = (p+i);
				if( item ->entUsers ) {
					wbParse_Free_Keda_EntUsers(item ->entUsers,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


//--------------------  2012-02-29 add by sunfei v1.0接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_Users_Data(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Users_Data)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Users_Data(void *pvoid,int size)
	{
		t_wbParse_Keda_Users_Data *p = (t_wbParse_Keda_Users_Data*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Users_Data *item = (p+i);
				if( item ->user ) {
					wbParse_Free_Keda_User(item ->user,item->userNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_Users(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Users)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Users(void *pvoid,int size)
	{
		t_wbParse_Keda_Users *p = (t_wbParse_Keda_Users*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Users *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_Users_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_Topic(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Topic)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Topic(void *pvoid,int size)
	{
		t_wbParse_Keda_Topic *p = (t_wbParse_Keda_Topic*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Topic *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Like(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Like)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Like(void *pvoid,int size)
	{
		t_wbParse_Keda_Like *p = (t_wbParse_Keda_Like*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Like *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_GroupStatistics(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_GroupStatistics)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_GroupStatistics(void *pvoid,int size)
	{
		t_wbParse_Keda_GroupStatistics *p = (t_wbParse_Keda_GroupStatistics*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_GroupStatistics *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Group(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Group)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Group(void *pvoid,int size)
	{
		t_wbParse_Keda_Group *p = (t_wbParse_Keda_Group*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Group *item = (p+i);
				if( item ->groupStatistics ) {
					wbParse_Free_Keda_GroupStatistics(item ->groupStatistics,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Groups_Data(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Groups_Data)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Groups_Data(void *pvoid,int size)
	{
		t_wbParse_Keda_Groups_Data *p = (t_wbParse_Keda_Groups_Data*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Groups_Data *item = (p+i);
				if( item ->group ) {
					wbParse_Free_Keda_Group(item ->group,item ->groupNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_Groups(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Groups)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Groups(void *pvoid,int size)
	{
		t_wbParse_Keda_Groups *p = (t_wbParse_Keda_Groups*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Groups *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_Groups_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_GroupCreate(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_GroupCreate)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_GroupCreate(void *pvoid,int size)
	{
		t_wbParse_Keda_GroupCreate *p = (t_wbParse_Keda_GroupCreate*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_GroupCreate *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}




	// 2012-03-02 add by sunfei

	EXTERN void *wbParse_Malloc_Keda_Sender(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Sender)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Sender(void *pvoid,int size)
	{
		t_wbParse_Keda_Sender *p = (t_wbParse_Keda_Sender*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Sender *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Receiver(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Receiver)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Receiver(void *pvoid,int size)
	{
		t_wbParse_Keda_Receiver *p = (t_wbParse_Keda_Receiver*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Receiver *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_PrivateFeed(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivateFeed)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivateFeed(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivateFeed *p = (t_wbParse_Keda_PrivateFeed*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivateFeed *item = (p+i);
				if( item ->sender ) {
					wbParse_Free_Keda_Sender(item ->sender,1);
				}
				if( item ->receiver ) {
					wbParse_Free_Keda_Receiver(item ->receiver,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_PrivateContacter(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivateContacter)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivateContacter(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivateContacter *p = (t_wbParse_Keda_PrivateContacter*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivateContacter *item = (p+i);
				if( item ->privateFeed ) {
					wbParse_Free_Keda_PrivateFeed(item ->privateFeed,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_PrivCont_Data(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivCont_Data)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivCont_Data(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivCont_Data *p = (t_wbParse_Keda_PrivCont_Data*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivCont_Data *item = (p+i);
				if( item ->privateContacter ) {
					wbParse_Free_Keda_PrivateContacter(item ->privateContacter,item->privContNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_PrivateContacters(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivateContacters)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivateContacters(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivateContacters *p = (t_wbParse_Keda_PrivateContacters*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivateContacters *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_PrivCont_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_PrivFeeds_Data(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivFeeds_Data)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivFeeds_Data(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivFeeds_Data *p = (t_wbParse_Keda_PrivFeeds_Data*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivFeeds_Data *item = (p+i);
				if( item ->privateFeed ) {
					wbParse_Free_Keda_PrivateFeed(item ->privateFeed,item ->privateFeedNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_PrivateFeeds(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_PrivateFeeds)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_PrivateFeeds(void *pvoid,int size)
	{
		t_wbParse_Keda_PrivateFeeds *p = (t_wbParse_Keda_PrivateFeeds*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_PrivateFeeds *item = (p+i);
				if( item ->data ) {
					wbParse_Free_Keda_PrivFeeds_Data(item ->data,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	// add end

//-----------------------------------  end  ------------------------------------------


//--------------------  2012-01-09 add by sunfei v0.1接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_UserProfile(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_UserProfile)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_UserProfile(void *pvoid,int size)
	{
		t_wbParse_Keda_UserProfile *p = (t_wbParse_Keda_UserProfile*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_UserProfile *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_OnlineState(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_OnlineState)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_OnlineState(void *pvoid,int size)
	{
		t_wbParse_Keda_OnlineState *p = (t_wbParse_Keda_OnlineState*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_OnlineState *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Dept(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Dept)*size);
		return p;
	}

	EXTERN void  wbParse_Free_Keda_Dept(void *pvoid,int size)
	{
		t_wbParse_Keda_Dept *p = (t_wbParse_Keda_Dept*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Dept *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Depts(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Depts)*size);
		return p;
	}

	EXTERN void  wbParse_Free_Keda_Depts(void *pvoid,int size)
	{
		t_wbParse_Keda_Depts *p = (t_wbParse_Keda_Depts*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Depts *item = (p+i);
				if( item ->dept ) {
					wbParse_Free_Keda_OnlineState(item ->dept,item->deptNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_User(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_User)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_User(void *pvoid,int size)
	{
		t_wbParse_Keda_User *p = (t_wbParse_Keda_User*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_User *item = (p+i);
				if( item ->profile ) {
					wbParse_Free_Keda_UserProfile(item ->profile,1);
				}
				if( item ->onlineState ) {
					wbParse_Free_Keda_OnlineState(item ->onlineState,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Meta(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Meta)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Meta(void *pvoid,int size)
	{
		t_wbParse_Keda_Meta *p = (t_wbParse_Keda_Meta*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Meta *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}




	// 2012-01-16 add by sunfei 

	EXTERN void *wbParse_Malloc_Keda_UserData(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_UserData)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_UserData(void *pvoid,int size)
	{
		t_wbParse_Keda_UserData *p = (t_wbParse_Keda_UserData*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_UserData *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_UserEntry(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_UserEntry)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_UserEntry(void *pvoid,int size)
	{
		t_wbParse_Keda_UserEntry *p = (t_wbParse_Keda_UserEntry*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_UserEntry *item = (p+i);
				if( item ->userData ) {
					wbParse_Free_Keda_UserData(item ->userData,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_RelatedUsers(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_RelatedUsers)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_RelatedUsers(void *pvoid,int size)
	{
		t_wbParse_Keda_RelatedUsers *p = (t_wbParse_Keda_RelatedUsers*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_RelatedUsers *item = (p+i);
				if( item ->userEntry ) {
					wbParse_Free_Keda_UserEntry(item ->userEntry,item ->userEntryNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_GroupData(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_GroupData)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_GroupData(void *pvoid,int size)
	{
		t_wbParse_Keda_GroupData *p = (t_wbParse_Keda_GroupData*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_GroupData *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_GroupEntry(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_GroupEntry)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_GroupEntry(void *pvoid,int size)
	{
		t_wbParse_Keda_GroupEntry *p = (t_wbParse_Keda_GroupEntry*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_GroupEntry *item = (p+i);
				if( item ->groupData ) {
					wbParse_Free_Keda_GroupData(item ->groupData,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_RelatedGroups(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_RelatedGroups)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_RelatedGroups(void *pvoid,int size)
	{
		t_wbParse_Keda_RelatedGroups *p = (t_wbParse_Keda_RelatedGroups*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_RelatedGroups *item = (p+i);
				if( item ->groupEntry ) {
					wbParse_Free_Keda_GroupEntry(item ->groupEntry,item ->groupEntryNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_TopicData(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_TopicData)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_TopicData(void *pvoid,int size)
	{
		t_wbParse_Keda_TopicData *p = (t_wbParse_Keda_TopicData*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_TopicData *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_TopicEntry(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_TopicEntry)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_TopicEntry(void *pvoid,int size)
	{
		t_wbParse_Keda_TopicEntry *p = (t_wbParse_Keda_TopicEntry*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_TopicEntry *item = (p+i);
				if( item ->topicData ) {
					wbParse_Free_Keda_TopicData(item ->topicData,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_RelatedTopics(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_RelatedTopics)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_RelatedTopics(void *pvoid,int size)
	{
		t_wbParse_Keda_RelatedTopics *p = (t_wbParse_Keda_RelatedTopics*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_RelatedTopics *item = (p+i);
				if( item ->topicEntry ) {
					wbParse_Free_Keda_TopicEntry(item ->topicEntry,item ->topicEntryNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_RelatedData(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_RelatedData)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_RelatedData(void *pvoid,int size)
	{
		t_wbParse_Keda_RelatedData *p = (t_wbParse_Keda_RelatedData*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_RelatedData *item = (p+i);
				if( item ->relatedUsers ) {
					wbParse_Free_Keda_RelatedUsers(item ->relatedUsers,1);
				}
				if( item ->relatedGroups ) {
					wbParse_Free_Keda_RelatedGroups(item ->relatedGroups,1);
				}
				if( item ->relatedTopics ) {
					wbParse_Free_Keda_RelatedTopics(item ->relatedTopics,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

//-----------------------------------  end  ------------------------------------------



	EXTERN void *wbParse_Malloc_Keda_FeedMessage(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_FeedMessage)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_FeedMessage(void *pvoid,int size)
	{
		t_wbParse_Keda_FeedMessage *p = (t_wbParse_Keda_FeedMessage*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_FeedMessage *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}







	// 2012-02-06 add by sunfei

	EXTERN void *wbParse_Malloc_Keda_Id(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Id)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Id(void *pvoid,int size)
	{
		t_wbParse_Keda_Id *p = (t_wbParse_Keda_Id*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Id *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_ReferencedUserIds(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_ReferencedUserIds)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_ReferencedUserIds(void *pvoid,int size)
	{
		t_wbParse_Keda_ReferencedUserIds *p = (t_wbParse_Keda_ReferencedUserIds*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_ReferencedUserIds *item = (p+i);
				if( item ->referencedUserId ) {
					wbParse_Free_Keda_Id(item ->referencedUserId, item->referencedUserIdNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_TopicIds(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_TopicIds)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_TopicIds(void *pvoid,int size)
	{
		t_wbParse_Keda_TopicIds *p = (t_wbParse_Keda_TopicIds*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_TopicIds *item = (p+i);
				if( item ->topicId ) {
					wbParse_Free_Keda_Id(item ->topicId, item->topicIdNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_LikeByIds(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_LikeByIds)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_LikeByIds(void *pvoid,int size)
	{
		t_wbParse_Keda_LikeByIds *p = (t_wbParse_Keda_LikeByIds*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_LikeByIds *item = (p+i);
				if( item ->likeById ) {
					wbParse_Free_Keda_Id(item ->likeById, item->likeByIdNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Picture(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Picture)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Picture(void *pvoid,int size)
	{
		t_wbParse_Keda_Picture *p = (t_wbParse_Keda_Picture*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Picture *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_Pictures(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Pictures)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Pictures(void *pvoid,int size)
	{
		t_wbParse_Keda_Pictures *p = (t_wbParse_Keda_Pictures*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Pictures *item = (p+i);
				if( item ->picture ) {
					wbParse_Free_Keda_Picture(item ->picture, item->pictureNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}




	EXTERN void *wbParse_Malloc_Keda_Attachment(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Attachment)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Attachment(void *pvoid,int size)
	{
		t_wbParse_Keda_Attachment *p = (t_wbParse_Keda_Attachment*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Attachment *item = (p+i);
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Attachments(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Attachments)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Attachments(void *pvoid,int size)
	{
		t_wbParse_Keda_Attachments *p = (t_wbParse_Keda_Attachments*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Attachments *item = (p+i);
				if( item ->attachment ) {
					wbParse_Free_Keda_Attachment(item ->attachment, item->attachmentNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	// add end


	EXTERN void *wbParse_Malloc_Keda_Subject(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Subject)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Subject(void *pvoid,int size)
	{
		t_wbParse_Keda_Subject *p = (t_wbParse_Keda_Subject*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Subject *item = (p+i);

				// 2012-02-06 add by sunfei
				if( item ->referencedUserIds ) {
					wbParse_Free_Keda_ReferencedUserIds(item ->referencedUserIds,1);
				}
				if( item ->topicIds ) {
					wbParse_Free_Keda_TopicIds(item ->topicIds,1);
				}
				if( item ->likeByIds ) {
					wbParse_Free_Keda_LikeByIds(item ->likeByIds,1);
				}
				if( item ->pictures ) {
					wbParse_Free_Keda_Pictures(item ->pictures,1);
				}
				if( item ->attachments ) {
					wbParse_Free_Keda_Attachments(item ->attachments,1);
				}
				// add end

				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_RefReply(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_RefReply)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_RefReply(void *pvoid,int size)
	{
		t_wbParse_Keda_RefReply *p = (t_wbParse_Keda_RefReply*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_RefReply *item = (p+i);

				// 2012-02-06 add by sunfei
				if( item ->referencedUserIds ) {
					wbParse_Free_Keda_ReferencedUserIds(item ->referencedUserIds,1);
				}
				if( item ->topicIds ) {
					wbParse_Free_Keda_TopicIds(item ->topicIds,1);
				}
				if( item ->likeByIds ) {
					wbParse_Free_Keda_LikeByIds(item ->likeByIds,1);
				}
				if( item ->pictures ) {
					wbParse_Free_Keda_Pictures(item ->pictures,1);
				}
				if( item ->attachments ) {
					wbParse_Free_Keda_Attachments(item ->attachments,1);
				}
				// add end

				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}

	EXTERN void *wbParse_Malloc_Keda_Reply(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Reply)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Reply(void *pvoid,int size)
	{
		t_wbParse_Keda_Reply *p = (t_wbParse_Keda_Reply*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Reply *item = (p+i);

				// 2012-02-06 add by sunfei
				if( item ->referencedUserIds ) {
					wbParse_Free_Keda_ReferencedUserIds(item ->referencedUserIds,1);
				}
				if( item ->topicIds ) {
					wbParse_Free_Keda_TopicIds(item ->topicIds,1);
				}
				if( item ->likeByIds ) {
					wbParse_Free_Keda_LikeByIds(item ->likeByIds,1);
				}
				if( item ->pictures ) {
					wbParse_Free_Keda_Pictures(item ->pictures,1);
				}
				if( item ->attachments ) {
					wbParse_Free_Keda_Attachments(item ->attachments,1);
				}
				// add end

				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Replies(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Replies)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Replies(void *pvoid,int size)
	{
		t_wbParse_Keda_Replies *p = (t_wbParse_Keda_Replies*)pvoid;
		if( p )
		{
			int i = 0;
			int num = 0;
			do
			{
				t_wbParse_Keda_Replies *item = (p+i);

				if( item ->feedMessage ) {
					wbParse_Free_Keda_FeedMessage(item ->feedMessage,item ->feedMessNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_SubjectStyleThread(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_SubjectStyleThread)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_SubjectStyleThread(void *pvoid,int size)
	{
		t_wbParse_Keda_SubjectStyleThread *p = (t_wbParse_Keda_SubjectStyleThread*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_SubjectStyleThread *item = (p+i);
				if( item ->subject ) {
					wbParse_Free_Keda_Subject(item ->subject,1);
				}
				if( item ->replies ) {
					wbParse_Free_Keda_Replies(item ->replies,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_ReplyStyleThread(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_ReplyStyleThread)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_ReplyStyleThread(void *pvoid,int size)
	{
		t_wbParse_Keda_ReplyStyleThread *p = (t_wbParse_Keda_ReplyStyleThread*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_ReplyStyleThread *item = (p+i);
				if( item ->subject ) {
					wbParse_Free_Keda_Subject(item ->subject,1);
				}
				if( item ->refReply ) {
					wbParse_Free_Keda_RefReply(item ->refReply,1);
				}
				if( item ->reply ) {
					wbParse_Free_Keda_Reply(item ->reply,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}



	EXTERN void *wbParse_Malloc_Keda_ThreadMessage(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_ThreadMessage)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_ThreadMessage(void *pvoid,int size)
	{
		t_wbParse_Keda_ThreadMessage *p = (t_wbParse_Keda_ThreadMessage*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_ThreadMessage *item = (p+i);

				if( item ->subjectStyleThread ) {
					wbParse_Free_Keda_SubjectStyleThread(item ->subjectStyleThread,item ->subjectStyleThreadNum);
				}
				if( item ->replyStyleThread ) {
					wbParse_Free_Keda_ReplyStyleThread(item ->replyStyleThread,item ->replyStyleThreadNum);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


	EXTERN void *wbParse_Malloc_Keda_Messages(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Keda_Messages)*size);
		return p;
	}

	EXTERN void wbParse_Free_Keda_Messages(void *pvoid,int size)
	{
		t_wbParse_Keda_Messages *p = (t_wbParse_Keda_Messages*)pvoid;
		if( p )
		{
			int i = 0;
			do
			{
				t_wbParse_Keda_Messages *item = (p+i);
				if( item ->meta ) {
					wbParse_Free_Keda_Meta(item ->meta,1);
				}
				if( item ->threadMessage ) {
					wbParse_Free_Keda_ThreadMessage(item ->threadMessage,1);
				}
				if( item ->relatedData ) {
					wbParse_Free_Keda_RelatedData(item ->relatedData,1);
				}
				i++;
			}
			while(i<size);
			//
			PARSE_STRUCT_FREE(pvoid);
		}
	}


// add end

// ------------------------------

	// error 
	EXTERN void* wbParse_Malloc_Error(int size)
	{
		void *p = NULL;
		PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_Error)*size);
		return p;
	}
	EXTERN void wbParse_Free_Error(void* pvoid,int size)
	{
		PARSE_STRUCT_FREE(pvoid);
	}




    EXTERN void *wbParse_Malloc_MeetingList(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_meeting_list)*size);
        return p;
    }

    EXTERN void wbParse_Free_MeetingList(void *pvoid,int size)
    {
        t_wbParse_m_meeting_list *p = (t_wbParse_m_meeting_list*)pvoid;
        if( p )
        {
            if ( p->related_rooms.items )
            {
                PARSE_STRUCT_FREE(p->related_rooms.items);
            }

            if ( p->meetings.items )
            {
                PARSE_STRUCT_FREE(p->meetings.items);
            }

            PARSE_STRUCT_FREE(p);
        }
    }

    EXTERN void *wbParse_Malloc_MeetingInfo(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_meeting_info)*size);
        return p;
    }

    EXTERN void  wbParse_Free_MeetingInfo(void *pvoid,int size)
    {
        t_wbParse_m_meeting_info *p = (t_wbParse_m_meeting_info*)pvoid;
        if ( p )
        {
            if ( p->rooms.items )
            {
                PARSE_STRUCT_FREE(p->rooms.items);
            }

            if ( p->participants.items )
            {
                PARSE_STRUCT_FREE(p->participants.items);
            }

            PARSE_STRUCT_FREE(p);
        }
    }



    EXTERN void *wbParse_Malloc_Meeting_RoomList(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_room_list)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_RoomList(void *pvoid,int size)
    {
        t_wbParse_m_room_list *p = (t_wbParse_m_room_list*)pvoid;
        if ( p )
        {
            if ( p->rooms.items )
            {
                PARSE_STRUCT_FREE(p->rooms.items);
            }
            PARSE_STRUCT_FREE(pvoid);
        }
    }



    EXTERN void *wbParse_Malloc_Meeting_NormalRoomList(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_normal_room_list)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_NormalRoomList(void *pvoid,int size)
    {
        t_wbParse_m_normal_room_list *p = (t_wbParse_m_normal_room_list*)pvoid;
        if( p )
        {
            if ( p->rooms.items )
            {
                PARSE_STRUCT_FREE( p->rooms.items );
            }
            PARSE_STRUCT_FREE(pvoid);
        }
    }

    EXTERN void *wbParse_Malloc_RoomInfo(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_room_info)*size);
        return p;
    }

    EXTERN void  wbParse_Free_RoomInfo(void *pvoid,int size)
    {
        t_wbParse_m_room_info *p = (t_wbParse_m_room_info*)pvoid;
        if( p )
        {
            PARSE_STRUCT_FREE(pvoid);
        }
    }

    EXTERN void *wbParse_Malloc_RoomStatus(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_room_status)*size);
        return p;
    }

    EXTERN void  wbParse_Free_RoomStatus(void *pvoid,int size)
    {
        t_wbParse_m_room_status *p = (t_wbParse_m_room_status*)pvoid;
        if( p )
        {
            PARSE_STRUCT_FREE(p);
        }
    }

    //EXTERN void *wbParse_Malloc_Meeting_CommonItems(int size)
    //{
    //    void *p = NULL;
    //    PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_common_items)*size);
    //    return p;
    //}

    //EXTERN void wbParse_Free_Meeting_CommonItems(void *pvoid,int size)
    //{
    //    t_wbParse_m_common_items *p = (t_wbParse_m_common_items*)pvoid;
    //    if( p )
    //    {
    //        if ( p->items )
    //        {
    //            PARSE_STRUCT_FREE(p->items);
    //        }
    //        PARSE_STRUCT_FREE(pvoid);
    //    }
    //}

    EXTERN void *wbParse_Malloc_Meeting_TemplateList(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_templateList)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_TemplateList(void *pvoid,int size)
    {
        t_wbParse_m_templateList *p = (t_wbParse_m_templateList*)pvoid;
        if( p )
        {
            if ( p->templates.items )
            {
                PARSE_STRUCT_FREE( p->templates.items );
            }
            PARSE_STRUCT_FREE(pvoid);
        }
    }


    EXTERN void *wbParse_Malloc_Meeting_TemplateInfo(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_template_info)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_TemplateInfo(void *pvoid,int size)
    {
        t_wbParse_m_template_info *p = (t_wbParse_m_template_info*)pvoid;
        if( p )
        {
            if ( p->persons.items )
            {
                PARSE_STRUCT_FREE( p->persons.items );
            }
            PARSE_STRUCT_FREE(pvoid);
        }
    }


    EXTERN void *wbParse_Malloc_Meeting_ListNotifyByMax(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_notifys)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_ListNotifyByMax(void *pvoid,int size)
    {
        t_wbParse_m_notifys *p = (t_wbParse_m_notifys*)pvoid;
        if( p )
        {
            if ( p->items )
            {
                WBParseINT i;
                for ( i = 0; i < p->count; i++ )
                {
                    if ( p->items[i].content )
                    {
                        PARSE_STRUCT_FREE( p->items[i].content );
                    }
                }

                PARSE_STRUCT_FREE(p->items);
            }

            PARSE_STRUCT_FREE(p);
        }
    }

    EXTERN void *wbParse_Malloc_Meeting_Notify(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_notify)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_Notify(void *pvoid,int size)
    {
        t_wbParse_m_notify *p = (t_wbParse_m_notify*)pvoid;
        if( p )
        {
            if ( p->content )
            {
                PARSE_STRUCT_FREE( p->content );
            }
            PARSE_STRUCT_FREE(p);
        }
    }

    EXTERN void *wbParse_Malloc_Meeting_Regular(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_m_regular_info)*size);
        return p;
    }

    EXTERN void wbParse_Free_Meeting_Regular(void *pvoid,int size)
    {
        t_wbParse_m_regular_info *p = (t_wbParse_m_regular_info*)pvoid;
        if( p )
        {
            if ( p->meeting_ids.items )
            {
                PARSE_STRUCT_FREE( p->meeting_ids.items );
            }
            PARSE_STRUCT_FREE(p);
        }
    }



    EXTERN void *wbParse_Malloc_App_GetAllNotifys(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_app_notifys)*size);
        return p;
    }

    EXTERN void wbParse_Free_App_GetAllNotifys(void *pvoid,int size)
    {
        t_wbParse_app_notifys *p = (t_wbParse_app_notifys*)pvoid;
        if( p )
        {
            if ( p->items )
            {
                PARSE_STRUCT_FREE( p->items );
            }
            PARSE_STRUCT_FREE(p);
        }
    }


    EXTERN void *wbParse_Malloc_Monitor_Group(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_monitor_group)*size);
        return p;
    }

    EXTERN void wbParse_Free_Monitor_Group(void *pvoid,int size)
    {
        t_wbParse_monitor_group *p = (t_wbParse_monitor_group*)pvoid;
        if( p )
        {
            if ( p->devices.items )
            {
                PARSE_STRUCT_FREE( p->devices.items );
            }
            PARSE_STRUCT_FREE(p);
        }
    }


    EXTERN void *wbParse_Malloc_Monitor_Groups(int size)
    {
        void *p = NULL;
        PARSE_STRUCT_MALLOC(p,sizeof(t_wbParse_monitor_groups)*size);
        return p;
    }

    EXTERN void wbParse_Free_Monitor_Groups(void *pvoid,int size)
    {
        t_wbParse_monitor_groups *p = (t_wbParse_monitor_groups*)pvoid;
        if( p )
        {
            if ( p->items )
            {
                WBParseINT  i;
                for ( i = 0; i < p->count; i++ )
                {
                    if ( p->items[i].devices.items )
                    {
                        PARSE_STRUCT_FREE(p->items[i].devices.items);
                    }
                }

                PARSE_STRUCT_FREE(p->items);
            }

            PARSE_STRUCT_FREE(p);
        }
    }
}
