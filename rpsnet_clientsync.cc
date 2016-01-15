// Implementation of a simple RpsNet_client.

#include <iostream>
#include <fstream>
#include <grpc++/grpc++.h>
#include <unistd.h>
#include "rpsnet_client.h"
#include "rpsnet.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using Rpsnet::CandidateRequest;
using Rpsnet::CandidateResponse;
using Rpsnet::EmptyMessage;
using Rpsnet::RpsNet;
using Rpsnet::RpsnetClientSync;
using Rpsnet::WorkDoneRequest;
using std::string;

int main(int argc, char** argv) {
  string cpu_id("test cpu");
  int server_id = 1;

  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  RpsnetClientSync client(grpc::CreateChannel(
      "104.197.186.246:50051", grpc::InsecureChannelCredentials()));

  while (1) {
    int cand_k = 0;
    int cand_n = 0;
    string pgen_line;
    sleep(1);

    printf("calling GetWork\n");
    if (client.GetWork(cpu_id, cand_k, cand_n, &pgen_line)) {
      printf("got k=%d, n=%d\n", cand_k, cand_n);
      if (cand_k != 0) {
        printf("%d*2^%d-1, s%d\n", cand_k, cand_n, server_id);
        FILE* llr_file = fopen("rpsnetllr", "w");
        if (!llr_file) {
          printf("could not open file rpsnetllr");
          return 1;
        }
        fprintf(llr_file, "%s\n%d %d", pgen_line.c_str(), cand_k, cand_n);
        fclose(llr_file);
        system("rm lresults.txt");
        system("./sllr64 -oPriority=10 rpsnetllr");
        string lresults;
        std::ifstream myfile("lresults.txt");
        getline (myfile, lresults);
        printf("report work %s\n", lresults.c_str());
        client.ReportWork(lresults, cand_k, cand_n, true);
      }
    }
  }
}

namespace Rpsnet {
bool RpsnetClientSync::GetWork(string cpu_id, int& k, int& n,
                               string *pgen_line) {
  CandidateRequest request;
  CandidateResponse response;
  ClientContext context;

  request.set_cpu_id(cpu_id);
  Status status =stub_->GetWork(&context, request, &response);

  if (!status.ok()) {
    printf("RPC failed");
    return false;
  } else {
    k = response.rps_k();
    n = response.rps_n();
    *pgen_line = response.pgen_line();
    return true;
  }
}

void RpsnetClientSync::ReportWork(const string& work_output,
                                  const int result_k,
                                  const int result_n,
                                  const bool isOk) {
  WorkDoneRequest request;
  EmptyMessage response;
  request.set_result_output(work_output);
  request.set_result_k(result_k);
  request.set_result_n(result_n);
  request.set_result_ok(isOk);

  for (int i = 0; i < 10; i++) {
    ClientContext context;
    printf("ReportWork, request %s\n", request.ShortDebugString().c_str());
    Status status = stub_->ReportWork(&context, request, &response);
    if (status.ok()) {
      return;
    }
    printf("ReportWork RPC failed, code %d, msg %s\n", status.error_code(), status.error_message().c_str());
    sleep(10);
  }
}
}  // namespace Rpsnet
