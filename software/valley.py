#!/bin/python3
import argparse
import asyncio
import coloredlogs
import json
import logging
import time

import jsonpath_ng
import requests
import serial_asyncio


logger = logging.getLogger(__name__)


COMMANDS = {}


def register_action():
    global COMMANDS

    def decorator(fn):
        name = fn.__name__
        COMMANDS[name] = fn

        return fn

    return decorator


@register_action()
def get_json(url, path=None):
    result = requests.get(url, headers={'Accept': 'application/json'})
    result.raise_for_status()

    try:
        result = result.json()
        if path:
            expr = jsonpath_ng.parse(path)
            return {
                'matches': [v.value for v in expr.find(result)]
            }
        return result
    except json.JSONDecodeError:
        logger.exception(
            "Unable to decode content as JSON: ",
            result.content
        )
        raise


@register_action()
def hello():
    return {
        'time': time.time()
    }


@register_action()
def debug(message):
    logger.info(message)


@register_action()
def heartbeat(**kwargs):
    return {
        'time': time.time()
    }


class ValleyDriver(asyncio.Protocol):
    def connection_made(self, transport):
        logger.debug('Serial connection opened...')
        self.transport = transport
        self.transport.serial.rts = False

        self._remaining = bytearray()

    def data_received(self, data):
        data = self._remaining + data

        message = bytearray()
        for byte in data:
            if byte == ord('\n'):
                try:
                    if message.strip():
                        request = Request(message.decode('utf8'))
                        self.handle_message(request)
                except json.JSONDecodeError:
                    logger.error(
                        "Unable to decode: %s",
                        message.decode('utf8').strip()
                    )
                message = bytearray()
            message.append(byte)

        self._remaining = message

    def handle_message(self, request):
        logger.debug('Handling message: %s.', request)
        try:
            cmd = COMMANDS[request.command]
        except KeyError:
            logger.error(
                "No command named '%s' available.",
                request.command,
            )
            return

        ok = True
        try:
            result = cmd(**request.params)
        except Exception as e:
            logger.exception(
                "Processing %s failed: %s",
                request,
                e
            )
            ok = False
            result = str(e)

        if result:
            reply = Response(
                request,
                result,
                ok=ok,
            )
            reply_bytes = reply.as_bytes()
            logger.info(
                'Sending reply: %s (%s bytes).',
                reply_bytes,
                len(reply_bytes),
            )
            self.transport.write(reply_bytes)

    def connection_lost(self, exc):
        logger.info('Connection lost.')
        asyncio.get_event_loop().stop()


class Request(object):
    def __init__(self, data):
        self._data = json.loads(data.strip())

        if not isinstance(self._data, dict):
            raise ValueError("Invalid request: %s" % self._data)

    @property
    def message_id(self):
        return self._data.get('msg_id')

    @property
    def command(self):
        return self._data.get('cmd')

    @property
    def params(self):
        return self._data.get('params')

    def __str__(self):
        return json.dumps(self._data)


class Response(object):
    def __init__(self, request, result, ok=True):
        self._message_id = request.message_id
        self._result = result
        self._ok = ok

    def as_dict(self):
        return {
            'msg_id': self._message_id,
            'result': self._result,
            'ok': self._ok,
        }

    def as_bytes(self):
        return json.dumps(
            self.as_dict(),
            separators=(',', ':'),
        ).encode('utf-8')

    def __str__(self):
        return self._result


def main(port, baud):
    loop = asyncio.get_event_loop()
    coro = serial_asyncio.create_serial_connection(
        loop,
        ValleyDriver,
        port,
        baudrate=baud,
    )
    loop.run_until_complete(coro)
    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(e)
    finally:
        loop.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('port')
    parser.add_argument('baud')
    parser.add_argument('--loglevel', default='INFO')
    parser.add_argument('--forever', default=False)
    args = parser.parse_args()

    coloredlogs.install(level=args.loglevel)
    while True:
        try:
            logger.info('Starting...')
            main(args.port, args.baud)

            if not args.forever:
                break
        except KeyboardInterrupt:
            break
        except Exception:
            pass
