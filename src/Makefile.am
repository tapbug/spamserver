
AM_CXXFLAGS = -Werror -Wall -Wno-strict-aliasing -D_FILE_OFFSET_BITS=64 -D_REENTRANT -fPIC -g -ggdb3 ${CXXEXTRAFLAGS}

moduledir = $(prefix)/lib/threadserver/handlers/modules

module_LTLIBRARIES = spamserver.la

spamserver_la_LDFLAGS = -module -avoid-versions

spamserver_la_SOURCES = \
    cfg.cc \
    core.cc \
    frpcinterface.cc \
    mailer.cc \
    server.cc

spamserver_la_LIBADD = \
    -lthreadserver \
    /usr/lib/threadserver/handlers/cppfrpchandler.la

install:

