
#include "mailer.h"
#include <vmime/platforms/posix/posixHandler.hpp>

Mailer_t::Mailer_t(const std::string &smtpAddress,
                   const int smtpPort)
  : session(0),
    smtpAddress(smtpAddress),
    smtpPort(smtpPort)
{
    vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

    session = vmime::create<vmime::net::session>();
    session->getProperties().setProperty("server.tls", true);
}

void Mailer_t::send(Mailer_t::Message_t &message)
{
    std::stringstream urlStream;
    urlStream << "smtp://" << smtpAddress << ":" << smtpPort << "/";

    vmime::ref<vmime::net::transport> transport(
        session->getTransport(
            vmime::utility::url(urlStream.str())));

    transport->connect();

    transport->send(message.messageBuilder.construct());

    transport->disconnect();
}

Mailer_t::Message_t::Message_t()
  : messageBuilder()
{
}

void Mailer_t::Message_t::setSender(const std::string &address)
{
    messageBuilder.setExpeditor(vmime::mailbox(address));
}

void Mailer_t::Message_t::addRecipient(const std::string &address)
{
    messageBuilder.getRecipients().appendAddress(
        vmime::create<vmime::mailbox>(address));
}

void Mailer_t::Message_t::addCCRecipient(const std::string &address)
{
    messageBuilder.getCopyRecipients().appendAddress(
        vmime::create<vmime::mailbox>(address));
}

void Mailer_t::Message_t::addBCCRecipient(const std::string &address)
{
    messageBuilder.getBlindCopyRecipients().appendAddress(
        vmime::create<vmime::mailbox>(address));
}

void Mailer_t::Message_t::setSubject(const std::string &subject)
{
    messageBuilder.setSubject(vmime::text(subject));
}

void Mailer_t::Message_t::setBody(const std::string &plain,
                                  const std::string &html)
{
    messageBuilder.constructTextPart(
        vmime::mediaType(
            vmime::mediaTypes::TEXT, vmime::mediaTypes::TEXT_HTML));

    vmime::ref<vmime::htmlTextPart> textPart(
        messageBuilder.getTextPart().dynamicCast<vmime::htmlTextPart>());

    textPart->setCharset(vmime::charsets::UTF_8);

    textPart->setText(
        vmime::create<vmime::stringContentHandler>(
            html.empty() ? plain : html));

    textPart->setPlainText(
        vmime::create<vmime::stringContentHandler>(
            plain));
}

void Mailer_t::Message_t::addAttachment(const std::string &data,
                                        const std::string &contentType,
                                        const std::string &filename,
                                        const std::string &description)
{
    vmime::ref<vmime::defaultAttachment> attachment(
        vmime::create<vmime::defaultAttachment>(
            vmime::create<vmime::stringContentHandler>(data),
            vmime::mediaType(contentType),
            description.empty() ? vmime::NULL_TEXT : vmime::text(description),
            filename.empty() ? vmime::NULL_WORD : vmime::word(filename)));

    messageBuilder.attach(attachment);
}

