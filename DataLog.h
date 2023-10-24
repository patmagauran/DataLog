#pragma once
#include <string>
#include "concurrent/readerwriterqueue.h"
#include <unordered_map>
#include <thread>
#include "DataRow.h"
#include <map>
#include "PlotDataContainer.h"
#include "CSVLogger.h"
#include "PlotUI.h"
#include <iostream>
#include <fstream>
#include <chrono>
enum class EventType {
	MESSAGE,
	STAGE,
	DONE,
	PAUSE,
	RESUME
};
class DataLog
{


	inline static bool initialized = false;
	inline static std::string filename = "";
	inline static DataLog * datalogInstance = nullptr;

	inline static PlotUI * plotUIInstance = nullptr;
	inline static CSVLogger * csvLoggerInstance = nullptr;

	inline static std::thread dataThread = std::thread();

	inline static std::atomic_bool paused = false;
	inline static std::atomic_bool done = false;
	inline static std::atomic_bool uiShouldBeRunning = true;
	inline static double lastTsSim = -1.0f;
	inline static auto lastTsProg = std::chrono::steady_clock::now();
	inline static auto startTime = std::chrono::steady_clock::now();

	static void startDataThread();

	DataLog();

	void run();

	inline static moodycamel::ReaderWriterQueue<DataRow> dataQueue = moodycamel::ReaderWriterQueue<DataRow>();

	inline static std::unordered_map<std::string, double> currentData = std::unordered_map<std::string, double>();

	inline static std::shared_ptr<PlotDataContainer> plotData = std::make_shared<PlotDataContainer>();

public:
	static void initialize(std::string filename, bool newRun = true);
	static void logData(std::string name, double value);
	static void pushTimestamp(double timestamp);
	static void pushEvent(EventType eventType, std::string message); //TODO: Implement
	static bool isDone();
	static bool uiSHouldBeRunning();
	static bool isPaused();
	static void cleanup(bool close = true);

};

