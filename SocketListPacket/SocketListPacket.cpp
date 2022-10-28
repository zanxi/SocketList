// SocketListPacket.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "ServerDBrw.h"
#include "ConcurrentMap.h"

const int socketN = 100;
const int tMilliseconds = 2;
const int nThreads = 2;
RWLockDBServer m_lock;
mutex _mtx;
//std::list<int> socketList;
contfree_safe_ptr< std::list<int> > safe_socketList;


void task_delete_odd(contfree_safe_ptr< std::list<int> > safe_socketList)
{
	//srand((unsigned int)hash<thread::id>()(this_thread::get_id()));	// ensures that all threads have a different seed for the random number generator
	//m_lock.lockW();
	
	while (!safe_socketList->empty()) {
		
		  if (safe_socketList->size() < 1)
			  break;		
		_mtx.lock();
		  list<int>::iterator it_begin = safe_socketList->begin();
		  if (it_begin == safe_socketList->end())continue;
		  std::advance(it_begin, 0); // взяли 1-й элемент						
		  cout << "{ odd : " << *it_begin << "}" <<" |";
		  it_begin = safe_socketList->erase(it_begin);

		  //cout << "{ удаляем нечетные биты : " << *(safe_socketList->begin()) << "}" << endl;
		  //cout << "{ нечет : " << *it_begin << "}" << endl;
		
		 this_thread::sleep_for(chrono::milliseconds(tMilliseconds));
	    _mtx.unlock();
	}
	
	//m_lock.unlockW();
};

void task_delete_even(contfree_safe_ptr< std::list<int> > safe_socketList)
{	
	while (!safe_socketList->empty()) {
		
		  if (safe_socketList->size() < 2)
			  break;
		 _mtx.lock();
		  list<int>::iterator it_begin = safe_socketList->begin();
		  if (it_begin == safe_socketList->end())continue;
		  std::advance(it_begin, 1); // взяли 2-й элемент						
		  cout << "{ even : " << *it_begin << "}" << " |";
		  it_begin = safe_socketList->erase(it_begin);		

		  //cout << "{ удаляем четные биты : " << *(safe_socketList->begin()) << "}" << endl;		  
		
		  this_thread::sleep_for(chrono::milliseconds(tMilliseconds));
		 _mtx.unlock();
	}
	
	//m_lock.unlockW();
};



int main()
{			
    std::cout << "Socket list!\n";
	srand((unsigned int)10000);
	
	for (int i = 0; i < socketN; i++)safe_socketList->push_back(rand()%255);
	
	for (list<int>::iterator it = safe_socketList->begin(); 
		it!= safe_socketList->end();
		it++)
	{
		cout << " | " << *it;
	}
	cout<< "\n" << endl;

	mutex mtx;							// synchronized access to standard output cout	
	thread t[nThreads];					// thread pool	
		
	cout << "main thread id = " << this_thread::get_id() << ", hw concurrency = " << thread::hardware_concurrency() << endl;

	t[0] = thread(task_delete_odd, safe_socketList);	// thread uses move-semantics: t[i] will run the task and the temporary thread will immediately finish	
	t[1] = thread(task_delete_even, safe_socketList);
	cout << "Delete even & odd bits" << endl;
	while (!safe_socketList->empty())
	{
		//if (safe_socketList->size() != 0)cout << "[" << safe_socketList->size() << " : " << *(safe_socketList->begin()) << "]" << endl;
		this_thread::sleep_for(chrono::milliseconds(tMilliseconds));
	}


	t[0].join();
	t[1].join();

	cout << "\n\nPress key!!!!!!!!!!\n" << endl;

	/*for (auto& v : map_acc)
	{
		std::cout << "[" << v.first << "; " << v.second << "]" << endl;
	}*/
		
	system("pause");

}
