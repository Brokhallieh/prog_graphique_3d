cmake_minimum_required(VERSION 3.0)

project(TP_Prog_Graphique_3D LANGUAGES CXX)

set (TP_SHADERS_PATH $ENV{HOME} CACHE PATH "Path where to put GLSL shaders ...")

add_definitions("-DDEF_SHADERS_PATH=${TP_SHADERS_PATH}")

macro(compile)
  add_executable(${ARGV0} ${ARGV0}.cpp)
  target_link_libraries(${ARGV0} easycppogl)
endmacro()

set( tp_list

  #tp1

  #tuto_1

  #tuto2_1

  #tuto2_2

  #tuto2_3

  #tuto2_4

  #tuto3

  #tuto4

  #tp3

  #tp4

  #tp5

  #tp6

  #tp7

  #tp8

  #tp10-1

  #tp10-2

  projet

)

foreach( exec_tp ${tp_list})
	compile(${exec_tp})
endforeach()

# nice vs solution
set_target_properties(${tp_list} PROPERTIES FOLDER TP_Prog_Graphique_3D)