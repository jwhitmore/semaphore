#/*
# * makefile:
# * makefile for semaphore class
# * Copyright (c) 2013 Justin Whitmore
# ***********************************************************************
# * makefile is part of semaphore:
# *
# * semaphore is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * semaphore is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with semaphore.  If not, see <http://www.gnu.org/licenses/>.
# ***********************************************************************
#*/

include makefile.mki

BASE=.
SRCD=$(BASE)

OBJS=\
	$(OBJD)/semaphore.o
LIBS=\
	$(LIBD)/libsemaphore.a

all: ${OBJD} ${LIBD} $(OBJS) $(LIBS) 

$(OBJD):
	$(MK_PATH) $(OBJD)

$(LIBD):	
	$(MK_PATH) $(LIBD)

$(OBJD)/semaphore.o: $(SRCD)/semaphore.cc $(SRCD)/semaphore.h
	$(CCdepend) semaphore.cc -o $@

$(LIBD)/libsemaphore.a: $(OBJD)/semaphore.o
	$(AR) $(LIBD)/libsemaphore.a $(OBJD)/semaphore.o
  
install_lib:
	$(CP) $(LIBD)/libsemaphore.a $(LIBINSTALL)/libsemaphore.a

#  Cleanup generated files
clean:
	rm -f $(OBJS) $(LIBS) 
