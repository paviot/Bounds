AC_PREREQ([2.68])
AC_INIT([bounds], [1.0.0], [emmanuel.paviot-adet@parisdescartes.fr])

m4_esyscmd([./bin/generate-files])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_SRCDIR([src/main.cpp])

AC_PROG_CXX

AC_CHECK_HEADER_STDBOOL
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
