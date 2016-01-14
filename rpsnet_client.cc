// Copyright 2007 Google Inc. All Rights Reserved.
// Author: piotrk@google.com (Piotr Kaminski)

// Command-line driver for the fortune seekers.
// Can drive either the synchronous or asynchronous implementation, selected
// by the value of the --sync flag.

#include <time.h>
#include <cctype>
#include <algorithm>

#include "experimental/users/lsoule/rpsnet/rpsnet_client.h"

#include "base/google.h"
#include "base/commandlineflags.h"
#include "util/time/walltime.h"


DEFINE_string(server, "localhost:10000",
              "address of server to connect to");
DEFINE_string(server2, "localhost:10000",
              "address of server2 to connect to");
DEFINE_bool(rpsnet_sync, true,
            "use synchronous implementation of fortune seeker");
DEFINE_string(cpu_id, "myvm",
              "cpu-id requesting work");


namespace {

// Replace 0-valued *month and/or *day with values from the current time.
// Non-zero values of *month and/or *day are left untouched.
void FillMonthDay(WallTime current_time, int* month, int* day) {
  struct tm now;
  double subsecond;
  WallTime_Split(current_time, &now, &subsecond);
  if (*month == 0) *month = now.tm_mon + 1;
  if (*day == 0) *day = now.tm_mday;
}

}  // namespace


int main(int argc, char **argv) {
  InitGoogle(argv[0], &argc, &argv, true);
  string candidate;
  int month, day;
  FillMonthDay(WallTime_Now(), &month, &day);

  Rpsnet::ExecuteCommandSync(FLAGS_server,
                             FLAGS_server2,
                             FLAGS_cpu_id,
                             candidate,
                             5,
                             333333);

  return 0;
}
