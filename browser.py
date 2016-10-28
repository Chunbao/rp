#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import gtk
import webkit
import gobject

gobject.threads_init()
win = gtk.Window()
bro = webkit.WebView()
bro.open("http://www.google.com")
win.add(bro)
win.show_all()
gtk.main()