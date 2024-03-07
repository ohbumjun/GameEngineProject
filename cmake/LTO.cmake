function(target_enable_lto)
    # two parameters : TARGET and ENABLE
    # two parameters be in 'oneValueArgs'
    # oneValueArgs : specify that both parameters are expected to have single value 
    set(oneValueArgs TARGET ENABLE)

    # cmake_parse_arguments : pase command-line arguments passed to function
    # in this case. every value set in 'onValueArgs' will be prefixed by 'LTO_
    # ex) LTO_TARGET, LTO_ENABLE created
    cmake_parse_arguments(
        LTO
        "${options}"
        "${oneValueArgs}" # single parameter
        "${multiValueArgs}" # multiple parameter
        ${ARGN})

    # predefined cmake module
    # can check lto is supported or not
    include(CheckIPOSupported)

    check_ipo_supported(RESULT result OUTPUT output)

    if(result)
        message(STATUS "IPO/LTO is supported: ${LTO_TARGET}")
        # INTERPROCEDURAL_OPTIMIZATION : predefined option by cmakelist
        # below : enable lto for certain target
        set_property(
            TARGET ${LTO_TARGET} 
            PROPERTY 
            INTERPROCEDURAL_OPTIMIZATION
            ${LTO_ENABLE}
        )
    else()
        message(WARNING "IPO/LTO is not supported: ${LTO_TARGET}")
    endif()
endfunction()
