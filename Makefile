
all: build

autoreconf:
	autoreconf -i -f src
	if [ ! -e /src/Makefile ]; then cd src && ./configure --prefix=/usr ; fi

build:
	make -C src

clean:
	if [ -e src/Makefile ]; then make -C src clean ; fi

install: all
	mkdir -p $(DESTDIR)/srv/spamserver/bin
	ln -sf /usr/sbin/threadserver $(DESTDIR)/srv/spamserver/bin/spamserver
	mkdir -p $(DESTDIR)/srv/spamserver/conf
	cp conf/spamserver.conf $(DESTDIR)/srv/spamserver/conf/spamserver.conf
	cp conf/teng.conf $(DESTDIR)/srv/spamserver/conf/teng.conf
	mkdir -p $(DESTDIR)/srv/spamserver/help
	cp help/* $(DESTDIR)/srv/spamserver/help/
	mkdir -p $(DESTDIR)/srv/spamserver/libexec
	cp src/.libs/spamserver.so $(DESTDIR)/srv/spamserver/libexec/spamserver.so
	mkdir -p $(DESTDIR)/srv/spamserver/sql
	cp sql/*.sql $(DESTDIR)/srv/spamserver/sql/
	mkdir -p $(DESTDIR)/etc/init.d
	cp etc/initscript $(DESTDIR)/etc/init.d/spamserver
	mkdir -p $(DESTDIR)/etc/logrotate.d
	cp etc/logrotate $(DESTDIR)/etc/logrotate.d/spamserver
	mkdir -p $(DESTDIR)/etc/service/spamserver
	cp etc/run $(DESTDIR)/etc/service/spamserver/

deb:
	dpkg-buildpackage -rfakeroot -uc -us

debnc:
	dpkg-buildpackage -rfakeroot -uc -us -nc

test: deb
	sudo dpkg -i ../spamserver_`head -n1 debian/changelog | cut -d\( -f2 | cut -d\) -f1`_`dpkg --print-architecture`.deb
	sudo /etc/init.d/spamserver restart

testnc: debnc
	sudo dpkg -i ../spamserver_`head -n1 debian/changelog | cut -d\( -f2 | cut -d\) -f1`_`dpkg --print-architecture`.deb
	sudo /etc/init.d/spamserver restart

.PHONY: all build clean install deb test

