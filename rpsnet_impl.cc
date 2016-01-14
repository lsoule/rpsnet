// Implementation of a simple Rpsnet server.

#include <grpc++/grpc++.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "rpsnet_impl.h"
#include <google/protobuf/text_format.h>

#include <google/protobuf/message.h>
using namespace std;
using google::protobuf::TextFormat;
namespace Rpsnet {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

RpsnetImpl::RpsnetImpl()
    : next_candidate_(0) {
}

// COV_NF_END

RpsnetImpl::~RpsnetImpl() {
}

// This is the corresponding method on the server that runs for each
// GetWork() Stubby call.  The stubby server may be running these
// concurrently so we use locks to guard shared data.  Essentially this
// fille data into the response, calling Stubby's done callback when done.
Status RpsnetImpl::GetWork(ServerContext* context,
                           const CandidateRequest* request,
                           CandidateResponse* response) {
  try {
  // lock_.Lock();
  printf("Rpsnet.GetWork: cpu_id= %s\n", request->cpu_id().c_str());

  GetCandidate(request->cpu_id(), response);
  if (response->rps_k() == 0) {
    WriteProgressFile();
    return Status::CANCELLED;
  }

  printf("Rpsnet.GetWork: cpu_id= %s, response=%s\n", request->cpu_id().c_str(),
         response->ShortDebugString().c_str());
  //lock_.Unlock();
  } catch (...) { cout << "default exception" << endl; }
  return Status::OK;
}

bool RpsnetImpl::ReadPgenFile() {
  ifstream pgen_file;
  pgen_file.open(file_base_name_.c_str());
  cout << "pgen is open " << pgen_file.is_open() << endl;
  string header;
  pgen_file >> header;
  cout << "got header of " << header << endl;
  progress_.set_pgen_line(header);
  while (pgen_file.good()) {
    int rps_k, rps_n;
    CandidateProgress *cp = progress_.add_progress();
    pgen_file >> rps_k >> rps_n;
    cout << "got k/n " << rps_k << ", " << rps_n << endl;
    cp->mutable_candidate()->set_pgen_line(header);
    cp->mutable_candidate()->set_rps_k(rps_k);
    cp->mutable_candidate()->set_rps_n(rps_n);
    cp->set_sent_out(false);
  }
  return true;
}

bool RpsnetImpl::WriteProgressFile() {
  string filename(file_base_name_+".progress");
  ofstream progress_file;
  progress_file.open(filename.c_str());
  progress_.SerializeToOstream(&progress_file);
  printf("wrote progress file %s\n", filename.c_str());
  progress_file.close();
 
  string filename_txt(file_base_name_+"_progress.txt");
  ofstream progress_file_txt;
  progress_file_txt.open(filename_txt.c_str());
  string file_contents;
  TextFormat::PrintToString(progress_, &file_contents);
  // cout <<  "contents are " << file_contents << endl;
  progress_file_txt << file_contents;
  printf("wrote text progress file %s\n", filename_txt.c_str());
  progress_file_txt.close();
  string lresults_file_name(file_base_name_+".lresults");
  FILE* fp = fopen(lresults_file_name.c_str(), "w");
  for(int i=0; i < progress_.progress_size(); i++) {
    int len = strlen(progress_.progress(i).llr_return().c_str());
    if ((len > 0) && (len < 5)) {
      fprintf(fp, "Short llr_return len of %d\n",
              (int)strlen(progress_.progress(i).llr_return().c_str()));
      progress_.mutable_progress(i)->clear_llr_return();
      progress_.mutable_progress(i)->set_sent_out(false);
      if (i < next_candidate_)
        next_candidate_ = i;
    } else {
      fprintf(fp, "%s\n", progress_.progress(i).llr_return().c_str());
    }
#if 0
    if (progress_.mutable_progress(i)->sent_out()) {
       // && (((int64)WallTime_Now()) - progress_.progress(i).time_sent_out() > (3600*3))) {
      progress_.mutable_progress(i)->clear_llr_return();
      progress_.mutable_progress(i)->set_sent_out(false);
      if (i < next_candidate_)
        next_candidate_ = i;
      fprintf(fp, "Timeout: ");
    }
#endif
  }
  fclose(fp);

  return true;
}

bool RpsnetImpl::ReadProgressFile() {
  bool reset_sent = false;
  string filename(file_base_name_+".progress");
  ifstream progress_file;
  progress_file.open(filename.c_str());
  if (!progress_file.is_open()) {
    cout << "Cannot open progress file " << filename << endl;
    return false;
  }
  progress_.ParseFromIstream(&progress_file);
  progress_file.close();
  int results = 0;
  for(int i=0; i < progress_.progress_size(); i++) {
    if (strlen(progress_.progress(i).llr_return().c_str()) < 5) {
      progress_.mutable_progress(i)->clear_llr_return();
      progress_.mutable_progress(i)->set_sent_out(false);
    } else {
      results++;
    }
  }
  if (reset_sent) {
    printf("resetting sent flag\n");
    for(int i=0; i < progress_.progress_size(); i++) {
      if (progress_.progress(i).llr_return() == "") {
        progress_.mutable_progress(i)->set_sent_out(false);
      } else if (!strncmp(progress_.progress(i).llr_return().c_str(),
                          "Iteration", 9)) {
        printf("Found Iteration in llr_return: %s",
               progress_.progress(i).llr_return().c_str());
        progress_.mutable_progress(i)->clear_llr_return();
      }
    }
  }
  printf("read %d candidates, %d with results\n", progress_.progress_size(),
         results);
  return true;
}

bool RpsnetImpl::SetFileBase(const string file_base_name, const bool read) {
  cout << "SetFileBase " << file_base_name << ", read=" << read << endl;
  file_base_name_ = file_base_name;
  if (!read) return true;
  // Already have the progress file so don't read the raw pgen file
  if (ReadProgressFile()) return true;
  cout << "no progress file - try pgen" << endl;
  return ReadPgenFile();
}

void RpsnetImpl::GetCandidate(const string cpu_id,
                              CandidateResponse *response) {
  while (next_candidate_ < progress_.progress_size()) {
    if ((progress_.progress(next_candidate_).llr_return() == "") &&
        !progress_.progress(next_candidate_).sent_out()) {
      response->set_pgen_line(progress_.pgen_line());
      response->set_rps_k(progress_.progress(next_candidate_).candidate().rps_k());
      response->set_rps_n(progress_.progress(next_candidate_).candidate().rps_n());
      progress_.mutable_progress(next_candidate_)->set_sent_out(true);
      // progress_.mutable_progress(next_candidate_)->set_time_sent_out(
      //     (int64)WallTime_Now());
      printf("Sending candidate k=%d, n=%d at %lld\n", response->rps_k(),
             response->rps_n(),
             progress_.mutable_progress(next_candidate_)->time_sent_out());
      next_candidate_++;
      if ((next_candidate_ % 10) == 0) WriteProgressFile();
      return;
    }
    next_candidate_++;
  }
  response->set_pgen_line("");
  response->set_rps_k(0);
  response->set_rps_n(0);
  printf("Out of candidates, sending k=0, n=0\n");
}

Status RpsnetImpl::ReportWork(ServerContext* context,
                              const WorkDoneRequest* request,
                              EmptyMessage* response) {
  // MutexLock lock(&lock_);

  printf("Report work k=%d n=%d, output=%s\n",
         request->result_k(),
         request->result_n(),
         request->result_output().c_str());
  if (strlen(request->result_output().c_str()) == 0) {
    printf("Bad llr return value!\n");
    return Status::CANCELLED;
  }
  for(int i=0; i < progress_.progress_size(); i++) {
    if ((request->result_k() ==
        progress_.progress(i).candidate().rps_k()) &&
        (request->result_n() ==
            progress_.progress(i).candidate().rps_n())) {
      printf("found at index %d\n", i);
      progress_.mutable_progress(i)->set_llr_return(request->result_output());
      return Status::OK;
    }
  }
  printf("not found!\n");
  FILE* fp = fopen("orphans", "a");
  if (!fp) {
    cout << "could not open file orphans" << endl;
  } else {
    const string result_out(request->result_output());
    fprintf(fp, "%s\n", request->result_output().c_str());
  }
  fclose(fp);
  return Status::OK;
}


}  // namespace rpsnet
