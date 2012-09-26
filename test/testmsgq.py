import msgq
import unittest

class MsgqTestInternals(unittest.TestCase):
    def test_is_defined_IPC_CREAT(self):
        self.assertEqual(msgq.IPC_CREAT, 512)

    def test_is_defined_IPC_EXCL(self):
        self.assertEqual(msgq.IPC_CREAT, 1024)

    def test_is_defined_IPC_NOWAIT(self):
        self.assertEqual(msgq.IPC_NOWAIT, 2048)

    def test_is_defined_IPC_RMID(self):
        self.assertEqual(msgq.IPC_RMID, 0)

    def test_is_defined_IPC_SET(self):
        self.assertEqual(msgq.IPC_SET, 1)

    def test_is_defined_IPC_STAT(self):
        self.assertEqual(msgq.IPC_STAT, 2)

    def test_is_defined_IPC_INFO(self):
        self.assertEqual(msgq.IPC_INFO, 3)

    def test_is_defined_IPC_PRIVATE(self):
        self.assertEqual(msgq.IPC_PRIVATE, 0)


class MsgqTestSystemCalls(unittest.TestCase):
    def setUp(self):
        pass

    def test_get_System_V_IPC_key(self):
        pass

    def test_get_message_queue_identifier(self):
        pass
    
    def test_remove_message_queue(self):
        pass
    
    def tearDown(self):
        pass


#import msgq
#key = msgq.ftok("msgq.so", 42)
#ID = msgq.msgget(key, 0666 | msgq.IPC_CREAT)
#rm queue
#msgq.msgctl(ID, msgq.IPC_RMID)
