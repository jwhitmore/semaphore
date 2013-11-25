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

EXES=\
		$(BIND)/semutil

all: $(BIND) $(EXES)

$(BIND):
	$(MK_PATH) $(BIND)

$(BIND)/semutil: $(SRCD)/semutil.cc
	$(CCcomp) $(SRCD)/semutil.cc -I$(BASE) -L$(LIBD) -lsemaphore -lcurses -o $@

#  Cleanup generated files
clean:
	rm -f $(EXES)
