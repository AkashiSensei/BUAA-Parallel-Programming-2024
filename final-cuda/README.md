# CUDA 大作业

基于 Stanford CS149 Assignment 3 的本科并行课程大作业。原作业分为 SAXPY、并行前缀和/重复元素查找和圆形渲染器三个部分。

- [完整课程提交工程](code/)：课程框架加作者实现。
- [课程框架基线](support/starter/)：基于本地原 Git 对象导出的上游快照。
- [中文作业说明](docs/assignment.zh-CN.md)及 [PDF](docs/assignment.zh-CN.pdf)：原翻译稿。
- [云平台配置说明译文](docs/aws-setup.zh-CN.md)：移除账号号段和账户配置截图的历史说明。
- [作者报告](report.pdf)及[报告源码](report-source/)：身份封面已删除，正文保留。

作者功能性修改集中在 `code/saxpy/saxpy.cu`、`code/scan/scan.cu` 和 `code/render/cudaRenderer.cu`。框架中的参考 CPU 渲染器、图像加载、场景生成、检查器、共享内存 scan 辅助函数等来自课程，不计作独立开发。

`support/starter/` 的 TODO/空实现属于原课程框架；`code/` 是完成后的提交。报告中的硬件架构与内存模型图来自原报告引用资料，不是作者自行设计的 GPU 架构。

构建、来源提交、运行条件与验证范围见仓库根目录 README 和 `docs/`。
