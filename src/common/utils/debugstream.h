/*
 * Project Name: DCIS - Drone Collective Intelligence System
 * Copyright (C) 2022 Artyom Lazyan, Agit Atashyan, Davit Hayrapetyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DCIS_COMMON_UTILS_DEBUGSTREAM_H_
#define DCIS_COMMON_UTILS_DEBUGSTREAM_H_

#include <QDebug>
// App includes

// Qt includes
#include <QString>
#include <QTextEdit>

namespace dcis::common::utils
{

/*enum class LogLevel : uint32_t { Error = 1, Warning, Info, Debug, Verbose };

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
        if ( level <= logLevel_ )
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
*/
} // end namespace dcis::common::utils
#endif // DCIS_COMMON_UTILS_DEBUGSTREAM_H_
