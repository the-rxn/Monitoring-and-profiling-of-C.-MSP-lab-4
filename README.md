# Lab №4 MSP. Monitoring and profiling of C/Java code.

## The task:

1. Для своей программы из лабораторной работы #3 по дисциплине "Веб-программирование" реализовать:
MBean, считающий общее число установленных пользователем точек, а также число точек, не попадающих в область. В случае, если количество установленных пользователем точек стало кратно 15, разработанный MBean должен отправлять оповещение об этом событии.
MBean, определяющий площадь получившейся фигуры.

2. С помощью утилиты JConsole провести мониторинг программы:
Снять показания MBean-классов, разработанных в ходе выполнения задания 1.
Определить наименование и версию JVM, поставщика виртуальной машины Java и номер её сборки.
3. С помощью утилиты VisualVM провести мониторинг и профилирование программы:
Снять график изменения показаний MBean-классов, разработанных в ходе выполнения задания 1, с течением времени.
Определить имя потока, потребляющего наибольший процент времени CPU.

4. С помощью утилиты VisualVM и профилировщика IDE NetBeans, Eclipse или Idea локализовать и устранить проблемы с производительностью в программе. По результатам локализации и устранения проблемы необходимо составить отчёт, в котором должна содержаться следующая информация:
Описание выявленной проблемы.
Описание путей устранения выявленной проблемы.
Подробное (со скриншотами) описание алгоритма действий, который позволил выявить и локализовать проблему.
Студент должен обеспечить возможность воспроизведения процесса поиска и локализации проблемы по требованию преподавателя.

**P.S. We decided to use C instead of Java and therefore used a different toolset. Yet, the task is fully covered.**

## Used tools: 

1. Valgrind toolkit
2. HeapTrack: Massif Visualizer - very nice for memory profiling (draws dynamic beautiful graphs of memusage)
3. Hotspot - GUI for perf: https://github.com/KDAB/hotspot 
4. strace for mmap
5. strace-parser by gitLab - interpreting strace log https://gitlab.com/gitlab-com/support/toolbox/strace-parser


## Results:

### Memory leak
1. After using different tools for monitoring and profiling, we saw that the shared library (./src/floatbin.c) that looks for binary float numbers in files was allocating and not freeing too much memory.
2. After checking the flamegraphs from heaptrack and valgrind info we still couldn't find the leak.

>
    (inside ./src)

    $ heaptrack ./lab1aotN3246 --float-bin 1.5 ~/.config/

    $ heaptrack_print --print-leaks \
    --print-histogram histogram.data \
    --print-massif massif.data \
    --print-flamegraph flamegraph.data \
    --file .heaptrack.lab1aotN3246.171725.zst > report.txt

    $ ./FlameGraph/flamegraph.pl flamegraph.data > flamegraph.svg

![Flamegraph](./src/logs/flamegraph.svg?raw=true "Flamegraph")

4. After using strace [$ strace -f -T -tt -o straceLog.txt ./lab1aotN3246 --float-bin 1.5 ~/.config/] and piping the logfile into GitLab's strace-parser we found out that all the data that was 'mmap()'ed was never 'munmap()'ed.
>
    $ ./strace-parser src/straceLog.txt list-pids

    Details of Top 1 PIDs by Active Time
    -----------

    PID 216893

      197208 syscalls, active time: 836.103ms, user time: 90145.477ms, total time: 90981.578ms
      start time: 22:04:26.159115    end time: 22:05:57.140694

      syscall                 count    total (ms)      max (ms)      avg (ms)      min (ms)    errors
      -----------------    --------    ----------    ----------    ----------    ----------    --------
      newfstatat              76014       302.231         0.142         0.004         0.002
      openat                  38189       189.771         0.498         0.005         0.003    ENOENT: 10   ENXIO: 2
      mmap                    35264       161.050         0.741         0.005         0.003
      close                   38177       138.468         0.094         0.004         0.002
      getdents64               4497        26.422         0.125         0.006         0.003
      fcntl                    4396        13.223         0.073         0.003         0.002
      munmap                     91         1.939         0.050         0.021         0.005
      read                      384         1.402         0.017         0.004         0.002
      write                      84         0.865         0.141         0.010         0.003
      brk                        48         0.323         0.024         0.007         0.003
      execve                      1         0.163         0.163         0.163         0.163
      pread64                    30         0.095         0.004         0.003         0.002
      mprotect                    9         0.062         0.013         0.007         0.004
      getcwd                     16         0.059         0.006         0.004         0.003
      arch_prctl                  2         0.006         0.003         0.003         0.003    EINVAL: 1
      access                      1         0.006         0.006         0.006         0.006    ENOENT: 1
      getrandom                   1         0.004         0.004         0.004         0.004
      set_robust_list             1         0.004         0.004         0.004         0.004
      prlimit64                   1         0.004         0.004         0.004         0.004
      rseq                        1         0.003         0.003         0.003         0.003
      set_tid_address             1         0.003         0.003         0.003         0.003
      exit_group                  1           n/a           n/a           n/a           n/a
      ---------------

      Program Executed: ./lab1aotN3246 --float-bin 1.5 /home/mertz/.config/
      Time: 22:04:26.159115
      Exit: 0

4. After expolring the library source code we found the bug. Afterwards, after we added 'munmap()' in the end of the function 'plugin_process_file()' (./src/floatbin.c:248) the memory usage problem got fixed.
>
    $ ./strace-parser src/straceLogMUNMAP.txt list-pids

    Details of Top 1 PIDs by Active Time
    -----------

    PID 217796

      232333 syscalls, active time: 1174.287ms, user time: 92255.250ms, total time: 93429.539ms
      start time: 22:08:47.487286    end time: 22:10:20.916818

      syscall                 count    total (ms)      max (ms)      avg (ms)      min (ms)    errors
      -----------------    --------    ----------    ----------    ----------    ----------    --------
      newfstatat              76017       356.356         1.984         0.005         0.002
      munmap                  35207       218.458         1.289         0.006         0.002
      openat                  38191       216.741         0.881         0.006         0.003    ENOENT: 10   ENXIO: 2
      close                   38179       167.443         2.222         0.004         0.002
      mmap                    35265       163.393         0.685         0.005         0.003
      getdents64               4497        31.178         0.122         0.007         0.003
      fcntl                    4396        17.607         0.029         0.004         0.002
      read                      385         1.494         0.028         0.004         0.002
      write                      84         0.915         0.025         0.011         0.005
      brk                        48         0.265         0.009         0.006         0.003
      execve                      1         0.138         0.138         0.138         0.138
      pread64                    30         0.120         0.005         0.004         0.003
      getcwd                     16         0.081         0.012         0.005         0.003
      mprotect                    9         0.057         0.008         0.006         0.005
      arch_prctl                  2         0.008         0.005         0.004         0.003    EINVAL: 1
      prlimit64                   1         0.007         0.007         0.007         0.007
      access                      1         0.006         0.006         0.006         0.006    ENOENT: 1
      rseq                        1         0.005         0.005         0.005         0.005
      getrandom                   1         0.005         0.005         0.005         0.005
      set_tid_address             1         0.005         0.005         0.005         0.005
      set_robust_list             1         0.005         0.005         0.005         0.005
      exit_group                  1           n/a           n/a           n/a           n/a
      ---------------

      Program Executed: ./lab1aotN3246 --float-bin 1.5 /home/mertz/.config/
      Time: 22:08:47.487286
      Exit: 0

### Performance 
1. Yet againg, after examining the flamegraph we figured out that the performance is heavily dependant on plugin_process_file() and KMPalg().
Diving deeper into the source code we found out that instead of taking the input as it is in binary, the author of the library translated everything into a char array with '1's and '0's.
So, replacing it with a little more sane code boosted the performance 97 times. ~(1.37 min -> 1s)
2. Refactoring the code made the library work for roughly O(n) instead of O(n^4)


### Logging(internal):
1. We have also implemented some logging functions that log the starting command, timestamp, all the processed files and some summary of each run. 
2. The logging is done each run of the program.
3. Logging is done both in the program and shared library.
4. The example of internal logging:
>
    CMD: $ ./lab1aotN3246 --ipv4-addr-bin 127.0.0.1 /home/mertz/dev/uni/mispi/lab4/src/ 
    Started at:14-06-2022 03:43:30.000163
    #------------
    1. /home/mertz/dev/uni/mispi/lab4/src/test-float-bin CONTAINS
    2. /home/mertz/dev/uni/mispi/lab4/src/test DOESN'T CONTAIN
    3. /home/mertz/dev/uni/mispi/lab4/src/1.txt DOESN'T CONTAIN
        ....
    217. /home/mertz/dev/uni/mispi/lab4/src/libaotN3246.so DOESN'T CONTAIN
    218. /home/mertz/dev/uni/mispi/lab4/src/lab1aotN3246 DOESN'T CONTAIN
    219. /home/mertz/dev/uni/mispi/lab4/src/fileFinder14_6_2022_3.43.log DOESN'T CONTAIN

    #------------
    Found options from 4 plugins:

    entropy:Target value of entropy
    offset-from:Start offset
    offset-to:End offset
    float-bin:looking for float numbers in file
    bug-float-bin:looking for float numbers in file
    ipv4-addr-bin:IPv4 address
    #------------
    Finished at:14-06-2022 03:43:30.000218
    Total files checked: 246
    Total 'FOUND' files: 1

P.S. Total files checked also include directories.

## Used sources:

1. Check this out: https://habr.com/ru/company/oleg-bunin/blog/340394/
2. Then check this out: https://habr.com/ru/post/482040/

