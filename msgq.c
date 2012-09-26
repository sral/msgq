#include <Python.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stddef.h>
#include <string.h>
#include "msgq.h"

PyObject *pickle;

static PyObject *
msgq_ftok(PyObject *self, PyObject *args) 
{
  const char *pathname;
  int proj_id;
  key_t key;

  if (!PyArg_ParseTuple(args, "si", &pathname, &proj_id)) {
    return NULL;
  }

  if ((key = ftok(pathname, proj_id)) == -1) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  return Py_BuildValue("i", key);
}

static PyObject *
msgq_msgget(PyObject *self, PyObject *args)
{
  key_t key;
  int msgflg, id;

  if (!PyArg_ParseTuple(args, "ii", &key, &msgflg)) {
    return NULL;
  }

  if ((id = msgget(key, msgflg)) == -1) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  return Py_BuildValue("i", id);
}

static PyObject *
msgq_msgsnd(PyObject *self, PyObject *args)
{
  int msqid, msgflg, rv;
  const char *cstring;
  size_t msgsz;
  PyObject *dumps, *data, *string;

  struct size_msgbuf size_msg;
  struct msgbuf *data_msg;

  if (!PyArg_ParseTuple(args, "iiO", &msqid, &msgflg, &data)) {
    return NULL;
  }

  // Pickle object
  dumps = PyObject_GetAttr(pickle, Py_BuildValue("s", "dumps"));
  string = PyObject_CallFunctionObjArgs(dumps, data, NULL);
  cstring = PyString_AsString(string);
  
  data_msg = PyMem_Malloc(offsetof(struct msgbuf, mtext) + strlen(cstring) + 1);
  if (data_msg == NULL) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  data_msg->mtype = DATA_MSG;
  if (memcpy(&data_msg->mtext, cstring, strlen(cstring) + 1) == NULL) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }
 
  // Send message size
  size_msg.mtype = SIZE_MSG;
  size_msg.size = strlen(cstring) + 1;
  msgsz = sizeof(struct size_msgbuf) - sizeof(long);
  if ((rv = msgsnd(msqid, &size_msg, msgsz, msgflg)) == -1) {
    PyMem_Free(data_msg);
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  msgsz = offsetof(struct msgbuf, mtext) + size_msg.size - sizeof(long);
  // Send message
  if ((rv = msgsnd(msqid, data_msg, msgsz, msgflg)) == -1) {
    PyMem_Free(data_msg);
    // If this fails we need to also remove last size message from the queue
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  PyMem_Free(data_msg);
  return Py_BuildValue("i", rv);
}

static PyObject *
msgq_msgrcv(PyObject *self, PyObject *args)
{
  int msqid, msgflg;
  size_t msgsz;
  PyObject *loads, *data;

  struct size_msgbuf size_msg;
  struct msgbuf *data_msg;
  
  if (!PyArg_ParseTuple(args, "ii", &msqid, &msgflg)) {
    return NULL;
  }

  // Get message size
  msgsz = sizeof(struct size_msgbuf) - sizeof(long);
  if (msgrcv(msqid, &size_msg, msgsz, SIZE_MSG, 0) == -1) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  if ((data_msg = PyMem_Malloc(offsetof(struct msgbuf, mtext) + size_msg.size)) == NULL) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  // Get message
  if (msgrcv(msqid, data_msg, size_msg.size, DATA_MSG, msgflg) == -1) {
    PyMem_Free(data_msg);
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  // Convert back to Python object
  loads = PyObject_GetAttr(pickle, Py_BuildValue("s", "loads"));
  data = PyObject_CallFunctionObjArgs(loads, Py_BuildValue("s", data_msg->mtext), NULL);

  //data = Py_BuildValue("s", data_msg->mtext);
  PyMem_Free(data_msg);
  return data;
}

static PyObject *
msgq_msgctl(PyObject *self, PyObject *args)
{
  int msqid, cmd, rv;
  struct msqid_ds *buf = NULL;
 
  if (!PyArg_ParseTuple(args, "ii", &msqid, &cmd)) {
    return NULL;
  }

  if (cmd != IPC_RMID) {
    // create msqid_ds struct
    // and set buf to point to it
    buf = NULL;
  }

  if ((rv = msgctl(msqid, cmd, NULL)) == -1) {
    return NULL;
  }

  return Py_BuildValue("i", rv);
}

static PyMethodDef msgq_methods[] = {
  {"ftok", msgq_ftok, METH_VARARGS,
   "ftok - convert a pathname and a project identifier to a System V IPC key"},
  {"msgget", msgq_msgget, METH_VARARGS,
   "msgget - get a message queue identifier"},
  {"msgsnd", msgq_msgsnd, METH_VARARGS,
   "msgrcv, msgsnd - message operations"},
  {"msgrcv", msgq_msgrcv, METH_VARARGS,
   "msgrcv, msgsnd - message operations"},    
  {"msgctl", msgq_msgctl, METH_VARARGS,
   "msgctl - message control operations"},
  {NULL, NULL, 0, NULL}   // Sentinel
};

PyDoc_STRVAR(msgq_doc, 
"This module provides access to System V messages queues (IPC) by\n\
wrapping relevant system calls:\n\
ftok, msgget, msgsnd, msgrcv and msgctl\n\n\
See man-pages for further information.");

PyMODINIT_FUNC
initmsgq(void)
{
  PyObject *m;

  m = Py_InitModule3("msgq", msgq_methods, msgq_doc);
  if (m == NULL) {
    return;
  }

  // import json module
  if ((pickle = PyImport_ImportModule("pickle")) == NULL) {
    return;
  }
  Py_INCREF(pickle);

  PyModule_AddIntConstant(m, "IPC_CREAT", IPC_CREAT);
  PyModule_AddIntConstant(m, "IPC_EXCL", IPC_EXCL);
  PyModule_AddIntConstant(m, "IPC_NOWAIT",  IPC_NOWAIT);
  PyModule_AddIntConstant(m, "IPC_RMID", IPC_RMID);
  PyModule_AddIntConstant(m, "IPC_SET", IPC_SET);
  PyModule_AddIntConstant(m, "IPC_STAT", IPC_STAT);
  PyModule_AddIntConstant(m, "IPC_INFO", IPC_INFO);
  PyModule_AddIntConstant(m, "IPC_PRIVATE", IPC_PRIVATE);
}

