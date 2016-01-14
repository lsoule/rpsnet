#ifndef RPSNET_CLIENT_H__
#define RPSNET_CLIENT_H__

#include <string>

#include "rpsnet.pb.h"
#include "rpsnet.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using Rpsnet::CandidateRequest;
using Rpsnet::CandidateResponse;
using Rpsnet::EmptyMessage;
using Rpsnet::WorkDoneRequest;
using Rpsnet::RpsNet;
using std::string;

namespace Rpsnet {

void ExecuteCommandSync(const string& server,
                        const string& server2,
                        const string cpu_id,
                        const string& suggestion,
                        const int result_k,
                        const int result_n);


class RpsnetClientSync {
 public:
  RpsnetClientSync(std::shared_ptr<Channel> channel)
      : stub_(RpsNet::NewStub(channel)) {}

  // Get a K/N pair for primality testing along with the first
  // pgen line.
  bool GetWork(const string cpu_id, int& k, int& n, string *pgen_line);

  // Reports the LLR result for a k/n pair
  void ReportWork(const string& result,
                  const int result_k,
                  const int result_n,
                  const bool isOk);

 private:
  std::unique_ptr<RpsNet::Stub> stub_;
};
}  // namespace Rpsnet

#endif  // RPSNET_CLIENT_H__
