/*
 * semaphore.cc:
 * Generic *nix semaphore class
 * Copyright (c) 2013 Justin Whitmore
 ***********************************************************************
 * semaphore.cc is part of Semaphore:
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include "semaphore.h"
 
#define IPCPERM S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH


//=============================================================================
// Constructor - initializes the semkey and semid of the file passed in
//=============================================================================
SEMAPHORE::SEMAPHORE()
{
  reset();
}

//=============================================================================
// Initializes the semid
//=============================================================================
bool SEMAPHORE::initialize(key_t semkey)
{
  if ( semkey != -1 ) { 
    set_sem_key(semkey); 
  }
  if ( _semkey == -1 ) { return false; }
  _semid = semget( _semkey, 0, 0 );
  if ( _semid != -1 ) {
    _nsem = get_sem_qty();
  }

  return true;
}

//=============================================================================
// Resets the class variables
//=============================================================================
void SEMAPHORE::reset()
{
  _semid   = -1;
  _sempath = "";
  _semkey  = -1;
  _nsem    = 0;
}

//=============================================================================
// Checks if the semaphore set is valid by a populated semid
//=============================================================================
bool SEMAPHORE::is_valid()
{
  return ( _semid > 0 && this != NULL );
}

//=============================================================================
// Makes the semaphore set if not already a valid semaphore set
//=============================================================================
bool SEMAPHORE::make_sem(int nsem)
{
  if ( nsem > 0 ) {
    set_sem_qty(nsem);
  }
  if ( _nsem <= 0 ) {
    fprintf( stderr, "error: sem qty not initialized %d\n", _nsem );
    return false; 
  } else if ( _semkey == -1 ) {
    fprintf( stderr, "error: sem key not initialized %d\n", _semkey );
    return false;
  } else if ( _semid != -1 ) {
    fprintf( stderr, "semid %d already exists\n", _semid );
    return false;
  }


  if ( _semid == -1 ) {
    _semid = semget(_semkey, _nsem, IPC_CREAT | IPCPERM);
    if ( _semid == -1 ) {
      fprintf( stderr, "error: semget IPC_CREATE, semid=%d, errno=%d\n", _semid, errno );
      return false;
    } 
  }

  return true;
}

//=============================================================================
// Removes or destroys the semaphore set
//=============================================================================
bool SEMAPHORE::remove_sem()
{
  int result = 0;
  
  if ( _semid != -1 ) {

    result = semctl( _semid, 0, IPC_RMID, 0 );

    if( result == -1 )
    {
      return false;
    }
  }
  
  reset();
  return true;
}

//=============================================================================
// Waits for supplied semaphore to be set to supplied sem_val or until timeout
//=============================================================================
int SEMAPHORE::sem_hang(int sem_num, int sem_val, int timeout)
{
  int result;
  time_t t;
  if (timeout > 0) {
    t = time(NULL);
  }
  
  do
  {
    result = -1;
    if (((time(NULL) - t)) > timeout && timeout != 0) {
      break;
    }
    
    result = get_sem_value(sem_num);
    
    usleep(100000); // .1 sec
  } while (result != sem_val);
  
  return result;
}

//=============================================================================
// Does a semop call it is defaulted to do a semop subtract one SEM_UNDO
//=============================================================================
int SEMAPHORE::sem_op(int sem_num, int sem_op, int sem_flag)
{
  struct sembuf sop[1];
  sop[0].sem_num = sem_num;
  sop[0].sem_op  = sem_op;
  sop[0].sem_flg = sem_flag;
  return semop(_semid, sop, 1); 
}

//=============================================================================
// Clears supplied semaphore, sets to 0 if semaphore not supplied it clears all
//=============================================================================
void SEMAPHORE::clear_sem(int sem_num)
{
  if ( sem_num != -1 ) {
    set_sem_value(sem_num,0);
  } else {
    for (int i = 0; i < get_sem_qty(); i++) {
      set_sem_value(i, 0);
    }
  }
}

//=============================================================================
// Prints semaphore stats
//=============================================================================
std::string SEMAPHORE::to_string()
{
  char modebuf[]="-----------";
  char otimebuf[]="no entry                       ";
  char ctimebuf[]="no entry                       ";
  struct semid_ds semidds;
  struct tm* ptm;
  std::ostringstream str;

  if( semctl( _semid, 0, IPC_STAT, &semidds ) == -1 )
  {
    fprintf( stderr, "error:  semctl IPC_STAT returned errno %d\n", errno );
    return "";
  }

  modebuf[2] = (semidds.sem_perm.mode & S_IRUSR ) ? 'r' : '-';
  modebuf[3] = (semidds.sem_perm.mode & S_IWUSR ) ? 'w' : '-';
  modebuf[5] = (semidds.sem_perm.mode & S_IRGRP ) ? 'r' : '-';
  modebuf[6] = (semidds.sem_perm.mode & S_IWGRP ) ? 'w' : '-';
  modebuf[8] = (semidds.sem_perm.mode & S_IROTH ) ? 'r' : '-';
  modebuf[9] = (semidds.sem_perm.mode & S_IWOTH ) ? 'w' : '-';
#ifdef _AIX
  str << "key: " << semidds.sem_perm.key << " ";
  str << to_hex(semidds.sem_perm.key) << std::endl;
#else
  str << "key: " << semidds.sem_perm.__key << " ";
  str << to_hex(semidds.sem_perm.__key) << std::endl;
#endif
  str << "semid:   " << _semid << std::endl;
  str << "nsem:    " << semidds.sem_nsems << std::endl;
  str << "mode:    " << modebuf << std::endl;
  str << "uid:     " << uid_to_user(semidds.sem_perm.uid) << std::endl;
  str << "gid:     " << gid_to_group(semidds.sem_perm.gid) << std::endl;
  str << "cuid:    " << uid_to_user(semidds.sem_perm.cuid) << std::endl;
  str << "cgid:    " << gid_to_group(semidds.sem_perm.cgid) << std::endl;
  ptm = localtime( &semidds.sem_otime );

  sprintf( otimebuf, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec );

  ptm = localtime( &semidds.sem_ctime );

  sprintf( ctimebuf, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec );

  str << "otime:   " << otimebuf << std::endl;
  str << "ctime:   " << ctimebuf << std::endl;

  return str.str(); 
}

//=============================================================================
// Returns hexadecimal string of decimal
//=============================================================================
std::string SEMAPHORE::to_hex(int dec)
{
  std::ostringstream os;
  os << std::showbase << std::internal << std::setfill('0') << std::hex << dec;
  return os.str();    
}

//=============================================================================
// Returns semid_ds
//=============================================================================
void SEMAPHORE::sem_stat(struct semid_ds &ret_semidds)
{
  struct semid_ds semidds;

  if( semctl( _semid, 0, IPC_STAT, &semidds ) == -1 )
  {
    fprintf( stderr, "error:  semctl IPC_STAT returned errno %d\n", errno );
    return;
  }
  ret_semidds = semidds;
  return; 
}

//=============================================================================
// Returns a username from a uid
//=============================================================================
char* SEMAPHORE::uid_to_user(uid_t uid)
{
  struct passwd *result;

  result = getpwuid(uid);
  if (result == NULL) {
    return NULL;
  } else {
    return result->pw_name;
  }  
}

//=============================================================================
// Returns group name from a gid
//=============================================================================
char*  SEMAPHORE::gid_to_group(gid_t gid)
{
  struct group *result;
  
  result = getgrgid(gid);
  if (result == NULL) {
    return NULL;
  } else {
    return result->gr_name;
  }  
}

//-----------------------------------------------------------------------------
//----------------- Get Functions ---------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
// Returns the value of the supplied semaphore
//=============================================================================
int SEMAPHORE::get_sem_value(int sem_num)
{
  int result;

  result = semctl( _semid, sem_num, GETVAL, NULL);

  if( result == -1 )
  {
    fprintf( stderr, "error: semctl semnum=%d, errno=%d\n", sem_num, errno );
  }

  return result;
}

//=============================================================================
// Returns the count of semaphores in semaphore set
//=============================================================================
int SEMAPHORE::get_sem_qty()
{
  int result;
  struct semid_ds semidds;

  result = semctl( _semid, 0, IPC_STAT, &semidds);

  if( result == -1 )
  {
    fprintf( stderr, "error: semctl IPC_STAT (%d)\n", errno );
    return -1;
  }

  return semidds.sem_nsems;
}

//=============================================================================
// Returns the semaphore id
//=============================================================================
int SEMAPHORE::get_sem_id()
{
  return _semid;
}

//=============================================================================
// Returns the semaphore key
//=============================================================================
key_t SEMAPHORE::get_sem_key()
{
  return _semkey;
}

//=============================================================================
// Returns the semaphore path
//=============================================================================
std::string SEMAPHORE::get_sem_path()
{
  return _sempath;
}

//=============================================================================
// Gets ftok from a file
//=============================================================================
key_t SEMAPHORE::get_ftok(std::string file)
{
  key_t key = -1;
  _sempath = file;
  key = ftok(_sempath.c_str(), 1);
  if ( key == -1 ) {
    fprintf( stderr, "error: ftok=%s, errno=%d\n", _sempath.c_str(), errno );
    reset();
  }
  return key;
}

//-----------------------------------------------------------------------------
//----------------- Set Functions ---------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
// Sets a semaphore value with the given sem_value
//=============================================================================
bool SEMAPHORE::set_sem_value(int sem_num, int sem_value)
{
  int result = 0;
  semun arg;
  arg.val = sem_value;
  if ( _semid != -1 ) {
    result = semctl( _semid, sem_num, SETVAL , arg);
    if (result == -1) {
      fprintf( stderr, "error: semctl SETVAL semid=%d, semnum=%d, semval=%d\n", _semid, sem_num, arg );
      return false;
    }
    return true;
  }
  return false;
}

//=============================================================================
// Sets the semaphore qty
//=============================================================================
void SEMAPHORE::set_sem_qty(int nsems)
{
  _nsem = nsems;
}

//=============================================================================
// Sets the semaphore key
//=============================================================================
void SEMAPHORE::set_sem_key(key_t key)
{
  _semkey = key;
}
