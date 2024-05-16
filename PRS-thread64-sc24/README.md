### How to build docker image

```bash
cd docker
./build_PRS_images.sh
```

### How to build

```bash
make clean; make
```

### How to use

```bash
./PRS <instance> [config=config_filename] [--option=param]
```

For example, 

```bash
./PRS ./test.cnf --clause_sharing=1 --DCE=1 --preprocessor=1 --nThreads=32 --cutoff=5000
```

### Parameters and Options

instance: input CNF 

nThreads: the number of workers in PRS

cutoff: the wall time for SAT solving

clause_sharing: whether use clause sharing (1: enable; 0: disable) 

preprocessor: whether use preprocessing (1: enable; 0: disable)
