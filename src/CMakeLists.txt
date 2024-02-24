add_library(auc
    graphemebreakproperty_lookup.hpp
    emojiproperty_lookup.hpp

    property.cpp
    graphemecluster.cpp
    u8char.cpp
    u8text.cpp
)

set(AUC_INCLUDE_ROOT_DIR ${PROJECT_SOURCE_DIR}/include)
set(AUC_INCLUDE_PROJ_DIR ${AUC_INCLUDE_ROOT_DIR}/auc)
target_sources(auc
    PUBLIC
        FILE_SET auc_public_headers
        TYPE HEADERS
        BASE_DIRS ${AUC_INCLUDE_ROOT_DIR}/
        FILES
            ${AUC_INCLUDE_PROJ_DIR}/codepoint.hpp
            ${AUC_INCLUDE_PROJ_DIR}/property.hpp
            ${AUC_INCLUDE_PROJ_DIR}/graphemecluster.hpp
            ${AUC_INCLUDE_PROJ_DIR}/u8char.hpp
            ${AUC_INCLUDE_PROJ_DIR}/u8text.hpp
)
target_include_directories(auc
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
