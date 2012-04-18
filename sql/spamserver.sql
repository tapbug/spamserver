
USE spamserver;

SET NAMES utf8;

DROP TABLE IF EXISTS email_template;
DROP TABLE IF EXISTS boolean;

CREATE TABLE boolean (
    id TINYINT(1) UNSIGNED NOT NULL COMMENT 'Boolean value ID',
    name VARCHAR(32) NOT NULL COMMENT 'Boolean value name',
    PRIMARY KEY (id)
)
ENGINE=InnoDB
DEFAULT CHARSET=utf8
COLLATE=utf8_general_ci
COMMENT='Boolean';

INSERT INTO boolean VALUES (0, 'false'), (1, 'true');

CREATE TABLE email_template (
    id INT(11) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Email template ID',
    name VARCHAR(64) NOT NULL COMMENT 'Email template name',
    `from` VARCHAR(255) NOT NULL COMMENT 'Email template sender address',
    subject VARCHAR(255) NOT NULL COMMENT 'Email template subject',
    body_plain TEXT NOT NULL COMMENT 'Email template plain body',
    body_html TEXT NOT NULL COMMENT 'Email template HTML body',
    PRIMARY KEY (id),
    UNIQUE KEY emailtemplate_uidx_name (name)
)
ENGINE=InnoDB
DEFAULT CHARSET=utf8
COLLATE=utf8_general_ci
COMMENT='Email templates';


