set -x
for i in ~/test_benchs/polybench-c-3.2/build/*
do
  echo $i
  for j in {1..1}
  do
    echo $j
    for p in {5..6}
    do
      mem=$(( 1 << $p ));
      echo $mem;
      #python simulate.py --policy LRU --mem $mem --target "/home/jeesoo/test_benchs/hpcg_test/xhpcg /home/jeesoo/test_benchs/hpcg_test/hpcg.dat"
      #python simulate.py --policy LRU --mem $mem --target ~/test_benchs/polybench-c-3.2/build/fdtd-2d_time
      python simulate.py --policy LRU --mem $mem --target $i
    done
  done
done

