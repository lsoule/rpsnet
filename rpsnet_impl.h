#ifndef RPSNET_IMPL_H__
#define RPSNET_IMPL_H__

#include <string>
#include <vector>

#include <grpc++/grpc++.h>
#include "rpsnet.pb.h"
#include "rpsnet.grpc.pb.h"

namespace Rpsnet {

using std::string;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


class RpsnetImpl : public RpsNet::Service {
  friend class RpsnetTest;

 public:
  // Creates and initializes the server without touching the network or creating
  // any threads.
  RpsnetImpl();

  virtual ~RpsnetImpl();

  // Base file name for pgen output and rpsnet progress
  bool SetFileBase(const string file_base_name, const bool read);
  bool ReadPgenFile();
  bool ReadProgressFile();
  bool WriteProgressFile();

  virtual Status GetWork(ServerContext* context,
                         const CandidateRequest* request,
                         CandidateResponse* response);

  virtual Status ReportWork(ServerContext* contex,
                            const WorkDoneRequest* request,
                            EmptyMessage* response);

 private:
  void GetCandidate(const string cpu_id, CandidateResponse *response);
  string file_base_name_;
  ProgressFile progress_;
  int next_candidate_;
};

}  // namespace Rpsnet

#endif  // RPSNET_IMPL_H__
