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
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
 
#include <semaphore.h>

#define ERROR -1
#define SUCCESS 0

void semview(SEMAPHORE* sem);
void start_curses();
void print_help();
void view_menu(SEMAPHORE* semaphore = NULL);
void end_curses();
bool confirmation();
std::string pgm;
std::string basename(const std::string& path);

//=============================================================================
// Returns true if char is '\\' or '/'
//=============================================================================
struct IsDirectoryChar
{
  bool operator()( char ch ) const
  {
    return ch == '\\' || ch == '/';
  }
};

//=============================================================================
// Main
//=============================================================================
int main(int argc, char *argv[])
{
  pgm = basename(argv[0]);
  bool getall, remove_sem;
  std::string file;
  SEMAPHORE* sem = new SEMAPHORE();
  if ( argc > 1 ) {
    if ( strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 ) {
      print_help();
      return 0;
    } else if ( strcmp(argv[1], "--menu") == 0 || strcmp(argv[1], "-m") == 0 ) {
      view_menu();
      return 0;
    } else {    
      file = argv[1];
      sem->set_sem_key(sem->get_ftok(file));
      sem->initialize();
      if ( !sem->is_valid() ) {
        std::cout << "error initializing semaphore file [" << file << "]" << std::endl;
      }
    }
  } else {
    print_help();
    return 0;
  }

  for (int i = 1; i < argc; i++) {
    if ( strcmp(argv[i], "--remove") == 0 || strcmp(argv[i], "-r") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      int semid = sem->get_sem_id();
      if (sem->remove_sem()) {
        std::cout << "semid " << semid << " removed" << std::endl;
      } else {
        std::cout << "semid " << semid << " remove failed" << std::endl;
      }
      break;
    } else if ( strcmp(argv[i], "--getall") == 0 || strcmp(argv[i], "-G") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      for (int j = 0; j < sem->get_sem_qty(); j++) {
        std::cout << "sem [" << j << "] value [" << sem->get_sem_value(j) << "]" << std::endl;
      }
      break;
    } else if ( strcmp(argv[i], "--setall") == 0 || strcmp(argv[i], "-S") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      i++;
      if ( i < argc ) {
        for (int j = 0; j < sem->get_sem_qty(); j++) {
          sem->set_sem_value(j, atoi(argv[i]));
        }
      }
      break;
    } else if ( strcmp(argv[i], "--set") == 0 || strcmp(argv[i], "-s") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      i++;
      if ( i < argc ) {
        int sem_num = atoi(argv[i]);
        i++;
        if ( i < argc ) {
          int sem_val = atoi(argv[i]);
          sem->set_sem_value(sem_num, sem_val);
        }
      }
      break;
    } else if ( strcmp(argv[i], "--get") == 0 || strcmp(argv[i], "-g") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      i++;
      if ( i < argc ) {
        int sem_num = atoi(argv[i]);
        std::cout << "sem [" << sem_num << "] value [" << sem->get_sem_value(sem_num) << "]" << std::endl;
      }
      break;
    } else if ( strcmp(argv[i], "--hang") == 0 || strcmp(argv[i], "-H") == 0 ) {
      if ( !sem->is_valid() ) { 
        std::cout << "please use the --make [SEM_QTY] switch" << std::endl;
        break; 
      }
      i++;
      int timeout = 0;
      if ( i < argc ) {
        int sem_num = atoi(argv[i]);
        i++;
        if ( i < argc ) {
          int sem_val = atoi(argv[i]);
          i++;
          if ( i < argc ) {
            timeout = atoi(argv[i]);
          }
          if (sem->sem_hang(sem_num,sem_val,timeout) == ERROR) {
            std::cout << "Timeout on sem_hang occured" << std::endl;
          } else {
            std::cout << "sem_hang successful" << std::endl;
          }
        }
      }
      break;
    } else if ( strcmp(argv[i], "--make") == 0 || strcmp(argv[i], "-M") == 0 ) {
      i++;
      if ( i < argc ) {
        int semcount = atoi(argv[i]);        
        if (sem->make_sem(semcount)) {
          std::cout << "semid " << sem->get_sem_id() << " created successfully" << std::endl;
        } else {
          std::cout << "error creating semaphore set" << std::endl;
        }
      }
      break;
    } else if ( strcmp(argv[i], "--view") == 0 || strcmp(argv[i], "-v") == 0 ) {
      semview(sem);
      break;
    } else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0  ) {
      print_help();
      break;
    } else if ( strcmp(argv[i], "--menu") == 0 || strcmp(argv[i], "-m") == 0 ) {
      if ( sem->is_valid() ) {
        view_menu(sem);
      } else {
        view_menu();
      }
      break;
    }
  }
  return 0;
}

//=============================================================================
// Returns basename of file path
//=============================================================================
std::string basename(const std::string& path)
{
  return std::string( std::find_if( path.rbegin(), path.rend(),
                      IsDirectoryChar() ).base(),
                      path.end() 
                    );
}

//=============================================================================
// prints help or usage
//=============================================================================
void print_help() {
  std::ostringstream os;
   
  os << "Usage: " << pgm << " [SEM FILE] [OPTION]..." << std::endl
     << "  or:  "  << pgm << " [-m] or [--menu]... for menu" << std::endl
     << "  or:  "  << pgm << " [-h] or [--help]... for help" << std::endl << std::endl
  
     << "OPTIONS: executes first option then quits." << std::endl
     << " -M, --make   [ARGS]  " << "Makes semaphore set. ARGS = sem_count." << std::endl
     << "                      " << " sem_count = number of semaphores to create in semaphore set." << std::endl     
     << " -g, --get    [ARGS]  " << "Gets the value of semaphore. ARGS = sem_num." << std::endl
     << "                      " << " sem_num = semaphore to get value from." << std::endl
     << " -G, --getall         " << "Gets all semaphore values in semaphore set." << std::endl
     << " -s, --set    [ARGS]  " << "Sets semaphore to value. ARGS = sem_num sem_value" << std::endl
     << "                      " << " sem_num   = semaphore to set." << std::endl
     << "                      " << " sem_value = value to set semaphore too." << std::endl
     << " -S, --setall [ARGS]  " << "Sets all semaphores to value. ARGS = sem_value." << std::endl
     << "                      " << " sem_value = value to set all semaphores too." << std::endl
     << " -H, --hang   [ARGS]  " << "Hangs on semaphore until given value is set or timeout." << std::endl
     << "                      " << " ARGS = sem_num sem_value timeout." << std::endl
     << "                      " << " sem_num   = semaphore to hang on." << std::endl
     << "                      " << " sem_value = hang on semaphore until it hits this value." << std::endl
     << "                      " << " timeout   = time to hang on sem until timeout, 0 means no timeout." << std::endl
     << " -r, --remove         " << "Removes/destroys semaphore set." << std::endl
     << " -v, --view           " << "Opens curses window to view semaphore set." << std::endl
     << " -m, --menu           " << "Displays menu with multiple options." << std::endl
     << " -h, --help           " << "Display this screen and exit." << std::endl;
     
  std::cout << os.str() << std::endl;  
}

//=============================================================================
// Opens menu with more options to modify semaphores
//=============================================================================
void view_menu(SEMAPHORE* semaphore)
{
  char hit_while_wait;
  std::string input;
  int  choice = 0;
  SEMAPHORE* sem = NULL;
  if ( semaphore != NULL ) {
    sem = semaphore;
  } else {
    sem = new SEMAPHORE();
  }
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
      return;
    }
    if ( stm >> choice && stm.eof() && choice < 0 && choice > 99 ) {
      choice = 99;
    }
    if ( choice >= 0 && choice != 1 && choice != 2 ) {
      if ( !sem->is_valid() ) {
        std::cout << "Semaphore not initialized" << std::endl;
        std::cout << std::endl;
        std::cin.get(hit_while_wait);
        std::cout << "Hit Enter To Continue... " << std::endl;
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
    std::cout<< "Hit Enter To Continue... " << std::endl;
    std::cin.get(hit_while_wait);

  } while (99);
}

//=============================================================================
// curses screen that prints the semaphores
//=============================================================================
void semview(SEMAPHORE* sem)
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
    return;
  }  
  
  semcount = max = sem->get_sem_qty();

  if( semcount == ERROR ) return;

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
}

//=============================================================================
// prints help or usage
//=============================================================================
void start_curses()
{
  initscr();
  raw();
  noecho();
  nonl();
  nodelay( stdscr, TRUE );
  curs_set( 0 );
}

//=============================================================================
// prints help or usage
//=============================================================================
void end_curses()
{
  refresh();
  endwin();
}

//=============================================================================
// returns true if you enter Y/y 
//=============================================================================
bool confirmation()
{
  std::string ans;
  std::cout << "Are you sure? Y/N" << std::endl;
  std::cin >> ans;
  return ( ans == "Y" || ans == "y" );
}
