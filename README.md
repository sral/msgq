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
</pre>

To build the extension type in the following:
$ python setup.py build
And to install the extenstion try this:
$ python setup.py install


Good luck... here, have my card:
<pre>
            (__)
      (  ( ((oo)) ) )
   .-------- \/
  / //_____ //  
 * ^^      ^^  lars.djerf@gmail.com
</pre>
