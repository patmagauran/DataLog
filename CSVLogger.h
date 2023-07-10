#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "concurrent/readerwriterqueue.h"
#include <map>
#include <vector>
#include "DataRow.h"
#include <iterator>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <thread>

#define FLUSH_EVERY 1000
class CSVLogger
{
private:
	std::ofstream out_;
	inline static std::thread csvLogThread = std::thread();
	long numRowsBuffered = 0;
	std::map<std::string, int> columns;
	moodycamel::ReaderWriterQueue<DataRow> dataQueue = moodycamel::ReaderWriterQueue<DataRow>();
	void writeHeader();
	void writeRow(DataRow row);
	void run();
public:
	CSVLogger(std::string filename);
~CSVLogger();
	void addRow(DataRow row);
};

