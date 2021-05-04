#include "pch.h"
#include ".\Damage Meter\Timer.h"

Timer::Timer() {
	_status = TIMER_STATUS::end;
	_startTimePoint = std::chrono::system_clock::now();
	_suspendTimePoint = std::chrono::system_clock::now();
	_suspendedTime = 0.f;
}

Timer::~Timer() {

}

VOID Timer::Run() {
	
	if (_status == TIMER_STATUS::end) {
		_startTimePoint = std::chrono::system_clock::now();
		_suspendedTime = 0.f;
		_status = TIMER_STATUS::run;
	}
	else if (_status == TIMER_STATUS::suspend) {
		std::chrono::duration<FLOAT> suspendedTime;
		suspendedTime = std::chrono::system_clock::now() - _suspendTimePoint;
		_suspendedTime += suspendedTime.count();
		_status = TIMER_STATUS::run;
	}
	else {

	}
}

VOID Timer::Suspend() {

	if (_status == TIMER_STATUS::run) {
		_suspendTimePoint = std::chrono::system_clock::now();
		_status = TIMER_STATUS::suspend;
	}
	else {

	}
}

VOID Timer::Stop() {
	
	_status = TIMER_STATUS::end;
}

BOOL Timer::isRun() {
	if (_status == TIMER_STATUS::run)
		return TRUE;
	else
		return FALSE;
}

FLOAT Timer::GetTime() {

	std::chrono::duration<FLOAT> time;

	if (_status == TIMER_STATUS::run) {
		time = std::chrono::system_clock::now() - _startTimePoint;
	}
	else if (_status == TIMER_STATUS::suspend) {
		time = _suspendTimePoint - _startTimePoint;
	}
	else {
		return 0;
	}

	return time.count() - _suspendedTime;
}