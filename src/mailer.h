
#ifndef SPAMSERVER_MAILER_H
#define SPAMSERVER_MAILER_H

#include <vmime/vmime.hpp>

class Mailer_t {
public:
    class Message_t {
    friend class Mailer_t;
    public:
        Message_t();

        void setSender(const std::string &address);
        void addRecipient(const std::string &address);
        void addCCRecipient(const std::string &address);
        void addBCCRecipient(const std::string &address);
        void setSubject(const std::string &subject);
        void setBody(const std::string &plain,
                     const std::string &html = "");
        void addAttachment(const std::string &data,
                           const std::string &contentType,
                           const std::string &filename = "",
                           const std::string &description = "");

    protected:
        vmime::messageBuilder messageBuilder;
    };

    Mailer_t(const std::string &smtpAddress,
             const int smtpPort);

    void send(Message_t &message);

protected:
    vmime::ref<vmime::net::session> session;
    std::string smtpAddress;
    int smtpPort;
};

#endif // SPAMSERVER_MAILER_H

