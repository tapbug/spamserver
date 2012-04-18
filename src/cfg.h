
#ifndef CFG_H
#define CFG_H

#include <threadserver/threadserver.h>

/**
* Server configuration class
**/
class Config_t {
public:
    /**
    * Constructor
    * Parameters:
    *   threadServer    Reference to ThreadServer instance
    **/
    Config_t(ThreadServer::ThreadServer_t &threadServer);

private:
    /**
    * Get mandatory value from configuration file
    * Parameters:
    *   name    Variable name in format section.VariableName
    *           spamserver:: prefix is automatically added to section name
    * Returns value as desired type
    **/
    template<class T_t>
    T_t get(const std::string &name)
    {
        return threadServer.configuration.get<T_t>("spamserver::" + name);
    }

    /**
    * Get mandatory boolean value from configuration file
    * Parameters:
    *   name    Variable name in format section.VariableName
    *           spamserver:: prefix is automatically added to section name
    * Returns value as boolean
    **/
    bool getBool(const std::string &name)
    {
        std::string value(threadServer.configuration.get<std::string>("spamserver::" + name));
        if (value == "true" || value == "on" || value == "1") {
            return true;
        } else if (value == "false" || value == "off" || value == "0") {
            return false;
        } else {
            throw std::runtime_error(
                (std::string("Invalid configuration value '")
                    + value
                    + "' for boolean variable spamserver::"
                    + name).c_str());
        }
    }

    /**
    * Get optional value from configuration file
    * Parameters:
    *   name            Variable name in format section.VariableName
    *                   spamserver:: prefix is automatically added to section name
    *   defaultValue    Default value used, when variable is not present
    * Returns value as desired type
    **/
    template<class T_t>
    T_t get(const std::string &name, const T_t &defaultValue)
    {
        return threadServer.configuration.get<T_t>("spamserver::" + name, defaultValue);
    }

    /**
    * Get optional boolean value from configuration file
    * Parameters:
    *   name            Variable name in format section.VariableName
    *                   spamserver:: prefix is automatically added to section name
    *   defaultValue    Default value used, when variable is not present
    * Returns value as boolean
    **/
    bool getBool(const std::string &name, const bool defaultValue)
    {
        std::string value(threadServer.configuration.get<std::string>("spamserver::" + name, defaultValue ? "true" : "false"));
        if (value == "true" || value == "on" || value == "1") {
            return true;
        } else if (value == "false" || value == "off" || value == "0") {
            return false;
        } else {
            throw std::runtime_error(
                (std::string("Invalid configuration value '")
                    + value
                    + "' for boolean variable spamserver::"
                    + name).c_str());
        }
    }

    /**
    * Get value vector from configuration file
    * Parameters:
    *   name    Variable name in format section.VariableName
    *           spamserver:: prefix is automatically added to section name
    * Returns values as vector of desired type
    **/
    template<class T_t>
    std::vector<T_t> getVector(const std::string &name)
    {
        return threadServer.configuration.getVector<T_t>("spamserver::" + name);
    }

public:
    /**
    * SMTP server settings
    **/
    struct {
        /** SMTP server hostname/IP address **/
        std::string address;

        /** SMTP server port **/
        uint16_t port;
    } smtp;

protected:
    /** Reference to ThreadServer instance **/
    ThreadServer::ThreadServer_t &threadServer;
};

#endif // CFG_H

