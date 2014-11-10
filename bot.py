import os
import requests
import zulipbot


class CarBot(object):
    def __init__(self, email, key):
        self.client = zulipbot.Bot(email, key)
        self.client.subscribe_all()
        self.car_server = 'http://10.0.7.167'

    def move(self, direction):
        return requests.get(self.car_server + '/{}'.format(direction))

    def process_message(self, msg):
        content = msg['content'].split()
        # sender_email = msg['sender_email']

        direction = content[1]

        if content[0] == 'car' or content[0] == '@**Car Bot**':
            if msg['type'] == 'stream':
                self.move(direction)
                self.send_stream_message(msg, 'I am going {}'.format(direction))


def main():
    bot = CarBot(os.environ['ZULIP_EMAIL'], os.environ['ZULIP_KEY'])
    bot.client.call_on_each_message(bot.process_message)


if __name__ == '__main__':
    main()
