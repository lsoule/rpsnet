# -*- mode: python; -*-

# Description: Sample code for Stubby codelab.
# =========================================================

# Interface definitions

proto_library(name = "rpsnet_proto",
              cc_api_version = 2,
              py_api_version = 2,
              java_api_version = 1,
              has_services = 1,
              srcs = [ "rpsnet.proto" ])

# Server code

cc_library(name = "rpsnet_impl",
           srcs = [ "rpsnet_impl.h",
                    "rpsnet_impl.cc",
                  ],
           deps = [ ":rpsnet_proto",
                    "//base",
                    "//file/base",
                    "//net/http",
                    "//net/rpc2:rpc2_noloas",
                    "//thread:thread",
                    "//util/random",
                    "//util/time:clock",
                    "//net/proto2/bridge/public:stubby_glue",                   
                  ])

cc_binary(name = "rpsnet",
          linkstatic = 1,
          linkopts = [ "-static" ],
          srcs = [ "rpsnet.cc"],
          deps = [ ":rpsnet_impl",
                   "//base",
                   "//net/base",
                   "//net/http",
                 ])

cc_test(name = "rpsnet_test",
        size = "small",
        srcs = [ "rpsnet_test.cc" ],
        data = [ "testpgen" ],
        deps = [ ":rpsnet_impl",
                 "//base",
                 "//net/rpc/testing:donechecker",
                 "//testing/base/public:gunit_main",
                 "//util/time:clock",
               ])

# Client code

cc_library(name = "rpsnet_client_impl",
           srcs = [ "rpsnet_client.h",
                    "rpsnet_clientsync.cc",
                    "rpsnet_clientutil.cc",
                  ],
           deps = [ ":rpsnet_proto",
                    "//base",
                    "//net/rpc2:rpc2_noloas",
                    "//strings",
                  ])

cc_binary(name = "rpsnet_client",
          linkstatic = 1,
          linkopts = [ "-static" ],
          srcs = [ "rpsnet_client.cc" ],
          deps = [ ":rpsnet_client_impl",
                   "//base",
                   "//net/rpc2:rpc2_noloas",
                   "//util/time:walltime",
                   "//net/proto2/bridge/public:stubby_glue",
                 ])

cc_test(name = "rpsnet_clientsync_test",
        size = "small",
        srcs = [ "rpsnet_clientsync_test.cc" ],
        deps = [ ":rpsnet_client_impl",
                 "//base",
                 "//net/rpc/testing/public:servicemocker",
                 "//strings",
                 "//testing/base/public:gunit_main",
               ])


