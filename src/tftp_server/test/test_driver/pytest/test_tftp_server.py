import pytest

from signal import SIGINT, SIGTERM
from subprocess import PIPE, Popen
from shutil import copyfile, rmtree
from os import path, makedirs, getcwd
from pathlib import PurePath
from tempfile import gettempdir
from filecmp import cmp


class ServerProc:
    def __enter__(self):
        self.server_proc = Popen(['./TftpServerTestDriver', ''])

    def __exit__(self, exc_type, exc_value, traceback):
        self.server_proc.send_signal(SIGINT)
        self.server_proc.communicate()


# could use pytest tempdir plugin instead
def temp_dir():
    return path.join(gettempdir(),'zwiibac_tftp_server_test_driver_bb18da59-5513-44cd-9a56-38b89db54b64')


def setup_dirs(*dirs):
    for dir in dirs:
        if path.exists(dir):
            rmtree(dir)
        makedirs(dir, exist_ok=True)


def setup_file(directory, file):
    src_file = path.join(getcwd(), 'pytest', file)
    dest_file = path.join(directory, file)
    copyfile(src_file, dest_file)
    return (src_file, dest_file)


@pytest.fixture
def server_dir():
    directory = path.join(temp_dir(), 'server_root')
    setup_dirs(directory)
    yield directory


@pytest.fixture
def download_dir():
    directory = path.join(temp_dir(), 'download')
    setup_dirs(directory)
    yield directory


@pytest.mark.parametrize('file', ['lorem_1.txt', 'lorem_50.txt', 'lorem_100.txt'])
def test_get(server_dir, download_dir, file):
    expected_file,_ = setup_file(server_dir, file)
    actual_file = path.join(download_dir, file)
    with ServerProc():
        client_proc = Popen(['tftp', '-m', 'octet', '-v', '127.0.0.1', '1025'], stdin=PIPE, universal_newlines=True)
        client_proc.communicate(input=f'get {file} {actual_file}\ntrace\nquit\n')   
    assert cmp(actual_file, expected_file)


@pytest.mark.parametrize('file', ['lorem_1.txt', 'lorem_50.txt', 'lorem_100.txt'])
def test_put(server_dir, file):
    expected_file = path.join(getcwd(), 'pytest', file)
    actual_file = path.join(server_dir, file)
    with ServerProc():
        client_proc = Popen(['tftp', '-m', 'octet', '-v', '127.0.0.1', '1025'], stdin=PIPE, universal_newlines=True)
        client_proc.communicate(input=f'put {expected_file} {file}\ntrace\nquit\n')
    assert cmp(actual_file, expected_file)


@pytest.mark.parametrize('file', ['lorem_50.txt'])
def test_get_parallel(server_dir, download_dir, file):
    expected_file,_ = setup_file(server_dir, file)
    with ServerProc():
        get_requets = [start_get_file(download_dir, file, suffix) for suffix in range(100)]
        for proc,_ in get_requets: proc.communicate()
    for _,actual_file in get_requets: assert cmp(actual_file, expected_file)


def start_get_file(download_dir, file, suffix):
    file_path = PurePath(file)
    dest_path = PurePath(download_dir)/f'{file_path.stem}-{suffix}{file_path.suffix}'
    client_proc = Popen(['tftp', '-m', 'octet', '-v', '127.0.0.1', '1025', '-c', 'get', file, dest_path])
    return (client_proc, dest_path)


@pytest.mark.parametrize('file', ['lorem_50.txt'])
def test_put_parallel(server_dir, download_dir, file):
    expected_file = path.join(getcwd(), 'pytest', file)
    with ServerProc():
        put_requets = [start_put_file(server_dir, expected_file, suffix) for suffix in range(10)]
        for proc,_ in put_requets: proc.communicate()
    for _,actual_file in put_requets: assert cmp(actual_file, expected_file)


def start_put_file(server_dir, file, suffix):
    file_path = PurePath(file)
    dest_path = PurePath(f'{file_path.stem}-{suffix}{file_path.suffix}')
    client_proc = Popen(['tftp', '-m', 'octet', '-v', '127.0.0.1', '1025', '-c', 'put', file, dest_path])
    return (client_proc, path.join(server_dir, dest_path))
