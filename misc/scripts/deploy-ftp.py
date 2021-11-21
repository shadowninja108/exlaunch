import ftplib
import os
from ftplib import FTP
from os import environ
from os import path
import pathlib

def get_and_assert_nonempty_var(name):
    val = environ.get(name)
    if val:
        return val
    print(f"{name} is empty!")
    exit()

console_ip =        get_and_assert_nonempty_var("FTP_IP")
console_port =      get_and_assert_nonempty_var("FTP_PORT")
console_username =  get_and_assert_nonempty_var("FTP_USERNAME")
sd_out =            get_and_assert_nonempty_var("SD_OUT")
local_out =         get_and_assert_nonempty_var("OUT")

# Password may be empty.
console_password =  environ["FTP_PASSWORD"]


with FTP() as ftp:
    # Login/connect to console
    ftp.connect(console_ip, int(console_port))
    ftp.login(console_username, console_password)

    # I don't feel comfortable doing this...
    # ftp.rmd(sd_out)

    # Make SD out directory recursively.
    pl_sd_out = pathlib.PurePosixPath(sd_out)
    for i in range(len(pl_sd_out.parents)-2, -1, -1):
        d = pl_sd_out.parents[i]
        ftp.mkd(str(d))
    ftp.mkd(str(pl_sd_out))
    
    # Move to created directory
    ftp.cwd(str(pl_sd_out))

    for local_filename in os.listdir(local_out):
        # Get absolute path for the source file.
        local_filepath  = path.join(local_out, local_filename)
        
        # Only copy files.
        if path.isdir(local_filepath):
            continue

        # Upload file.
        with open(local_filepath, "rb") as f:
            ftp.storbinary(f"STOR {local_filename}", f)
