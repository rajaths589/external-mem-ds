size_t getDeviceBlockSize()
{
	const char diskblkfile = "~/../../sys/block/sdX/queue/physical_block_size"
	ifstream fptr(diskblkfile);
	size_t blksize;
	ifstream >> blksize;
	return blksize;
}

template <typename BlkSize>
class BufferedFile {
private:
	int fd;
	long lastblk;	
	inline long getblockoffset(long blknbr);
public:
	BufferedFile(const char* filepath);
	~BufferedFile();
	void readBlock(long blknbr, void* buffer);
	void writeBlock(long blknbr, void* buffer);
	long allotBlock();
	void readHeader(void* buffer);
	void writeHeader(void* buffer);
	void deleteBlock(long blknbr);
	void deletefile();
};