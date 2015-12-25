#include "buffer.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <string>

using namespace std;

template<typename T>
class Vector {

private:
	int len;
	int blk_size;
	int elem_size;
	int elem_per_blk;
	string *name_on_disk;
	BufferedFile *extern_file;
	void* header;
	bool isFull();
public:
	Vector(string _name_on_disk);
	~Vector();
	void pushBack(T _elem);
	T* pop();
	T* getElem(int index);
	void deletePersistentCopy();

};

template<typename T>
Vector<T>::Vector(string _name_on_disk, string _devicename)
{
	name_on_disk = new string(_name_on_disk);
	blk_size = getDeviceBlockSize(_devicename.c_str());
	elem_size = sizeof(T);
	elem_per_blk = blk_size / elem_size;

	extern_file = new BufferedFile(blk_size, this->name_on_disk->c_str());
	header = (void*)malloc(blk_size);
	try
	{
		extern_file->readHeader(header);
		int curr_pos = 0;
		memcpy(&len, (void*)(((uint8_t*)header) + curr_pos), sizeof(len));
		curr_pos += sizeof(len);

		//continue header info extraction.

	} catch(exception e)	//add a 'Blank File' exception
	{
		len = 0;
		int curr_pos = 0;
		memcpy((void*)(((uint8_t*)header) + curr_pos), &len, sizeof(len));
		curr_pos += sizeof(len);

		//continue header info writing

		extern_file->writeHeader(header);
	}
}

template<typename T>
bool Vector<T>::isFull()
{
	return len % elem_per_blk ? false : true;
}

template<typename T>
void Vector<T>::pushBack(T _elem)
{
	if(isFull())
	{
		int elem_blk_num = extern_file->allotBlock();
		void* buff = malloc(blk_size);
		memcpy(buff, (void*)&_elem, sizeof(_elem));
		extern_file->writeBlock(elem_blk_num, buff);
	}
	else
	{
		int elem_blk_num = len / elem_per_blk;
		int blk_offset = len % elem_blk_num;
		void* buff = malloc(blk_size);
		extern_file->readBlock(elem_blk_num, buff);
		memcpy((void*)(((uint8_t*)buff) + blk_offset), (void*)&_elem, sizeof(_elem));
		extern_file->writeBlock(elem_blk_num, buff);
	}
}

template<typename T>
T* Vector<T>::pop()
{
	if (len == 0) return NULL;
	void* buff = malloc(blk_size);
	void* new_buff = malloc(blk_size);

	// read the whole block
	extern_file->readBlock(1, buff);

	// keep everything else except the last elem
	memcpy(new_buff, buff, (blk_size - elem_size));

	// write back the rest
	extern_file->writeBlock(1, new_buff);

	// ?? - to return the last element which was popped out
	T* temp = (T*) malloc(sizeof(T));
	memcpy(temp, buff, sizeof(T));

	return temp;
}

template<typename T>
T* Vector<T>::getElem(int index)
{
	int elem_blk_num = index / elem_per_blk;
	int blk_offset = (index % elem_per_blk) * elem_size;

	void *buff = (void*)malloc(blk_size);
	extern_file->readBlock(elem_blk_num, buff);

	T* temp = (T*) malloc(sizeof(T));
	memcpy(temp, (void*)(((uint8_t*)buff) + blk_offset), sizeof(T));
	return temp;
}

template<typename T>
void Vector<T>::deletePersistentCopy()
{

}