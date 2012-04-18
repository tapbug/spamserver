
#ifndef MAGIC_H
#define MAGIC_H

#include <string>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include <magic.h>

class Magic_t {
public:
    Magic_t()
      : magic(0)
    {
        magic = magic_open(MAGIC_MIME_TYPE);
        if (!magic) {
            throw std::runtime_error((std::string("Can't initialise libmagic: ") + strerror(errno)).c_str());
        }
        if (magic_load(magic, 0) == -1) {
            std::string errorMessage("Can't load magic database: ");
            errorMessage += magic_error(magic);
            magic_close(magic);
            throw std::runtime_error(errorMessage.c_str());
        }
    }

    ~Magic_t()
    {
        magic_close(magic);
    }

    std::string operator()(const std::string &data)
    {
        const char *result(magic_buffer(magic, data.data(), data.size()));
        if (!result) {
            throw std::runtime_error((std::string("Can't get content type: ") + magic_error(magic)).c_str());
        }
        return result;
    }

private:
    magic_t magic;
};

#endif // MAGIC_H
