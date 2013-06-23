msgq: System V IPC Message Queue Python Extension Module
========================================================

This extension wraps and makes system calls related to System V message queues
available to Python applications. The following system calls are exposed:

- ftok: convert a pathname and a project identifier to a System V IPC key
- msgget: get a message queue identifier
- msgsnd: send message to queue
- msgrcv: receive message from queu
- msgctl: message control operations

For more information on the system calls go visit your man pages.

The idea is that Python objects are passed between processes through a message
queue. The objects you wish to pass must support "pickling". Here's an example:
<pre>
>>> import msgq
>>> key = msgq.ftok("msgq.so", 42)
>>> ID = msgq.msgget(key, 0666 | msgq.IPC_CREAT)
>>> msgq.msgsnd(ID, 0, (1,2, {'foo' : 'bar'}))
0
>>> msgq.msgrcv(ID, 0)
(1, 2, {'foo': 'bar'})
>>> msgq.msgctl(ID, msgq.IPC_STAT)
{'msg_rtime': 1350377060, 'msg_lrpid': 19453, 'msg_lspid': 19453, 'msg_ctime': 1350377056, 'msg_qbytes': 16384, 'msg_perm': {'uid': 1000, 'cgid': 100, 'gid': 100, 'mode': 438, 'cuid': 1000, '__key': 704726339, '__seq': 56}, 'msg_qnum': 0, 'msg_stime': 1350377056}
>>> msgq.msgctl(ID, msgq.IPC_RMID)
0
</pre>

To build the extension type in the following:
$ setup.py build
And to install the extenstion try this:
$ setup.py install

Good luck ...

--l (lars.djerf@gmail.com)
