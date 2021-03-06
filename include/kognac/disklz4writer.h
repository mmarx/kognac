#ifndef _DISK_LZ4_WRITER
#define _DISK_LZ4_WRITER

#include <kognac/lz4io.h>

#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <assert.h>
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std;

class DiskLZ4Writer {
protected:
    struct BlockToWrite {
        int idpart;
        int idxfile;
        char *buffer;
        size_t sizebuffer;
    };

    struct FileInfo {
        int idxfiletowrite;

        char *buffer;
        size_t sizebuffer;
        char *compressedbuffer;
        size_t pivotCompressedBuffer;

        FileInfo() {
            buffer = new char[SIZE_SEG];
            sizebuffer = 0;
            compressedbuffer = NULL;
            pivotCompressedBuffer = 0;
            idxfiletowrite = 0;
        }

        ~FileInfo() {
            delete[] buffer;
        }
    };

    std::mutex mutexBlockToWrite;
    std::condition_variable cvBlockToWrite;
    std::chrono::duration<double> time_waitingwriting;
    std::chrono::duration<double> time_rawwriting;
    std::chrono::duration<double> time_waitingbuffer;
    size_t addedBlocksToWrite;
    int currentWriteFileID;
    std::list<BlockToWrite> *blocksToWrite;

    const int npartitions;

    std::vector<char*> buffers;
    std::mutex mutexAvailableBuffer;
    std::condition_variable cvAvailableBuffer;

    std::thread currentthread;
    bool processStarted;

    std::vector<FileInfo> fileinfo;

    DiskLZ4Writer(int npartitions, int nbuffersPerFile);

    void flush(const int id);

private:

    string inputfile;
    std::ofstream stream;
    std::vector<std::vector<int64_t> > startpositions;

    int nterminated;

    //Store the raw buffers to be written
    std::vector<char*> parentbuffers;


    std::mutex mutexTerminated;

    bool areAvailableBuffers();

    void compressAndQueue(const int id);

public:
	KLIBEXP DiskLZ4Writer(string file, int npartitions, int nbuffersPerFile);

	KLIBEXP void writeByte(const int id, const int value);

	KLIBEXP void writeVLong(const int id, const int64_t value);

	KLIBEXP void writeLong(const int id, const int64_t value);

	KLIBEXP void writeRawArray(const int id, const char *buffer, const size_t sizebuffer);

	KLIBEXP void writeString(const int id, const char *buffer, const size_t sizebuffer);

	KLIBEXP void writeShort(const int id, const int value);

	KLIBEXP void setTerminated(const int id);

    virtual void run();

	KLIBEXP bool areBlocksToWrite();

    virtual ~DiskLZ4Writer();
};

#endif
