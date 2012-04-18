
#ifndef SPAMSERVER_TYPES_H
#define SPAMSERVER_TYPES_H

class Mail_t {
public:
    class Attachment_t {
    public:
        Attachment_t()
          : data(),
            filename(),
            description()
        {
        }

        std::string data;
        std::string filename;
        std::string description;
    };

    Mail_t()
      : from(),
        to(),
        cc(),
        bcc(),
        subject(),
        plainBody(),
        htmlBody(),
        attachments()
    {
    }

    std::string from;
    std::vector<std::string> to;
    std::vector<std::string> cc;
    std::vector<std::string> bcc;
    std::string subject;
    std::string plainBody;
    std::string htmlBody;
    std::vector<Attachment_t> attachments;
};

class TemplatedMail_t {
public:
    TemplatedMail_t()
      : templateName(),
        to(),
        cc(),
        bcc(),
        data(),
        attachments()
    {
    }

    std::string templateName;
    std::vector<std::string> to;
    std::vector<std::string> cc;
    std::vector<std::string> bcc;
    Teng::Fragment_t data;
    std::vector<Mail_t::Attachment_t> attachments;

private:
    TemplatedMail_t(const TemplatedMail_t&);
    TemplatedMail_t operator=(const TemplatedMail_t&);
};

struct Template_t {
    enum {
        MASK_NAME      = 0x00000001,
        MASK_FROM      = 0x00000002,
        MASK_SUBJECT   = 0x00000004,
        MASK_PLAINBODY = 0x00000008,
        MASK_HTMLBODY  = 0x00000010
    };

    Template_t()
      : id(0),
        name(),
        from(),
        subject(),
        plainBody(),
        htmlBody(),
        mask(0)
    {
    }

    int id;
    std::string name;
    std::string from;
    std::string subject;
    std::string plainBody;
    std::string htmlBody;

    int mask;
};

typedef std::vector<Template_t> TemplateList_t;

struct TemplateListingAttributes_t {
    TemplateListingAttributes_t()
      : pageSize(50),
        page(0)
    {
    }

    size_t pageSize;
    size_t page;
};

#endif // SPAMSERVER_TYPES_H

