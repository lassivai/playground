#include <cstdio>
#include <ctime>
#include <chrono>
#include <string>

std::string getTimeString(const char *format = "%F_%T") {
  auto tp = std::chrono::system_clock::now();
  time_t tt = std::chrono::system_clock::to_time_t(tp);
  tm *ti = localtime(&tt);
  char buf[256];
  strftime(buf, 256, "%F_%T", ti);
  //printf("%s\n", buf);
  return std::string(buf);
}

int main()
{
  printf("%s\n", getTimeString().c_str());
}
