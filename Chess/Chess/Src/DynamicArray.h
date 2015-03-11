#include "stdafx.h"

template <class T>
class DynamicArray
{
private:
protected:
public:
	int		iCount;
	int		iLength;
	T *		Arr;

	DynamicArray(int length);
	~DynamicArray();

	void AddItem(T ptr);

	void RemoveItem(int num);

	int GetNumberOfItems() {return iCount;}

	void Sort(int (*compar)(const void*, const void*));

	bool Contains(T ptr);

	void ClearArray();

};

template <class T> DynamicArray<T>::DynamicArray(int length)
{
	iCount = 0;
	iLength = length;
	Arr = new T[length];
}

template <class T> DynamicArray<T>::~DynamicArray()
{

}

// This is going to grow the array by 5 when an item is trying to be added to an already full array
template <class T> void DynamicArray<T>::AddItem(T ptr)
{
	if (iCount < iLength)
	{
		Arr[iCount] = ptr;
		iCount++;
	}
	else
	{
		T* temparr = new T[iLength];
		memcpy(temparr, Arr, sizeof(T)*iLength);
		delete [] Arr;
		Arr = temparr;
		iLength += 5;

		Arr[iCount] = ptr;
		iCount++;
	}
}

// This is going to shrink the array every time there are 5 open slots on the end
template <class T> void DynamicArray<T>::RemoveItem(int num)
{
	for (int i = num; i < (iLength - 1); i++)
	{
		Arr[i] = Arr[i + 1];
	}
	iCount--;
}

template <class T> void DynamicArray<T>::Sort(int (*compar)(const void*, const void*))
{
	qsort(Arr, iCount, sizeof(Card*), compar); // changed iLength to iCount
}

template <class T> bool DynamicArray<T>::Contains(T ptr)
{
	for (int i = 0; i < iCount; i++)
	{
		if (Arr[i] == ptr)
		{
			return true;
		}
	}
	return false;
}

template <class T> void DynamicArray<T>::ClearArray()
{
	Arr = new T[iLength];
	iCount = 0;
}