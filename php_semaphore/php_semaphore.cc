/*
 * php_semaphore.cc:
 * php wrapper for semaphore class
 * Copyright (c) 2013 Justin Whitmore
 ***********************************************************************
 * php_semaphore.cc is part of Semaphore:
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

#include <sstream>
#include "php_semaphore.h"
#include "semaphore.h"

zend_object_handlers semaphore_object_handlers;
struct semaphore_object {
  zend_object std;
  SEMAPHORE *semaphore;
};

zend_class_entry *semaphore_ce;

void semaphore_free_storage(void *object TSRMLS_DC)
{
  semaphore_object *obj = (semaphore_object *)object;
  delete obj->semaphore; 

  zend_hash_destroy(obj->std.properties);
  FREE_HASHTABLE(obj->std.properties);

  efree(obj);
}

zend_object_value semaphore_create_handler(zend_class_entry *type TSRMLS_DC)
{
  zval *tmp;
  zend_object_value retval;

  semaphore_object *obj = (semaphore_object *)emalloc(sizeof(semaphore_object));
  memset(obj, 0, sizeof(semaphore_object));
  obj->std.ce = type;

  ALLOC_HASHTABLE(obj->std.properties);
  zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
  #if PHP_VERSION_ID < 50399	
	  zend_hash_copy(obj->std.properties, &(type->default_properties),
	                (copy_ctor_func_t)zval_add_ref, (void *)(&tmp),
	                sizeof(zval *));
  #else
	  object_properties_init(&(obj->std), type);
  #endif
  
  retval.handle = zend_objects_store_put(obj, NULL,
      semaphore_free_storage, NULL TSRMLS_CC);
  retval.handlers = &semaphore_object_handlers;

  return retval;
}

PHP_METHOD(SEMAPHORE, __construct)
{
  SEMAPHORE *semaphore = NULL;
  zval *object = getThis();
  
  semaphore = new SEMAPHORE();
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(object TSRMLS_CC);
  obj->semaphore = semaphore;
}

PHP_METHOD(SEMAPHORE, initialize)
{
  long semkey = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &semkey) == FAILURE) {
    RETURN_NULL();
  }
  
  SEMAPHORE *semaphore;  
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;  
  if (semaphore != NULL) {
    RETURN_BOOL(semaphore->initialize(semkey));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, reset)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    semaphore->reset();
  }
}

PHP_METHOD(SEMAPHORE, is_valid)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_BOOL(semaphore->is_valid());
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, make_sem)
{
  long nsems = 0;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &nsems) == FAILURE) {
    RETURN_NULL();
  }
  
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_BOOL(semaphore->make_sem(nsems));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, remove_sem)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_BOOL(semaphore->remove_sem());
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, sem_hang)
{
  long  sem_num = 0, sem_val = 0, timeout = 0;    

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|l", &sem_num, &sem_val, &timeout) == FAILURE) {
    RETURN_NULL();
  }
    
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->sem_hang(sem_num,sem_val,timeout));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, clear_sem)
{
  long sem_num = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &sem_num) == FAILURE) {
    RETURN_NULL();
  }
    
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    semaphore->clear_sem(sem_num);
  }
}

PHP_METHOD(SEMAPHORE, get_sem_value)
{
  long sem_num = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &sem_num) == FAILURE) {
    RETURN_NULL();
  }
    
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->get_sem_value(sem_num));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, get_sem_qty)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->get_sem_qty());
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, get_sem_id)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->get_sem_id());
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, get_sem_key)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->get_sem_key());
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, get_sem_path)
{
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_STRING(semaphore->get_sem_path().c_str(),1);
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, get_ftok)
{
  char* filename;
  int   filename_len;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename,&filename_len) == FAILURE) {
    RETURN_NULL();
  }
  
  std::stringstream ss;
  ss << filename;
  
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_LONG(semaphore->get_ftok(ss.str()));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, set_sem_value)
{
  long sem_num = -1, sem_value = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &sem_num,&sem_value) == FAILURE) {
    RETURN_NULL();
  }
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    RETURN_BOOL(semaphore->set_sem_value(sem_num,sem_value));
  }
  RETURN_NULL();
}

PHP_METHOD(SEMAPHORE, set_sem_qty)
{
  long nsems = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &nsems) == FAILURE) {
    RETURN_NULL();
  }
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    semaphore->set_sem_qty(nsems);
  }
}

PHP_METHOD(SEMAPHORE, set_sem_key)
{
  long  semkey = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &semkey) == FAILURE) {
    RETURN_NULL();
  }
  
  SEMAPHORE *semaphore;
  semaphore_object *obj = (semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
  semaphore = obj->semaphore;
  if (semaphore != NULL) {
    semaphore->set_sem_key(semkey);
  }
}

zend_function_entry semaphore_methods[] = {
  PHP_ME(SEMAPHORE,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(SEMAPHORE,  initialize,      NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  reset,           NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  is_valid,        NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  make_sem,        NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  remove_sem,      NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  sem_hang,        NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  clear_sem,       NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_sem_value,   NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_sem_qty,     NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_sem_id,      NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_sem_key,     NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_sem_path,    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  get_ftok,        NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  set_sem_value,   NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  set_sem_qty,     NULL, ZEND_ACC_PUBLIC)
  PHP_ME(SEMAPHORE,  set_sem_key,     NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(php_semaphore)
{
  zend_class_entry ce;
  INIT_CLASS_ENTRY(ce, "SEMAPHORE", semaphore_methods);
  semaphore_ce = zend_register_internal_class(&ce TSRMLS_CC);
  semaphore_ce->create_object = semaphore_create_handler;
  memcpy(&semaphore_object_handlers,
      zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  semaphore_object_handlers.clone_obj = NULL;
  return SUCCESS;
}

zend_module_entry php_semaphore_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_SEMAPHORE_EXTNAME,
  NULL,                  /* Functions */
  PHP_MINIT(php_semaphore),
  NULL,                  /* MSHUTDOWN */
  NULL,                  /* RINIT */
  NULL,                  /* RSHUTDOWN */
  NULL,                  /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_SEMAPHORE_EXTVER,
#endif
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHP_SEMAPHORE
extern "C" {
ZEND_GET_MODULE(php_semaphore)
}
#endif
