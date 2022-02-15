#ifndef TASK_H
#define TASK_H

#include <Windows.h>

/**
 * @brief An imitation of the Android AsyncTask class
 * 
 * @tparam T The type of the argument passed to the task function
 * @tparam E The type of the progress argument
 * @tparam V The type of the result argument
*/
template <typename T, typename E, typename V>
class Task
{
public:
	Task() = default;

	virtual void run(T* arg) final
	{
		pArg = arg;
		tHandle = CreateThread(
			NULL,
			0,
			&Task::threadFunction,
			this,
			0,
			&dwThreadId);
	}
protected:
	virtual V doInBackground(T* arg) = 0;
	virtual void onProgressUpdate(E progress) {}
	virtual void onPostExecute(V result) {}

	virtual void postProgress(E progress) final
	{
		onProgressUpdate(progress);
	}

	DWORD dwThreadId;
	HANDLE tHandle;

private:
	T* pArg;
	static DWORD threadFunction(void* tParam)
	{
		Task* instance = reinterpret_cast<Task*>(tParam);
		V result = instance->doInBackground(instance->pArg);
		instance->onPostExecute(result);

		return 0;
	}
};

#endif