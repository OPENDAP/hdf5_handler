# GCTP source directory Makefile

# Library to build
# Added 'noinst' to suppress installation of this library. jhrg 5/25/17
noinst_LIBRARIES = libGctp.a

# Include the libGCTP include directory
AM_CPPFLAGS = -I$(srcdir)/../include/

# Some of this code was translated from FORTRAN an is a little old. It
# predates things like C99. jhrg 9/21/20
AM_CFLAGS = -fpic -Wno-implicit-function-declaration

AM_CXXFLAGS=
AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

# Library source files
libGctp_a_SOURCES = gctp.c alberfor.c alberinv.c alconfor.c alconinv.c      \
          azimfor.c aziminv.c bceafor.c bceainv.c br_gctp.c ceafor.c         \
          ceainv.c cproj.c eqconfor.c eqconinv.c equifor.c equiinv.c         \
          for_init.c gnomfor.c gnominv.c goodfor.c goodinv.c gvnspfor.c      \
          gvnspinv.c hamfor.c haminv.c imolwfor.c imolwinv.c inv_init.c      \
          isinusfor.c isinusinv.c lamazfor.c lamazinv.c lamccfor.c           \
          lamccinv.c merfor.c merinv.c millfor.c millinv.c molwfor.c         \
          molwinv.c obleqfor.c obleqinv.c omerfor.c omerinv.c orthfor.c      \
          orthinv.c paksz.c polyfor.c polyinv.c psfor.c psinv.c report.c     \
          robfor.c robinv.c sinfor.c sininv.c sphdz.c somfor.c sominv.c      \
          sterfor.c sterinv.c stplnfor.c stplninv.c tmfor.c tminv.c untfz.c  \
          utmfor.c utminv.c vandgfor.c vandginv.c wivfor.c wivinv.c          \
          wviifor.c wviiinv.c

