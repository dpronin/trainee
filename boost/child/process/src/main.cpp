#include <iostream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <initializer_list>
#include <thread>
#include <chrono>
#include <utility>

#include <boost/asio/io_service.hpp>
#include <boost/filesystem.hpp>
#include <boost/process/async.hpp>
#include <boost/process/child.hpp>
#include <boost/process/pipe.hpp>
#include <boost/process/io.hpp>

using namespace std::literals;

class Process
{
public:
    using ProcessExitHandlerType = std::function<void(int, std::error_code)>;

    explicit Process(boost::filesystem::path const &exe_path, ProcessExitHandlerType process_exit_handler = nullptr)
        : exe_path_(std::move(exe_path))
        , process_exit_handler_(process_exit_handler
            ? std::move(process_exit_handler)
            : std::bind(&Process::ProcessExitHandler, this, std::placeholders::_1, std::placeholders::_2))
    {
        if (exe_path_.empty())
            throw std::invalid_argument("exe_path cannot be empty");
    }

    ~Process()
    {
        Stop();
    }

    boost::filesystem::path const& exe_path() const noexcept { return exe_path_; }

    std::vector<std::string> const& exe_args() const noexcept { return exe_args_; }

    std::string process_cmd_line() const
    {
        std::ostringstream oss;
        oss << exe_path_.generic_string() << " ";
        std::copy(exe_args_.begin(), exe_args_.end(), std::ostream_iterator<std::string>(oss, " "));
        return oss.str();
    }

    void AppendArguments(std::initializer_list<std::string> const &args)
    {
        exe_args_.insert(exe_args_.end(), args);
    }

    std::error_code Spawn(boost::process::opstream &pipe) noexcept
    {
        std::error_code ec;

        if (Joinable())
            return ec;

        std::cout << "spawning process '" << process_cmd_line() << "'" << std::endl;

        try
        {
            child_process_ = boost::process::child(exe_path_, exe_args_, io_service_, boost::process::on_exit = process_exit_handler_, boost::process::std_in < pipe);
        }
        catch (std::exception const &ex)
        {
            std::cerr << "failed launching process '" << process_cmd_line() << "', reason: " << ex.what() << std::endl;
            return ec;
        }

        std::cout << "successfully launched process '" << process_cmd_line() << "' with PID #" << child_process_.id() << std::endl;;

        io_service_thread_ = std::thread([this]{ IoServiceThread(); });

        return ec;
    }

    void Stop() noexcept
    {
        if (!Joinable())
            return;

        std::cout << "stopping process '" << process_cmd_line() << "' with PID #" << child_process_.id() << std::endl;
        io_service_.stop();
        io_service_thread_.join();

        auto constexpr kChildProcessTimeout = 30s;
        // @note how to cleanly stop a process ??

        // child_process_.wait();
        if (!child_process_.wait_for(kChildProcessTimeout))
        {
            std::cerr << "process '" << process_cmd_line() << "' with PID #" << child_process_.id()
                  << " hasn't exited within timeout " << kChildProcessTimeout.count() << " seconds. Terminating..." << std::endl;
            Terminate();
        }

        std::cout << "process '" << process_cmd_line() << "' with PID #" << child_process_.id() << " finished" << std::endl;
    }

    bool Joinable()
    {
        return child_process_.joinable() && io_service_thread_.joinable();
    }

    bool Running() noexcept
    {
        return child_process_.running();
    }

    void Join()
    {
        if (Joinable())
            child_process_.join();
    }

protected:
    void ProcessExitHandler(int exit_code, std::error_code ec)
    {
        std::cout << "process '" << process_cmd_line() << "' exited, exit-code #" << exit_code << std::endl;
        if (ec)
            std::cerr << "process '" << process_cmd_line() << "' failed with error-code #" << ec << std::endl;
    }

private:
    void Terminate()
    {
        if (!Joinable())
            return;

        std::cout << "terminating process '" << process_cmd_line() << "' with PID #" << child_process_.id() << std::endl;

        std::error_code ec;
        child_process_.terminate(ec);

        if (ec)
        {
            std::cerr << "couldn't terminate process '" << process_cmd_line() << "', reason: " << ec << std::endl;
        }
        else
        {
            std::cout << "process '" << process_cmd_line() << "' with PID #" << child_process_.id() << " terminated" << std::endl;
        }
    }

    void IoServiceThread()
    {
        std::cout << "io-service run" << std::endl;
        io_service_.run();
        std::cout << "io-service finished" << std::endl;
    }

private:
    boost::filesystem::path const exe_path_;
    ProcessExitHandlerType const process_exit_handler_;

    std::vector<std::string> exe_args_;

    std::thread io_service_thread_;
    boost::asio::io_service io_service_;

    boost::process::child child_process_;
};

int main(int argc, const char** argv)
{
    using namespace std::literals;

    std::cout << "Hello, world!" << std::endl;

    Process p("./program");
    std::unique_ptr<boost::process::opstream> pipe(new boost::process::opstream());
    if (auto const ec = p.Spawn(*pipe))
    {
        std::cerr << "couldn't start process '" << p.process_cmd_line() << "'";
        return ec.value();
    }

    std::this_thread::sleep_for(3s);
    pipe.reset();
    std::cerr << "stop reset" << std::endl;
    p.Join();

    return 0;
}
