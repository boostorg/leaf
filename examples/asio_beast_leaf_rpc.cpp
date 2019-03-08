// Copyright (c) 2019 Sorin Fetche

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// PLEASE NOTE: At the time of this writing, this file requires the latest develop branch of both
// Boost Asio and Boost Beast. It will probably work with version 1.70 of the official Boost distro.
// To build with an older Boost version, the newer Boost Asio and Boost Beast include paths must
// appear before the Boost include path passed to the compiler.

// Example of a composed asynchronous operation which uses the LEAF library for error handling and reporting.
//
// An example of running:
// - in one terminal: ./asio_beast_leaf_demo_rpc.exe 0.0.0.0 8080
// - in another: telnet localhost 8080
//      sum 0 1 2 3
//      div 1 0
//      mod 1
//      quit
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/async_base.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/format.hpp>
#include <boost/leaf/all.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <deque>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace leaf = boost::leaf;
namespace net = boost::asio;

namespace {

using error_code = boost::system::error_code;

template <typename CharRange> leaf::result<std::pair<std::string, bool>> execute_command(CharRange const &line);
template <typename CharRange> decltype(auto) make_execute_command_error_handler_all();

// A composed asynchronous operation that implements a basic remote calculator.
// It receives from the remote side commands such as:
//      sum 1 2 3
//      div 3 2
//      mod 1 0
// and then sends back the result.
//
// From the error handling perspective there are three parts of the implementation which are more or less decoupled:
// - the execution of the command we receive which knows of the error conditions this can lead to
//      (see execute_command())
// - the handling of these error condtions by constructing a response message suitable to be sent to the remote side
//      (see make_execute_command_error_handler_all())
// - this composed asynchronous operation which calls them (and sets the actual type for the range of characters
//      they will use)
// .
// It is based on:
// https://github.com/boostorg/beast/blob/b02f59ff9126c5a17f816852efbbd0ed20305930/example/echo-op/echo_op.cpp#L1
// https://github.com/chriskohlhoff/asio/blob/e7b397142ae11545ea08fcf04db3008f588b4ce7/asio/src/examples/cpp11/operations/composed_5.cpp
template <class AsyncStream, class DynamicReadBuffer, class DynamicWriteBuffer, typename CompletionToken>
auto async_demo_rpc(AsyncStream &stream, DynamicReadBuffer &read_buffer, DynamicWriteBuffer &write_buffer,
                    CompletionToken &&token) ->
    typename net::async_result<typename std::decay<CompletionToken>::type, void(error_code)>::return_type {

    static_assert(beast::is_async_stream<AsyncStream>::value, "AsyncStream requirements not met");
    static_assert(net::is_dynamic_buffer<DynamicReadBuffer>::value, "DynamicBuffer type requirements not met");
    static_assert(net::is_dynamic_buffer<DynamicWriteBuffer>::value, "DynamicBuffer type requirements not met");

    using read_buffers_type = typename DynamicReadBuffer::const_buffers_type;
    using read_buffers_iterator = net::buffers_iterator<read_buffers_type>;
    using read_buffers_range = boost::iterator_range<read_buffers_iterator>;
    using execute_error_handler_t = decltype(make_execute_command_error_handler_all<read_buffers_range>());

    using write_buffers_type = typename DynamicWriteBuffer::mutable_buffers_type;
    using write_buffers_iterator = net::buffers_iterator<write_buffers_type>;

    using handler_type = typename net::async_completion<CompletionToken, void(error_code)>::completion_handler_type;
    using base_type = beast::async_base<handler_type, beast::executor_type<AsyncStream>>;
    struct internal_op : base_type {
        AsyncStream &m_stream;
        DynamicReadBuffer &m_read_buffer;
        DynamicWriteBuffer &m_write_buffer;
        execute_error_handler_t m_execute_error_handler;
        bool m_write_and_quit;

        internal_op(AsyncStream &stream, DynamicReadBuffer &read_buffer, DynamicWriteBuffer &write_buffer,
                    handler_type &&handler)
            : base_type{std::move(handler), stream.get_executor()}, m_stream{stream}, m_read_buffer{read_buffer},
              m_write_buffer{write_buffer},
              m_execute_error_handler{make_execute_command_error_handler_all<read_buffers_range>()}, m_write_and_quit{
                                                                                                         false} {

            // Simulating that we received an empty line so that we send the "help" message as soon as a client connects
            using mutable_iterator = net::buffers_iterator<typename DynamicReadBuffer::mutable_buffers_type>;
            auto b = m_read_buffer.prepare(1);
            auto i = mutable_iterator::begin(b);
            [[maybe_unused]] auto end = mutable_iterator::end(b);
            *i++ = '\n';
            assert(i == end);
            (*this)({}, 1, false);
        }

        void operator()(error_code ec, std::size_t bytes_transferred = 0, bool is_continuation = true) {
            if (!ec) {
                if (m_write_buffer.size() == 0) {
                    // We read something.
                    m_read_buffer.commit(bytes_transferred);
                    std::size_t pos_nl = find_newline(m_read_buffer.data());
                    if (pos_nl == 0) {
                        // Read some more until we get a newline
                        return start_read_some();
                    }

                    // Process the line we received.
                    auto line_begin = read_buffers_iterator::begin(m_read_buffer.data());
                    auto line_end = line_begin + pos_nl;
                    read_buffers_range line{line_begin, line_end};
                    std::string response = leaf::remote_try_handle_all(
                        [line, this]() -> leaf::result<std::string> {
                            LEAF_AUTO(pair_response_quit, execute_command(line));
                            m_write_and_quit = pair_response_quit.second;
                            return std::move(pair_response_quit.first);
                        },
                        [&](leaf::error_info const &error) { return m_execute_error_handler(error); });
                    // After processing and/or error handling we can consume the read buffer.
                    m_read_buffer.consume(pos_nl);

                    // Prepare the response buffer
                    // (including fixing the newlines to be '\r\n' for remote telnet clients)
                    response.push_back('\n');
                    boost::algorithm::erase_all(response, "\r");
                    boost::algorithm::replace_all(response, "\n", "\r\n");

                    std::size_t write_size = response.size();
                    auto response_buffers = m_write_buffer.prepare(write_size);
                    std::copy(response.begin(), response.end(), write_buffers_iterator::begin(response_buffers));
                    m_write_buffer.commit(write_size);
                    return net::async_write(m_stream, response_buffers, std::move(*this));
                }

                // If getting here, we completed a write operation.
                assert(m_write_buffer.size() == bytes_transferred);
                m_write_buffer.consume(bytes_transferred);
                // And start reading a new message if not quitting.
                if (!m_write_and_quit) {
                    return start_read_some();
                }
            }
            // Operation complete if we get here.
            this->complete(is_continuation, ec);
        }

        void start_read_some() {
            std::size_t bytes_to_read = 3; // A small value for testing purposes
                                           // (multiple `operator()` calls for most messages)
            m_stream.async_read_some(m_read_buffer.prepare(bytes_to_read), std::move(*this));
        }

        // Same as:
        // https://github.com/boostorg/beast/blob/c82237512a95487fd67a4287f79f4458ba978f43/example/echo-op/echo_op.cpp#L199
        static std::size_t find_newline(read_buffers_type const &buffers) {
            auto begin = read_buffers_iterator::begin(buffers);
            auto end = read_buffers_iterator::end(buffers);
            auto result = std::find(begin, end, '\n');
            if (result == end) {
                return 0;
            }
            return result + 1 - begin;
        }
    };

    auto initiation = [](auto &&completion_handler, AsyncStream *stream, DynamicReadBuffer *read_buffer,
                         DynamicWriteBuffer *write_buffer) {
        internal_op op{*stream, *read_buffer, *write_buffer,
                       std::forward<decltype(completion_handler)>(completion_handler)};
    };

    return net::async_initiate<CompletionToken, void(error_code)>(initiation, token, &stream, &read_buffer,
                                                                  &write_buffer);
}

// The location of a int64 parse error.
// It refers the range of characters from which the parsing was done.
template <typename CharRange> struct e_parse_int64_error {
    using location_base = std::pair<CharRange const, typename boost::range_iterator<CharRange>::type>;
    struct location : public location_base {
        using location_base::location_base;

        friend std::ostream &operator<<(std::ostream &os, location const &value) {
            std::string s{boost::begin(value.first), boost::end(value.first)};
            std::string_view sv{s};
            std::size_t pos = std::distance(boost::begin(value.first), value.second);
            if (pos == 0) {
                os << "->\"" << sv << "\"";
            } else if (pos < sv.size()) {
                os << "\"" << sv.substr(0, pos) << "\"->\"" << sv.substr(pos) << "\"";
            } else {
                os << "\"" << sv << "\"<-";
            }
            return os;
        }
    };

    location value;
};

// Parses an integer from a range of characters.
template <typename CharRange> leaf::result<std::int64_t> parse_int64(CharRange const &word) {
    [[maybe_unused]] auto const begin = boost::begin(word);
    [[maybe_unused]] auto const end = boost::end(word);
    std::int64_t value = 0;
    auto i = begin;
    bool result = boost::spirit::qi::parse(i, end, boost::spirit::long_long, value);
    if (!result || i != end) {
        return leaf::new_error(e_parse_int64_error<CharRange>{std::make_pair(word, i)});
    }
    return value;
}

// The command being executed while we get an error.
// It refers the range of characters from which the command was extracted.
template <typename CharRange> struct e_command { CharRange value; };

// The details about an incorrect number of arguments error
// Some commands may accept a variable number of arguments (e.g. greater than 1 would mean [2, SIZE_MAX]).
struct e_unexpected_arg_count {
    struct arg_info {
        std::size_t count;
        std::size_t min;
        std::size_t max;

        friend std::ostream &operator<<(std::ostream &os, arg_info const &value) {
            os << value.count << " (required: ";
            if (value.min == value.max) {
                os << value.min;
            } else if (value.max < SIZE_MAX) {
                os << "[" << value.min << ", " << value.max << "]";
            } else {
                os << "[" << value.min << ", MAX]";
            }
            os << ")";
            return os;
        }
    };

    arg_info value;
};

struct e_error_quit {
    struct none_t {};
    none_t value;
};

// Processes a remote command.
// Returns the response and a flag indicating this is the last command to execute.
template <typename CharRange> leaf::result<std::pair<std::string, bool>> execute_command(CharRange const &line) {
    // Split the command in words.
    // Note that split() doesn't elimintate leading and trailing empty substrings.
    std::deque<CharRange> words; // or std::list<CharRange> words;

    boost::algorithm::split(words, line, boost::is_any_of("\t \r\n"), boost::algorithm::token_compress_on);
    while (!words.empty() && boost::empty(words.front())) {
        words.pop_front();
    }
    while (!words.empty() && boost::empty(words.back())) {
        words.pop_back();
    }

    static char const *const help = "Help:\n"
                                    "    quit                        End the session\n"
                                    "    error-quit                  Simulated error to end the session\n"
                                    "    sum <int64>*                Addition\n"
                                    "    sub <int64>+                Substraction\n"
                                    "    mul <int64>*                Multiplication\n"
                                    "    div <int64>+                Division\n"
                                    "    mod <int64> <int64>         Remainder\n"
                                    "    <anything else>             This message";

    if (words.empty()) {
        return std::make_pair(std::string{help}, false);
    }

    auto command = words.front();
    words.pop_front();

    auto load_cmd = leaf::preload(e_command<CharRange>{command});
    std::string response;
    bool quit = false;

    // We need to compare the command (a boost::iterator_range of chars) against null terminated strings converted to
    // std::string_view so they are proper ranges. We use the string_view literal for this conversion.
    using namespace std::literals::string_view_literals;
    if (command == "quit"sv) {
        response = "quitting";
        quit = true;
    } else if (command == "error-quit"sv) {
        return leaf::new_error(e_error_quit{});
    } else if (command == "sum"sv) {
        std::int64_t sum = 0;
        for (auto const &w : words) {
            LEAF_AUTO(i, parse_int64(w));
            sum += i;
        }
        response = std::to_string(sum);
    } else if (command == "sub"sv) {
        if (words.size() < 2) {
            return leaf::new_error(e_unexpected_arg_count{words.size(), 2, SIZE_MAX});
        }
        LEAF_AUTO(sub, parse_int64(words.front()));
        words.pop_front();
        for (auto const &w : words) {
            LEAF_AUTO(i, parse_int64(w));
            sub -= i;
        }
        response = std::to_string(sub);
    } else if (command == "mul"sv) {
        std::int64_t mul = 1;
        for (auto const &w : words) {
            LEAF_AUTO(i, parse_int64(w));
            mul *= i;
        }
        response = std::to_string(mul);
    } else if (command == "div"sv) {
        if (words.size() < 2) {
            return leaf::new_error(e_unexpected_arg_count{words.size(), 2, SIZE_MAX});
        }
        LEAF_AUTO(div, parse_int64(words.front()));
        words.pop_front();
        for (auto const &w : words) {
            LEAF_AUTO(i, parse_int64(w));
            if (i == 0) {
                // In some cases this command execution function might throw, not just return an error.
                throw std::runtime_error{"division by zero"};
            }
            div /= i;
        }
        response = std::to_string(div);
    } else if (command == "mod"sv) {
        if (words.size() != 2) {
            return leaf::new_error(e_unexpected_arg_count{words.size(), 2, 2});
        }
        LEAF_AUTO(i1, parse_int64(words.front()));
        words.pop_front();
        LEAF_AUTO(i2, parse_int64(words.front()));
        words.pop_front();
        if (i2 == 0) {
            // In some cases this command execution function might throw, not just return an error.
            throw leaf::exception(std::runtime_error{"division by zero"});
        }
        response = std::to_string(i1 % i2);
    } else {
        response = help;
    }

    return std::make_pair(response, quit);
}

// Creates an error handler for errors coming out of `execute_command`.
// It constructs a response message to send back to the remote client.
// It uses leaf::remote_handle_all (see https://zajo.github.io/leaf/#remote_try_handle_all)
template <typename CharRange> decltype(auto) make_execute_command_error_handler_all() {
    using e_command_r = e_command<CharRange>;
    using e_parse_int64_error_r = e_parse_int64_error<CharRange>;

    auto e_prefix = [](e_command_r const *cmd) {
        if (cmd != nullptr) {
            return boost::str(boost::format("Error (%1%):") % cmd->value);
        }
        return std::string("Error:");
    };
    auto diag_s = [](leaf::verbose_diagnostic_info const &diag) {
        auto str = boost::str(boost::format("%1%") % diag);
        boost::algorithm::replace_all(str, "\n", "\n    ");
        return "\nDetailed error diagnostic:\n----\n" + str + "\n----";
    };

    return [e_prefix, diag_s](leaf::error_info const &error) {
        return leaf::remote_handle_all(
            error, [](e_error_quit const &) -> std::string { throw std::runtime_error("error_quit"); },
            [&](e_parse_int64_error_r const &e, e_command_r const *cmd, leaf::verbose_diagnostic_info const &diag) {
                return boost::str(boost::format("%1% int64 parse error: %2%") % e_prefix(cmd) % e.value) + diag_s(diag);
            },
            [&](e_unexpected_arg_count const &e, e_command_r const *cmd, leaf::verbose_diagnostic_info const &diag) {
                return boost::str(boost::format("%1% wrong argument count: %2%") % e_prefix(cmd) % e.value) +
                       diag_s(diag);
            },
            [&](leaf::catch_<std::exception> e, e_command_r const *cmd, leaf::verbose_diagnostic_info const &diag) {
                return boost::str(boost::format("%1% %2%") % e_prefix(cmd) % e.value().what()) + diag_s(diag);
            },
            [&](e_command_r const *cmd, leaf::verbose_diagnostic_info const &diag) {
                return boost::str(boost::format("%1% unknown failure") % e_prefix(cmd)) + diag_s(diag);
            });
    };
}

// A bare-bones on-scope-exit utility.
template <typename F> struct [[nodiscard]] scope_exit {
    explicit scope_exit(F && f) : m_f(std::move(f)) {}
    scope_exit(scope_exit const &) = delete;
    scope_exit(scope_exit &&) = delete;
    scope_exit &operator=(scope_exit const &) = delete;
    scope_exit &operator=(scope_exit &&) = delete;
    ~scope_exit() { m_f(); }
    F const m_f;
};
template <typename F> decltype(auto) on_scope_exit(F &&f) { return scope_exit<F>(std::forward<F>(f)); }

} // namespace

int main(int argc, char **argv) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <address> <port>" << std::endl;
            std::cerr << "Example:\n    " << argv[0] << " 0.0.0.0 8080" << std::endl;
            return -1;
        }

        auto const address{net::ip::make_address(argv[1])};
        auto const port{static_cast<std::uint16_t>(std::atoi(argv[2]))};
        net::ip::tcp::endpoint const endpoint{address, port};

        net::io_context io_context;

        // Set up a worker thread that runs the io_context in background.
        auto threads_io_work = net::make_work_guard(io_context);
        std::thread thread_io_context{[&io_context] {
            try {
                io_context.run();
            } catch (std::exception const &e) {
                std::cerr << "Server-thread error: " << e.what() << std::endl;
            }
        }};
        // And our cleanup work at the end of the scope to stop this thread.
        auto cleanup = on_scope_exit([&] {
            threads_io_work.reset();
            thread_io_context.join();
        });

        // Start the server acceptor and wait for a client.
        net::ip::tcp::acceptor acceptor{io_context, endpoint};
        auto local_endpoint = acceptor.local_endpoint();
        std::cout << "Server: Started on: " << local_endpoint << std::endl;
        std::cout << "Try in a different terminal:\n    telnet " << local_endpoint.address() << " "
                  << local_endpoint.port() << "\n    help<ENTER>" << std::endl;

        auto socket = acceptor.accept();
        std::cout << "Server: Client connected: " << socket.remote_endpoint() << std::endl;

        // Start the `async_demo_rpc` operation and wait for its completion.
        beast::multi_buffer read_buffer; // or beast::flat_buffer read_buffer;
        beast::flat_buffer write_buffer;
        std::future<void> f = async_demo_rpc(socket, read_buffer, write_buffer, net::use_future);
        try {
            f.get();
            std::cout << "Server: Client work completed successfully" << std::endl;
        } catch (std::exception const &e) {
            std::cout << "Server: Client work completed with error: " << e.what() << std::endl;
        }

        // Let the remote side know we are shutting down.
        error_code ignored;
        socket.shutdown(net::ip::tcp::socket::shutdown_both, ignored);

    } catch (std::exception const &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
    return 0;
}
