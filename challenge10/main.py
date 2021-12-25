import json
import logging
import sys


def main():
    # tshark -r icmps.pcap -T json --no-duplicate-keys > icmps_nodup.json
    with open("icmps_nodup.json") as a_file:
        jobj = json.load(a_file)
        items = []
        idents = []
        for packet in jobj:
            icmp = packet["_source"]["layers"]["icmp"]
            ident = icmp["icmp.ident"][0]
            seq = int(icmp["icmp.seq"])
            idents.append(chr(int(ident)))
            d = icmp["data"]["data.data"]
            items.append((seq, d))
        logging.info("icmp.ident: %s", "".join(idents))
        items.sort()
        bs = bytearray()
        for seq, d in items:
            bs.append(int(d, 16))
    with open("qr.png", "wb") as a_file:
        a_file.write(bs)
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())