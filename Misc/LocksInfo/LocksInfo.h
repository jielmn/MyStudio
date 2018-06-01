#ifndef  __LOCK_INFO__
#define  __LOCK_INFO__

enum  ELockState
{
    LOCK_STATE_NONE,
    LOCK_STATE_TRY_TO_LOCK,
    LOCK_STATE_LOCKED,
    LOCK_STATE_TRY_TO_UNLOCK,
    LOCK_STATE_UNLOCKED,
};

void   SET_LOCK_STATE ( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState );
void   PRINT_ALL_LOCK_INFO();










#endif