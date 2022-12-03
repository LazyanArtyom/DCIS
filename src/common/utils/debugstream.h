#ifndef DCIS_COMMON_UTILS_DEBUGSTREAM_H_
#define DCIS_COMMON_UTILS_DEBUGSTREAM_H_

// App includes
#include <string>
#include <vector>
#include <iostream>
#include <streambuf>

// Qt includes
#include <QTextEdit>

/*
namespace dcis::common::utils {

namespace {

enum class LogLevel {
    ERROR = 1, INFO, DEBUG, VERBOSE
};

} // end namespace

template <typename... Ts>
void log(LogLevel level, Ts&&... args)
{
    if (level <= LogLevel::VERBOSE)
    {
        std::cout << "=> ";
        (std::cout << ... << args);
        std::cout << std::endl;
    }
}

class DebugStream : public std::basic_streambuf<char>
{
public:

    DebugStream(std::ostream &stream, QTextEdit *textEdit) : stream_(stream)
    {
        logWindowWidget_ = textEdit;
        oldBuf_ = stream.rdbuf();
        stream.rdbuf(this);
    }

    ~DebugStream() override
    {
        if (!string_.empty())
        {
            logWindowWidget_->append(string_.c_str());
        }

        stream_.rdbuf(oldBuf_);
    }

protected:
    int_type overflow(int_type type) override
    {
        if (type == '\n')
        {
            logWindowWidget_->append(string_.c_str());
            string_.erase(string_.begin(), string_.end());
        }
        else
        {
            string_ += std::to_string(type);
        }

        return type;
    }

    std::streamsize xsputn(const char *p, std::streamsize size) override
    {
        string_.append(p, p + size);
        size_t pos = 0;
        while (pos != (size_t) std::string::npos)
        {
            pos = string_.find('\n');
            if (pos != (size_t) std::string::npos)
            {
                std::string tmp(string_.begin(), string_.begin() + pos);
                logWindowWidget_->append(tmp.c_str());
                string_.erase(string_.begin(), string_.begin() + pos + 1);
            }
        }

        return size;
    }

private:
    std::string    string_;
    std::ostream&  stream_;
    std::streambuf* oldBuf_;

    QTextEdit* logWindowWidget_;
};
} // end namespace dcis::common::utils

*/
#endif // DCIS_COMMON_UTILS_DEBUGSTREAM_H_
