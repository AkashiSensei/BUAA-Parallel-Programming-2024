以下是斯坦福 cs149 《并行计算》课程[第三次作业](https://github.com/stanford-cs149/asst3/tree/master)的云服务器配置说明，由于没有对应的账号，因此无法直接按照该说明配置环境，但仍有一定的参考价值。这份翻译基于以下提交 `fd0f70543f5dbe689b9bf8231ff385dfd35be097` 中的 `cloud_readme.md` 文件。

# AWS 设置说明 #

为了进行性能测试，你需要在 Amazon Web Services (AWS) 上的虚拟机实例上运行此作业。以下是如何在 AWS 上进行设置的步骤。

到现在你应该已经收到了 AWS 登录凭证。如果没有，请在 Ed 上发一个私密帖子，我们会为你创建登录凭证。

注意：__请不要忘记在一天结束时关闭你的实例！__

## 连接到虚拟机 ##

1. 通过输入你的凭证登录 [AWS 控制台](https://cs149-fall23.signin.aws.amazon.com/console)。账户 ID 是 `[course-account-id]`，账户别名是 `cs149-fall23`。你可以输入其中任意一个。

2. 现在你应该看到 AWS 管理控制台。在左上角的搜索栏中搜索 **Lightsail**。
   [云平台截图已从整理版移除]

3. 进入 Lightsail 页面后，点击顶部中间的 **Lightsail for Research**。
   [云平台截图已从整理版移除]

4. 点击左上角的菜单按钮，你应该会在“虚拟计算机”下看到你的实例（实例名称应包括你的 SUNet id）。

*10月25日下午2:42：Lightsail 网页当前存在问题，你会看到许多红色错误框。你可以忽略这些错误（关闭所有红框），继续使用你的实例。*
[云平台截图已从整理版移除]

5. 选择你的实例。你应该可以通过点击右上角的“启动计算机”来启动你的实例。实例启动后，你可以通过点击 **launch Ubuntu** 来启动 GUI。
   [云平台截图已从整理版移除]

__注意：实例在 15 分钟不活动（CPU 使用率 < 2%）后会自动关闭，请确保频繁保存你的工作！__

6. 现在你已经登录到你的实例了！要将本地文件复制到实例，只需将它们拖到 GUI 中即可。要打开终端，点击 GUI 左上角的 **Activities**，然后点击底部中间的终端图标：
   [云平台截图已从整理版移除]

__注意：网页 GUI 只接受一个连接，所以两个人不能同时使用 GUI。如果你希望多人同时使用实例，请使用 SSH，这需要更复杂的设置。__

### 如何设置 SSH 连接到虚拟机 ###

**注意：** 第一步针对 MacOS/Linux 用户，如果你使用 Windows，你可以进行类似的过程来生成你的密钥对（https://www.purdue.edu/science/scienceit/ssh-keys-windows.html），然后继续执行步骤 2。如果你有任何问题，请不要犹豫，在 Ed 上发帖子或去办公室时间！

1. 使用 `ssh-keygen` 生成一对密钥，其中包括一个公钥（名为 `<key-name>.pub`）和一个私钥（名为 `<key-name>`）。对话框将如下所示。选择你的密钥对的保存位置（如下例中的 `./mykey`），密码可以为空（直接按回车）。

# AWS 设置说明 #

为了进行性能测试，你需要在 Amazon Web Services (AWS) 上的虚拟机实例上运行此作业。以下是如何在 AWS 上进行设置的步骤。

到现在你应该已经收到了 AWS 登录凭证。如果没有，请在 Ed 上发一个私密帖子，我们会为你创建登录凭证。

注意：__请不要忘记在一天结束时关闭你的实例！__

## 连接到虚拟机 ##

1. 通过输入你的凭证登录 [AWS 控制台](https://cs149-fall23.signin.aws.amazon.com/console)。账户 ID 是 `[course-account-id]`，账户别名是 `cs149-fall23`。你可以输入其中任意一个。

2. 现在你应该看到 AWS 管理控制台。在左上角的搜索栏中搜索 **Lightsail**。
   [云平台截图已从整理版移除]

3. 进入 Lightsail 页面后，点击顶部中间的 **Lightsail for Research**。
   [云平台截图已从整理版移除]

4. 点击左上角的菜单按钮，你应该会在“虚拟计算机”下看到你的实例（实例名称应包括你的 SUNet id）。

*10月25日下午2:42：Lightsail 网页当前存在问题，你会看到许多红色错误框。你可以忽略这些错误（关闭所有红框），继续使用你的实例。*
[云平台截图已从整理版移除]

5. 选择你的实例。你应该可以通过点击右上角的“启动计算机”来启动你的实例。实例启动后，你可以通过点击 **launch Ubuntu** 来启动 GUI。
   [云平台截图已从整理版移除]

__注意：实例在 15 分钟不活动（CPU 使用率 < 2%）后会自动关闭，请确保频繁保存你的工作！__

6. 现在你已经登录到你的实例了！要将本地文件复制到实例，只需将它们拖到 GUI 中即可。要打开终端，点击 GUI 左上角的 **Activities**，然后点击底部中间的终端图标：
   [云平台截图已从整理版移除]

__注意：网页 GUI 只接受一个连接，所以两个人不能同时使用 GUI。如果你希望多人同时使用实例，请使用 SSH，这需要更复杂的设置。__

### 如何设置 SSH 连接到虚拟机 ###

**注意：** 第一步针对 MacOS/Linux 用户，如果你使用 Windows，你可以进行类似的过程来生成你的密钥对（https://www.purdue.edu/science/scienceit/ssh-keys-windows.html），然后继续执行步骤 2。如果你有任何问题，请不要犹豫，在 Ed 上发帖子或去办公室时间！

1. 使用 `ssh-keygen` 生成一对密钥，其中包括一个公钥（名为 `<key-name>.pub`）和一个私钥（名为 `<key-name>`）。对话框将如下所示。选择你的密钥对的保存位置（如下例中的 `./mykey`），密码可以为空（直接按回车）。
~~~~
$ ssh-keygen         
Generating public/private rsa key pair.
Enter file in which to save the key (/.ssh/id_rsa): ./mykey
Enter passphrase (empty for no passphrase): 
Enter same passphrase again: 
Your identification has been saved in mykey
Your public key has been saved in mykey.pub
~~~~

2. 使用 `cat <path-to-your-public-key>` 命令打印公钥的内容（或者你也可以使用文本编辑器打开文件）。复制内容，我们将把公钥上传到实例中。

3. 打开网页 GUI 并启动一个终端。在文件夹 `~/.ssh/` 下使用你喜欢的编辑器创建一个 ssh 配置文件 `authorized_keys`。我们将使用 `nano` 作为示例。在文件中添加如下行中的公钥。完成编辑后保存文件并退出。
   [云平台截图已从整理版移除]

4. 我们需要更改创建文件的权限，还要更改其父目录的权限。

~~~~
chmod 600 /home/lightsail-user/.ssh/authorized_keys
chmod 700 /home/lightsail-user/.ssh
chmod go-w /home/lightsail-user
~~~~

5. 现在我们已经完成了密钥对的设置。在控制台中找到你实例的 IP 地址。启动你的 Lightsail 实例后，你可以在这里找到它的 IP 地址。如果为空，请尝试刷新页面。**注意：每次重启实例时，实例的 IP 地址都会改变！**
   [云平台截图已从整理版移除]

6. 最后，你可以使用生成的私钥通过以下命令 SSH 进入你的实例！

~~~~
ssh -i <path-to-your-private-key> lightsail-user@<instance-IP-addr>
~~~~

## 设置虚拟机环境 ##

1. 如果你在实例中运行 `nvcc` 并发现它没有安装，你需要在 asst3 仓库中运行 `install.sh` 脚本来重新安装 CUDA 12。同样，如果 `nvidia-smi` 显示 CUDA 版本是 11.4，你也需要运行 `install.sh`。你可能需要先删除现有的 NVIDIA 驱动程序：`sudo apt remove --purge '^nvidia-.*'`。使用以下命令将作业仓库克隆到你的实例中。

~~~~
git clone https://github.com/stanford-cs149/asst3.git
~~~~

2. 添加执行权限，并运行安装脚本。如果遇到任何问题，请在 Ed 上发帖子！

~~~~
chmod +x ./asst3/install.sh
./asst3/install.sh
~~~~

3. 运行以下命令更新你的路径。

~~~~
source ~/.bashrc
~~~~

4. 运行脚本后，CUDA 应该已经安装。你可以使用 `nvidia-smi` 再次检查 CUDA 版本，应该是 **12.3**。我们使用的 GPU 是 **Tesla T4**。

（如果命令出错，尝试重新启动终端/重新启动实例，如果错误仍然存在，请在 Ed 上发帖子，CAs 会帮助你！）

~~~~
lightsail-user@ip-172-26-12-153:~$ nvidia-smi
Mon Oct 23 16:08:43 2023       
+---------------------------------------------------------------------------------------+
| NVIDIA-SMI 545.23.06              Driver Version: 545.23.06    CUDA Version: 12.3     |
|-----------------------------------------+----------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |         Memory-Usage | GPU-Util  Compute M. |
|                                         |                      |               MIG M. |
|=========================================+======================+======================|
|   0  Tesla T4                       On  | 00000000:00:1E.0 Off |                    0 |
| N/A   43C    P0              26W /  70W |    255MiB / 15360MiB |      0%      Default |
|                                         |                      |                  N/A |
+-----------------------------------------+----------------------+----------------------+
                                                                                         
+---------------------------------------------------------------------------------------+
| Processes:                                                                            |
|  GPU   GI   CI        PID   Type   Process name                            GPU Memory |
|        ID   ID                                                             Usage      |
|=======================================================================================|
|    0   N/A  N/A      2527      C   /usr/lib/x86_64-linux-gnu/dcv/dcvagent      249MiB |
+---------------------------------------------------------------------------------------+
~~~~

## 从 AWS 获取你的代码 ##

完成作业后，你可以使用文件存储控制台下载你的代码，点击左上角的双箭头按钮：
[云平台截图已从整理版移除]
[云平台截图已从整理版移除]

如果你使用 SSH，你可以使用 `scp` 命令在本地机器上获取你的代码，如下所示：

~~~~
scp -i <path-to-your-private-key> lightsail-user@<instance-IP-addr>:/path/to/file /path/to/local_file
~~~~

## 关闭虚拟机 ##

使用虚拟机完毕后，你可以通过网页上的“停止计算机”按钮关闭它，或者在终端中使用以下命令：

~~~~
sudo shutdown -h now
~~~~
