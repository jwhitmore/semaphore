dnl
dnl $Id: config.m4 110 2013-03-01 05:04:42Z justin $
dnl

PHP_ARG_ENABLE(php_semaphore,whether to enable Unix semaphore support,
[  --enable-php_semaphore        Enable Unix semaphore support])

if test "$PHP_SEMAPHORE" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_SUBST(PHP_SEMAPHORE_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, PHP_SEMAPHORE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(php_semaphore, php_semaphore.cc semaphore.cc, $ext_shared)
fi
