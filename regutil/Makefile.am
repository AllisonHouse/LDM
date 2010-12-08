# See file ../COPYRIGHT for copying and redistribution conditions.
#
# Process this file with automake to produce Makefile.in

AWK			= @AWK@
PERL			= @PERL@
LD_GEN			= @LD_GEN@
LDMHOME			= $(DESTDIR)@LDMHOME@
LOGS_DIR		= $(DESTDIR)@LOGS_DIR@
QUEUE_DIR		= $(DESTDIR)@QUEUE_DIR@
ETC_DIR			= $(DESTDIR)$(sysconfdir)
DATA_DIR		= $(DESTDIR)@DATA_DIR@
HOSTNAME		= @HOSTNAME@
NTPDATE			= @NTPDATE@
LOG_FILE		= $(LOGS_DIR)/ldmd.log
METRICS_FILE		= $(LOGS_DIR)/metrics.txt
NETSTAT			= @NETSTAT@
TOP			= @TOP@
bin_PROGRAMS		= regutil
EXTRA_DIST              = \
    checkRegistry \
    installRegistry \
    regpar.tab.in \
    regutil.txt \
    substPaths
TAGS_FILES              = \
    *.c \
    $(top_srcdir)/registry/*.c \
    $(top_srcdir)/registry/*.h \
    $(top_srcdir)/*.h \
    $(top_srcdir)/ulog/*.c $(top_srcdir)/ulog/*.h \
    $(top_srcdir)/protocol/*.c \
    $(top_srcdir)/libxml2/*.c \
    /usr/local/include/CUnit/CUnit.h \
    /usr/local/include/CUnit/Basic.h
AM_CPPFLAGS		= \
    -I$(top_srcdir) \
    -I$(top_srcdir)/registry \
    -I$(top_srcdir)/ulog \
    -I$(top_srcdir)/protocol \
    -I$(top_srcdir)/libxml2/include \
    -DREGISTRY_DIR='"$(ETC_DIR)"'
LDADD			= \
    $(top_builddir)/lib/libldm.la \
    $(top_builddir)/libxml2/libxml2.la \
    $(LD_GEN)

dist_man1_MANS		= regutil.1
CLEANFILES		= regutil.out regpar.tab

all:		regpar.tab

regpar.tab:	$(srcdir)/regpar.tab.in
	sed -e 's;@''LDMHOME@;$(LDMHOME);g' \
	    -e 's;@''LOGS_DIR@;$(LOGS_DIR);g' \
	    -e 's;@''QUEUE_DIR@;$(QUEUE_DIR);g' \
	    -e 's;@''DATA_DIR@;$(DATA_DIR);g' \
	    -e 's;@''HOSTNAME@;$(HOSTNAME);g' \
	    -e 's;@''NTPDATE@;$(NTPDATE);g' \
	    -e 's;@''LOG_FILE@;$(LOG_FILE);g' \
	    -e 's;@''METRICS_FILE@;$(METRICS_FILE);g' \
	    -e 's;@''NETSTAT@;$(NETSTAT);g' \
	    -e 's;@''TOP@;$(TOP);g' \
	    -e 's;@''ETC_DIR@;$(ETC_DIR);g' \
	    $(srcdir)/regpar.tab.in >$@.tmp
	mv $@.tmp $@

if MAINTAINER
BUILT_SOURCES		= substPaths

substPaths:	regpar.tab.in
	$(AWK) -F : 'BEGIN {print "sed \\"} \
	    {print "    -e '\''s:regpath{" $$1 "}:" $$2 ":g'\'' \\"} \
	    END {print ""}' regpar.tab.in >$@.tmp
	mv $@.tmp $@
	chmod +x $@

registry.h:	registry.hin registry.c regpar.tab.in
	$(AWK) '/@PARAMETER_NAME_DEFINITIONS@/{exit}{print}' \
	    $(srcdir)/registry.hin >$@.tmp1
	$(AWK) -F : '{print "#define REG_" $$1 " \"" $$2 "\""}' \
	    <regpar.tab.in >>$@.tmp1
	$(AWK) '{if (doPrint)print}/@PARAMETER_NAME_DEFINITIONS@/{doPrint=1}' \
	    $(srcdir)/registry.hin >>$@.tmp1
	$(top_srcdir)/extractDecls $@.tmp1 $(srcdir)/registry.c >$@.tmp2
	mv -f $@.tmp2 $@
	rm -f $@.tmp1 $@.sed

endif

.c.i:
	$(COMPILE) $(lib_la_CPPFLAGS) -E -o $@ $<

install-exec-local:	regpar.tab regutil $(ETC_DIR)
	$(PERL) $(srcdir)/installRegistry regpar.tab ./regutil \
	    $(ETC_DIR)/ldmadmin-pl.conf $(ETC_DIR)

if HAVE_CUNIT
check_PROGRAMS		= testRegistry
testRegistry_CPPFLAGS	= \
    -I$(top_srcdir) \
    -I$(top_srcdir)/registry \
    -I$(top_srcdir)/protocol \
    -I$(top_srcdir)/ulog \
    -I$(top_srcdir)/libxml2/include \
    @CPPFLAGS_CUNIT@

testRegistry_LDADD		= \
    $(top_builddir)/lib/libldm.la \
    $(top_builddir)/libxml2/libxml2.la \
    @LIBS_CUNIT@

TESTS			= checkRegistry

debug:		testRegistry
	$(TESTS_ENVIRONMENT) $(LIBTOOL) --mode=execute gdb testRegistry

debug-regutil:		regutil
	$(TESTS_ENVIRONMENT) $(LIBTOOL) --mode=execute gdb regutil

valgrind:	testRegistry
	$(TESTS_ENVIRONMENT) $(LIBTOOL) --mode=execute valgrind \
	    --leak-check=full --show-reachable=yes ./testRegistry
endif