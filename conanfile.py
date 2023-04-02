from conan import ConanFile
from conan.tools.files import copy

class AlexandriaConan(ConanFile):
    ############################################################################
    ## Package info.                                                          ##
    ############################################################################
    
    name = "alexandria"
    
    description = "..."
    
    url = "https://github.com/TimZoet/Alexandria"

    ############################################################################
    ## Settings.                                                              ##
    ############################################################################

    python_requires = "pyreq/1.0.0@timzoet/v1.0.0"
    
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
        
        self.requires("common/1.0.0@timzoet/v1.0.0")
        self.requires("cppql/0.2.0@timzoet/v0.2.0")
        self.requires("dot/1.0.0@timzoet/v1.0.0")
        self.requires("parsertongue/1.3.0@timzoet/v1.3.0")
        self.requires("stduuid/1.2.3")

        if self.options.build_tests:
            self.requires("bettertest/1.0.0@timzoet/v1.0.0")

    def package_info(self):
        self.cpp_info.components["core"].libs = ["alexandria-core"]
        self.cpp_info.components["core"].requires = ["common::common", "cppql::cppql", "dot::dot"]
        self.cpp_info.components["query"].libs = ["alexandria-query"]
        self.cpp_info.components["query"].requires = ["alexandria-core", "common::common", "cppql::cppql", "dot::dot"]
    
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
