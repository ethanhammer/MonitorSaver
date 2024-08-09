#ifndef BACKEND_H
#define BACKEND_H

#include "MonitorConfig.h"
#include <string>
#include <vector>
#include <windows.h>

#endif // BACKEND_H

class backEnd {

public:

    const bool save(std::string& errorMessage);

    const bool load(std::string& errorMessage);

    const std::string getLastSaveDate();



private:


    BOOL ApplyMonitorConfig(const MonitorConfig& config, bool isPrimary);

    std::vector<MonitorConfig> _configs;

    const bool checkIfEmpty();

    static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
};
