#include "gtest/gtest.h"

#include "libear_a/Storage.h"
#include "libear_a/Session.h"
#include "libear_a/Environment.h"

namespace {

    struct Environment : public ::testing::Test {
        ::ear::LibrarySession session;

        Environment() noexcept
                : session()
        { }
    };

    TEST_F(Environment, dont_crash_on_nullptr) {
        EXPECT_EQ(nullptr, ::ear::environment::capture(session, nullptr));
    }

    TEST_F(Environment, returns_nullptr_when_missing) {
        const char *envp[] = { "this=is", "these=are", nullptr };

        EXPECT_EQ(nullptr, ::ear::environment::capture(session, envp));
    }

    TEST_F(Environment, capture_correct_env_values) {
        const char *envp[] = {
                "EAR_REPORT_DESTINATION=/tmp/intercept.random",
                "EAR_SESSION_LIBRARY=/usr/libexec/libear.so",
                "EAR_REPORT_COMMAND=/usr/bin/intercept",
                nullptr
        };

        EXPECT_EQ(&session, ::ear::environment::capture(session, envp));

        EXPECT_STREQ("/tmp/intercept.random", session.session.destination);
        EXPECT_STREQ("/usr/libexec/libear.so", session.library);
        EXPECT_STREQ("/usr/bin/intercept", session.session.reporter);
    }

}