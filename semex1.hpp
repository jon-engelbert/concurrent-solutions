struct FunctionTimer {
  std::string mName;
  clock_t mStartTime;
  static std::map<std::string, clock_t> mFunctionTimes;

  FunctionTimer(char const * name) : mName(name), mStartTime(clock()) { }
  ~FunctionTimer();

  static void printTime(std::pair<const std::string, clock_t>& profile); // could be a class static method as well

  static void report();
};
