import threading 

class StopableThread(threading.Thread):
    running = False
    stopping = False

    def start(self):
        self.running = True
        super(StopableThread, self).start()

    def stop(self):
        self.stopping = True

    def IsRunning(self):
        return self.running

    def ToStop(self):
        return self.stopping