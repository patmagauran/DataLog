#pragma once
#include <unordered_map>
#include <string>
class DataRow {

public: 
	double timestamp;
	std::unordered_map<std::string, double> data;
	DataRow(double timestamp, std::unordered_map<std::string, double> data) {
		this->timestamp = timestamp;
		this->data = data;
	}
	DataRow() {

	}
};