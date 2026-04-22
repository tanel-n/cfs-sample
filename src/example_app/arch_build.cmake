###########################################################
#
# EXAMPLE_APP platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the EXAMPLE_APP configuration
set(EXAMPLE_APP_PLATFORM_CONFIG_FILE_LIST
  example_app_internal_cfg_values.h
  example_app_platform_cfg.h
  example_app_perfids.h
  example_app_msgids.h
  example_app_msgid_values.h
)

generate_configfile_set(${EXAMPLE_APP_PLATFORM_CONFIG_FILE_LIST})

