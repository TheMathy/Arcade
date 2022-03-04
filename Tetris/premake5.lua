workspace "Tetris"
    architecture "x86_64"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["GLFW"] = "%{wks.location}/src/vendor/GLFW/include"
    --IncludeDir["GLEW"] = "%{wks.location}/RocketEngine/vendor/GLEW/include"
    IncludeDir["Glad"] = "%{wks.location}/src/vendor/Glad/include"
    IncludeDir["glm"] = "%{wks.location}/src/vendor/glm"
    IncludeDir["stb_image"] = "%{wks.location}/src/vendor/stb_image"
    --IncludeDir["stb_image"] = "%{wks.location}/RocketEngine/vendor/stb_image"

    group "Dependencies"
	    include "src/vendor/GLFW"
	    include "src/vendor/Glad"
    group ""

    project "Tetris"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++1z"

        targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "src/Tetris/**.h",
            "src/Tetris/**.cpp",
            "src/vendor/stb_image/**.cpp",
            "src/vendor/stb_image/**.h",
        }

        includedirs
        {
            "src/Tetris",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.Glad}",
            "%{IncludeDir.glm}",
            "%{IncludeDir.stb_image}"
        }

        defines
        {
            "GLFW_INCLUDE_NONE"
        }

        links
        {
            "GLFW",
            "Glad",
            "OpenGL.framework",
            "Cocoa.framework",
            "IOKit.framework"
        }

        postbuildcommands 
        {
            "{COPY} %{wks.location}/res %{cfg.targetdir}"
        }

        filter "configurations:Debug"
		    runtime "Debug"
		    symbols "on"

	    filter "configurations:Release"
		    runtime "Release"
		    optimize "on"