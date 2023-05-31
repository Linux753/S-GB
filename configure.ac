AC_INIT([S;GB], [0.1], [rasduinopi@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])

AC_CHECK_LIB([portaudio], [Pa_Initialize])
AC_CHECK_LIB([SDL2], [SDL_Init], [SDL2LIB=-lSD2main -lSDL2])
AC_SUBST([SDL2LIB])
AC_CHECK_LIB([math], [sin], [MATHLIB=-lm])
AC_SUBST([MATHLIB])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_ARG_ENABLE([debug],
            [AS_HELP_STRING([--enable-feature],
              [enable debug build])],
            [],
            [with_readline=no])

AM_PROG_AR
AC_PROG_RANLIB
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT