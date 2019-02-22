// study.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <deque>
#include <list>
#include <chrono>
#include <random>
#include <future>
#include <queue>


//1. template singleton study
/*
template Singleton using std::mutex and double check
*/
template<typename T, typename ...Args>
class Singleton_Mutex_DoubleCheck
{
public:
	static std::shared_ptr<T> getInstance(Args...args)
	{
		if (!m_instance)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!m_instance)
			{
				m_instance.reset(new T(args...));
			}
		}
		return m_instance;
	}

protected:
		Singleton_Mutex_DoubleCheck(){}
public:
		Singleton_Mutex_DoubleCheck(const Singleton_Mutex_DoubleCheck&) = delete;
		Singleton_Mutex_DoubleCheck& operator=(const Singleton_Mutex_DoubleCheck&) = delete;
		Singleton_Mutex_DoubleCheck(Singleton_Mutex_DoubleCheck&&) = delete;
		Singleton_Mutex_DoubleCheck& operator=(Singleton_Mutex_DoubleCheck&&) = delete;

private:
	static std::shared_ptr<T> m_instance = nullptr;
	static std::mutex m_mutex;
};

template<typename T, typename ...Args> std::shared_ptr<T> Singleton_Mutex_DoubleCheck<T, Args...>::m_instance;
template<typename T, typename ...Args> std::mutex Singleton_Mutex_DoubleCheck<T, Args...>::m_mutex;

/*
template Singleton using std::call_once and std::once_flag
*/
template<typename T, typename ...Args>
	class Singleton_Call_Once
{
public:
	static std::shared_ptr<T> getInstance(Args... args)
	{
		std::call_once(m_init_flag, createInstance, args...);
		return m_Instance;
	}

protected:
	Singleton_Call_Once(){}

public:
	Singleton_Call_Once(const Singleton_Call_Once&) = delete;
	Singleton_Call_Once& operator=(const Singleton_Call_Once&) = delete;
	Singleton_Call_Once(Singleton_Call_Once&&) = delete;
	Singleton_Call_Once& operator=(Singleton_Call_Once&&) = delete;

	static void createInstance(Args... args)
	{
		m_Instance.reset(new T(args...));
	}

private:
	static std::shared_ptr<T> m_Instance = nullptr;
	static std::once_flag m_init_flag;
};

template<typename T, typename ...Args>
std::shared_ptr<T> Singleton_Call_Once<T, Args...>::m_Instance;
template<typename T, typename ...Args>
std::once_flag Singleton_Call_Once<T, Args...>::m_init_flag;


/*
template Singleton with static local variable
*/
template<typename T, typename ...Args>
class Singleton_Static
{
public:
	static T& getInstance(Args... args)
	{
		static T t(args...);
		return t;
	}

protected:
	Singleton_Static(){}

public:
	virtual ~Singleton_Static(){}
	Singleton_Static(const Singleton_Static&) = delete;
	Singleton_Static& operator=(const Singleton_Static&) = delete;
	Singleton_Static(Singleton_Static&&) = delete;
	Singleton_Static& operator=(Singleton_Static&&) = delete;
};

class IPerson
{
public:
	virtual void DoWork() = 0;
};

class Worker :public IPerson, public Singleton_Static<Worker>
{
public:
	void DoWork()
	{
		std::cout << "hello Woker:" << this << std::endl;
	}

};

class Farmer :public IPerson, public Singleton_Static<Farmer>
{
public:
	explicit Farmer(int id)
		:m_id(id)
	{}
	void DoWork()
	{
		std::cout << "hello Farmer: id " << m_id << ", " << this << std::endl;
	}

private:
	int m_id;
};

//2. thread_safe queue version 1
template<typename T>
class Thread_safe_queue
{
private:
	std::deque<T> m_deque;
	mutable std::mutex m_mutex;
	std::condition_variable m_cond_variable;

public:
	Thread_safe_queue(){}
	Thread_safe_queue(const Thread_safe_queue& rhs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_deque = rhs.m_deque;
	}
	Thread_safe_queue& operator=(const Thread_safe_queue&) = delete;
	Thread_safe_queue(Thread_safe_queue&&)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_deque = rhs.m_deque;
	}
	Thread_safe_queue& operator=(Thread_safe_queue&&) = delete;

	void push(const T& x)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_deque.push_back(x);
		m_cond_variable.notify_one();
	}
	void push(T&& x)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_deque.push_back(x);
		m_cond_variable.notify_one();
	}

	bool try_pop(T& x)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_deque.empty())
		{
			x = m_deque.pop_front();
			return true;
		}
		return false;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_deque.empty())
		{
			return std::make_shared<T>(m_deque.pop_front());
		}
		return nullptr;
	}

	void wait_and_pop(T& x)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond_variable.wait(lock, []{return !m_deque.empty(); });
		x = m_deque.pop_front();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond_variable.wait(lock, []{return !m_deque.empty(); });
		return std::make_shared<T>(m_deque.pop_front());
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_deque.empty();
	}

	int size()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_deque.size();
	}

	template<class ...Args> void emplace(Args... args)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_deque.emplace_back(args...);
		m_cond_variable.notify_one();
	}
};

//store shared_ptr<T> in underlying queue
template<typename T>
class Thread_safe_queue2
{
private:
	std::queue<std::shared_ptr<T>> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cond;

public:
	Thread_safe_queue2(){}
	Thread_safe_queue2(const Thread_safe_queue2& rhs) 
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_queue = rhs.m_queue;
	}

	Thread_safe_queue2& operator=(const Thread_safe_queue2&) = delete;
	Thread_safe_queue2(Thread_safe_queue2&&) = delete;
	Thread_safe_queue2& operator=(Thread_safe_queue2&&) = delete;

	void push(T value)
	{
		std::shared_ptr<T> data(std::make_shared<T>(std::move(value)));
		std::lock_guard<std::mutex> lk(m_mutex);
		m_queue.push(data);
		m_cond.notify_one();
	}

	void push(T&& value)
	{
		std::shared_ptr<T> data(std::make_shared<T>(std::move(value)));
		std::lock_guard<std::mutex> lk(m_mutex);
		m_queue.push(data);
		m_cond.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [] {return !m_queue.empty(); });
		value = std::move(*m_queue.pop());
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [] {return !m_queue.empty(); });
		return m_queue.pop();
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		if (m_queue.empty())
		{
			return false;
		}

		value = std::move(*m_queue.pop());
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		if (m_queue.empty())
		{
			return nullptr;
		}
		return m_queue.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		return m_queue.empty();
	}

};

//fine-grained mutex and thread_safe_queue
template<typename T>
class Queue
{
	struct node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;

		node()
		{}
	};

private:
	std::unique_ptr<node> head;
	node* tail;
	std::mutex head_mutex;
	std::mutex tail_mutex;
	std::condition_variable m_data_cond;

	node* getTail()
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}

	std::unique_ptr<node> pop_head()
	{
		auto old_head = std::move(head);
		head = std::move(old_head->next);
		return old_head;
	}

	std::unique_lock<std::mutex> wait_for_data()
	{
		std::unique_lock<std::mutex> head_lock(head_mutex);
		m_data_cond.wait(head_lock, [this] {return head.get() != getTail(); });
		return std::move(head_lock);
	}

	std::unique_ptr<node> wait_pop_head()
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		return pop_head();
	}

	std::unique_ptr<node> wait_pop_head(T& value)
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		value = std::move(*(head->data));
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == getTail())
		{
			return nullptr;
		}
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head(T& value)
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == getTail())
		{
			return nullptr;
		}
		value = *(head->data);
		return pop_head();
	}


public:
	Queue()
		: head(new node)
		, tail(head.get())
	{}

	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;
	Queue(Queue&&) = delete;
	Queue& operator=(Queue&&) = delete;

	void push(T value)
	{
		auto new_data = std::make_shared<T>(std::move(value));
		std::unique_ptr<node> new_node(new node);
		node* const new_tail = new_node.get();
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			tail->data = new_data;
			tail->next = std::move(new_node);
			tail = new_tail;
		}
		m_data_cond.notify_one();
	}

	bool try_pop(T& value)
	{
		std::unique_ptr<node> const old_head = try_pop_head(value);
		return old_head;		
	}

	std::shared_ptr<T> try_pop()
	{
		std::unique_ptr<node> const old_head = try_pop_head();
		return old_head ? old_head->data : nullptr;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		return (head.get() == getTail());
	}

	void wait_and_pop(T& value)
	{
		std::unique_ptr<node> const old_head(wait_and_pop(value));
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_ptr<node> const old_head(wait_and_pop());
		return old_head->data;
	}

};


//functional programming for quick sort
//version1 sequential version
template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}

	std::list<T> result;
	result.splice(result.begin(), input, input.begin());
	const T& pivot = *result.begin();
	auto divid_point = std::partition(input.begin(), input.end(), [&](const T& x) { return x < pivot; });
	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divid_point);
	auto new_lower(sequential_quick_sort(std::move(lower_part)));
	auto higher(sequential_quick_sort(std::move(input)));
	result.splice(result.end(), higher);
	result.splice(result.begin(), new_lower);
	return result;
}

//parallel_quick_sort
template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}
	std::list<T> result;
	//auto mid_point = std::advance(input.begin(), abs(rand()) / input.size());
	result.splice(result.begin(), input, input.begin());
	const T& pivot = *result.begin();
	auto divid_point = std::partition(input.begin(), input.end(), [&](const T& x) {return x < pivot; });
	std::list<T> lower_part;
	lower_part.splice(lower_part.begin(), input, input.begin(), divid_point);
	std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));
	auto higher_part = sequential_quick_sort(std::move(input));
	result.splice(result.end(), higher_part);
	result.splice(result.begin(), new_lower.get());
	return result;
}

//spinlock_mutx
class spinlock_mutex
{
public:
	spinlock_mutex()
	{}

	spinlock_mutex(const spinlock_mutex&) = delete;
	spinlock_mutex& operator = (const spinlock_mutex&) = delete;
	spinlock_mutex(spinlock_mutex&&) = delete;
	spinlock_mutex& operator=(spinlock_mutex&&) = delete;
public:
	void lock()
	{
		while (m_flag.test_and_set(std::memory_order_acquire));
	}
	void unlock()
	{
		m_flag.clear(std::memory_order_release);
	}

private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};




int main()
{
	try
	{
		int thread_number = std::thread::hardware_concurrency();
		if (!thread_number)
		{
			thread_number = std::max(thread_number, 4);
		}

		auto lamdafn = []() {
			//auto aWorker = Singleton_Mutex_DoubleCheck<Worker>::getInstance();
			//auto aWorker = Singleton_Call_Once<Worker>::getInstance();
			//aWorker->DoWork();

			//auto aFarmer = Singleton_Mutex_DoubleCheck<Farmer, int>::getInstance(1);
			//auto aFarmer = Singleton_Call_Once<Farmer, int>::getInstance(10);
			//aFarmer->DoWork();

			auto& aWorkerInstance = Singleton_Static<Worker>::getInstance();
			auto& aFarmerInstance = Singleton_Static<Farmer, int>::getInstance(100);
			aWorkerInstance.DoWork();
			aFarmerInstance.DoWork();
		};

		std::vector<std::thread> threads(thread_number - 1);

		for (auto i = 0; i < thread_number - 1; i++)
		{
			threads.push_back(std::thread(lamdafn));
		}

		lamdafn();
		//std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		for (auto& thd:threads)
		{
			if (thd.joinable())
			{
				thd.join();
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	const int number = 10000000;
	std::list<double> score;
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(1.0, 100.0);
	for (auto i = 0; i < number; i++)
	{
		score.push_back(dis(gen));
	}
	//for (auto& element : score)
	//{
	//	std::cout << element << std::endl;
	//}

	auto score2 = score;
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto result = sequential_quick_sort(score);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> diff = end - start;
		std::cout << "sequential_quick_sort took time: " << diff.count() << "us" << std::endl;
		//for (auto& element: result)
		//{
		//	std::cout << element << std::endl;
		//}
	}

	{
		auto start = std::chrono::high_resolution_clock::now();
		auto result = parallel_quick_sort(score2);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> diff = end - start;
		std::cout << "parallel_quick_sort took time: " << diff.count() << "us" << std::endl;
		//for (auto& element : result)
		//{
		//	std::cout << element << std::endl;
		//}
	}
	
    return 0;
}

