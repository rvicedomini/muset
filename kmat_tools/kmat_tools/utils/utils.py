from re import M
import sys
from xmlrpc.client import boolean
import subprocess
from datetime import datetime
from colorama import Fore, Style


def eprint(*args, **kwargs):
    log = open('kmat_tools.log', 'a')
    log.write(*args)
    log.write('\n')
    print(*args, file=sys.stderr, **kwargs)

def datetime_now():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

def print_error(msg):
    eprint(f'[{datetime_now()}] [' + f'{Fore.RED}error{Style.RESET_ALL}' + f'] {msg}')

def print_warning(msg):
    eprint(f'[{datetime_now()}] [' + f'{Fore.YELLOW}warning{Style.RESET_ALL}' + f'] {msg}')

def print_status(msg):
    eprint(f'[{datetime_now()}] [' + f'{Fore.GREEN}status{Style.RESET_ALL}' + f'] {msg}')

def remove_files(path):
    subprocess.run(["rm", "-rf", path],capture_output=True,text=True)

