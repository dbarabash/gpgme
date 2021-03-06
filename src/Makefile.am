# Copyright (C) 2000 Werner Koch (dd9jn)
# Copyright (C) 2001, 2002, 2003, 2004, 2005, 2007 g10 Code GmbH
#
# This file is part of GPGME.
#
# GPGME is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# GPGME is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
# Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, see <http://www.gnu.org/licenses/>.

## Process this file with automake to produce Makefile.in

# Note: moc_kdpipeiodevice should actually be a dependcy below.
EXTRA_DIST = gpgme-config.in gpgme.m4 libgpgme.vers ChangeLog-2011 \
	     gpgme.h.in versioninfo.rc.in gpgme.def moc_kdpipeiodevice.cpp

bin_SCRIPTS = gpgme-config
m4datadir = $(datadir)/aclocal
m4data_DATA = gpgme.m4
nodist_include_HEADERS = gpgme.h

if HAVE_PTHREAD
ltlib_gpgme_pthread = libgpgme-pthread.la
else
ltlib_gpgme_pthread =
endif

if BUILD_W32_GLIB
ltlib_gpgme_glib = libgpgme-glib.la
else
ltlib_gpgme_glib =
endif

if BUILD_W32_QT
ltlib_gpgme_qt = libgpgme-qt.la
else
ltlib_gpgme_qt =
endif

lib_LTLIBRARIES = libgpgme.la $(ltlib_gpgme_glib) $(ltlib_gpgme_qt) \
	$(ltlib_gpgme_pthread)

if HAVE_LD_VERSION_SCRIPT
libgpgme_version_script_cmd = -Wl,--version-script=$(srcdir)/libgpgme.vers
else
libgpgme_version_script_cmd =
endif

if HAVE_DOSISH_SYSTEM
system_components = w32-util.c w32-sema.c
system_components_not_extra = w32-io.c
else
system_components = ath.h posix-util.c posix-sema.c posix-io.c
system_components_not_extra =
endif

if HAVE_W32CE_SYSTEM
system_components += w32-ce.h w32-ce.c
endif

if HAVE_GPGSM
gpgsm_components = engine-gpgsm.c
else
gpgsm_components =
endif

if HAVE_ASSUAN
assuan_components = assuan-support.c engine-assuan.c
else
assuan_components =
endif

if HAVE_GPGCONF
gpgconf_components = engine-gpgconf.c
else
gpgconf_components =
endif

if HAVE_G13
g13_components = engine-g13.c
else
g13_components =
endif

if HAVE_UISERVER
uiserver_components = engine-uiserver.c
else
uiserver_components =
endif

# These are the source files common to all library versions.  We used
# to build a non-installed library for that, but that does not work
# correctly on all platforms (in particular, one can not specify the
# right linking order with libtool, as the non-installed version has
# unresolved symbols to the thread module.
main_sources =								\
	util.h conversion.c get-env.c context.h ops.h		        \
	parsetlv.c parsetlv.h                                           \
	data.h data.c data-fd.c data-stream.c data-mem.c data-user.c	\
	data-compat.c data-identify.c					\
	signers.c sig-notation.c					\
	wait.c wait-global.c wait-private.c wait-user.c wait.h		\
	op-support.c							\
	encrypt.c encrypt-sign.c decrypt.c decrypt-verify.c verify.c	\
	sign.c passphrase.c progress.c					\
	key.c keylist.c trust-item.c trustlist.c			\
	import.c export.c genkey.c delete.c edit.c getauditlog.c        \
	opassuan.c passwd.c                                             \
	engine.h engine-backend.h engine.c engine-gpg.c status-table.c	\
	$(gpgsm_components) $(assuan_components) $(gpgconf_components)  \
	$(uiserver_components)						\
	$(g13_components) vfs-mount.c vfs-create.c			\
	gpgconf.c							\
	sema.h priv-io.h $(system_components) sys-util.h dirinfo.c	\
	debug.c debug.h gpgme.c version.c error.c

libgpgme_la_SOURCES = $(main_sources)					\
	ath.h ath.c $(system_components_not_extra)
libgpgme_pthread_la_SOURCES = $(main_sources)				\
	ath.h ath-pthread.c $(system_components_not_extra)

if BUILD_W32_GLIB
libgpgme_glib_la_SOURCES = $(main_sources) ath.h ath.c w32-glib-io.c
endif

if BUILD_W32_QT
libgpgme_qt_la_SOURCES = $(main_sources) ath.h ath.c w32-qt-io.cpp \
	kdpipeiodevice.h kdpipeiodevice.cpp kdpipeiodevice.moc
# FIXME: Add extra depedency: moc_kdpipeiodevice.cpp

# These are built sources (normally).
# moc_kdpipeiodevice.cpp: kdpipeiodevice.h
#	$(MOC4) -o $@ $<
#
# kdpipeiodevice.moc: kdpipeiodevice.cpp
#	$(MOC4) -o $@ $<
endif

# We use a global CFLAGS and CPPFLAGS setting for all library
# versions, because then every object file is only compiled once.
AM_CPPFLAGS = @GPG_ERROR_CFLAGS@ @QT4_CORE_CFLAGS@
AM_CFLAGS = @LIBASSUAN_CFLAGS@ @GLIB_CFLAGS@ @QT4_CORE_CFLAGS@

if HAVE_W32_SYSTEM
# Windows provides us with an endless stream of Tough Love.  To spawn
# processes with a controlled set of inherited handles, we need a
# wrapper process.
# Except on Windows CE.  There nothing is inheritable anyway.
if HAVE_W32CE_SYSTEM
libexec_PROGRAMS =
else
libexec_PROGRAMS = gpgme-w32spawn
endif

RCCOMPILE = $(RC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES)
LTRCCOMPILE = $(LIBTOOL) --mode=compile --tag=RC $(RCCOMPILE)

SUFFIXES = .rc .lo

.rc.lo:
	$(LTRCCOMPILE) -i "$<" -o "$@"

gpgme_res = versioninfo.lo
no_undefined = -no-undefined
export_symbols = -export-symbols $(srcdir)/gpgme.def

install-def-file:
	-$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) $(srcdir)/gpgme.def $(DESTDIR)$(libdir)/gpgme.def

uninstall-def-file:
	-rm $(DESTDIR)$(libdir)/gpgme.def

gpgme_deps = $(gpgme_res) gpgme.def

else
gpgme_res =
no_undefined =
export_symbols =
install-def-file:
uninstall-def-file:

gpgme_deps =
endif

libgpgme_la_LDFLAGS = $(no_undefined) $(export_symbols) \
	$(libgpgme_version_script_cmd) -version-info \
	@LIBGPGME_LT_CURRENT@:@LIBGPGME_LT_REVISION@:@LIBGPGME_LT_AGE@
libgpgme_la_DEPENDENCIES = @LTLIBOBJS@ $(srcdir)/libgpgme.vers $(gpgme_deps)
libgpgme_la_LIBADD = $(gpgme_res) @LIBASSUAN_LIBS@ @LTLIBOBJS@ \
	             @GPG_ERROR_LIBS@

libgpgme_pthread_la_LDFLAGS = $(no_undefined) $(export_symbols) \
	$(libgpgme_version_script_cmd) -version-info \
	@LIBGPGME_LT_CURRENT@:@LIBGPGME_LT_REVISION@:@LIBGPGME_LT_AGE@
libgpgme_pthread_la_DEPENDENCIES = @LTLIBOBJS@ $(srcdir)/libgpgme.vers
libgpgme_pthread_la_LIBADD = $(gpgme_res) @LIBASSUAN_LIBS@ @LTLIBOBJS@ \
	-lpthread @GPG_ERROR_LIBS@

if BUILD_W32_GLIB
libgpgme_glib_la_LDFLAGS = $(no_undefined) \
	$(export_symbols) $(libgpgme_version_script_cmd) -version-info \
	@LIBGPGME_LT_CURRENT@:@LIBGPGME_LT_REVISION@:@LIBGPGME_LT_AGE@
libgpgme_glib_la_DEPENDENCIES =	@LTLIBOBJS@ \
	$(srcdir)/libgpgme.vers $(gpgme_deps)
libgpgme_glib_la_LIBADD = $(gpgme_res) @LIBASSUAN_LIBS@ @LTLIBOBJS@ \
	@GPG_ERROR_LIBS@ @GLIB_LIBS@
endif

if BUILD_W32_QT
libgpgme_qt_la_LDFLAGS = $(no_undefined) \
	$(export_symbols) $(libgpgme_version_script_cmd) -version-info \
	@LIBGPGME_LT_CURRENT@:@LIBGPGME_LT_REVISION@:@LIBGPGME_LT_AGE@
libgpgme_qt_la_DEPENDENCIES = @LTLIBOBJS@ $(srcdir)/libgpgme.vers $(gpgme_deps)
libgpgme_qt_la_LIBADD = $(gpgme_res) @LIBASSUAN_LIBS@ @LTLIBOBJS@ \
	@GPG_ERROR_LIBS@ @QT4_CORE_LIBS@
endif

noinst_PROGRAMS = gpgme-tool
gpgme_tool_LDADD = libgpgme.la @LIBASSUAN_LIBS@

install-data-local: install-def-file

uninstall-local: uninstall-def-file
