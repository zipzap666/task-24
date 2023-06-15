import configparser
import os.path
import logging

class ServerSettingsManager():
    def __init__(self, path):
        self.config = configparser.ConfigParser()
        self.path = path

    def setDefault(self):
        #logging.debug('ServerSettingsManager::setDefault')
        #logging.info('Set default config')
        #logging.warning('File operation')

        self.config.clear()
        
        self.config['SERVER'] = {
            'port': 33333
        }

        self.config['LOGGER'] = {
            'level': 'DEBUG', 
        }

        try:
            with open(self.path, 'w') as configfile:
                self.config.write(configfile)
        except OSError as e:
            logging.error(e)
            self.configError.emit(-1, str(e))

    def loadConfig(self):
        #logging.debug('ServerSettingsManager::loadConfig')
        #logging.info('Get config from file')
        #logging.warning('File operation')

        if not os.path.exists(self.path):
            self.setDefault()
        self.config.read(self.path)

        output_dict=dict()
        for section in ['SERVER', 'LOGGER']:
            items=self.config.items(section)
            output_dict[section]=dict(items)
        return output_dict