semaphore
=========
Semaphore is a easy to manage generic wrapper for Unix/Linux semaphores.  With semaphore you can create/destroy semaphore sets, set/get semaphore values, make semop calls, and hang on semaphore values.  Within this repo is also Semutil which is a utility that you can run from the command line and perform semaphore operations from this class.  There is also php_semaphore which is a php extension around this class that will let you perform all these semaphore operations within your php site.
***
Features
--------
- Create and destroy semaphore sets
- Get and set semaphore values within your semaphore set
- Perform semop calls
- Sleep or hang on semaphore values

OS Requirements
---------
- Linux 2.6+ ( Could work on earlier versions of the kernel but only tested on 2.6 or greater )
- AIX 5.3+ ( Currently tested on AIX 5.3 and 7.1 )
- Has been tested with both 32bit and 64bit AIX and Linux
- Should theoretically work on most *nix OS's, just not currently tested on them

Build Prerequisites
---------
- g++ 4.2.4+ ( Also may work with earlier versions, but not tested )
- GNU make 3.8+ 

Get Source
---------
`git clone https://github.com/jwhitmore/semaphore.git semaphore`  
`cd semaphore`  
`git checkout master`  

Build From cmd line
---------
`cd semaphore`  
`gmake or make`
