#pragma once

#include <stdexcept>

#include "logging.h"

class Global {
    public:
        static Logger& logging()
        {
            if (internal_logger_ptr == nullptr)
            {
                throw new std::runtime_error("Attempt to log something before a logging instance was created!");
            }
            return *internal_logger_ptr;
        }
        static void provide_logging(Logger *new_logger) { internal_logger_ptr = new_logger; }

    private:
        static Logger *internal_logger_ptr;
};