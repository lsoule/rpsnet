// Copyright 2007 Google Inc. All Rights Reserved.
// Author: piotrk@google.com (Piotr Kaminski)

// Unit tests for the FortuneSeekerSync class.
// These tests are very similar to those in fortuneseekerasync_test.cc,
// differing only in how we deal with synchronous vs asynchronous calls.

#include "experimental/users/lsoule/rpsnet/rpsnet_client.h"

#include "base/google.h"
#include "base/scoped_ptr.h"
#include "net/rpc/testing/public/servicemocker.h"
#include "strings/strutil.h"
#include "testing/base/public/gunit.h"

using ::Rpsnet::Error;
using ::Rpsnet::RpsNet;
using ::Rpsnet::RpsnetClientSync;
using ::Rpsnet::CandidateRequest;
using ::Rpsnet::CandidateResponse;
using ::Rpsnet::WorkDoneRequest;
using ::Rpsnet::EmptyMessage;

using ::rpc::testing::AtLeast;
using ::rpc::testing::AtMost;
using ::rpc::testing::FailWith;
using ::rpc::testing::FailWithApplicationError;
using ::rpc::testing::Request;
using ::rpc::testing::RespondWith;
using ::rpc::testing::ServiceMocker;

namespace {

// Test suite for non-death-tests.
class RpsnetClientTest : public testing::Test {
 protected:
  virtual void SetUp() {
    seeker_.reset(new RpsnetClientSync(
        service_mocker_.CreateMockService<RpsNet>()));
  }

  scoped_ptr<RpsnetClientSync> seeker_;
  rpc::testing::ServiceMocker service_mocker_;
};

TEST_F(RpsnetClientTest, SeekFortune_SendsRequestAndReturnsFortune) {
  int k_ret;
  int n_ret;
  string pgen_line;
  EXPECT_RPC("GetWork")
    .With(Request<CandidateRequest>("cpu_id : 'myvm1'"))
    .WillOnce(RespondWith<CandidateResponse>(
      "rps_k : 5 rps_n : 333333 pgen_line : 'foo'"));
  ASSERT_TRUE(seeker_->GetWork("myvm1", k_ret, n_ret, &pgen_line));
  LOG(INFO) << "Getwork returned k = " << k_ret
            << ", n = " << n_ret;
  ASSERT_EQ(k_ret, 5);
  ASSERT_EQ(n_ret, 333333);
  ASSERT_STREQ(pgen_line.c_str(), "foo");
}

TEST_F(RpsnetClientTest, ReportWork_MakesCorrectCall) {
  EXPECT_RPC("ReportWork")
    .With(Request<WorkDoneRequest>(
      "result_output : 'hello world' result_ok : true result_k : 6 "
        "result_n : 123456"))
    .WillOnce(RespondWith<EmptyMessage>());
  seeker_->ReportWork("hello world", 6, 123456, true);
}
#if 0
TEST_F(RpsnetClientTest, SeekFortune_LogsRPCError) {
  ScopedMemoryLog log;

  EXPECT_RPC("TellFortune")
    .Times(AtLeast(1))  // We're OK with the client retrying the
                        // TellFortune call, so no upper bound.
    .WillRepeatedly(FailWith(RPC::UNREACHABLE));
  string output;
  // call with arbitrary but valid parameters
  ASSERT_FALSE(seeker_->GetWork("myvm5", &output));

  const vector<string>& messages = log.GetMessages(ERROR);
  ASSERT_EQ(1, messages.size());
  ASSERT_TRUE(HasSuffixString(messages[0], "UNREACHABLE"));
}

TEST_F(RpsnetClientTest, GetWork_LogsApplicationError) {
  ScopedMemoryLog log;

  EXPECT_RPC("GetWork")
    // Allow only one call; client should not retry on application error.
    .WillOnce(FailWithApplicationError(Error::INVALID_RESULT,
                                   "invalid birth month/day"));
  string output;
  // valid month/day to ensure we call server
  ASSERT_FALSE(seeker_->GetWork("myvm4", &output));

  const vector<string>& messages = log.GetMessages(ERROR);
  ASSERT_EQ(1, messages.size());
  ASSERT_TRUE(HasSuffixString(messages[0], "invalid birth month/day"));
}
#endif

// We must run all death tests first, since a non-death test that invokes the
// mock service will leave utility threads running that will make further death
// tests impossible.  By naming our test suite with a name ending in "DeathTest"
// gUnit knows to run it before other tests.
// If the death tests fail for you and you're running on Goobuntu, you probably
// want to take /usr/lib/debug out of your LD_LIBRARY_PATH (see buganizer issue
// 603021).
class RpsnetClientDeathTest : public testing::Test {
 protected:
  virtual void SetUp() {
    seeker_.reset(new RpsnetClientSync(
        service_mocker_.CreateMockService<RpsNet>()));
  }

  scoped_ptr<RpsnetClientSync> seeker_;
  rpc::testing::ServiceMocker service_mocker_;
};

#if 0
TEST_F(RpsnetClientDeathTest, ReportWork_DiesOnRPCError) {
  EXPECT_DEATH(
    { // NOLINT
      EXPECT_RPC("ReportWork")
        .Times(AtLeast(1))
        .WillRepeatedly(FailWith(RPC::SERVER_ERROR));
      seeker_->ReportWork("hello world", 1, 2, true);
    }, "SERVER_ERROR");
}
#endif
}  // namespace
