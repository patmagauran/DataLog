#pragma once
#include <unordered_map>
#include <string>
#include "concurrent/AppendOnlyVector.h"
#define DEFAULT_PLOT_SIZE 500000
class dataPoint {
public:
	AppendOnlyVector<double> x;
	AppendOnlyVector<double> y;
	inline dataPoint(size_t size= DEFAULT_PLOT_SIZE) : x(size), y(size) {}
	inline void push_back(double x, double y) {
		this->x.push_back(x);
		this->y.push_back(y);
	}
	inline dataPoint(const dataPoint& other) : x(other.x), y(other.y) {}
};
class PlotDataContainer
{
private:
	std::unordered_map<std::string, size_t> index_map;
	AppendOnlyVector<dataPoint> data;
	size_t allocated_plots;
	size_t allocated_points;
	public:
		PlotDataContainer(size_t plots = 255, size_t points = DEFAULT_PLOT_SIZE);
		void putData(std::string plot, double x, double y);
		const std::unordered_map<std::string, size_t>& getIndexMap() const;
		const AppendOnlyVector<dataPoint>& getData() const;
};


inline PlotDataContainer::PlotDataContainer(size_t plots, size_t points) : allocated_plots(plots), allocated_points(points), data(AppendOnlyVector<dataPoint>(plots))
{
}

inline void PlotDataContainer::putData(std::string plot, double xVal, double yVal)
{
	if (auto search = index_map.find(plot); search != index_map.end()) {
		data[search->second].push_back(xVal, yVal);
	}
	else {
		index_map[plot] = data.size();
		data.push_back(dataPoint(allocated_points));
		data[data.size() - 1].push_back(xVal, yVal);
	}
}

inline const std::unordered_map<std::string, size_t>& PlotDataContainer::getIndexMap() const
{
	return this->index_map;
}

inline const AppendOnlyVector<dataPoint> & PlotDataContainer::getData() const
{
	return this->data;
}

// : allocated_plots(plots), allocated_points(points), data(AppendOnlyVector<AppendOnlyVector<double>>(plots)