/*  Copyright (C) 2012-2017 by László Nagy
    This file is part of Bear.

    Bear is a tool to generate compilation database for clang tooling.

    Bear is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bear is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>

#include "libexec_a/Interface.h"
#include "libexec_a/Environment.h"
#include "libexec_a/Executor.h"
#include "libexec_a/Session.h"
#include "libexec_a/String.h"

namespace {

    struct LibC {
        using execve_t = int (*)(const char *path, char *const argv[], char *const envp[]);
        static execve_t resolve_execve() noexcept {
            return &execve;
        }
    };

    using Executor = ::ear::Executor<LibC>;

    /// It's a C++ compiler if the name ends with "++".
    bool is_cxx(const char * name) noexcept {
        const size_t length = ::ear::string::length(name);
        return (length > 2) && (name[length - 1] == '+') && (name[length - 2] == '+');
    }
}


int main(int argc, char *argv[], char *envp[]) {
    ::ear::WrapperSession session {};
    ::ear::WrapperSession const *session_ptr =
            ::ear::environment::capture(session, const_cast<const char **>(envp));
    if (session_ptr == nullptr)
        return -1;

    // Replace the compiler wrapper to the real compiler.
    if (argc <= 0)
        return -1;
    argv[0] = const_cast<char *>((is_cxx(argv[0])) ? session.cxx : session.cc);

    const Executor executor(&session.session);
    return executor.execve(argv[0], argv, envp);
}
