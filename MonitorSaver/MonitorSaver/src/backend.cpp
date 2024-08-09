#include "backend.h"
#include <fstream>
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <locale>
#include <codecvt>

BOOL backEnd::ApplyMonitorConfig(const MonitorConfig& config, bool isPrimary) {
    DEVMODE devMode;
    ZeroMemory(&devMode, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);
    devMode.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT;
    devMode.dmPosition.x = config.monitorRect.left;
    devMode.dmPosition.y = config.monitorRect.top;
    devMode.dmPelsWidth = config.monitorRect.right - config.monitorRect.left;
    devMode.dmPelsHeight = config.monitorRect.bottom - config.monitorRect.top;

    // Apply monitor's position and resolution first
    LONG result = ChangeDisplaySettingsEx(config.deviceName.c_str(), &devMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
    if (result != DISP_CHANGE_SUCCESSFUL) {
        return FALSE;
    }

    // Apply the primary monitor setting if needed
    if (isPrimary) {
        result = ChangeDisplaySettingsEx(config.deviceName.c_str(), NULL, NULL, CDS_SET_PRIMARY, NULL);
        if (result != DISP_CHANGE_SUCCESSFUL) {
            return FALSE;
        }
    }

    return TRUE;
}

const bool backEnd::save(std::string& errorMessage) {

    _configs.clear();

    if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&_configs))) {
        errorMessage += "Failed to gather monitor information.";
        return false;
    }

    std::wofstream wSaveFile(SAVE_FILE_PATH, std::ios::binary);
    if (!wSaveFile.is_open()) {
        errorMessage += "Failed to open save file. Check if the directory exists and has write permissions.";
        return false;
    }

    for (const MonitorConfig& config : _configs) {
        wSaveFile << config.deviceName << L" "
                  << config.monitorRect.left << L" " << config.monitorRect.top << L" "
                  << config.monitorRect.right << L" " << config.monitorRect.bottom << L" "
                  << config.isPrimary << std::endl;
    }

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    wSaveFile << (now->tm_year + 1900) << '-'
              << (now->tm_mon + 1) << '-'
              <<  now->tm_mday << "@"
              <<  now->tm_hour << ":"
              << now->tm_min;

    return true;
}

const bool backEnd::load(std::string& errorMessage) {
    _configs.clear();

    std::wifstream wSaveFile(SAVE_FILE_PATH, std::ios::binary);
    if (!wSaveFile.is_open()) {
        errorMessage += "Failed to open save file. Check if the directory exists and has write permissions.";
        return false;
    }

    // Read all lines into a vector
    std::vector<std::wstring> lines;
    std::wstring line;
    while (std::getline(wSaveFile, line)) {
        lines.push_back(line);
    }

    // Process all but the last line
    for (size_t i = 0; i < lines.size() - 1; ++i) {
        std::wistringstream wiss(lines[i]);
        MonitorConfig config;
        wiss >> config.deviceName
            >> config.monitorRect.left >> config.monitorRect.top
            >> config.monitorRect.right >> config.monitorRect.bottom
            >> config.isPrimary;

        if (!ApplyMonitorConfig(config, config.isPrimary)) {
            errorMessage += "Failed to apply configuration for " + std::string(config.deviceName.begin(), config.deviceName.end());
            return false;
        }
    }

    return true;
}

const std::string backEnd::getLastSaveDate() {

    if (checkIfEmpty()) {
        return "No last save data.";
    }

    std::wifstream wSaveFile(SAVE_FILE_PATH, std::ios::binary);
    if (!wSaveFile.is_open()) {
        return "Failed to open save file. Check if the directory exists and has write permissions.";
    }

    std::vector<std::wstring> lines;
    std::wstring line;

    while (std::getline(wSaveFile, line)) {
        lines.push_back(line);
    }

    // Check if there are lines in the vector
    if (lines.empty()) {
        return "No last save";
    }

    // Convert the last line from std::wstring to std::string
    std::wstring lastLine = lines.back();

    // Convert std::wstring to std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string lastSaveDate = converter.to_bytes(lastLine);

    return lastSaveDate;
}

const bool backEnd::checkIfEmpty() {
    std::ifstream saveFile(SAVE_FILE_PATH);
    return saveFile.peek() == std::ifstream::traits_type::eof();
}

BOOL CALLBACK backEnd::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    std::vector<MonitorConfig>* configs = reinterpret_cast<std::vector<MonitorConfig>*>(dwData);
    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);

    if (GetMonitorInfo(hMonitor, &mi)) {
        MonitorConfig config;
        config.deviceName = mi.szDevice;
        config.monitorRect = mi.rcMonitor;
        config.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;

        configs->push_back(config);
    }
    return TRUE;
}
