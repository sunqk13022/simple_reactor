#include "logging.h"

#include <stdio.h>
#include <iostream>
#include <string>

#include "time/timestamp.h"

using namespace simple_reactor;

FILE* g_file;
long  g_total;

void dummyOutput(const char* msg, int len) {
  g_total += len;
  if (g_file) {
    fwrite(msg, 1, len, g_file);
  }
}

void bench() {
  Logger::SetOutput(dummyOutput);

  Timestamp start = Timestamp::Now();

  g_total = 0;
  const int batch = 1000 * 1000;
  std::string long_str(3000, 'X');
  //std::cout << long_str;

  for (int i = 0; i < batch; ++i) {
    LOG_INFO << long_str << "\t" << i;
  }

  Timestamp end = Timestamp::Now();
  double seconds= (end.Ns() - start.Ns())/1000000;
  if (seconds < 0.000000000000001) seconds = 1;
  printf("%f seconds, %ld bytes, %.2f msg/s, %.2f MiB/s\n",
         seconds, g_total, batch / seconds, g_total / seconds / 1024 / 1024);

}

int main(int argc, char* argv[]) {
  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for (int i = 0; i < 10; ++i) {
    LOG_INFO << line << "\t" << i;
    usleep(1000);
  }

  bench();

  g_file = fopen("/dev/null", "w");
  bench();
  fclose(g_file);

  std::cout << "write to file,\tno cache\n";
  g_file = fopen("./log.txt", "w");
  //setbuffer(g_file, buffer, sizeof(buffer));
  bench();
  fclose(g_file);

  std::cout << "write to file,\thas cache\n";
  g_file = fopen("./log_cache.txt", "w");
  char buffer[64 * 1024];
  setbuffer(g_file, buffer, sizeof(buffer));
  bench();
  fclose(g_file);

}
