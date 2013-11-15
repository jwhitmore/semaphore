/*
 * php_semaphore.h:
 * php wrapper for semaphore class
 * Copyright (c) 2013 Justin Whitmore
 ***********************************************************************
 * php_semaphore.h is part of Semaphore:
 *
 * Semaphore is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Semaphore is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with semaphore.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
*/

/* $Id: php_semaphore.h 149 2013-07-26 19:12:27Z justin $ */

#ifndef PHP_SEMAPHORE_H
#define PHP_SEMAPHORE_H

#define PHP_SEMAPHORE_EXTNAME  "php_semaphore"
#define PHP_SEMAPHORE_EXTVER   "0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
}

extern zend_module_entry php_semaphore_module_entry;
#define phpext_semaphore_ptr &php_semaphore_module_entry;


#endif /* PHP_SEMAPHORE_H */
