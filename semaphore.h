/*
 * semaphore.h:
 * Generic *nix semaphore class
 * Copyright (c) 2013 Justin Whitmore
 ***********************************************************************
 * semaphore.h file is part of semaphore:
 *
 * semaphore is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * semaphore is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with semaphore.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <iostream>
#include <string>
 
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
    /* union semun is defined by including <sys/sem.h> */
#else
  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
  };
#endif

class SEMAPHORE
{
public:
    SEMAPHORE();
    
    bool  initialize(key_t semkey = -1);
    void  reset();
    bool  is_valid();
    
    bool  make_sem(int nsem = 0);
    bool  remove_sem();
    
    int   sem_hang(int sem_num, int sem_val, int timeout = 0);
    int   sem_op(int sem_num, int sem_op = -1, int sem_flag = SEM_UNDO);
    void  clear_sem(int sem_num = -1);
    char* to_string();
    void  sem_stat(struct semid_ds &ret_semidds);
    char* uid_to_user(uid_t uid);
    char* gid_to_group(gid_t uid);
    
    int   get_sem_value(int sem_num);
    int   get_sem_qty();
    int   get_sem_id();
    key_t get_sem_key();
    std::string get_sem_path();
    key_t get_ftok(std::string file);

    bool  set_sem_value(int sem_num, int sem_value);
    void  set_sem_qty(int nsems);    
    void  set_sem_key(key_t key);
private:
    key_t       _semkey;
    std::string _sempath;
    int         _semid;
    int         _nsem;
};

#endif // SEMAPHORE_H
