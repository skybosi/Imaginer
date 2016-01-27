#ifndef _BASETHREAD_H_
#define _BASETHREAD_H_
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include "Mdef.h"
#include <pthread.h>
using namespace std;
class Basethread
{
	private:
		pthread_t pth;
		bool autoDelete_;
		bool _detach;
		pthread_attr_t attr;
		static void *runhere(void *args);
	public:
		Basethread(bool detach);
		Basethread();
		void SetAutoDelete(bool autoDelete);
		virtual ~Basethread();
		/*{
			if(_detach)
			{
				pthread_attr_destroy(&attr);
				cout << "destroy the detach state..." << endl;
			}
			cout << "Basethread will dead!" << endl;
		}*/
		bool start();
		void join();//等待线程结束并且收回被等待线程的资源
		virtual int run() = 0;
		pthread_t gethreadid();
		bool getdetachstate();
};

#endif
// Basethread.h
