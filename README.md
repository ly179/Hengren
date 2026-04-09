# hengren_cdsem_limbs

##### 项目初始化

1. 配置Conan

- 下载Conan

https://conan.io/downloads

![img.png](readme_img/img.png)

- 解压，路径最好不要有空格和中文
- 配置一个CONAN_HOME变量，指向Conan的工作目录，用于缓存各种包文件，推荐放在conan.exe所在目录的.conan2目录下
- 将conan.exe所在目录配置到系统环境变量Path中，方便在终端中运行conan命令，如果你遵循上一步操作，则可直接配置为： %CONAN_HOME%\..\
- 初始化本地仓库
```shell
#初始化conan默认配置
conan profile detect
而后到%CONAN_HOME%/profiles/default文件中将
compiler.cppstd=14 # 由14改为17

# 添加缓存映射
conan remote add Conan_Group http://39.98.45.96:8802/repository/Conan_Group/
conan remote add cdsem_package http://39.98.45.96:8802/repository/cdsem_package/
# 登录并保存账户
conan remote login cdsem_package admin -p
conan remote login Conan_Group admin -p
```

2. 配置项目

*注意，项目统一使用MSVC2019环境，Visual Studio使用2022 19.44.35209，禁止使用MinGW*
![img.png](readme_img/img4.png)

- CLion中的配置
  ![img_1.png](readme_img/img_1.png)

- QC(QtCreator)中的配置

如果QtCreator里执行CMake找不到conan时，在项目根目录创建一个conan_path.cmake文件（此文件不提交到git），里面输入：
`set(ENV{CONAN_HOME} "你的ConanHome全路径")`

3. 初始化Git

```shell
# 在项目根目录下执行以下命令
git submodule sync --recursive
# 拉取子仓库的所有代码
# 这里推荐加上--remote参数，用于保持子仓库代码最新版本而不是追随主仓库的控制
git submodule update --init --recursive --remote
# 更新完毕后的子仓库分支处于游离状态，使用该命令让其关联到分支
# 必须将branch替换为你在第一步（命令中一共有3个branch，记得全替换）.gitmodules文件中看到的branch。
# 如果branch不同，则不能使用foreach，必须挨个手动关联。
git submodule foreach --recursive "git checkout branch || git checkout -b branch origin/branch"
```

4. 执行/Reload CMake