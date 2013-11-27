#/*
# * makefile:
# * makefile for semutil
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

include ../makefile.mki

BASE=..
SRCD=.

OBJS=\
		$(OBJD)/semaphore.o
EXES=\
		$(BIND)/semutil

all: $(BIND) $(OBJS) $(EXES)

$(BIND):
	$(MK_PATH) $(BIND)

$(OBJD)/semaphore.o: $(BASE)/semaphore.cc $(BASE)/semaphore.h
	$(CCdepend) $(BASE)/semaphore.cc -o $@

$(BIND)/semutil: $(SRCD)/semutil.cc $(OBJD)/semaphore.o
	$(CCcomp) $(SRCD)/semutil.cc $(OBJD)/semaphore.o -I$(BASE) -lcurses -o $@

#  Cleanup generated files
clean:
	rm -f $(EXES)
