cmake_minimum_required(VERSION 2.8)

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

option( INSTALL_QROBOT_APP_PLUGIN "Check to install qRobotAppPlugin plugin" ON )

if (INSTALL_QROBOT_APP_PLUGIN)
	#CloudCompare 'qRobotAppPlugin' plugin
	project( QROBOT_APP_PLUGIN )

	include( ../CMakePluginTpl.cmake )

	if ( USE_QT5 )
		qt5_use_modules(${PROJECT_NAME} Network)
	else()
		if ( MSVC )
			#We need QtMain to use 'WIN32' mode (/subsystem:windows) with MSVC
			find_package( Qt4 ${QT_VERSION} COMPONENTS QtMain QtNetwork REQUIRED )
		else()
			find_package( Qt4 ${QT_VERSION} COMPONENTS QtNetwork REQUIRED )
		endif()
		if( NOT QT_FOUND )
			message( SEND_ERROR "Qt required, but not found with 'find_package()'" )
		else()
			include( ${QT_USE_FILE} )
		endif()
		
		list( APPEND EXTERNAL_LIBS_INCLUDE_DIR ${QT_INCLUDE_DIR} )
		list( APPEND EXTERNAL_LIBS_LIBRARIES ${QT_LIBRARIES} )
	endif()
	
	# Export Qt Network Dll to specified destinations
	function( install_Qt_Network_Dll ) # 2 arguments: ARGV0 = release destination / ARGV1 = debug destination
	if( WIN32 )
		if( ${ARGC} EQUAL 1 )
			
			#All Qt Dlls (release mode)
			set(QT_RELEASE_DLLS)
			
			if ( NOT USE_QT5 )
				set( QT_RELEASE_DLLS_BASE_NAME QtNetwork${QT_VERSION_MAJOR} )
			else()
				set( QT_BINARY_DIR ${QT5_ROOT_PATH}/bin )
				#standard DLLs
				set( QT_RELEASE_DLLS_BASE_NAME Qt5Network)
			endif()

			#specific case for the MinGW version of Qts
			if( MINGW )
				list( APPEND QT_RELEASE_DLLS_BASE_NAME libgcc )
				list( APPEND QT_RELEASE_DLLS_BASE_NAME mingwm )
			endif()

			#generate full path of release Dlls
			foreach( element ${QT_RELEASE_DLLS_BASE_NAME} )
				list( APPEND QT_RELEASE_DLLS ${QT_BINARY_DIR}/${element}.dll)
			endforeach()

			foreach( qtDLL ${QT_RELEASE_DLLS} )
				if( NOT CMAKE_CONFIGURATION_TYPES )
					install( FILES ${qtDLL} DESTINATION ${ARGV0} )
				else()
					install( FILES ${qtDLL} CONFIGURATIONS Release DESTINATION ${ARGV0} )
				endif()
			endforeach()
			
			# for mutli-config compiler only
			if( CMAKE_CONFIGURATION_TYPES )
			
				#release with debug info version
				foreach( qtDLL ${QT_RELEASE_DLLS} )
					install( FILES ${qtDLL} CONFIGURATIONS RelWithDebInfo DESTINATION ${ARGV0}_withDebInfo )
				endforeach()
				
				#debug version
				set( QT_DEBUG_DLLS )
				
				if ( NOT USE_QT5 )
					set( QT_DEBUG_DLLS_BASE_NAME QtNetworkd${QT_VERSION_MAJOR})
				else()
					#standard DLLs
					set( QT_DEBUG_DLLS_BASE_NAME Qt5Networkd )
				endif()
				#specific case for the MinGW version of Qts
				if( MINGW )
					list( APPEND QT_DEBUG_DLLS_BASE_NAME libgcc )
					list( APPEND QT_DEBUG_DLLS_BASE_NAME mingwm )
				endif()
			
				#generate full path of release Dlls
				foreach( element ${QT_DEBUG_DLLS_BASE_NAME} )
					list( APPEND QT_DEBUG_DLLS ${QT_BINARY_DIR}/${element}.dll)
				endforeach()

				foreach( qtDLL ${QT_DEBUG_DLLS} )
					install( FILES ${qtDLL} CONFIGURATIONS Debug DESTINATION ${ARGV0}_debug )
				endforeach()

			endif()
		else()
			message( SEND_ERROR "function install_Qt_Dlls: invalid number of arguments! (need base destination)" )
		endif()
	endif()
	endfunction()
	
	if( WIN32 )
		install_Qt_Network_Dll( ${CLOUDCOMPARE_DEST_FOLDER} )
	endif()
	
endif()
