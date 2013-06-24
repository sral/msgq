/*
 * msgq.h: System V IPC Message Queue Python Extension Module.
 *
 * This extension module wraps and makes system calls related to System V
 * message queues available to Python applications.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2012 Lars Djerf <lars.djerf@gmail.com>
 */

#ifndef MSGQ_H
#define MSGQ_G

struct size_msgbuf
{
  long mtype;
  int size;
};

#define SIZE_MSG 2
#define DATA_MSG 3

#endif
