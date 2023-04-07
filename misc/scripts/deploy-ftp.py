#!/usr/bin/env python3

import ftplib
import os
from types import NoneType
from typing import Any, Dict, Optional, Self, Tuple, Union

# https://ftputil.sschwarzer.net
import ftputil


def getenv(key: str, default: Optional[str] = None, *args: Tuple[Any], **kwargs: Dict[str, Any]) -> str:
    '''Wrapper for os.getenv.'''
    
    value: Union[None, str] = os.getenv(key)
    if value is not None:
        return value
    elif default is not None:
        return default
    else:
        raise KeyError


PROGRAM_ID: str = getenv('PROGRAM_ID')

FTP_IP: str = getenv('FTP_IP')
FTP_PORT: int = int(getenv('FTP_PORT'))
FTP_USERNAME: str = getenv('FTP_USERNAME')
FTP_PASSWORD: str = getenv('FTP_PASSWORD', '')

OUT: str = getenv('OUT')
SD_OUT: str = getenv('SD_OUT')


class SessionFactory(ftplib.FTP):
    '''Session factory for FTPHost.'''
    
    def __init__(self: Self, ftp_ip: str = '192.168.0.235', ftp_port: int = 5000, ftp_username: str = 'anonymous',
                 ftp_password: str = '', *args: Tuple[Any], **kwargs: Dict[str, Any]) -> NoneType:
        super().__init__()
        
        try:
            self.connect(ftp_ip, ftp_port)  # connect to ftp_ip and ftp_port
        except ftputil.error.FTPOSError:
            raise
        else:
            try:
                self.login(ftp_username, ftp_password)  # login as ftp_username (ftp_password is optional)
            except ftputil.error.FTPOSError:
                raise
            else:
                pass
            finally:
                pass  # TODO
        finally:
            pass  # TODO


def main(*args: Tuple[Any], **kwargs: Dict[str, Any]) -> NoneType:
    with ftputil.FTPHost(FTP_IP, FTP_PORT, FTP_USERNAME, FTP_PASSWORD, session_factory=SessionFactory) as ftp_host:
        ftp_host.makedirs(SD_OUT, exist_ok=True)  # make SD_OUT
        ftp_host.upload(os.path.join(OUT, 'main'), ftp_host.path.join(SD_OUT, 'main.npdm'))  # upload main.npdm
        ftp_host.upload(os.path.join(OUT, 'subsdk9'), ftp_host.path.join(SD_OUT, 'subsdk9'))  # upload subsdk9


if __name__ == '__main__':
    main()
