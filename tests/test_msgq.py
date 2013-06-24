import msgq
import unittest


class TestMsgqInternals(unittest.TestCase):

    def test_is_defined_IPC_CREAT(self):
        """IPC_CREATE should equal 512"""

        self.assertEqual(msgq.IPC_CREAT, 512)

    def test_is_defined_IPC_EXCL(self):
        """IPC_EXCL should equal 1024"""
        self.assertEqual(msgq.IPC_EXCL, 1024)

    def test_is_defined_IPC_NOWAIT(self):
        """IPC_NOWAIT should equal 2048"""

        self.assertEqual(msgq.IPC_NOWAIT, 2048)

    def test_is_defined_IPC_RMID(self):
        """IPC_RMID should equal 0"""

        self.assertEqual(msgq.IPC_RMID, 0)

    def test_is_defined_IPC_SET(self):
        """IPC_SET should equal 1"""

        self.assertEqual(msgq.IPC_SET, 1)

    def test_is_defined_IPC_STAT(self):
        """IPC_STAT should equal 2"""

        self.assertEqual(msgq.IPC_STAT, 2)

    def test_is_defined_IPC_INFO(self):
        """IPC_INFO should equal 3"""

        self.assertEqual(msgq.IPC_INFO, 3)

    def test_is_defined_IPC_PRIVATE(self):
        """IPC_PRIVATE should equal 0"""

        self.assertEqual(msgq.IPC_PRIVATE, 0)


class TestPassObjects(unittest.TestCase):
    def setUp(self):
        self.key = msgq.ftok("msgq.so", 42)
        self.ID = msgq.msgget(self.key, 0666 | msgq.IPC_CREAT)

    def test_pass_none(self):
        """Pass None through queue.

        1.) Send None into queue
        2.) Receive from queue
        => We should get None back."""

        msgq.msgsnd(self.ID, 0, None)
        self.assertEqual(msgq.msgrcv(self.ID, 0), None)

    def test_pass_int(self):
        """Pass int through queue.

        1.) Send int into queue
        2.) Receive from queue
        => Received data should equal the sent data."""

        msgq.msgsnd(self.ID, 0, 13)
        self.assertEqual(msgq.msgrcv(self.ID, 0), 13)

    def test_pass_float(self):
        """Pass float through queue.

        1.) Send float into queue
        2.) Receive from queue
        => Received data should equal the sent data."""

        msgq.msgsnd(self.ID, 0, 2.0)
        self.assertEqual(msgq.msgrcv(self.ID, 0), 2.0)

    def test_pass_long(self):
        """Pass long through queue.

        1.) Send long into queue
        2.) Receive from queue
        => Received data should equal the sent data."""

        msgq.msgsnd(self.ID, 0, 2**32L)
        self.assertEqual(msgq.msgrcv(self.ID, 0), 2**32L)

    def test_pass_string(self):
        """Pass string through queue.

        1.) Send string into queue
        2.) Receive from queue
        => Received data should equal the sent data.
        """

        data = "Ad astra per alia porci"

        msgq.msgsnd(self.ID, 0, data)
        self.assertEqual(msgq.msgrcv(self.ID, 0), data)

    def test_pass_list(self):
        """Pass list through queue.

        1.) Send list into queue
        2.) Receive from queue
        => Received data should equal the sent data.
        """

        data = [1, 2, 3]

        msgq.msgsnd(self.ID, 0, data)
        self.assertEqual(msgq.msgrcv(self.ID, 0), data)

    def test_pass_dictonary(self):
        """Pass dictonary through queue.

        1.) Send dicontary into queue
        2.) Receive from queue
        => Received data should equal the sent data.
        """

        data = {1: 'foo', 'bar': 42}

        msgq.msgsnd(self.ID, 0, data)
        self.assertEqual(msgq.msgrcv(self.ID, 0), data)

    def test_pass_tuple(self):
        """Pass tuple through queue.

        1.) Send tuple into queue
        2.) Receive from queue
        => Received data should equal the sent data.
        """

        data = (1, 2, "foo", None)

        msgq.msgsnd(self.ID, 0, data)
        self.assertEqual(msgq.msgrcv(self.ID, 0), data)

    def test_pass_set(self):
        """Pass set through queue.

        1.) Send set into queue
        2.) Receive from queue
        => Received data should equal the sent data.
        """

        data = set([1, 7, 9, 13])

        msgq.msgsnd(self.ID, 0, data)
        self.assertEqual(msgq.msgrcv(self.ID, 0), data)

    def tearDown(self):
        msgq.msgctl(self.ID, msgq.IPC_RMID)
