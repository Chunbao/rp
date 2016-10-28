# import sys
# from browser import Ui_Dialog
# from PyQt5 import QtCore, QtGui
# from PyQt5.QtCore import QUrl
# from PyQt5.QtWebKit import QWebView

# class MyBrowser(QtGui.QDialog):
#     def __init__(self, parent=None):
#         QtGui.QWidget.__init__(self, parent)
#         QWebView.__init__(self)
#         self.ui = Ui_Dialog()
#         self.ui.setupUi(self)
#         self.ui.lineEdit.returnPressed.connect(self.loadURL)

#     def loadURL(self):
#         url = self.ui.lineEdit.text()
#         self.ui.qwebview.load(QUrl(url))
#         self.show()  
#         #self.ui.lineEdit.setText("")

# if __name__ == "__main__":
#     app = QtGui.QApplication(sys.argv)
#     myapp = MyBrowser()
#     myapp.ui.qwebview.load(QUrl('http://www.talkera.org/python'))
#     myapp.show()
#     sys.exit(app.exec_())


import sys
#from browser import Ui_Dialog
import browser
from PyQt4 import QtWebKit
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import QUrl
from PyQt4.QtWebKit import QWebView
from PyQt4.QtWebKit import QWebSettings
 
class MyBrowser(QtGui.QDialog):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        QWebView.__init__(self)
        self.ui = browser.Ui_Dialog()
        self.ui.setupUi(self)
        #self.ui.lineEdit.returnPressed.connect(self.loadURL)
 
    def loadURL(self):
        #url = self.ui.lineEdit.text()
        #self.ui.qwebview.load(QUrl(url))
        self.ui.webView.load(QUrl('https://www.baidu.com/'))
        self.show()  
        #self.ui.lineEdit.setText("")
 
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    myapp = MyBrowser()

    #myapp.ui.webView.setAttribute(QWebSettings.JavascriptEnabled, True)
    #myapp.ui.webView.setAttribute(QWebSettings.PluginsEnabled, True)
    settings = QtWebKit.QWebSettings.globalSettings()
    settings.setAttribute(QWebSettings.PluginsEnabled, True)
    settings.setAttribute(QWebSettings.JavascriptEnabled, True)

    myapp.ui.webView.load(QUrl('http://test.alltobid.com/'))
    myapp.show()
    sys.exit(app.exec_())