import unittest
import os
import pathlib
from contextlib import suppress

class TestPythonQt(unittest.TestCase):
    def __init__(self, *args):
        super().__init__(*args)
        self.core_fn = 'test_core.txt'
        self.pyqt_obj = 'test_pyqt_obj.txt'
        self.pyqt_cls = 'test_pyqt_cls.txt'
        self.pyqt_cls_dec  = 'test_pyqt_cls_dec.txt'

    def assert_file_exists(self, path):
        if not pathlib.Path(path).resolve().is_file():
            raise AssertionError(f"File does not exists: {path}")

    def setUp(self):
        # delete all test files
        with suppress(FileNotFoundError):
            os.remove(self.core_fn)
            os.remove(self.pyqt_obj)
            os.remove(self.pyqt_cls)
            os.remove(self.pyqt_cls_dec)

    def tearDown(self):
        # delete all test files
        with suppress(FileNotFoundError):
            os.remove(self.core_fn)
            os.remove(self.pyqt_obj)
            os.remove(self.pyqt_cls)
            os.remove(self.pyqt_cls_dec)

    def test_pythoncore(self):
        # write file
        with open(self.core_fn, 'wt') as f:
            f.write("test case 1")

        self.assert_file_exists(self.core_fn)


    def test_pythonqt_obj(self):
        pass

    def test_pythonqt_cls(self):
        pass

    def test_pythonqt_cls_dec(self):
        pass

# unittest.main(verbosity=3)
