from conans import ConanFile

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

    python_requires = "pyreq/1.0.0@timzoet/stable"
    
    python_requires_extend = "pyreq.BaseConan"
    
    ############################################################################
    ## Base methods.                                                          ##
    ############################################################################
    
    def set_version(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.set_version(self, "alexandriaVersionString.cmake", "ALEXANDRIA_VERSION")
    
    def init(self):
        base = self.python_requires["pyreq"].module.BaseConan
        self.generators = base.generators + self.generators
        self.settings = base.settings + self.settings
        self.options = {**base.options, **self.options}
        self.default_options = {**base.default_options, **self.default_options}
    
    ############################################################################
    ## Building.                                                              ##
    ############################################################################
    
    def export_sources(self):
        self.copy("alexandriaVersionString.cmake")
        self.copy("CMakeLists.txt")
        self.copy("license")
        self.copy("readme.md")
        self.copy("applications/*")
        self.copy("cmake/*")
        self.copy("modules/*")
    
    def config_options(self):
        base = self.python_requires["pyreq"].module.BaseConan
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def configure(self):
        self.options["bettertest"].build_alexandria = False
    
    def requirements(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.requirements(self)
        
        self.requires("common/1.0.0@timzoet/stable")
        self.requires("cppql/0.1.0@timzoet/stable")
        self.requires("dot/1.0.0@timzoet/stable")
        self.requires("parsertongue/1.1.0@timzoet/stable")
        self.requires("stduuid/1.0.0@timzoet/stable")

        if self.options.build_tests:
            self.requires("bettertest/1.0.0@timzoet/stable")

    def package_info(self):
        self.cpp_info.components["alexandria"].libs = ["alexandria"]
        self.cpp_info.components["alexandria"].requires = ["common::common", "cppql::cppql", "dot::dot"]
    
    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)
        tc.generate()
        
        deps = base.generate_deps(self)
        deps.generate()

    def build(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        cmake.install()