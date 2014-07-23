DESTDIR=
prefix=/usr

all:
	

install:
	install -m 0755 buildeb $(DESTDIR)$(prefix)/bin/
	install -m 0755 config_parser.pl $(DESTDIR)$(prefix)/bin/
	install -m 0755 correct_lintian.pl $(DESTDIR)$(prefix)/bin/
	install -m 0644 build_config.yml $(DESTDIR)$(prefix)/share/buildeb/

clean:
	

.PHONY: all clean install
