#include <time.h>
#include <cctype>
#include <algorithm>
#include <grpc++/grpc++.h>
#include "rpsnet_client.h"
#include "rpsnet.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using Rpsnet::CandidateRequest;
using Rpsnet::CandidateResponse;
using Rpsnet::EmptyMessage;
using Rpsnet::RpsNet;
using Rpsnet::WorkDoneRequest;
using std::string;

namespace {

#if 0
// Replace 0-valued *month and/or *day with values from the current time.
// Non-zero values of *month and/or *day are left untouched.
void FillMonthDay(WallTime current_time, int* month, int* day) {
  struct tm now;
  double subsecond;
  WallTime_Split(current_time, &now, &subsecond);
  if (*month == 0) *month = now.tm_mon + 1;
  if (*day == 0) *day = now.tm_mday;
}
#endif
}  // namespace


int main(int argc, char **argv) {
  string candidate;

  Rpsnet::ExecuteCommandSync(FLAGS_server,
                             FLAGS_server2,
                             FLAGS_cpu_id,
                             candidate,
                             5,
                             333333);

  return 0;
}
