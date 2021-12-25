import logging
import sys


def get_code(array, offset):
    block = array[2] - 0x80
    char = array[3] - 0x80
    return block * 0x40 + char - offset


def convert_array(array, offset):
    if len(array) != 4:
        return "<wrong length of {}>".format(len(array))
    if array[:2].hex() != "f3a0":
        return "<unknown prefix of {}>".format(array[:2].hex())
    code = get_code(array, offset)
    if 0 <= code <= 127:
        return chr(code)
    return "<unknown code {}>".format(code)


def convert_arrays(arrays, offset):
    parts = [convert_array(array, offset) for array in arrays]
    result = "".join(parts)
    if "<unknown " in result:
        return "<bad decoding>"
    return result


def main():
    filename = "Invictus.txt"
    with open(filename, "rb") as a_file:
        s = a_file.read()
        non_ascii = []
        cur_group = []
        for byte in s:
            if byte > 127:
                cur_group.append(byte)
            elif cur_group:
                non_ascii.append(bytearray(cur_group))
                cur_group = []
        if cur_group:
            non_ascii.append(bytearray(cur_group))
            cur_group = []
        for offset in range(-128, 127):
            decoding = convert_arrays(non_ascii, offset)
            if decoding == "<bad decoding>":
                continue
            logging.info("OFFSET: %d", offset)
            logging.info("decoding: %s", decoding)
            if "MANDELA" in decoding:
                print(decoding)
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())