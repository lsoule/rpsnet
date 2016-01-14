// Copyright 2007 Google Inc. All Rights Reserved.
// Author: piotrk@google.com (Piotr Kaminski)

// Some utility functions and definitions common to both sync and async clients.

#include "experimental/users/lsoule/rpsnet/rpsnet_client.h"

#include "base/logging.h"
#include "strings/stringprintf.h"

DEFINE_double(fortuneseeker_deadline, 100.0,
              "timeout for calls to the server (in seconds)");

namespace rpsnet {

void LogCallStats(const RPC& rpc) {
  LOG(INFO) << StringPrintf("request size: %d bytes; response size: %d bytes\n",
                            rpc.transferred_arg_size(),
                            rpc.transferred_result_size());
  LOG(INFO) << StringPrintf("time spent:  %.3fs in network, %.3fs on server\n",
                            rpc.network_rtt(),
                            rpc.server_elapsed_time());
}

}  // namespace rpsnet
