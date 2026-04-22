###########################################################
#
# EXAMPLE_APP mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the EXAMPLE_APP configuration
set(EXAMPLE_APP_MISSION_CONFIG_FILE_LIST
  example_app_fcncode_values.h
  example_app_interface_cfg_values.h
  example_app_mission_cfg.h
  example_app_perfids.h
  example_app_msg.h
  example_app_msgdefs.h
  example_app_msgstruct.h
  example_app_tbl.h
  example_app_tbldefs.h
  example_app_tblstruct.h
  example_app_topicid_values.h
)

generate_configfile_set(${EXAMPLE_APP_MISSION_CONFIG_FILE_LIST})

