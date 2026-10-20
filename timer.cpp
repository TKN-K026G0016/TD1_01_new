#include "timer.h"

void ResetTimeData(TimeData& data) {
	data.frame = 0;
	data.sec = 0;
	data.min = 0;
}

void CountTimeData(TimeData& data) {
	data.frame++;
	if (data.frame >= 60) {
		data.frame = 0;
		data.sec++;
	}
	if (data.sec >= 60) {
		data.sec = 0;;
		data.min++;
	}
}

int ConvertTimeDataToInt(const TimeData& data) {
	int value = 0;
	value += data.frame;
	value += data.sec * 60;
	value += data.min * 3600;

	return value;
}