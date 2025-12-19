find_package(OpenGL QUIET REQUIRED)

bambustudio_add_cmake_project(TIFF
    # Using official OSGeo download (more reliable than GitLab)
    URL https://download.osgeo.org/libtiff/tiff-4.1.0.zip
    # URL_HASH SHA256 will differ from GitLab archive format
    DEPENDS ${ZLIB_PKG} ${PNG_PKG} ${JPEG_PKG}
    CMAKE_ARGS
        -Dlzma:BOOL=OFF
        -Dwebp:BOOL=OFF
        -Djbig:BOOL=OFF
        -Dzstd:BOOL=OFF
        -Dpixarlog:BOOL=OFF
)
