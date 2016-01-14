// Copyright 2007 Google Inc. All Rights Reserved.
// Author: piotrk@google.com (Piotr Kaminski)

// Unit tests for rpsnet

#include <string>

#include "experimental/users/lsoule/rpsnet/rpsnet_impl.h"

#include "base/callback.h"
#include "base/commandlineflags.h"
#include "base/google.h"
#include "net/rpc2/rpc2.h"
#include "net/rpc/testing/public/donechecker.h"
#include "testing/base/public/gunit.h"
#include "net/proto2/bridge/public/stubby_glue.h"

using rpc::testing::DoneChecker;

namespace Rpsnet {

class RpsnetTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }

  // Check that a call to TellFortune returns the expected application error
  // code.
  void CheckTellFortuneError(string cpu_id, int error_code) {
    DoneChecker done;
    CandidateRequest request;
    CandidateResponse response;
    request.set_cpu_id("myvm1");
    rpsnet_.GetWork(&rpc_, &request, &response, done.callback());
  }

  // Check that a call to GetWork succeeds and returns the expected
  // message and zodiac sign.
  void CheckGetWork(
      string cpu_id, int k, int n) {

    DoneChecker done;
    CandidateRequest request;
    CandidateResponse response;
    request.set_cpu_id("myvm1");
    rpsnet_.GetWork(&rpc_, &request, &response, done.callback());

    ASSERT_EQ(k, response.rps_k());
    ASSERT_EQ(n, response.rps_n());
  }

 protected:
  // Check that a call to ReportWork returns the expected application error
  // code.
  void CheckReportWorkError(string result, int error_code) {
    DoneChecker done;
    WorkDoneRequest request;
    EmptyMessage response;
    request.set_result_output(result);
    rpsnet_.ReportWork(&rpc_, &request, &response, done.callback());

//    ASSERT_EQ(RPC::APPLICATION_ERROR, rpc_.status());
//    ASSERT_EQ(error_code, rpc_.application_error());
  }

  // Check that a call to ReportWork sets the horoscope correctly in the
  // internal table.
  void CheckReportWork(
      const char* result_output) {
    DoneChecker done;
    WorkDoneRequest request;
    EmptyMessage response;
    request.set_result_output(result_output);
    request.set_result_ok(true);
    rpsnet_.ReportWork(&rpc_, &request, &response, done.callback());
  }

  // An instance of the service under test.  The default initialization is
  // sufficient; we explicitly *don't* export the service over the network.
  RpsnetImpl rpsnet_;

  // An instance of the RPC descriptor for all tests to reuse.  The default
  // constructor initializes it to a good-enough state to pass directly to
  // service methods, as long as the server doesn't look at it too closely.
  proto2::bridge::StubbyRpcController rpc_;
};

// The following tests are one-liners feeding in different sets of arguments
// to the fixture methods above.

TEST_F(RpsnetTest, GetWorkOk) {
  ASSERT_EQ(rpsnet_.SetFileBase("experimental/users/lsoule/rpsnet/testpgen",
                                  true),
            true);
  CheckGetWork("myvm1", 12753, 726000);
}

TEST_F(RpsnetTest, ReadPgenFileOk) {
  ASSERT_EQ(rpsnet_.SetFileBase("experimental/users/lsoule/rpsnet/testpgen",
                                true),
            true);
}
TEST_F(RpsnetTest, WriteProgressFileOk) {
  ASSERT_EQ(rpsnet_.SetFileBase("experimental/users/lsoule/rpsnet/testpgen",
                                  true),
            true);
  ASSERT_EQ(rpsnet_.SetFileBase("/tmp/testpgen",
                                  false),
            true);
  ASSERT_EQ(rpsnet_.WriteProgressFile(), true);
  LOG(INFO) << "write ok - try read of generated progress";
  RpsnetImpl second_rpsnet;
  ASSERT_EQ(second_rpsnet.SetFileBase(
      "/tmp/testpgen", true), true);
}
TEST_F(RpsnetTest, MultipleGetWorksOk) {
  ASSERT_EQ(rpsnet_.SetFileBase("experimental/users/lsoule/rpsnet/testpgen",
                                  true),
            true);
  CheckGetWork("myvm1", 12753, 726000);
  CheckGetWork("myvm1", 25077, 726001);
  CheckGetWork("myvm1", 28605, 726002);
  CheckGetWork("myvm1", 13845, 726003);
//  CheckGetWork("myvm1", 0, 0);
}

}  // namespace Rpsnet
