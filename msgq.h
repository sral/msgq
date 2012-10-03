/*
 * System V IPC Message Queue Python Extension Module.
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
