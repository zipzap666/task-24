import asyncio
import datetime
import logging
from google.protobuf.internal.encoder import _VarintBytes

import sys
from os.path import dirname, join, abspath
sys.path.insert(0, abspath(join(dirname(__file__), '..')))
from common.parser.parsTools import *
from common.proto.message_pb2 import WrapperMessage


class Server():
    def __init__(self, port=33333):
        self.port = port
        self.server = None
        self.connections = 0

    async def start(self):
        server = await asyncio.start_server(self.handle_read, "127.0.0.1", self.port)

        async with server:
            await server.serve_forever()

    async def handle_read(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        id = writer.get_extra_info("peername")[1]
        logging.info(f"CLIENT {id} CONNECTED")
        parser = DelimitedMessagesStreamParser(WrapperMessage)
        self.connections += 1
        try:
            while True:
                data = await reader.read(1)
                if not data:
                    break

                data = parser.parse(data)
                if len(data) == 0:
                    continue
                message = ""
                for item in data:
                    if item.HasField("request_for_slow_response"):
                        message = await self.slowResponse(item.request_for_slow_response.time_in_seconds_to_sleep)
                    elif item.HasField("request_for_fast_response"):
                        message = await self.fastResponse()
                writer.write(message)
                await writer.drain()
        except ConnectionResetError:
            pass
        finally:
            writer.close()
            logging.info(f"CLIENT {id} DISCONNECTED")
            self.connections -= 1

    async def fastResponse(self):
        wrapperMessage = WrapperMessage()
        wrapperMessage.fast_response.current_date_time = datetime.datetime.now(
        ).strftime('%Y%m%dT%H%M%S.%f')[:-3]
        message = _VarintBytes(wrapperMessage.ByteSize()) + \
            wrapperMessage.SerializeToString()

        return message

    async def slowResponse(self, timeToSleep):
        wrapperMessage = WrapperMessage()
        wrapperMessage.slow_response.connected_client_count = self.connections
        message = _VarintBytes(wrapperMessage.ByteSize()) + \
            wrapperMessage.SerializeToString()
        await asyncio.sleep(timeToSleep/1000)

        return message
