/*
 * msgq.c: System V IPC Message Queue Python Extension Module
 *
 * This extension module wraps and makes system calls related to System V 
 * message queues available to Python applications.
 *
 * Copyright (c) 2012 Lars Djerf <lars.djerf@gmail.com>
 */
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
    PyErr_SetString(PyExc_IOError, "Failed to copy data into memory.");
    return NULL;
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

  PyMem_Free(data_msg);
  return data;
}

static PyObject *
msgq_msgctl(PyObject *self, PyObject *args)
{
  int msqid, cmd, rv;
  struct msqid_ds ipc_stat_ds, *buf = NULL;
  struct msginfo ipc_info_ds;
  PyObject *dict_msqid_ds, *dict_ipc_perm, *tmp_obj;

  if (!PyArg_ParseTuple(args, "ii", &msqid, &cmd)) {
    return NULL;
  }

  switch(cmd) {
  case IPC_RMID:
    break;
  case IPC_SET:
    PyErr_SetString(PyExc_NotImplementedError, "IPC_SET not supported");
    return NULL;
  case IPC_STAT:
    buf = (struct msqid_ds *) &ipc_info_ds;
    break;
  case IPC_INFO:
    buf = &ipc_stat_ds;
    break;
  default:
    PyErr_SetString(PyExc_ValueError, "Invalid value for command");
    return NULL;
  }

  if ((rv = msgctl(msqid, cmd, buf)) == -1) {
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  if (buf != NULL) {
    dict_ipc_perm = PyDict_New();
    tmp_obj = Py_BuildValue("i", buf->msg_perm.__key);
    PyDict_SetItemString(dict_ipc_perm, "__key",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_perm.uid);
    PyDict_SetItemString(dict_ipc_perm, "uid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_perm.gid);
    PyDict_SetItemString(dict_ipc_perm, "gid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_perm.cuid);
    PyDict_SetItemString(dict_ipc_perm, "cuid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_perm.cgid);
    PyDict_SetItemString(dict_ipc_perm, "cgid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("H", buf->msg_perm.mode);
    PyDict_SetItemString(dict_ipc_perm, "mode",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("H", buf->msg_perm.__seq);
    PyDict_SetItemString(dict_ipc_perm, "__seq",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    dict_msqid_ds = PyDict_New();
    PyDict_SetItemString(dict_msqid_ds, "msg_perm", dict_ipc_perm);
    Py_DECREF(dict_ipc_perm);

    tmp_obj = Py_BuildValue("i", buf->msg_stime);
    PyDict_SetItemString(dict_msqid_ds, "msg_stime", 
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_rtime);
    PyDict_SetItemString(dict_msqid_ds, "msg_rtime",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_ctime);
    PyDict_SetItemString(dict_msqid_ds, "msg_ctime",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_qnum);
    PyDict_SetItemString(dict_msqid_ds, "msg_qnum",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_qbytes);
    PyDict_SetItemString(dict_msqid_ds, "msg_qbytes",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_lspid);
    PyDict_SetItemString(dict_msqid_ds, "msg_lspid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    tmp_obj = Py_BuildValue("i", buf->msg_lrpid);
    PyDict_SetItemString(dict_msqid_ds, "msg_lrpid",
			 tmp_obj);
    Py_DECREF(tmp_obj);

    return dict_msqid_ds;
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

