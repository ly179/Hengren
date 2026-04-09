from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout
from conan.tools.microsoft import is_msvc
import os

class Hengren_CD_SEM(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
        self.options["boost"].header_only = True

    def requirements(self):
        self.requires("boost/1.84.0")
        self.requires("opencv/4.10.0")
        self.requires("mvsdk/1.2.0")
        self.requires("postgre_binaries/17.4.2")
        self.requires("mysql_binaries/8.0.37")
        self.requires("jkqtplotter/4.0.3")
        self.requires("sentinel_ldk/1.0.0")

        if self.settings.build_type == "Debug":
            self.requires("hengren_cdsem_base/[~0.0.1-debug, include_prerelease]")
            self.requires("hengren_cdsem_brain/[~0.0.1-debug, include_prerelease]")
            self.requires("qxlsx/[~1.0.0-debug, include_prerelease]")
        else:
            self.requires("hengren_cdsem_base/0.0.1")
            self.requires("hengren_cdsem_brain/0.0.1")
            self.requires("qxlsx/1.0.0")

    def package_info(self):
        # 修复 debug 模式的库依赖
        if self.settings.build_type == "Debug":
            # 如果 ui_cored 是来自 hengren_cdsem_base 的库
            # 强制设置库名称
            hengren_base = self.dependencies["hengren_cdsem_base"].cpp_info
            if "ui_cored" not in hengren_base.libs:
                hengren_base.libs.append("ui_cored")

            # 或者添加为系统库
            # hengren_base.system_libs.append("ui_cored")

    def generate(self):
        dst_dir = os.path.join(self.build_folder, "../../project_all")
        for dep in self.dependencies.values():
            for dep_bindir in dep.cpp_info.bindirs:
                copy(self, "*.dll", src=dep_bindir, dst=dst_dir, keep_path=False)

    def layout(self):
        cmake_layout(self)