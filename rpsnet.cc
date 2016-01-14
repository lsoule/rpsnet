#include <iostream>
#include "rpsnet_impl.h"

#include <signal.h>

using Rpsnet::RpsnetImpl;
using grpc::ServerBuilder;
using grpc::Server;
using std::string;

RpsnetImpl rpsnet;
void QuitRpsNet(int p) {
  rpsnet.WriteProgressFile();
  exit(0);
}

int main(int argc, char** argv) {
  int port = 10000;
  string pgenfile("servertest");
  std::string server_address("0.0.0.0:50051");

  try {
  printf("main\n");
  signal(SIGINT, QuitRpsNet);
  signal(SIGQUIT, QuitRpsNet);
  signal(SIGTERM, QuitRpsNet);
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&rpsnet);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  rpsnet.SetFileBase(pgenfile, true);
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
  } catch (...) {printf("exception in main\n");}

  return 0;
}
