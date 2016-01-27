#ifndef _LMUTEX_H_
#define _LMUTEX_H_

#include <iostream>
#include <string>
#include "Mdef.h"
#ifdef _LUNIX
	#include <pthread.h>
	#include <unistd.h>
	#include <cstdlib>
	#include <signal.h>
#else
	#include <windows.h>
#endif
enum lockway
{
	ATOMIC,
	MUTEXS,
};
using namespace std;
class lmutex
{
	private:
#ifdef _LUNIX
		static int lock;
		static int unlock;
		int symbol_synx;
		pthread_mutex_t     mutex;
		pthread_mutexattr_t     mutexattr;
#else
		volatile long m_lock;
		CRITICAL_SECTION  mutex;
#endif

	public:
		lmutex();
		~lmutex()
		{
			//cout << "delete the lock" << endl;
#ifdef _LUNIX
		//	if(mutex != NULL && mutexattr != NULL)
			{
				pthread_mutexattr_destroy(&mutexattr);
				pthread_mutex_destroy(&mutex);
			}
#else
			::DeleteCriticalSection(&mutex);
#endif
		}
		void setlock(lockway chose = ATOMIC);
		void setunlock(lockway chose = ATOMIC);
};

#endif
// lmutex.h
