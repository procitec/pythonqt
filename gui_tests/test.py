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
        """
        Test that we can call python methods. 
        Creates a new file and checks if it was written to hard disk
        """
        with open(self.core_fn, 'wt') as f:
            f.write("test case 1")

        self.assert_file_exists(self.core_fn)

    def test_pythonqt_obj(self):
        """
        Test that we have a gui and a context object
        Test the file creation with the gui object
        """
        self.assertTrue(gui != None)
        self.assertTrue(meta != None)

        # create file with gui object
        gui.create_file(self.pyqt_obj)
        self.assert_file_exists(self.pyqt_obj)

    def test_pythonqt_cls(self):
        """
        Test the class wrapper by creating with WrapperFactory. 
        First create a class wrapper with the create_filehandle method of meta
        object.

        Than use methods of the class wrapper to test by creating and deleting files. 
        """
        # create file with gui object
        f = meta.create_filehandle(self.pyqt_cls)

        # make sure file is not existent
        f.delete_file()

        # file should not exist
        with self.assertRaises(AssertionError):
            self.assert_file_exists(self.pyqt_cls)

        # create file
        f.create_file()

        self.assertTrue(f.file_exists())

        # file should exist
        self.assert_file_exists(self.pyqt_cls)

    def test_pythonqt_cls_dec(self):
        """
        Test the class decorator to create c++ instance WITHOUT factory. 
        Instance only has exists() method
        """
        from PythonQt.example import FileDec

        fd = FileDec("not_existing.txt")
        self.assertFalse(fd.exists())

        # now create file
        with open(self.pyqt_cls_dec, 'wt') as f:
            f.write("test case 4")

        fd = FileDec(self.pyqt_cls_dec)
        self.assertTrue(fd.exists())

unittest.main(module=__name__, verbosity=3, exit=False)
