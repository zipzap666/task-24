import socket
import sys
from parser.parsTools import *
from proto.message_pb2 import WrapperMessage
from google.protobuf.internal.encoder import _VarintBytes

if __name__ == "__main__":
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
    server_address = ('localhost', 33333)
    sock.connect(server_address)

    msg1, msg2, msg3, msg4 = WrapperMessage(), WrapperMessage(), WrapperMessage(), WrapperMessage()
    msg1.fast_response.current_date_time = "10"
    msg2.slow_response.connected_client_count = 10
    msg3.request_for_fast_response.SetInParent()
    msg4.request_for_slow_response.time_in_seconds_to_sleep = 10
    
    messages = _VarintBytes(msg1.ByteSize()) + msg1.SerializeToString()
    messages += _VarintBytes(msg2.ByteSize()) + msg2.SerializeToString()
    messages += _VarintBytes(msg3.ByteSize()) + msg3.SerializeToString()
    messages += _VarintBytes(msg4.ByteSize()) + msg4.SerializeToString()



    try:
    
        # Send data
        message = 'This is the message.  It will be repeated.'
        sock.sendall(messages)
        data = sock.recv(16)
        print(data)
    finally:
        sock.close()