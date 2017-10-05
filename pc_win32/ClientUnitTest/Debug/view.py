

def msg(text, time = 1000):
	import sys
	sys.argv = ["fnome"]
	import Tkinter
	label = Tkinter.Label(text=text, font=('Times','40'), fg='green', bg='white')
	label.master.overrideredirect(True)
	label.master.geometry("+50+50")
	label.master.lift()
	label.master.wm_attributes("-topmost", True)
	label.master.wm_attributes("-disabled", True)
	label.master.wm_attributes("-transparentcolor", "white")
	label.pack()
	label.after(time, label.master.destroy)
	label.mainloop()


def alert(text, time = 1000):
	import sys
	sys.argv = ["fnome"]
	import Tkinter
	label = Tkinter.Label(text=text, font=('Times','40'), fg='red', bg='white')
	label.master.overrideredirect(True)
	label.master.geometry("+400+50")
	label.master.lift()
	label.master.wm_attributes("-topmost", True)
	label.master.wm_attributes("-disabled", True)
	label.master.wm_attributes("-transparentcolor", "white")
	label.pack()
	label.after(time, label.master.destroy)
	label.mainloop()


'''

 #Demonstrate the use of python threading

import time
import threading

class MyThread(threading.Thread):

    text = ""
    time = 1000;
    color = "green"
    
    def __init__(self):
        threading.Thread.__init__(self)
        
    def run(self):
        import sys
        sys.argv = ["fnome"]
        import Tkinter
        label = Tkinter.Label(text=self.text, font=('Times','30'), fg=self.color, bg='white')
        label.master.overrideredirect(True)
        label.master.geometry("+50+50")
        label.master.lift()
        label.master.wm_attributes("-topmost", True)
        label.master.wm_attributes("-disabled", True)
        label.master.wm_attributes("-transparentcolor", "white")
        label.pack()
        label.after(self.time, label.master.destroy)
        label.mainloop()

def msg(text, time = 1000):
    print 'Create and run MyThread'
    background = MyThread()
    background.text = text;
    background.time = time;
    background.color = "green";
    background.start()
    return
    #background.join()

def alert(text, time = 1000):
    print 'Create and run MyThread'
    background = MyThread()
    background.text = text;
    background.time = time;
    background.color = "red";
    background.start()
    return
    #background.join()


#msg("oI1",3000)
#msg("oI2",3000)


    



def msg(text,time=1000):
	#for i in range(1000):
	print text

def alert(text,time=1000):
	for i in range(1000):
		print text

'''