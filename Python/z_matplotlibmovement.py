import sys
import matplotlib.pyplot as plt
import time
import numpy
from scipy.io import wavfile 

fileName = './sound/superpenguin.wav'
rate, data = wavfile.read(fileName)

fig = plt.figure()
ax = fig.add_subplot(111)

max_height = max(data)
min_height = min(data)
n_pts = len(data)
y1 = [min_height, max_height]
x1 = [0, 0]
y = data
x = numpy.arange(0, len(data))

# draw the data
line1, = ax.plot(x, y, color='blue')

# fix the limits of the plot
ax.set_ylim(min_height, max_height)
ax.set_xlim(0, len(data))

# draw the plot so that we can capture the background and then use blitting
plt.show(block=False)

# get the canvas object
canvas = ax.figure.canvas
background = canvas.copy_from_bbox(ax.bbox)

# add the progress line.
# XXX consider using axvline
line, = ax.plot(x1, y1, color='r', animated=True) 


starttime=time.time()
mytimer=0
mytimer_ref=0

def update(canvas, line, ax):
    # revert the canvas to the state before any progress line was drawn
    canvas.restore_region(background)

    # compute the distance that the progress line has made (based on running time) 
    t = time.time() - starttime
    mytimer = t + mytimer_ref
    x1 = [mytimer,mytimer]
    # update the progress line with its new position
    line.set_xdata(x1)
    # draw the line, and blit the axes
    ax.draw_artist(line)
    canvas.blit(ax.bbox)

def onclick(event):
    global starttime
    starttime=time.time()
    global mytimer_ref
    mytimer_ref=event.xdata
    print "starttime",starttime

# when i click the mouse over a point, line goes to that point and start moving from there. 
cid=line1.figure.canvas.mpl_connect('button_press_event',onclick) 
timer=fig.canvas.new_timer(interval=3)
args=[canvas,line,ax]
timer.add_callback(update,*args) # every 100ms it calls update function
timer.start()
plt.show()