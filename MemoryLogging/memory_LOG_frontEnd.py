import gtk
import sys
import serial
import time

# e.g. for Glade-tutorial
# http://www.learningpython.com/2006/05/07/creating-a-gui-using-pygtk-and-glade/

class PyApp(gtk.Window):

    def __init__(self):
	  super(PyApp, self).__init__()

	  self.serialPort = "/dev/ttyUSB0"

	  # "MAIN BOX"
	  self.set_title("Data Logger Test")
	  self.set_size_request(260, 150)
	  self.set_position(gtk.WIN_POS_CENTER)
	  self.setup_serial()

	  headerVbox = gtk.VBox(True,0)
	  headerLabel1 = gtk.Label("Data Logger Test for Mini Ultra+")
	  headerLabel2 = gtk.Label("Written by Petteri Teikari")
	  headerVbox.pack_start(headerLabel1)
	  headerVbox.pack_end(headerLabel2)

	  # REWARD button
	  rHbox = gtk.HBox(True,0)
	  rLabel = gtk.Label("Collect")
	  rHbox.pack_start(rLabel)

	  self.rewardButton = gtk.Button("CollectButton")
	  self.rewardButton.set_name("collect")	  
	  self.rewardButton.set_size_request(120, 25)	  
	  self.rewardButton.connect("clicked", self.buttonPress)
	  rHbox.pack_end(self.rewardButton)

	  # TRIAL button
	  tHbox = gtk.HBox(True,0)
	  tLabel = gtk.Label("Read Data")
	  tHbox.pack_start(tLabel)

	  self.trialButton = gtk.Button("ReadButton")
	  self.trialButton.set_name("read")	  
	  self.trialButton.set_size_request(120, 25)	  
	  self.trialButton.connect("clicked", self.buttonPress)
	  tHbox.pack_end(self.trialButton)

	  vbox = gtk.VBox(True,0)
	  vbox.pack_start(headerVbox)
	  vbox.pack_start(rHbox)
	  vbox.pack_end(tHbox)

	  self.add(vbox)
	  self.connect("destroy", lambda w: gtk.main_quit())
	  self.show_all()

    def buttonPress(self, widget):		      
	  name = widget.get_name()
	  if name == "collect":
		self.ser.write("c")
	  elif name == "read":
		self.ser.write("r#FFFFFF")
	  else:
		print "ERROR: Invalid widget name, in on_changed function"  	

    def setup_serial(self):
	self.ser = serial.Serial()
	self.ser.setPort(self.serialPort)
	self.ser.baudrate = 57600
	self.ser.open()
	if (self.ser.isOpen()):
	  print "Serial Open"
	else:
	  print "Serial Closed"

PyApp()
gtk.main() 
