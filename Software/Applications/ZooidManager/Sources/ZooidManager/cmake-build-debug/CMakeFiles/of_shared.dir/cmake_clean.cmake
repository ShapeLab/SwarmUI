file(REMOVE_RECURSE
  "../bin/ZooidManager.app/Contents/Frameworks/libopenFrameworks.pdb"
  "../bin/ZooidManager.app/Contents/Frameworks/libopenFrameworks.dylib"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/of_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
