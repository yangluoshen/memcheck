#include <fstream>
#include <sstream>
#include <ctime>
#include <unistd.h>

#include "loghandle.h"

LogHandle& LogHandle::get_instance()
{
    static LogHandle s_instance;
    return s_instance;
}

void LogHandle::log_consist(enum log_level level, const std::string& content, std::string& log)
{
    (void)get_systime(log);
    switch(level)
    {
        case LOG_INFO:
            log.append(":[I] ");
            break;
        case LOG_DEBUG:
            log.append(":[D] ");
            break;
        case LOG_ERROR:
            log.append(":[E] ");
            break;
        default:
            break;        
    }

    log.append(content);

    return;
}

void LogHandle::log_print(const std::string& log)
{
    if (log_file_name.empty())
    {
        get_logname(this->log_file_name); 
    }
    
    std::ofstream fout(this->log_file_name.c_str(), std::ofstream::out | std::ofstream::app);
    if (!fout.is_open())
    {
        return;
    }

    std::string log_buf(log);
    log_buf.append("\r\n");

    fout.write(log_buf.c_str(), log_buf.size());

    fout.close();
    return;
}

void LogHandle::log_debug(const std::string& content)
{
    std::string log;
    log_consist(LOG_DEBUG, content, log);   
    log_print(log);
}

void LogHandle::log_error(const std::string& content)
{
    std::string log;
    log_consist(LOG_ERROR, content, log);
    log_print(log);
    return;
}

void LogHandle::get_logname(std::string& log_name)
{
    pid_t pid = getpid();
    std::ostringstream os("");
    os << pid;
    os << ".log";

    log_name.append("/tmp/fizz.memcheck.");
    log_name.append(os.str());
    return;
}

void LogHandle::get_systime(std::string& now)
{
    time_t now_time;
    now_time = time(NULL);
    char time_buf[20] = {0}; //buffer of the string time ,20 is enough
    strftime(time_buf, sizeof (time_buf), "%H:%M:%S", localtime(&now_time));
    
    now.assign(time_buf);
    return;
}

void LogHandle::log_head_info(LogHeadInfo head_info)
{
    std::string output("/**************************\n");
    output.append("* leak num: ");

    std::ostringstream num_of_leak("");
    num_of_leak << head_info.leak_sum;
    output.append(num_of_leak.str());

    output.append("\n************************/\n\n");

    log_print(output);
}

