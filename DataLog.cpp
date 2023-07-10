#include "DataLog.h"



void DataLog::initialize(std::string filename) {
	if (initialized) return;
	DataLog::initialized = true;
	dataThread = std::thread(DataLog::startDataThread);
	plotUIInstance = new PlotUI(plotData);
	csvLoggerInstance = new CSVLogger(filename);

}
void DataLog::cleanup() {
	if (!initialized) return;
	initialized = false;
	dataThread.join();
	plotUIInstance->~PlotUI();
	csvLoggerInstance->~CSVLogger();
	//dataThread.~thread();
}
void DataLog::startDataThread()
{
	datalogInstance = new DataLog();
	datalogInstance->run();
}
void DataLog::logData(std::string name, double value)
{
	currentData[name] = value;
}
void DataLog::pushTimestamp(const double timestamp)
{
	double timestampMs = timestamp * 1000;
	auto now = std::chrono::steady_clock::now();

	if (timestampMs < lastTsSim) {
		lastTsSim = timestampMs;
		logData("simSpeed", 0);

	}
	else if (lastTsSim < 0) {
		lastTsSim = timestampMs;
		lastTsProg = std::chrono::steady_clock::now();
		logData("simSpeed",0);

	}
	else {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTsProg);
		auto diff = timestampMs - lastTsSim;
		logData("simSpeed", diff / elapsed.count());
		lastTsSim = timestampMs;
		lastTsProg = now;
	}
	logData("systemTime", std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count());

	DataRow row(timestampMs, currentData);
	dataQueue.enqueue(row);
	currentData.clear();
}
void DataLog::pushEvent(EventType eventType, std::string message)
{
	switch (eventType) {
	case EventType::MESSAGE:
		std::cout << "[INFO] " << message << std::endl;
		break;
	case EventType::STAGE:
		std::cout << "[STAGE] " << message << std::endl;
		break;
	case EventType::PAUSE:
		std::cout << "[PAUSE] " << message << std::endl;
		paused.store(true);
		break;
	case EventType::RESUME:
		std::cout << "[RESUME] " << message << std::endl;
		paused.store(false);
		break;
	case EventType::DONE:
		std::cout << "[DONE] " << message << std::endl;
		done.store(true);
		break;
	}
}
bool DataLog::isDone()
{
	return done.load();
}
bool DataLog::isPaused()
{
	return paused.load();
}
DataLog::DataLog()
{
}
void DataLog::run() {
	while (!done) {

		DataRow currentRow;

		while (dataQueue.try_dequeue(currentRow)) {
			csvLoggerInstance->addRow(currentRow);
			for (const auto& [name, value] : currentRow.data) {
				plotData.putData(name, currentRow.timestamp, value);
			}
		}
	}
}



