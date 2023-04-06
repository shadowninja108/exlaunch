#!/usr/bin/env python3

import ftplib
import os
from types import NoneType
from typing import Any, Dict, Self, Tuple

# https://ftputil.sschwarzer.net
import ftputil

PROGRAM_ID: str = os.getenv('PROGRAM_ID', '0100801011c3e000')

FTP_IP: str = os.getenv('FTP_IP', '192.168.0.235')
FTP_PORT: int = int(os.getenv('FTP_PORT', '5000'))
FTP_USERNAME: str = os.getenv('FTP_USERNAME', 'anonymous')
FTP_PASSWORD: str = os.getenv('FTP_PASSWORD', '')

OUT: str = os.getenv('OUT', os.path.abspath('deploy'))
SD_OUT: str = os.getenv('SD_OUT', f'atmosphere/contents/{PROGRAM_ID}/exefs')


class SessionFactory(ftplib.FTP):
    
    def __init__(self: Self, ftp_ip: str = '192.168.0.235', ftp_port: int = 5000, ftp_username: str = 'anonymous',
                 ftp_password: str = '', *args: Tuple[Any], **kwargs: Dict[str, Any]) -> NoneType:
        super().__init__()
        
        try:
            self.connect(ftp_ip, ftp_port)
        except ftputil.error.FTPOSError:
            raise
        else:
            try:
                self.login(ftp_username, ftp_password)
            except ftputil.error.FTPOSError:
                raise
            else:
                pass
            finally:
                pass
        finally:
            pass


def main(*args: Tuple[Any], **kwargs: Dict[str, Any]) -> NoneType:
    with ftputil.FTPHost(FTP_IP, FTP_PORT, FTP_USERNAME, FTP_PASSWORD, session_factory=SessionFactory) as ftp_host:
        ftp_host.makedirs(SD_OUT, exist_ok=True)
        # ftp_host.upload(os.path.join(OUT, 'main'), ftp_host.path.join(SD_OUT, 'main'))
        # ftp_host.upload(os.path.join(OUT, 'subsdk9'), ftp_host.path.join(SD_OUT, 'subsdk9'))
        for name in os.listdir(OUT):
            if os.path.isfile(os.path.join(OUT, name)):
                ftp_host.upload(os.path.join(OUT, name), ftp_host.path.join(SD_OUT, name))


if __name__ == '__main__':
    main()
