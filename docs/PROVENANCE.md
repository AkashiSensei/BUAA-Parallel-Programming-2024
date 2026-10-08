# 材料来源与归档规则

这是 2024 年本科并行程序设计课程的一套归档，与作者 2025 年《并行处理与体系结构》、2026 年研究生《并行程序设计 A》仓库分别维护。

## 编程作业

五份 `src/` 来自本地提交文件。重复出现的带姓名/学号文件和 ZIP 提交只保留一份，以中性文件名归档。MD5 的 `md5.c`、`md5.h` 和 `md5driver.c` 为作业要求明确提供的辅助库和驱动，保留原版权声明。CYK 的 `support/serial_CYK.cpp` 及三个输入文件来自课程附件；并行版本基于该程序完成，不主张串行基础代码的独立原创。

五份 `requirements.pdf` 为课程原作业要求，正文保留；清除 PDF 身份元数据。原说明中的历史截止时间不做订正，包括 π 作业说明里出现的 2023 日期，不据此改变本项目的 2024 归档年份。

没有找到单独第 4 次作业要求或自写排序提交。原 `sort/mpi_odd_even.c`、`sort/odd_even.c` 与 `chapter3.zip` 中同名课程例程逐字一致（统一换行后），所以只收录于 `assignments/reference-mpi-examples/support/chapter3/`。

## CUDA 大作业

原工程基于 Stanford CS149 “Assignment 3: A Simple CUDA Renderer”，来源：

- 上游：<https://github.com/stanford-cs149/asst3>
- 基线：`fd0f70543f5dbe689b9bf8231ff385dfd35be097`。
- 原始学生实现最后提交：`67c033a95fc7887e240d4dab7d3c715a7fdc6b41`。
- 学生实现提交日期范围：2024-06-06 至 2024-06-09。
- 功能性改动：`saxpy/saxpy.cu`、`scan/scan.cu`、`render/cudaRenderer.cu`。

`support/starter/` 从本地 Git 对象中的确切基线导出；`code/` 从完整本地提交目录归档。两者都保留框架头文件、场景数据、Makefile、检查器及原参考二进制。未携带旧 `.git`、历史作者邮箱、旧远程配置及重复版本。文本统一为 LF；姓名注释改为 GitHub 用户名。

`docs/assignment.zh-CN.md` 和 `docs/aws-setup.zh-CN.md` 是原提交中的中文翻译，保留原译文的来源说明；原英文文档与翻译文档中的课程 AWS 账号号段已移除。移除云平台账户、密钥和网络配置截图，保留算法/渲染示意图，避免截图里的身份信息随仓库再次传播。

`docs/assignment.zh-CN.pdf` 是原作业说明翻译，清除元数据后保留正文。AWS 配置 PDF 是 Markdown 的重复导出且包含云账号和账户截图，所以使用脱敏后的 Markdown 代替，不收入原 PDF。

原框架的历史安装脚本也保留为提供材料；整理验证不会自动执行这些脚本。

## 课程辅助代码

MPI、Pthreads、OpenMP 例程来自课程目录及教材配套源码附件。MPI TSP/N-body、CUDA eigenfaces 同样来自课程提供资料。它们按参考任务主题归档至 `assignments/reference-*/support/`，并在每个参考目录注明没有学生独立提交；未重新包装为学生独立实现。`reference-mpi-examples/support/hello-local/` 来自本地环境测试小包，无法证明独立原创，按来源未明确的示例处理。

保留原注释中的第三方作者和版权信息。提供代码未统一加上学生署名。

## 报告、数据与图示

两份原报告分别放在 `assignments/06-openmp-cyk/report.pdf` 和 `final-cuda/report.pdf`，各自的源码与配图放在同任务的 `report-source/`，实验数据及算法图示也放在同任务目录。两份原报告移除身份封面后保留全部后续页面。报告源码保留正文与实际引用配图；删除封面、未使用的模板示例和商业字体文件，改用 TeX Live 字体。模板及国标参考文献样式保留原有说明。原 PDF 没有用修改后的源码重编译。

报告图来自原报告源码包，不来自个人网站。架构图等属于原报告引用材料，并不都由作者绘制。CYK 工作簿和 CUDA 算法图示为本地实验辅助资料，移除作者元数据。

## 未纳入的文件

教材整本 PDF、整套课堂讲义、GPU 白皮书/参考读物、商业字体、压缩包重复件、旧 Git 历史、个人命名的提交压缩包、缓存、`.DS_Store`、目标文件与本地生成的渲染输出未纳入仓库。可用于复习的课程辅助源码、作业说明和原报告已保留。

`material-manifest.json` 记录材料分类、来源与归档文件 SHA-256；其本身不包含个人原始绝对路径或身份文件名。
