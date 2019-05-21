import socket
import pyaudio
import wave
import _thread
import sys
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


class MyWindow(Gtk.Window):

    def __init__(self, name):
        Gtk.Window.__init__(self, title="Voice call")

        self.status_message = "Connection in progress..."

        self.set_default_size(300, 300);

        grid = Gtk.Grid()
        grid.set_column_spacing(10)
        grid.set_row_spacing(10)
        grid.set_margin_bottom(20)
        grid.set_margin_top(5)
        grid.set_margin_end(20)
        grid.set_margin_start(20)
        #grid.set_size_request(300, 300);
        self.add(grid)

        self.name_label = Gtk.Label()
        self.name_label.set_markup("<big>"+name+"</big>")
        grid.attach(self.name_label, 0, 0, 2, 1)


        self.volume_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)

        self.volume_label = Gtk.Label("Volume:")
        self.volume_box.pack_start(self.volume_label, False, False, 0)

        self.volume_scale = Gtk.VScale.new_with_range(0, 100, 10)
        self.volume_scale.set_inverted(True)
        self.volume_scale.set_value(50)
        self.volume_scale.set_value_pos(Gtk.PositionType.LEFT)
        self.volume_box.pack_start(self.volume_scale, True, True, 0)

        self.volume_box.set_size_request(50, 250);
        grid.attach(self.volume_box, 0, 1, 1, 2)

        self.status_label = Gtk.Label(self.status_message)
        self.status_label.set_vexpand(True)
        self.status_label.set_hexpand(True)
        self.status_label.set_valign(Gtk.Align.START);
        grid.attach(self.status_label, 1, 1, 1, 1)

        self.end_btn = Gtk.Button(label="End Chat")
        self.end_btn.set_halign(Gtk.Align.END);
        self.end_btn.set_vexpand(False);
        self.end_btn.connect("clicked", self.on_button_clicked)
        grid.attach(self.end_btn, 1, 2, 1, 1)


        grid.set_vexpand(False);
        grid.set_hexpand(False);

    def on_button_clicked(self, widget):
        self.destroy()



# record
CHUNK = 1024 # 512
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 20000


ip = sys.argv[2]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((ip, 50000))

p = pyaudio.PyAudio()

receive_stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, output=True, frames_per_buffer=CHUNK)
send_stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK)

print("Voice chat running")

def receive_data():
    while True:
        try:
            data = s.recv(1024)
            receive_stream.write(data)
        except:
            pass


def send_data():
    while True:
        try:
            data = send_stream.read(CHUNK)
            s.sendall(data)
        except:
            pass

_thread.start_new_thread(receive_data, ())
_thread.start_new_thread(send_data, ())

name = sys.argv[1]
win = MyWindow(name)
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()
