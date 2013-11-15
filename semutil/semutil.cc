/*
 * semutil.cc:
 * Semaphore utility
 * Copyright (c) 2013 Justin Whitmore
 ***********************************************************************
 * semutil.cc is part of Semaphore:
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
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>
#include <curses.h>
 
#include <semaphore.h>

#define ERROR -1
#define SUCCESS 0

int semview(SEMAPHORE * sem);
int start_curses();
int end_curses();
bool confirmation();

int main(int argc, char *argv[])
{
  char hit_while_wait;
  std::string input;
  int  choice = 0;
  SEMAPHORE* sem = new SEMAPHORE();
  do
  {
    choice = -1;
    input.clear();
    system("clear");
    if ( sem->is_valid() ) {
      std::cout << sem->to_string() << std::endl;
    }
    std::cout << "---------------=====00=====---------------" << std::endl;
    std::cout << "                Main  Menu                " << std::endl;
    std::cout << "Please select an option that you need:"     << std::endl;
    std::cout << "  1. Initialize Sem"     << std::endl;
    std::cout << "  2. Make Sem"           << std::endl;
    std::cout << "  3. Remove Sem"         << std::endl;
    std::cout << "  4. Set Sem Value"      << std::endl;
    std::cout << "  5. Set All Sem Values" << std::endl;
    std::cout << "  6. Get Sem Value"      << std::endl;
    std::cout << "  7. Get All Sem Values" << std::endl;
    std::cout << "  8. Clear Sem"          << std::endl;
    std::cout << "  9. Clear All Sems"     << std::endl;
    std::cout << " 10. Sem Hang"           << std::endl;
    std::cout << " 11. Get Sem Qty"        << std::endl;
    std::cout << " 12. Sem View"           << std::endl;
    std::cout << "  X. Exit"               << std::endl;
    std::cout << "Your choice: ";
    std::cin >> input;
    std::istringstream stm(input);
    
    if ( input == "x" || input == "X" ) {
      std::cout << "Exiting program" << std::endl;
      return 0;
    }
    if ( stm >> choice && stm.eof() && choice < 0 && choice > 99 ) {
      choice = 99;
    }
    if ( choice >= 0 && choice != 1 && choice != 2 ) {
      if ( !sem->is_valid() ) {
        std::cout << "Semaphore not initialized" << std::endl;
        std::cout << std::endl;
        std::cin.get(hit_while_wait);
        std::cout<< "Hit Enter To Continue... " <<std::endl;
        std::cin.get(hit_while_wait);
        continue;
      }
    }
    switch (choice){
      case 1:
         {
           std::string file;
           std::cout << "Enter: {\"Semaphore File\"}" << std::endl;
           std::cin >> file;
           sem->set_sem_key(sem->get_ftok(file));
           if ( sem->initialize() ) {
             std::cout << "initialize successful" << std::endl;
           } else {
             std::cout << "initialize failed" << std::endl;
           }
           break;
         }
      case 2:
          {
            int sem_num = 0;
            if ( sem->is_valid() ) {
              std::cout << "Semaphore already created" << std::endl;
              break;
            }
            if ( sem->get_sem_path() == "" || sem->get_sem_key() == ERROR ) {
              std::cout << "Semaphore not initialized" << std::endl;
            } else {
              if ( sem->get_sem_qty() == ERROR ) {                
                std::cout << "Enter: {Sem Qty}" << std::endl;
                std::cin >> sem_num;
              }
              if ( sem->make_sem(sem_num) ) {
                std::cout << "semid " << sem->get_sem_id() << " created successfully" << std::endl;
              } else {
                std::cout << "error creating semaphore set" << std::endl;
              }
            }
            break;
          }
      case 3:
          {
            int semid = -1;
            semid = sem->get_sem_id();
            if ( !confirmation() ) {
              break;
            }
            if ( sem->remove_sem() ) {
              std::cout << "semid " << semid << " removed" << std::endl;
            } else {
              std::cout << "semid " << semid << " remove failed" << std::endl;
            }
            break;
          }
      case 4:
         {
           int sem_num = 0, sem_value = 0;
           std::cout << "Enter: {Sem Num} {Sem Value}" << std::endl;
           std::cin >> sem_num >> sem_value;
           if ( !confirmation() ) {
             break;
           }
           sem->set_sem_value(sem_num, sem_value);
           break;
         }
      case 5:
         {
           int sem_value = 0;
           std::cout << "Enter: {Sem Value}" << std::endl;
           std::cin >> sem_value;
           if ( !confirmation() ) {
             break;
           }
           for (int i = 0; i < sem->get_sem_qty(); i++) {
             sem->set_sem_value(i, sem_value);
           }
           break;
         }
      case 6:
         {
           int sem_num = 0;
           std::cout << "Enter: {Sem Num}" << std::endl;
           std::cin >> sem_num;
           std::cout << "SEM [" << sem_num << "] VALUE [" << sem->get_sem_value(sem_num) << "]" << std::endl;
           break;
         }
      case 7:
         {
           for (int i = 0; i < sem->get_sem_qty(); i++) {
             std::cout << "SEM [" << i << "] VALUE [" << sem->get_sem_value(i) << "]" << std::endl;
           }
           break;
         }
      case 8:
         {
           int sem_num = -1;
           std::cout << "Enter: {Sem Num}" << std::endl;
           std::cin  >> sem_num;
           if ( !confirmation() ) {
              break;
           }
           sem->clear_sem(sem_num);
           break;
         }
      case 9:
         {
           if ( !confirmation() ) {
             break;
           }
           sem->clear_sem(-1);
           break;
         }
      case 10:
         {
           int sem_num = -1, sem_value = 0, timeout = 0;
           std::cout << "Enter: {Sem Num} to hang from {Sem Value} to hang until and {Timeout} or 0 for no timeout" << std::endl;
           std::cin >> sem_num >> sem_value >> timeout;
           if ( sem->sem_hang(sem_num, sem_value, timeout) == ERROR ) {
             std::cout << "Timeout on sem_hang occured" << std::endl;
           }
           break;
         }
      case 11:
         {
           int sem_qty = -1;
           sem_qty = sem->get_sem_qty();
           std::cout << "Sem Qty [" << sem_qty << "]" << std::endl;
           break;
         }
      case 12:
         {
           semview(sem);
           end_curses();
           break;
         }
      default:
        
        break;
    }
    
    std::cout << std::endl;
    std::cin.get(hit_while_wait);
    std::cout<< "Hit Enter To Continue... " <<std::endl;
    std::cin.get(hit_while_wait);

  } while (99);
  return 0;
}

int semview(SEMAPHORE * sem)
{
  struct point{ int x; int y; int pagesize; } scr;

  int count    = 0;
  int keypress = 0;
  int max      = 0;
  int offset   = 0;
  int semcount = 0;
  int semval   = 0;
  int viewing  = 0;
  
  
  if ( !sem->is_valid() ) {
    fprintf( stderr, "error: semaphore set %s is not valid\n",sem->get_sem_path().c_str());
    return ERROR;
  }
  
  
  semcount = max = sem->get_sem_qty();

  if( semcount == ERROR ) return ERROR;

  start_curses();

  getmaxyx( stdscr, scr.y, scr.x );

  scr.pagesize = scr.y - 4;

  max = max > scr.pagesize ? scr.pagesize : max;


  viewing = true;

  while( viewing )
  {

    for(  count = 0 ; count < max ; count++ )
    {
      semval = sem->get_sem_value( count + offset );
      if (errno == 22 && semval == -1) { 
        viewing = false;
        break;
      }
      mvprintw( count, 1, (char*)"sem %-5d = %-5d", count+offset, semval );
      clrtoeol();
    }

    clrtobot();

    mvprintw( (scr.y-1), 1, (char*)"j-down  k-up  x-exit" );

    keypress = wgetch( stdscr );

    switch( keypress )
    {
      case 88:                /*  upper case X   */
      case 120:               /*  lower case x   */
        viewing = FALSE;
        break;

      case 74:                /*  upper case J   */
        offset += (scr.pagesize - 1);
      case 106:               /*  lower case j   */
        offset++;
        if( offset > semcount ) offset = semcount - 1;
        max = semcount - offset;
        if( max > scr.pagesize ) max = scr.pagesize;
        break;

      case 75:                /*  upper case K   */
        offset -= (scr.pagesize - 1);
      case 107:               /*  lower case k   */
        offset--;
        if( offset < 0 ) offset = 0;
        max = semcount - offset;
        if( max > scr.pagesize ) max = scr.pagesize;
        break;

      case ERROR:
        usleep( 100000 ); // .1 sec
        break;

      default:
        /* do nothing */
        break;
    }
  }

  end_curses();

  return SUCCESS;
}

int start_curses()
{
  initscr();
  raw();
  noecho();
  nonl();
  nodelay( stdscr, TRUE );
  curs_set( 0 );

  return( SUCCESS );
}

int end_curses()
{
  refresh();
  endwin();

  return( SUCCESS );
}

bool confirmation()
{
  std::string ans;
  std::cout << "Are you sure? Y/N" << std::endl;
  std::cin >> ans;
  if ( ans == "Y" || ans == "y" ) {
    return true;
  }
  return false;
  
}
