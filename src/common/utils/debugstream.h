#ifndef DCIS_COMMON_UTILS_DEBUGSTREAM_H_
#define DCIS_COMMON_UTILS_DEBUGSTREAM_H_

// App includes

// Qt includes
#include <QString>
#include <QTextEdit>

namespace dcis::common::utils {

enum class LogLevel : uint32_t { Error = 1, Warning, Info, Debug, Verbose };

class DebugStream
{
public:
    DebugStream() = default;
    ~DebugStream() = default;
    DebugStream(const DebugStream&) = delete;
    DebugStream& operator = (const DebugStream&) = delete;

    void setEditor(QTextEdit* textEdit)
    {
        logWindowWidget_ = textEdit;
    }

    void setLogLevel(LogLevel level)
    {
        logLevel_ = level;
    }

    void log(LogLevel level, const QString& text)
    {
        if (level >= logLevel_)
        {
            logWindowWidget_->append("=> " + text);
        }
    }

    static DebugStream& getInstance()
    {
      static DebugStream instance;
      return instance;
    }

private:
    QTextEdit* logWindowWidget_;
    LogLevel logLevel_ = LogLevel::Verbose;
};

} // end namespace dcis::common::utils
#endif // DCIS_COMMON_UTILS_DEBUGSTREAM_H_
