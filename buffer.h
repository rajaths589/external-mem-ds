#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

size_t getDeviceBlockSize(char* device_name)
{
	using namespace std;
	const char info_file_path = "/queue/physical_block_size";
	const char disk_path = "/sys/block/";
	char disk_blk_file[] = (char*)malloc(
		sizeof(char) * (strlen(info_file_path) + strlen(device_name) + strlen(disk_path))
	);
	strcat(disk_blk_file,disk_path);
	strcat(disk_blk_file,device_name);
	strcat(disk_blk_file,info_file_path);
	FILE* fptr = fopen(disk_blk_file,"r");
	size_t blk_size;
	fscanf(fptr,"%lu",&blk_size);
	return blk_size;
}

class BufferedFile {
private:
	int fd;
	long last_blk;
	const size_t blk_size;
	long getblockoffset(long blk_nbr);

public:
	BufferedFile(size_t blk_size, const char* file_path);
	~BufferedFile();
	void readBlock(long blk_nbr, void* buffer);
	void writeBlock(long blk_nbr, void* buffer);
	long allotBlock();
	void readHeader(void* buffer);
	void writeHeader(void* buffer);
	void deleteBlock(long blk_nbr);	
};

long BufferedFile::getblockoffset(long blk_nbr) {
	return blk_nbr*((long) blk_size);
}

long BufferedFile::allotBlock() {
	last_blk++;
	return last_blk-1;
}

BufferedFile::BufferedFile(size_t blk_size, const char* file_path) : blk_size(blk_size) {
	fd = open(file_path, O_RDWR|O_CREAT|O_APPEND, 0755);
	last_blk = lseek(fd, 0, SEEK_END)/blk_size;
	lseek(fd, 0, SEEK_SET);
}

void BufferedFile::readHeader(void* buffer) {
	pread(fd, buffer, (off_t) blk_size, getblockoffset(0));
}

void BufferedFile::writeHeader(void* buffer) {
	pwrite(fd, buffer, (off_t) blk_size, getblockoffset(0));
}

void BufferedFile::readBlock(long blk_nbr, void* buffer) {
	pread(fd, buffer, (off_t) blk_size, getblockoffset(blk_nbr));
}

void BufferedFile::writeBlock(long blk_nbr, void* buffer) {
	pwrite(fd, buffer, (off_t) blk_size, getblockoffset(blk_nbr));
}

void BufferedFile::deleteBlock(long blk_nbr) {
	return;
}

BufferedFile::~BufferedFile() {
	close(fd);
}
