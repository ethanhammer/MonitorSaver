#ifndef MONITORCONFIG_H
#define MONITORCONFIG_H

#include <Windows.h>

#include <string>


struct MonitorConfig {
    std::wstring deviceName;
    RECT monitorRect;
    bool isPrimary;
};

#endif // MONITORCONFIG_H
