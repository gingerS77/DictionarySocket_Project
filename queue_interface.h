#include <pthread.h>
#include <iostream>
#include <queue>

template<typename T, int size>    /// size is max elements it can contain
class Queue
{
	pthread_mutex_t m_buffer_lock;
	pthread_cond_t m_buffer_cond;
	std::queue<T> m_queue;

public:
	Queue(){
		pthread_mutex_init(&m_buffer_lock,NULL);
		pthread_cond_init(&m_buffer_cond,NULL);
		m_queue;
	}

	void Push(T elem){
		pthread_mutex_lock(&m_buffer_lock);
		while(size == m_queue.size())
			pthread_cond_wait(&m_buffer_cond,&m_buffer_lock);
		m_queue.push(elem);
		if(m_queue.size() == 1)
			//pthread_cond_signal(&m_buffer_cond);
			pthread_cond_broadcast(&m_buffer_cond);
		pthread_mutex_unlock(&m_buffer_lock);
	}

	const T&  Pop(){
		pthread_mutex_lock(&m_buffer_lock);
		while(m_queue.empty())
			pthread_cond_wait(&m_buffer_cond,&m_buffer_lock);
		T &temp = m_queue.front();
		m_queue.pop();
		if(m_queue.size() == size-1)
			//pthread_cond_signal(&m_buffer_cond);
			pthread_cond_broadcast(&m_buffer_cond);
		pthread_mutex_unlock(&m_buffer_lock);
		return temp;
	}


	
};