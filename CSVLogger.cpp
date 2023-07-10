#include "CSVLogger.h"
#include "DataLog.h"
//Some code similar to https://github.com/p-ranav/csv2/blob/master/include/csv2/writer.hpp

using namespace std;




CSVLogger::CSVLogger(std::string filename) : columns(), out_(std::ofstream(filename))
{
	columns["timestamp"] = 0;
	csvLogThread = std::thread(&CSVLogger::run, this);
}


CSVLogger::~CSVLogger()
{
	csvLogThread.join();
	out_.close();
}

void CSVLogger::writeHeader() {
	streampos pos = out_.tellp();
	out_.seekp(0, ios::beg);
	vector<string> strings = std::vector<string>(columns.size());

	for (const auto& [column, index] : columns) {
		strings[index] = column;
	}
	const auto delimiter_string = std::string(1, ',');
	std::copy(strings.begin(), strings.end() - 1,
		std::ostream_iterator<std::string>(out_, delimiter_string.c_str()));
	out_ << strings.back() << "\n";
	out_.seekp(0, ios::end);
}
void CSVLogger::writeRow(DataRow row) {
	vector<string> strings = std::vector<string>(columns.size() + row.data.size()+1);
	strings[0] = to_string(row.timestamp);
	int lastIndex = 0;
	bool newColumn = false;
	//loop through row.data
	for (const auto& [column, value] : row.data) {

		if (auto it = columns.find(column); it != columns.end()) {
			strings[it->second] = to_string(value);
			if (it->second > lastIndex) {
				lastIndex = it->second;
			}
		}
		else {
			int index = columns.size();
			columns[column] = index;
			strings[index] = to_string(value);
			lastIndex = index;
			newColumn = true;
		}
	}
	if (newColumn) {
		writeHeader();
	}


	const auto delimiter_string = std::string(1, ',');
	std::copy(strings.begin(), strings.begin() + lastIndex+1,
		std::ostream_iterator<std::string>(out_, delimiter_string.c_str()));
	out_ << strings.back() << "\n";
	if (numRowsBuffered++ > FLUSH_EVERY) {
		out_.flush();
		numRowsBuffered = 0;
	}
}
void CSVLogger::addRow(DataRow row)
{
	dataQueue.emplace(row);
}

void CSVLogger::run()
{
	while (!DataLog::isDone()) {

		DataRow currentRow;

		while (this->dataQueue.try_dequeue(currentRow)) {
			this->writeRow(currentRow);
		}
	}
}

