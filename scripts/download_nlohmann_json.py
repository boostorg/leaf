"""

    Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    This program downloads the nlohmann/json single header distribution.

    Usage:

        python3 download_nlohmann_json.py

"""

import urllib.request
import os

url = "https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp"
output_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "test", "nlohmann")
output_file = os.path.join(output_dir, "json.hpp")

def _main():
    if os.path.exists(output_file):
        print(f"{output_file} already exists, skipping download")
        return
    os.makedirs(output_dir, exist_ok=True)
    print(f"Downloading {url}...")
    urllib.request.urlretrieve(url, output_file)
    print(f"Saved to {output_file}")

if __name__ == "__main__":
    _main()
