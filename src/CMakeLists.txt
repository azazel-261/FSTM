add_library(rapidyaml external/rapidyaml.v0.16.0.singlesrc.cpp)

add_executable(fstm main.cc diff.cc snapshot.cc init.cc tracker.cc fs.cc)

if(DEFINED DEBUG)
    target_compile_definitions(fstm PRIVATE DEBUG)
endif ()