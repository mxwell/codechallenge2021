import gzip
import logging
import requests
import sys


USER = "plyba"
PASSWORD = "xvyy_nyy_uhznaf"
FILE_URL = "http://codechallenge-daemons.0x14.net:13963/here-is-the-position"


def get_creds():
    AB_SIZE = ord('z') - ord('a') + 1

    def shift_alpha(s, offset):
        result = []
        for c in s:
            if c.isalpha():
                rel = ord(c) - ord('a')
                rel = (rel + offset) % AB_SIZE
                result.append(chr(rel + ord('a')))
            else:
                result.append(c)
        return "".join(result)
    
    off = AB_SIZE // 2
    return shift_alpha(USER, off), shift_alpha(PASSWORD, off)


def download_file(user, password):
    logging.info("Using credentials %s:%s", user, password)
    response = requests.get(FILE_URL, auth=requests.auth.HTTPBasicAuth(user, password))
    text = response.text
    logging.info("URL %s responded with status %d and text of size %d", FILE_URL, response.status_code, len(text))
    return text   


def parse_hexdump(text):
    array = bytearray()
    for raw in text.split("\n"):
        double_space = raw.find("  ")
        h = raw[10:double_space].replace(" ", "")
        for i in range(0, len(h), 2):
            num = int(h[i:i+2], 16)
            array.append(num)
    return array


def get_zero_width(data):
    cur_group = bytearray()
    groups = []
    for b in data:
        if b > 127:
            cur_group.append(b)
        elif len(cur_group) > 0:
            if len(cur_group) == 3 and cur_group[2] != 0x99:
                groups.append(cur_group)
            cur_group = bytearray()
    logging.info("Got %d groups", len(groups))
    return groups


def parse_digits(groups):
    digits = []
    for i in range(0, len(groups), 8):
        val = 0
        # Recognize 'e2 80 8b' as 0,
        #       and 'e2 80 8c' as 1
        for group in groups[i:i + 8]:
            val *= 2
            if group[2] != 0x8b:
                val += 1
        digit = chr(val)
        digits.append(digit)
    logging.info("Digits: %s", "".join(digits))
    return digits


def main():
    user, password = get_creds()
    hexdump = download_file(user, password)
    gzipped_data = parse_hexdump(hexdump)
    logging.info("Got %d bytes of gzipped data", len(gzipped_data))
    unpacked = gzip.decompress(gzipped_data)
    logging.info("Unpacked text: %s", unpacked.decode())
    groups = get_zero_width(unpacked)
    digits = parse_digits(groups)
    print("".join(digits))
    
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())