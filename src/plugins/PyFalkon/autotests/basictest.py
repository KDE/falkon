import Falkon
import unittest

class BasicTest(unittest.TestCase):

    def test_module_version(self):
        self.assertEqual(Falkon.__version__.count('.'), 2)
        self.assertIsNotNone(Falkon.registerPlugin)

    def test_mainapplication(self):
        self.assertIsNotNone(Falkon.MainApplication.instance())

    def test_create_window(self):
        window = Falkon.MainApplication.instance().createWindow(Falkon.Qz.BW_NewWindow)
        self.assertIsNotNone(window)

    def test_sql_availability(self):
        self.assertTrue(hasattr(Falkon.SqlDatabase, 'database'))


suite = unittest.defaultTestLoader.loadTestsFromTestCase(BasicTest)
if unittest.TextTestRunner().run(suite).failures:
    raise(Exception("FAIL"))
