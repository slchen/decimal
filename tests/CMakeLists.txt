
add_executable(
    unit_tests
    unit_tests.cpp
)

set_target_properties(unit_tests PROPERTIES CXX_STANDARD 17)

target_link_libraries(unit_tests
  PRIVATE
    Decimal::Headers
)

