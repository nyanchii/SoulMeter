#include "pch.h"
#include ".\Damage Meter\Timer.h"

Timer::Timer() {
	_status = TIMER_STATUS::end;
	_startTimePoint = GetCurrentTimeStamp();
	_suspendTimePoint = GetCurrentTimeStamp();
	_suspendedTime = 0;
}

Timer::~Timer() {

}

VOID Timer::Run() {
	
	if (_status == TIMER_STATUS::end) {
		_startTimePoint = GetCurrentTimeStamp();
		_suspendedTime = 0;
		_status = TIMER_STATUS::run;
	}
	else if (_status == TIMER_STATUS::suspend) {
		_suspendedTime += GetCurrentTimeStamp() - _suspendTimePoint;
		_status = TIMER_STATUS::run;
	}
	else {

	}
}

VOID Timer::Suspend() {

	if (_status == TIMER_STATUS::run) {
		_suspendTimePoint = GetCurrentTimeStamp();
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

ULONG64 Timer::GetTime() {

	ULONG64 time;

	if (_status == TIMER_STATUS::run) {
		time = GetCurrentTimeStamp() - _startTimePoint;
	}
	else if (_status == TIMER_STATUS::suspend) {
		time = _suspendTimePoint - _startTimePoint;
	}
	else {
		return 0;
	}

	return time - _suspendedTime;
}