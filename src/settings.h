#pragma once

class Driver;

class Settings {
 private:
  static Settings* instance;

  bool debugMode = false;

  Settings() {}

 public:
  static Settings* getInstance();

  bool isDebugMode() { return debugMode; }

  // delete copy constructor and assignment operator
  Settings(const Settings&) = delete;
  Settings& operator=(const Settings&) = delete;

 private:
  ~Settings() {}

  friend class ::Driver;
};
