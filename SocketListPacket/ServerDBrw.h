#pragma once

#include "RWLockDBServer.h"
#include "libh.h"


class ServerDBrw {
	mutable RWLockDBServer m_lock;	// mutable: can be modified even in const methods
	double m_RecordDB = 0;	// Database accountDB balance

public:
	void UpdateRecordDB(double amount) {
		m_lock.lockW();
		m_RecordDB += amount;

		//std::cout << "\nUpdateRecordDB [" << m_RecordDB << "]\n" << endl;

		this_thread::sleep_for(chrono::milliseconds(90));
		m_lock.unlockW();
	}

	double getBalance() const {
		m_lock.lockR();
		this_thread::sleep_for(chrono::milliseconds(50));
		m_lock.unlockR();
		return m_RecordDB;
	}

	size_t getReaders() const {
		return m_lock.getReaders();
	}
};

void ex23()
{
	std::cout << "Пример чтения и записи в DataBase !!!!\n";

	const int nThreads = 8;
	const int nRuns = 8;

	mutex mtx;							// synchronized access to standard output cout
	ServerDBrw accountDB;				// synchronized Database accountDB
	double unsynchronizedaccountDB = 18;	// unsychronized Database accountDB
	thread t[nThreads];					// thread pool
	//std::map<int, int> *map_acc = new map<int,int>;
	std::map<int, int> map_acc;

	// parallel task
	auto task = [nRuns, &accountDB, &mtx, &unsynchronizedaccountDB, &map_acc] {
		srand((unsigned int)hash<thread::id>()(this_thread::get_id()));	// ensures that all threads have a different seed for the random number generator

		for (int i = 0; i < nRuns; i++) {
			if (i % 2) {
				const double amount = rand() % 10;// rand() * 1000 / RAND_MAX;
				const double b = unsynchronizedaccountDB + amount;
				accountDB.UpdateRecordDB(amount);
				//unsynchronizedaccountDB = b;
				mtx.lock();
				cout << "thread " << this_thread::get_id() << " UpdateRecord DataBase " << amount << endl;
				if (!(map_acc.find(amount) != map_acc.end()))map_acc.insert(pair<int, int>(amount, b));
				else
				{
					map_acc[amount] = b;
				}

				mtx.unlock();
			}
			const double balance = accountDB.getBalance();
			mtx.lock();
			//cout << "thread " << this_thread::get_id() << ": balance is = " << balance << ", unsychronized balance is = " << unsynchronizedaccountDB << endl;
			cout << "getReaders DataBase: [" << accountDB.getReaders() << "]" << endl;

			mtx.unlock();
		}
	};

	//map_acc->insert(pair<int,int>(1,1));

	cout << "main thread id = " << this_thread::get_id() << ", hw concurrency = " << thread::hardware_concurrency() << endl;

	// start threads
	for (int i = 0; i < nThreads; i++) {
		t[i] = thread(task);	// thread uses move-semantics: t[i] will run the task and the temporary thread will immediately finish
	}

	// join threads: main thread waits for parallel threads
	for (int i = 0; i < nThreads; i++) {
		//cout << "wait until thread " << t[i].get_id() << " has finished" << endl;
		t[i].join();
	}

	cout << "Press key!!!!!!!!!!\n" << endl;

	for (auto& v : map_acc)
	{
		std::cout << "[" << v.first << "; " << v.second << "]" << endl;
	}

	cout << "Press key!!!!!!!!!!" << endl;
	system("pause");
}