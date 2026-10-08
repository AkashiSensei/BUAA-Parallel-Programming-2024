# BUAA Parallel Programming 2024

北航本科《并行程序设计》课程作业归档（2024 年春）。将 MPI、Pthreads、OpenMP 编程作业与 CUDA 大作业整理在一个仓库中，保留原始实现、作业要求、课程辅助代码、实验数据与报告。

本仓库初始为 **Private**，供作者检查。整理过程中没有重写算法，也没有将新写的构建说明、整理脚本或验证工作计入原课程成果。

## 项目索引

| 内容 | 编程模型 | 实现与配套材料 |
| --- | --- | --- |
| [蒙特卡洛估算 π](assignments/01-mpi-pi/) | MPI | 自写实现；课程作业要求 |
| [MD5 密码搜索](assignments/02-mpi-md5/) | MPI | 自写并行枚举程序；课程提供的 MD5 库和示例驱动；作业要求 |
| [数值积分](assignments/03-pthreads-integration/) | Pthreads | 忙等待、互斥量、信号量三种归约方式；作业要求 |
| [任务队列](assignments/05-pthreads-task-queue/) | Pthreads | 条件变量唤醒工作线程，任务队列与链表操作；作业要求 |
| [CYK 算法并行化](assignments/06-openmp-cyk/) | OpenMP | 在课程串行参考程序上完成并行化；输入样例、原始计时脚本、报告和实验数据 |
| [CUDA 大作业](final-cuda/) | CUDA | 基于 Stanford CS149 Assignment 3 框架完成 SAXPY、并行前缀和/重复元素查找、圆形渲染；原始框架、实现、中文说明和报告 |

目录编号跟随本地作业说明。没有找到单独的第 4 次作业提交；`作业/sort` 中两份奇偶排序程序与课程提供的 MPI 示例完全相同，已归入 [课程辅助代码](course-support/mpi/)，不计作自写课设。

## 实现、框架与课程材料的边界

- `assignments/*/src/`：原课程提交实现；CYK 的基础数据结构与串行逻辑来自课程参考程序。
- `assignments/*/support/`：课程提供的库、串行实现、驱动或测试输入。
- `final-cuda/code/`：完整 CUDA 提交工程；**其中课程框架占有相当部分**，不能将整个目录视为从零编写。
- `final-cuda/starter/`：原始 Stanford CS149 框架，用于对照实现改动。
- `course-support/`：课程或教材示例，包括 MPI、Pthreads、OpenMP、TSP/N-body 和 CUDA eigenfaces；不属于作者独立开发成果。
- `reports/`、`notes/`：原始报告、实验数据、算法图示和理论作业笔记。报告中的结论与性能数据属于当时实验记录。

CUDA 框架基线来自 [stanford-cs149/asst3](https://github.com/stanford-cs149/asst3/tree/fd0f70543f5dbe689b9bf8231ff385dfd35be097)，提交 `fd0f70543f5dbe689b9bf8231ff385dfd35be097`。本地最后提交为 `67c033a95fc7887e240d4dab7d3c715a7fdc6b41`（2024-06-09）。与基线相比，仅以下三个实现文件发生功能性修改：

| 文件 | 原课程实现内容 |
| --- | --- |
| `saxpy/saxpy.cu` | 数据传输、CUDA kernel 执行与计时 |
| `scan/scan.cu` | 并行前缀和及重复元素查找 |
| `render/cudaRenderer.cu` | 圆形渲染并行化；分块筛选、共享内存与前缀和组织相关圆形 |

来源与整理细节见 [材料来源](docs/PROVENANCE.md) 和 [脱敏说明](docs/SANITIZATION.md)。未添加覆盖整个仓库的统一开源许可证；第三方材料保留原有声明及来源。

## 报告

- [OpenMP CYK 报告](reports/cyk/report.pdf)：原 15 页，移除身份封面后 14 页。
- [CUDA 大作业报告](reports/cuda/report.pdf)：原 28 页，移除身份封面后 27 页。
- [CYK 原始实验数据](reports/cyk/experiment-data.xlsx)。
- [CUDA 算法图示](reports/cuda/algorithm-diagrams.pptx)。

两份报告同时保留 `source/` 下的 LaTeX 源码和正文引用的原始配图。PDF 沿用原正文，不重新排版；LaTeX 源码删除身份封面，字体依赖改为 TeX Live 常见字体，未附带原来的商业字体文件。归档源码未重新编译，编译所得版式可能与原 PDF 不同。

## 构建与运行

CPU 作业建议使用 Linux、GCC/G++、Make、MPI（例如 Open MPI）和 OpenMP。根目录 Makefile 是归档时新加的构建入口。

```sh
make pthreads       # 积分与任务队列
make mpi            # π 与 MD5
make openmp         # CYK
make cpu            # 上述所有 CPU 作业
```

```sh
# π：进程 0 负责收集，其余进程采样，至少使用两个进程。
printf '1000000\n' | mpiexec -n 8 ./build/pi_mpi

# 积分：线程数和同步方式（1 忙等待，2 互斥量，3 信号量）。
printf '0 100 400\n' | ./build/trapezoid_pthreads 4 2

# 任务队列：线程数、任务数。
./build/taskqueue 4 8

# CYK：原实现从当前目录读取 input2.txt。
(cd assignments/06-openmp-cyk && ../../build/parallel_CYK 4)

# MD5：输入 32 位十六进制摘要，枚举六位小写字母/数字组合。
mpiexec -n 4 ./build/md5_crack
```

原实现保留课程提交时的行为与局限。积分程序的累计量类型、步长定义及忙等待同步没有在此次整理中修正；macOS 不支持它使用的未命名 POSIX 信号量。MD5 枚举不会在找到结果后全局提前退出，完整搜索空间较大。请不要把上述命令误认为全部正确性或性能检查已经通过。

CUDA 需要 Linux x86-64、NVIDIA GPU、CUDA Toolkit、C++ 编译器；渲染器还使用 OpenGL/GLUT。保留了课程提供的参考可执行文件和检查器，参考二进制仅适用于其原构建平台。

```sh
make -C final-cuda/code/saxpy
make -C final-cuda/code/scan
make -C final-cuda/code/render

(cd final-cuda/code/saxpy && ./cudaSaxpy)
(cd final-cuda/code/scan && perl checker.pl scan)
(cd final-cuda/code/scan && perl checker.pl find_repeats)
(cd final-cuda/code/render && python3 checker.py)
```

原 CUDA Makefile 保留历史工具链路径与选项，现代环境可能需要调整 `NVCC`、CUDA 库路径和架构选项。旧的云服务器配置文档仅供历史参考。

当前整理验证的范围及环境限制记录于 [VALIDATION.md](docs/VALIDATION.md)。
