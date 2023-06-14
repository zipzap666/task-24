import asyncio

from Server import Server

if __name__ == "__main__":
    server = Server()

    asyncio.run(server.start())