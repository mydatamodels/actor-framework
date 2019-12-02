cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(generators CXX)

add_executable(caf-generate-enum-strings caf-generate-enum-strings.cpp)
target_compile_features(caf-generate-enum-strings PUBLIC cxx_std_11)

function(enum_to_string relative_input_file relative_output_file)
  set(input "${CMAKE_CURRENT_SOURCE_DIR}/../${relative_input_file}")
  set(output "${CMAKE_CURRENT_SOURCE_DIR}/../${relative_output_file}")
  add_custom_command(OUTPUT "${output}"
                     COMMAND caf-generate-enum-strings "${input}" "${output}"
                     DEPENDS caf-generate-enum-strings "${input}")
  get_filename_component(target_name "${relative_output_file}" NAME)
  add_custom_target("${target_name}" ALL DEPENDS "${output}")
endfunction()

# core
enum_to_string("libcaf_core/caf/exit_reason.hpp" "libcaf_core/src/exit_reason_strings.cpp")
enum_to_string("libcaf_core/caf/intrusive/inbox_result.hpp" "libcaf_core/src/inbox_result_strings.cpp")
enum_to_string("libcaf_core/caf/intrusive/task_result.hpp" "libcaf_core/src/task_result_strings.cpp")
enum_to_string("libcaf_core/caf/invoke_message_result.hpp" "libcaf_core/src/invoke_msg_result_strings.cpp")
enum_to_string("libcaf_core/caf/message_priority.hpp" "libcaf_core/src/message_priority_strings.cpp")
enum_to_string("libcaf_core/caf/pec.hpp" "libcaf_core/src/pec_strings.cpp")
enum_to_string("libcaf_core/caf/sec.hpp" "libcaf_core/src/sec_strings.cpp")
enum_to_string("libcaf_core/caf/stream_priority.hpp" "libcaf_core/src/stream_priority_strings.cpp")

# io
enum_to_string("libcaf_io/caf/io/basp/message_type.hpp" "libcaf_io/src/io/basp/message_type_to_string.cpp")
enum_to_string("libcaf_io/caf/io/network/operation.hpp" "libcaf_io/src/io/network/operation_to_string.cpp")
