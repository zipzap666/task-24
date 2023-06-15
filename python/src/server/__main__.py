import asyncio
import logging

from Server import Server
from ServerSettingsManager import ServerSettingsManager

def logSet():
    fileh = logging.FileHandler('server.log', 'a')
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    fileh.setFormatter(formatter)
    
    log = logging.getLogger()
    for hdlr in log.handlers[:]:
        log.removeHandler(hdlr)
    log.addHandler(fileh)

if __name__ == "__main__":
    settings = ServerSettingsManager('server_config.ini')
    settings.setDefault()
    config = settings.loadConfig()
    logSet()
    level = eval('logging.' + config['LOGGER']['level'])
    logger = logging.getLogger().setLevel(level)
   
    server = Server(int(config['SERVER']['port']))

    asyncio.run(server.start())
