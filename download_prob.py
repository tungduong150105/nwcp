#!/usr/bin/env python3
"""
Download and setup problems from Competitive Companion

Usage:
  download_prob.py --echo
  download_prob.py [<name>... | -n <number> | -b <batches> | --timeout <timeout>] [--dryrun]

Options:
  -h --help                 Show this screen.
  --echo                    Just echo received responses and exit.
  --dryrun                  Don't actually create any problems
  -n COUNT --number COUNT   Number of problems.
  -b COUNT --batches COUNT  Number of batches. (Default 1)
  -t SEC   --timeout SEC    Timeout (seconds) for listening after first item.
"""

from docopt import docopt
import http.server
import json
import sys
import subprocess
from pathlib import Path
import re

# ------------- HTTP listener -------------

def listen_once(*, timeout=None):
    """Return one unmarshalled JSON object or None (on timeout)."""

    json_data = None

    class CompetitiveCompanionHandler(http.server.BaseHTTPRequestHandler):
        def do_POST(self):
            nonlocal json_data
            try:
                length = int(self.headers.get('Content-Length', '0'))
                body = self.rfile.read(length)
                json_data = json.loads(body.decode('utf-8'))
                # Ack so CC extension doesn't retry
                self.send_response(200)
                self.send_header("Content-Type", "text/plain; charset=utf-8")
                self.end_headers()
                self.wfile.write(b"ok\n")
            except Exception as e:
                self.send_response(400)
                self.end_headers()
                self.wfile.write(f"error: {e}\n".encode('utf-8'))

        # Quiet the default logging
        def log_message(self, *_):
            pass

    with http.server.HTTPServer(('127.0.0.1', 10046), CompetitiveCompanionHandler) as server:
        server.timeout = timeout
        server.handle_request()

    if json_data is not None:
        print(f"Got data {json.dumps(json_data)}")
    else:
        print("Got no data")
    return json_data


def listen_many(*, num_items=None, num_batches=None, timeout=None):
    if num_items is not None:
        return [listen_once(timeout=None) for _ in range(num_items)]

    if num_batches is not None:
        res, batches = [], {}
        while len(batches) < num_batches or any(need for need, tot in batches.values()):
            print(f"Waiting for {num_batches} batches:", batches)
            cur = listen_once(timeout=None)
            res.append(cur)
            cur_batch = cur.get('batch', {})
            bid, bsize = cur_batch.get('id'), cur_batch.get('size', 1)
            if bid not in batches:
                batches[bid] = [bsize, bsize]  # [remaining, total]
            assert batches[bid][0] > 0
            batches[bid][0] -= 1
        return res

    # Timed “keep listening until idle”
    res = []
    first = listen_once(timeout=None)
    if first is not None:
        res.append(first)
    while True:
        nxt = listen_once(timeout=timeout)
        if nxt is None:
            break
        res.append(nxt)
    return res

# ------------- Naming & saving -------------

NAME_PATTERN = re.compile(r'^(?:Problem )?([A-Z][0-9]*)\b')

def get_prob_name(data):
    if 'USACO' in data.get('group', ''):
        inp = data.get('input', {})
        out = data.get('output', {})
        if 'fileName' in inp and 'fileName' in out:
            a = inp['fileName'].removesuffix('.in')
            b = out['fileName'].removesuffix('.out')
            if a == b:
                return a

    url = data.get('url', '')
    if url.startswith('https://www.codechef.com'):
        return url.rstrip('/').rsplit('/', 1)[-1]

    m = NAME_PATTERN.search(data.get('name', ''))
    if m:
        return m.group(1)

    print(f"For data: {json.dumps(data, indent=2)}")
    return input("What name to give? ").strip() or "A"

def ensure_dir(p: Path):
    p.mkdir(parents=True, exist_ok=True)

def save_samples(data, prob_dir: Path):
    ensure_dir(prob_dir)
    (prob_dir / 'problem.json').write_text(json.dumps(data, indent=2), encoding='utf-8')
    for i, t in enumerate(data.get('tests', []), start=1):
        (prob_dir / f'sample{i}.in').write_text(t.get('input', ''), encoding='utf-8')
        (prob_dir / f'sample{i}.out').write_text(t.get('output', ''), encoding='utf-8')

# ------------- Maker -------------

def make_prob(data, name=None, *, auto_setup=False):
    if name is None:
        name = get_prob_name(data)

    prob_dir = Path('.') / name

    if name == '.':
        print("Using current directory…")
    elif prob_dir.is_dir():
        print(f"Already created problem {name}…")
    else:
        print(f"Creating problem {name}…")
        make_prob_sh = (Path(sys.path[0]) / 'make_prob.sh').resolve()
        if not make_prob_sh.exists():
            print(f"ERROR: {make_prob_sh} not found")
            return
        if not make_prob_sh.is_file():
            print(f"ERROR: {make_prob_sh} is not a file")
            return
        try:
            subprocess.check_call([str(make_prob_sh), name])
        except subprocess.CalledProcessError as e:
            print(f"make_prob.sh failed with {e.returncode}")
            return

    print("Saving samples…")
    save_samples(data, prob_dir if name != '.' else Path('.'))

    if auto_setup:
        setup = (prob_dir / 'setup') if name != '.' else Path('./setup')
        if setup.exists() and setup.is_file():
            try:
                print("Running setup…")
                subprocess.check_call([str(setup)], cwd=str(prob_dir if name != '.' else Path('.')))
            except subprocess.CalledProcessError as e:
                print(f"setup failed with {e.returncode}")

    print()

# ------------- CLI -------------

def main():
    args = docopt(__doc__)
    if args['--echo']:
        while True:
            print(listen_once())
        return

    dryrun = args['--dryrun']

    def run_make_prob(*a, **k):
        if dryrun:
            print(f"DRYRUN make_prob(args={a}, kwargs={k})")
        else:
            make_prob(*a, **k)

    if names := args['<name>']:
        datas = listen_many(num_items=len(names))
        for data, name in zip(datas, names):
            run_make_prob(data, name)
    elif cnt := args['--number']:
        datas = listen_many(num_items=int(cnt))
        for data in datas:
            run_make_prob(data)
    elif batches := args['--batches']:
        datas = listen_many(num_batches=int(batches))
        for data in datas:
            run_make_prob(data)
    elif timeout := args['--timeout']:
        datas = listen_many(timeout=float(timeout))
        for data in datas:
            run_make_prob(data)
    else:
        datas = listen_many(num_batches=1)
        for data in datas:
            run_make_prob(data)

if __name__ == '__main__':
    main()
