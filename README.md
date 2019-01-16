LEAF is a C++11 error handling library.

Official documentation: https://zajo.github.io/leaf/

Features:

* Header-only, no dependencies.

* No dynamic memory allocations (except when error objects are transported between threads).
* Any error-related object of any movable type is efficiently delivered to the correct error handler.
* Compatible with `std::error_code`, `errno` and any other error code type.
* Support for multi-thread programming.
* Can be used with or without exception handling.

Copyright (c) 2018-2019 Emil Dotchevski. Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

Please post questions and feedback on the Boost Developers Mailing List (LEAF is not part of Boost).
