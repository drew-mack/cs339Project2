import socket
import netifaces
import sys
import json
import time
import threading

class deviceManager:
    def __init__(self):
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.multicast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.MULTICAST_TTL = 32
        self.multicast_socket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, self.MULTICAST_TTL)
        self.server_socket.bind(('', 8000))
        self.MCAST_GRP = '224.0.0.1'
        self.MCAST_PORT = 5007
        self.server_socket.settimeout(3)
        self.server_address = '67.163.37.156'
        self.front_end_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.front_end_socket.bind(('0.0.0.0', 7999))
        #self.server_address = 'localhost'
        self.objects = []
        self.threads = []
        self.threads_front = []
    def listen(self):
        message = ''
        address = ''
        try:
            message, address = self.server_socket.recvfrom(1024)
            if str(message) == "b'aye'":
                self.server_socket.sendto("what up boo".encode(), address)
                return
        except socket.timeout:
            print("timeout")
            return
        self.parse_json(message)
    def getMask(self):
        interfaces = netifaces.interfaces()
        addresses = {}
        for i in interfaces:
            tempDict = netifaces.ifaddresses(i)
            if len(tempDict.keys()) > 0:
                addresses.update({i: (tempDict.get(2)[0]['netmask'], tempDict.get(2)[0]['addr'])})
        return addresses

    def parse_json(self, packet):
        try:
            json_message = eval(packet)
            if json_message['op'] == 'heartbeat':
                if(not self.name_check(json_message["port"])):
                    print('object added.')
                    self.objects.append((json_message["port"],json_message))
                    self.init_timeout_obj(json_message["port"])
                    return "done"
                else:
                    self.reset_timeout(json_message["port"])
                    return "Done"
            if json_message['op'] == 'delete':
                print('object deleted.')
                self.remove_Item(json_message["object"]["name"])
                print(self.objects)
                self.client_socket.sendto(str(json_message).encode(), (self.server_address, 7999))
                return "done"
            if json_message['op'] == 'list':
                return self.display_objects()
        except NameError:
            print('Incorrect Json format')
            return "Bad"

    def name_check(self, port):
        for i in self.objects:
            if i[0] == port:
                return True
        return False
    #multicast function
    def clapBack(self,port,ip):
        #self.client_socket.sendto("robot".encode(), (ip, int(port)))
        self.multicast_socket.sendto("robot".encode(), (self.MCAST_GRP, self.MCAST_PORT))
    def remove_Item(self,item):
        for i in self.objects:
            if item == i[0]:
                self.objects.remove(i)

    def object_scan(self):
        json_dictionary = {"op" : "routine", "objects" : '', "len" :len(self.objects)}
        tmpList =[]
        for i in self.objects:
            i[1]["op"] = "routine"
            self.client_socket.sendto(json.dumps(i[1]).encode(), (self.server_address, 7999))
            time.sleep(0.3)

    def init_timeout_obj(self, object_port):
        t = threading.Thread(target=self.timeout, args=(object_port,))
        self.threads.append((t, object_port))
        t.start()

    def init_front(self):
        t = threading.Thread(target=self.handle_front)
        self.threads_front.append(t)
        t.start()

    def reset_timeout(self,port):
        for i in self.threads:
            if(i[1] == port):
                i[0].do_run = True
                return

    def timeout(self,port):
        while True:
            t = threading.currentThread()
            if(getattr(t, "do_run", True)):
                time.sleep(2)
                t.do_run = False
            else:
                time.sleep(2)
                if(not getattr(t, "do_run", True)):
                    self.remove_Item(port)
                    print('removed '+ str(port))
                    return port
        return port

    def handle_front(self):
        self.front_end_socket.listen(2)
        connection, client_address = self.front_end_socket.accept()
        while True:
            try:
                message = connection.recv(1024)
                print(message)
                connection.send(self.parse_json(message).encode())
            except KeyboardInterrupt:
                self.front_end_socket.shutdown(socket.SHUT_RDWR)

    def display_objects(self):
        retStr = ""
        for i in self.objects:
            retStr += "Name: " + str(i[1]["name"]) + "\tOn: " + str(i[1]["switch"])
        return retStr

    def socket_close(self):
        self.server_socket.close()
        self.multicast_socket.close()
        self.client_socket.close()


def main():
    devices = deviceManager()
    devices.init_front()
    while True:
        try:
            devices.listen()
        except KeyboardInterrupt:
            devices.socket_close()
main()
