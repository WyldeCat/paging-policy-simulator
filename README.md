# Paging Policy Simulator
## Authors
* **kang-sooyeon** : 강수연
* **WyldeCat** : 이지수
* **han-minhee** : 한민희

## Requirements and Installation
* [Pin tool](https://software.intel.com/content/www/us/en/develop/articles/pin-a-dynamic-binary-instrumentation-tool.html) version >= 3.16
* Python version >= 3.7.4
* To install and build simulator
``` bash
$ git clone https://github.com/WyldeCat/paging-policy-simulator
$ cd paging-policy-simulator
$ make -j
```

## How to run
``` bash
$ cd ./scripts
$ python simulator.py -h
usage: simulator.py [-h] [--mem MEM] [--policy {FIFO,LRU,LFU}] --target TARGET

optional arguments:
  -h, --help            show this help message and exit
  --mem MEM             memory size(MB)
  --policy {FIFO,LRU,LFU}
                        paging policy
  --target TARGET       target program
```
