First Build:
Run phpize
Run ./configure --enable-php_semaphore
Run make
Run make install

phpize and configure only need ran the first time, unless the config.m4 is changed.

php.ini changes:

add the line extension=php_semaphore.so
restart apache