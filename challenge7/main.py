#! /usr/bin/env python3

"""
Sample session:

Welcome, my friend! You are at the beginning of a long dark dungeon. What do you want to do? Enter order (empty line or 'bye' for finish):
north - south - east - west - look - where am I - go to x,y - is exit? - bye
> where am I
(0, 0)
> is exit?
No. Sorry, traveller...
> look
Well, well, well, my friend. You could do these movements: west
> west
Great movement. Here is your new position: (1, 0)
> look
Well, well, well, my friend. You could do these movements: north west east
> is exit?
No. Sorry, traveller...
"""

import logging
import sys
import telnetlib

TN_HOST = "codechallenge-daemons.0x14.net"
TN_PORT = 4321
REQUEST_COUNT_LIMIT = 10000
GRID_OFFSET = 110
GRID_SIZE = GRID_OFFSET * 2 + 1
MOVEMENTS = {
    "west": (1, 0),
    "east": (-1, 0),
    "north": (0, 1),
    "south": (0, -1),
}


class Cell(object):
    def __init__(self):
        self.checked = False
        self.is_exit = False
        self.allowed_moves = None
        self.parent = None


class Queue(object):
    def __init__(self):
        self.items = []
        self.head = 0
    
    def empty(self):
        return self.head >= len(self.items)
    
    def pop_front(self):
        assert not self.empty()
        item = self.items[self.head]
        self.head += 1
        return item
    
    def push(self, item):
        self.items.append(item)


def init_grid():
    grid = []
    for i in range(GRID_SIZE):
        row = []
        for j in range(GRID_SIZE):
            row.append(Cell())
        grid.append(row)
    return grid


def read_new_position(response):
    hint = "Here is your new position: "
    pos = response.find(hint)
    assert pos >= 0
    pos += len(hint)
    point_parts = response[pos:].strip("()").split(", ")
    x, y = [int(p) for p in point_parts]
    return x, y


class Solution(object):

    def __init__(self, tn):
        self.tn = tn
        self.grid = init_grid()
        self.curx = 0
        self.cury = 0
        self.requests_count = 0
        self.found_exit = None
        self.queue = Queue()
        self.minx = 0
        self.maxx = 0
        self.miny = 0
        self.maxy = 0
    
    def read_invite(self):
        self.tn.read_until(b"- bye\n")
    
    def request(self, message):
        assert not message.endswith("\n")
        logging.info("Sending %s", message)
        self.tn.write(message.encode("ascii") + b"\n")
        response = self.tn.read_until(b"\n").decode("ascii").strip()
        logging.info("Received %s", response)
        self.requests_count += 1
        return response
    
    def get_cell(self, x, y):
        grid_x = GRID_OFFSET + x
        grid_y = GRID_OFFSET + y
        assert 0 <= grid_x < GRID_SIZE
        assert 0 <= grid_y < GRID_SIZE
        return self.grid[grid_x][grid_y]
    
    def get_current_cell(self):
        return self.get_cell(*self.get_current_point())
    
    def get_current_point(self):
        return (self.curx, self.cury)
    
    def check_current_cell(self):
        cur = self.get_current_cell()
        assert not cur.checked
        response = self.request("is exit?")
        cur.is_exit = not response.startswith("No")
        if cur.is_exit:
            self.found_exit = self.get_current_point()
        response = self.request("look")
        colon = response.find(":")
        if colon < 0:
            logging.error("Unexpected response: %s", response)
        else:
            allowed_directions = response[colon + 1:].strip()
            cur.allowed_moves = allowed_directions.split()
        cur.checked = True
        self.queue.push((self.curx, self.cury))
    
    def get_adjacent_cells(self, x, y):
        cell = self.get_cell(x, y)
        result = []
        if not cell.checked:
            logging.error("trying to get adjacent cells for unchecked cell")
            return result
        for move in cell.allowed_moves:
            dx, dy = MOVEMENTS[move]
            result.append((move, x + dx, y + dy))
        return result
    
    def handle_move(self, response, targetx, targety):
        x, y = read_new_position(response)
        assert targetx == x
        assert targety == y
        self.curx = x
        self.cury = y
        self.minx = min(self.minx, x)
        self.maxx = max(self.maxx, x)
        self.miny = min(self.miny, y)
        self.maxy = max(self.maxy, y)
        logging.info("Moved to %d,%d", x, y)        

    def move_to_unchecked(self, move, targetx, targety):
        assert move in MOVEMENTS
        parent = (self.curx, self.cury)
        response = self.request(move)
        self.handle_move(response, targetx, targety)
        target = self.get_cell(targetx, targety)
        target.parent = parent
    
    def move_to_checked(self, targetx, targety):
        response = self.request("go to {},{}".format(targetx, targety))
        self.handle_move(response, targetx, targety)
    
    def dequeue(self):
        assert not self.queue.empty()
        basex, basey = self.queue.pop_front()
        adjacent = self.get_adjacent_cells(basex, basey)
        for move, x, y in adjacent:
            cell = self.get_cell(x, y)
            if not cell.checked:
                if (basex, basey) != (self.curx, self.cury):
                    self.move_to_checked(basex, basey)
                self.move_to_unchecked(move, x, y)
                self.check_current_cell()
    
    def print_grid(self):
        rows = []
        for x in range(self.minx, self.maxx + 1):
            row = []
            for y in range(self.miny, self.maxy + 1):
                cell = self.get_cell(x, y)
                if cell.is_exit:
                    row.append("E")
                elif cell.checked:
                    if cell.parent is None:
                        row.append("S")
                    else:
                        row.append(".")
                else:
                    row.append("#")
            rows.append("".join(row))
        logging.info("###\n%s\n###", "\n".join(rows))
        logging.info("== Used %d requests so far, %d items in the queue ==", self.requests_count, len(self.queue.items))

    def find_exit(self):
        self.read_invite()
        self.check_current_cell()
        prev_print = 0
        while self.found_exit is None and self.requests_count < REQUEST_COUNT_LIMIT:
            if prev_print + 50 < self.requests_count:
                self.print_grid()
                prev_print = self.requests_count
            if self.queue.empty():
                logging.error("Error: have no cells to explore in the queue!")
                return False
            self.dequeue()
        if self.found_exit is None:
            logging.error("Exhausted number of requests, but didn't find the exit =(")
        self.print_grid()
        return not (self.found_exit is None)
    
    def trace_path(self):
        assert self.found_exit
        path = []
        x, y = self.found_exit
        while True:
            path.append((x, y))
            if len(path) % 10 == 9:
                logging.info("Collected %d cell in the path already", len(path))
            cell = self.get_cell(x, y)
            if cell.parent is None:
                break
            x, y = cell.parent
        return list(reversed(path))


def main():
    tn = telnetlib.Telnet(host=TN_HOST, port=TN_PORT)
    solution = Solution(tn)
    if solution.find_exit():
        s = str(solution.trace_path())
        print(s.strip("[]"))
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())
