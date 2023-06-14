import asyncio
from google.protobuf.internal.encoder import _VarintBytes

import sys
from os.path import dirname, join, abspath
sys.path.insert(0, abspath(join(dirname(__file__), '..')))

from common.parser.parsTools import *
from common.proto.message_pb2 import WrapperMessage

class ServerProtocol(asyncio.Protocol):
    def __init__(self) -> None:
        super().__init__()
        self.parser = DelimitedMessagesStreamParser(WrapperMessage)

    async def connection_made(self, transport):
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))
        self.transport = transport

    async def data_received(self, message):
        data = self.parser.parse(message)
        for item in data:
            if item.HasField("request_for_slow_response"):
                self.slowResponse(item.request_for_slow_response.time_in_seconds_to_sleep)
                print("slow")
            elif item.HasField("request_for_fast_response"):
                self.fastResponse()
                print("fast")
    
    async def fastResponse(self):
        wrapperMessage = WrapperMessage()
        wrapperMessage.fast_response.current_date_time = "10"
        message = _VarintBytes(wrapperMessage.ByteSize()) + wrapperMessage.SerializeToString()
        
        self.transport.write(message)

    async def slowResponse(self, timeToSleep):
        wrapperMessage = WrapperMessage()
        wrapperMessage.slow_response.connected_client_count = 1
        message = _VarintBytes(wrapperMessage.ByteSize()) + wrapperMessage.SerializeToString()
        await asyncio.sleep(timeToSleep)

        self.transport.write(message)