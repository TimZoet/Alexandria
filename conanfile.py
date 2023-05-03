from conan import ConanFile
from conan.tools.files import copy

class AlexandriaConan(ConanFile):
    ############################################################################
    ## Package info.                                                          ##
    ############################################################################
    
    name = "alexandria"
    
    description = "..."
    
    url = "https://github.com/TimZoet/Alexandria"

    @property
    def user(self):
        return getattr(self, "_user", "timzoet")
    
    @user.setter
    def user(self, value):
        self._user = value
    
    @property
    def channel(self):
        return getattr(self, "_channel", f"v{self.version}")
    
    @channel.setter
    def channel(self, value):
        self._channel = value

    ############################################################################
    ## Settings.                                                              ##
    ############################################################################

    python_requires = "pyreq/1.0.1@timzoet/v1.0.1"
    
    python_requires_extend = "pyreq.BaseConan"

    options = {
    }
    
    default_options = {
    }
    
    ############################################################################
    ## Base methods.                                                          ##
    ############################################################################
    
    def set_version(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.set_version(self, "alexandriaVersionString.cmake", "ALEXANDRIA_VERSION")
    
    def init(self):
        base = self.python_requires["pyreq"].module.BaseConan
        self.settings = base.settings
        self.options.update(base.options, base.default_options)
    
    ############################################################################
    ## Building.                                                              ##
    ############################################################################
    
    def export_sources(self):
        copy(self, "alexandriaVersionString.cmake", self.recipe_folder, self.export_sources_folder)
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "license", self.recipe_folder, self.export_sources_folder)
        copy(self, "readme.md", self.recipe_folder, self.export_sources_folder)
        copy(self, "applications/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "buildtools/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "modules/*", self.recipe_folder, self.export_sources_folder)
    
    def config_options(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.config_options(self)
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def configure(self):
        pass
    
    def requirements(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.requirements(self)
        
        self.requires("common/1.1.0@timzoet/v1.1.0")
        self.requires("cppql/0.2.2@timzoet/v0.2.2")
        self.requires("dot/1.0.1@timzoet/v1.0.1")
        #self.requires("stduuid/1.2.3")
        self.requires("stduuid/1.0.0@timzoet/stable")

        if self.options.build_tests:
            self.requires("bettertest/1.0.1@timzoet/v1.0.1")
        
        if self.options.build_tests or self.options.build_examples:
            self.requires("parsertongue/1.3.1@timzoet/v1.3.1")

    def package_info(self):
        self.cpp_info.components["core"].libs = ["alexandria-core"]
        self.cpp_info.components["core"].requires = ["cmake-modules::cmake-modules", "common::common", "cppql::cppql", "dot::dot", "stduuid::stduuid"]
        self.cpp_info.components["basic-query"].libs = ["alexandria-basic-query"]
        self.cpp_info.components["basic-query"].requires = ["core", "common::common", "cppql::cppql", "dot::dot"]
        self.cpp_info.components["extended-query"].libs = ["alexandria-extended-query"]
        self.cpp_info.components["extended-query"].requires = ["core", "basic-query", "common::common", "cppql::cppql", "dot::dot"]
    
    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)
        tc.generate()
        
        deps = base.generate_deps(self)
        deps.generate()
    
    def configure_cmake(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        return cmake

    def build(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        cmake.install()
