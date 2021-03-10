#============================================================================
# Copyright (C) 2013 - 2018, OpenJK contributors
#
# This file is part of the OpenJK source code.
#
# OpenJK is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#============================================================================

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(MPSharedDefines ${SharedDefines})

#    Add Game Project
if(BuildMPGame)
	add_subdirectory("${MPDir}/game")
endif(BuildMPGame)

#    Add CGame Project
if(BuildMPCGame)
	add_subdirectory("${MPDir}/cgame")
endif(BuildMPCGame)

#    Add UI Project
if(BuildMPUI)
	add_subdirectory("${MPDir}/ui")
endif(BuildMPUI)

#	 Add Vanilla JKA Renderer Project
if(BuildMPRdVanilla)
	add_subdirectory("${MPDir}/rd-vanilla")
endif(BuildMPRdVanilla)

#    Common things between Engine and Dedicated Server
if(BuildMPEngine OR BuildMPDed)

	#    Botlib
	# the files could arguably just be put into the engine and dedicated projects without having a library for it.
	set(MPBotlibDefines ${SharedDefines} "BUILD_BOTLIB")
	set(MPBotlibIncludeDirectories
		${SharedDir}
		${MPDir}
		"${GSLIncludeDirectory}"
		)
	set(MPBotlibBotlibFiles
		"${MPDir}/botlib/be_ea.hpp"
		"${MPDir}/botlib/be_interface.hpp"
		"${MPDir}/botlib/botlib.hpp"
		"${MPDir}/botlib/l_crc.hpp"
		"${MPDir}/botlib/l_libvar.hpp"
		"${MPDir}/botlib/l_log.hpp"
		"${MPDir}/botlib/l_memory.hpp"
		"${MPDir}/botlib/l_precomp.hpp"
		"${MPDir}/botlib/l_script.hpp"
		"${MPDir}/botlib/l_struct.hpp"
		"${MPDir}/botlib/l_utils.hpp"

		"${MPDir}/botlib/be_ea.cpp"
		"${MPDir}/botlib/be_interface.cpp"
		"${MPDir}/botlib/l_crc.cpp"
		"${MPDir}/botlib/l_libvar.cpp"
		"${MPDir}/botlib/l_log.cpp"
		"${MPDir}/botlib/l_memory.cpp"
		"${MPDir}/botlib/l_precomp.cpp"
		"${MPDir}/botlib/l_script.cpp"
		"${MPDir}/botlib/l_struct.cpp"
		)
	source_group("botlib" FILES ${MPBotlibBotlibFiles})
	set(MPBotlibFiles ${MPBotlibFiles} ${MPBotlibBotlibFiles})

	set(MPBotlibCommonFiles
		"${MPDir}/qcommon/cm_public.hpp"
		"${MPDir}/qcommon/disablewarnings.hpp"
		"${MPDir}/qcommon/q_shared.hpp"
		"${MPDir}/qcommon/q_common.hpp"
		"${MPDir}/qcommon/q_files.hpp"
		"${MPDir}/qcommon/q_type.hpp"
		"${MPDir}/qcommon/tags.hpp"

		${SharedCommonFiles}
		)
	source_group("common" FILES ${MPBotlibCommonFiles})
	set(MPBotlibFiles ${MPBotlibFiles} ${MPBotlibCommonFiles})

	set(MPBotlibServerFiles
		"${MPDir}/server/server.hpp"
		)
	source_group("server" FILES ${MPBotlibServerFiles})
	set(MPBotlibFiles ${MPBotlibFiles} ${MPBotlibServerFiles})

	add_library(${MPBotLib} STATIC ${MPBotlibFiles})

	# Hide symbols not explicitly marked public.
	set_property(TARGET ${MPBotLib} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

	set_target_properties(${MPBotLib} PROPERTIES COMPILE_DEFINITIONS "${MPBotlibDefines}")
	set_target_properties(${MPBotLib} PROPERTIES INCLUDE_DIRECTORIES "${MPBotlibIncludeDirectories}")
	set_target_properties(${MPBotLib} PROPERTIES PROJECT_LABEL "Bot Library")

	#    Common files/libraries/defines of both Engine and Dedicated Server

	# libraries: Botlib
	set(MPEngineAndDedLibraries ${MPBotLib})
	# Platform-specific libraries
	if(WIN32)
		set(MPEngineAndDedLibraries ${MPEngineAndDedLibraries} "winmm" "wsock32")
	endif(WIN32)

	# Include directories
	set(MPEngineAndDedIncludeDirectories ${MPDir} ${SharedDir} ${GSLIncludeDirectory}) # codemp folder, since includes are not always relative in the files

	# Transparently use our bundled minizip.
	list(APPEND MPEngineAndDedIncludeDirectories ${MINIZIP_INCLUDE_DIRS})
	list(APPEND MPEngineAndDedLibraries          ${MINIZIP_LIBRARIES})

	# Transparently use either bundled or system zlib.
	list(APPEND MPEngineAndDedIncludeDirectories ${ZLIB_INCLUDE_DIR})
	list(APPEND MPEngineAndDedLibraries          ${ZLIB_LIBRARIES})

	set(MPEngineAndDedCgameFiles
		"${MPDir}/cgame/cg_engine.hpp"
		)
	source_group("cgame" FILES ${MPEngineAndDedCgameFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedCgameFiles})

	set(MPEngineAndDedClientFiles
		"${MPDir}/client/cl_keycodes.hpp"
		"${MPDir}/client/cl_keys.hpp"
		"${MPDir}/client/cl_public.hpp"
		"${MPDir}/client/snd_public.hpp"
		)
	source_group("client" FILES ${MPEngineAndDedClientFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedClientFiles})

	set(MPEngineAndDedGameFiles
		"${MPDir}/game/anims.hpp"
		"${MPDir}/game/bg_public.hpp"
		"${MPDir}/game/bg_weapons.hpp"
		"${MPDir}/botlib/botlib.hpp"
		"${MPDir}/game/g_engine.hpp"
		)
	source_group("game" FILES ${MPEngineAndDedGameFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedGameFiles})

	set(MPEngineAndDedCommonFiles
		"${MPDir}/qcommon/cm_load.cpp"
		"${MPDir}/qcommon/cm_local.hpp"
		"${MPDir}/qcommon/cm_patch.cpp"
		"${MPDir}/qcommon/cm_patch.hpp"
		"${MPDir}/qcommon/cm_polylib.cpp"
		"${MPDir}/qcommon/cm_polylib.hpp"
		"${MPDir}/qcommon/cm_public.hpp"
		"${MPDir}/qcommon/cm_test.cpp"
		"${MPDir}/qcommon/cm_trace.cpp"
		"${MPDir}/qcommon/cmd.cpp"
		"${MPDir}/qcommon/com_cvar.hpp"
		# hack until we clean up renderer/engine cvars
		"${MPDir}/qcommon/com_cvars.cpp"
		"${MPDir}/qcommon/com_cvars.hpp"
		"${MPDir}/qcommon/com_inputField.cpp"
		"${MPDir}/qcommon/com_inputField.hpp"
		"${MPDir}/qcommon/cvar.cpp"
		"${MPDir}/qcommon/disablewarnings.hpp"
		"${MPDir}/qcommon/game_version.hpp"
		"${MPDir}/qcommon/GenericParser2.cpp"
		"${MPDir}/qcommon/GenericParser2.hpp"
		"${MPDir}/qcommon/huffman.cpp"
		"${MPDir}/qcommon/huffman.hpp"
		"${MPDir}/qcommon/md4.cpp"
		"${MPDir}/qcommon/md5.cpp"
		"${MPDir}/qcommon/md5.hpp"
		"${MPDir}/qcommon/MiniHeap.hpp"
		"${MPDir}/qcommon/msg.cpp"
		"${MPDir}/qcommon/matcomp.hpp"
		"${MPDir}/qcommon/matcomp.cpp"
		"${MPDir}/qcommon/net_chan.cpp"
		"${MPDir}/qcommon/net_ip.cpp"
		"${MPDir}/qcommon/persistence.cpp"
		"${MPDir}/qcommon/q_shared.cpp"
		"${MPDir}/qcommon/q_shared.hpp"
		"${MPDir}/qcommon/q_common.cpp"
		"${MPDir}/qcommon/q_common.hpp"
		"${MPDir}/qcommon/q_files.cpp"
		"${MPDir}/qcommon/q_files.hpp"
		"${MPDir}/qcommon/q_type.hpp"
		"${MPDir}/qcommon/RoffSystem.cpp"
		"${MPDir}/qcommon/RoffSystem.hpp"
		"${MPDir}/qcommon/sstring.hpp"
		"${MPDir}/qcommon/stringed_ingame.cpp"
		"${MPDir}/qcommon/stringed_ingame.hpp"
		"${MPDir}/qcommon/stringed_interface.cpp"
		"${MPDir}/qcommon/stringed_interface.hpp"
		"${MPDir}/qcommon/tags.hpp"
		"${MPDir}/qcommon/timing.hpp"
		"${MPDir}/qcommon/vm.cpp"
		"${MPDir}/qcommon/z_memman_pc.cpp"

		${SharedCommonFiles}
		)
	if(WIN32)
		set(MPEngineAndDedCommonFiles ${MPEngineAndDedCommonFiles})
	endif(WIN32)
	source_group("common" FILES ${MPEngineAndDedCommonFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedCommonFiles})

	set(MPEngineAndDedG2Files
		"${MPDir}/ghoul2/G2.hpp"
		"${MPDir}/ghoul2/G2_gore.hpp"
		"${MPDir}/ghoul2/ghoul2_shared.hpp"
		"${MPDir}/ghoul2/g2_local.hpp"
		)
	source_group("ghoul2" FILES ${MPEngineAndDedG2Files})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedG2Files})

	set(MPEngineAndDedIcarusFiles
		"${MPDir}/icarus/BlockStream.cpp"
		"${MPDir}/icarus/blockstream.hpp"
		"${MPDir}/icarus/GameInterface.cpp"
		"${MPDir}/icarus/GameInterface.hpp"
		"${MPDir}/icarus/icarus.hpp"
		"${MPDir}/icarus/Instance.cpp"
		"${MPDir}/icarus/instance.hpp"
		"${MPDir}/icarus/Interface.cpp"
		"${MPDir}/icarus/interface.hpp"
		"${MPDir}/icarus/interpreter.hpp"
		"${MPDir}/icarus/Memory.cpp"
		"${MPDir}/icarus/Q3_Interface.cpp"
		"${MPDir}/icarus/Q3_Interface.hpp"
		"${MPDir}/icarus/Q3_Registers.cpp"
		"${MPDir}/icarus/Q3_Registers.hpp"
		"${MPDir}/icarus/Sequence.cpp"
		"${MPDir}/icarus/sequence.hpp"
		"${MPDir}/icarus/Sequencer.cpp"
		"${MPDir}/icarus/sequencer.hpp"
		"${MPDir}/icarus/TaskManager.cpp"
		"${MPDir}/icarus/taskmanager.hpp"
		"${MPDir}/icarus/tokenizer.hpp"
		)
	source_group("icarus" FILES ${MPEngineAndDedIcarusFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedIcarusFiles})

	set(MPEngineAndDedServerFiles
		# hack until we clean up renderer/engine cvars
		"${MPDir}/server/server.hpp"
		"${MPDir}/server/sv_bot.cpp"
		"${MPDir}/server/sv_ccmds.cpp"
		"${MPDir}/server/sv_challenge.cpp"
		"${MPDir}/server/sv_client.cpp"
		"${MPDir}/server/sv_game.cpp"
		"${MPDir}/server/sv_init.cpp"
		"${MPDir}/server/sv_main.cpp"
		"${MPDir}/server/sv_net_chan.cpp"
		"${MPDir}/server/sv_snapshot.cpp"
		"${MPDir}/server/sv_world.cpp"
		"${MPDir}/server/sv_gameapi.cpp"
		"${MPDir}/server/sv_gameapi.hpp"
		)
	source_group("server" FILES ${MPEngineAndDedServerFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedServerFiles})

	set(MPEngineAndDedUiFiles
		"${MPDir}/ui/ui_engine.hpp"
		)
	source_group("ui" FILES ${MPEngineAndDedUiFiles})
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedUiFiles})

	set(MPEngineAndDedSysFiles
		"${SharedDir}/sys/snapvector.cpp"
		)
	set(MPEngineAndDedFiles ${MPEngineAndDedFiles} ${MPEngineAndDedSysFiles})
	source_group("sys" FILES ${MPEngineAndDedSysFiles})
endif(BuildMPEngine OR BuildMPDed)

#        Engine/Executable (jamp.exe)

if(BuildMPEngine)
	set(MPEngineLibraries ${MPEngineAndDedLibraries})
	set(MPEngineIncludeDirectories ${MPEngineAndDedIncludeDirectories})
	set(MPEngineFiles ${MPEngineAndDedFiles})
	set(MPEngineDefines ${MPSharedDefines})

	#    Dependencies

	if (NOT WIN32)
		# OpenGL
		find_package(OpenGL REQUIRED)
		set(MPEngineIncludeDirectories ${MPEngineIncludeDirectories} ${OPENGL_INCLUDE_DIR})
		set(MPEngineLibraries ${MPEngineLibraries} ${OPENGL_LIBRARIES})
	endif()

	# OpenAL (is optionally included for Windows)
	if (MSVC AND NOT WIN64)
		if(UseInternalOpenAL)
			# There's a libs/OpenAL32.lib
			# (mrw) TODO Is this actually required on windows? It is loaded at runtime, should not be linked?
			find_library(OpenALLibrary NAMES OpenAL32 PATHS ${OpenJKLibDir})
			if(NOT OpenALLibrary)
				message(FATAL_ERROR "UseInternalOpenAL enabled, but lib/OpenAL32.lib not found!")
			endif(NOT OpenALLibrary)
			set(MPEngineLibraries ${MPEngineLibraries} ${OpenALLibrary})
		else(UseInternalOpenAL)
			find_package(OpenAL REQUIRED)
			set(MPEngineIncludeDirectories ${MPEngineIncludeDirectories} ${OPENAL_INCLUDE_DIR})
			set(MPEngineLibraries ${MPEngineLibraries} ${OPENAL_LIBRARY})
		endif(UseInternalOpenAL)
	endif()

	if(UseInternalSDL2)
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(MPEngineLibraries
				${MPEngineLibraries}
				${OpenJKLibDir}/SDL2/lib/x86/SDL2main.lib
				${OpenJKLibDir}/SDL2/lib/x86/SDL2.lib
				)
		else()
			set(MPEngineLibraries
				${MPEngineLibraries}
				${OpenJKLibDir}/SDL2/lib/x64/SDL2main.lib
				${OpenJKLibDir}/SDL2/lib/x64/SDL2.lib
				)
		endif()

		set(MPEngineIncludeDirectories
			${MPEngineIncludeDirectories}
			${OpenJKLibDir}/SDL2/include
			)
	else()
		find_package(SDL2 REQUIRED)
		set(MPEngineIncludeDirectories ${MPEngineIncludeDirectories} ${SDL2_INCLUDE_DIR})
		set(MPEngineLibraries ${MPEngineLibraries} ${SDL2_LIBRARY})
	endif()

	# EAX is Windows-Only (right?)
	if(MSVC)
		set(MPEngineEAXFiles
			"${MPDir}/client/eax/eax.hpp"
			"${MPDir}/client/eax/EaxMan.hpp"
			)
		source_group("eax" FILES ${MPEngineEAXFiles})
		set(MPEngineFiles ${MPEngineFiles} ${MPEngineEAXFiles})
	endif(MSVC)

	#    Source Files

	set(MPEngineClientFiles
		"${MPDir}/client/cl_avi.cpp"
		"${MPDir}/client/cl_cgame.cpp"
		"${MPDir}/client/cl_cgameapi.cpp"
		"${MPDir}/client/cl_cgameapi.hpp"
		"${MPDir}/client/cl_cin.cpp"
		"${MPDir}/client/cl_console.cpp"
		"${MPDir}/client/cl_fonts.cpp"
		"${MPDir}/client/cl_fonts.hpp"
		"${MPDir}/client/cl_input.cpp"
		"${MPDir}/client/cl_keys.cpp"
		"${MPDir}/client/cl_lan.cpp"
		"${MPDir}/client/cl_lan.hpp"
		"${MPDir}/client/cl_local.hpp"
		"${MPDir}/client/cl_main.cpp"
		"${MPDir}/client/cl_net_chan.cpp"
		"${MPDir}/client/cl_parse.cpp"
		"${MPDir}/client/cl_scrn.cpp"
		"${MPDir}/client/cl_ui.cpp"
		"${MPDir}/client/cl_uiapi.cpp"
		"${MPDir}/client/cl_uiapi.hpp"
		"${MPDir}/client/FXExport.cpp"
		"${MPDir}/client/FXExport.hpp"
		"${MPDir}/client/FxPrimitives.cpp"
		"${MPDir}/client/FxPrimitives.hpp"
		"${MPDir}/client/FxScheduler.cpp"
		"${MPDir}/client/FxScheduler.hpp"
		"${MPDir}/client/FxSystem.cpp"
		"${MPDir}/client/FxSystem.hpp"
		"${MPDir}/client/FxTemplate.cpp"
		"${MPDir}/client/FxUtil.cpp"
		"${MPDir}/client/FxUtil.hpp"
		"${MPDir}/client/snd_ambient.cpp"
		"${MPDir}/client/snd_ambient.hpp"
		"${MPDir}/client/snd_dma.cpp"
		"${MPDir}/client/snd_local.hpp"
		"${MPDir}/client/snd_mem.cpp"
		"${MPDir}/client/snd_mix.cpp"
		"${MPDir}/client/snd_mp3.cpp"
		"${MPDir}/client/snd_mp3.hpp"
		"${MPDir}/client/snd_music.cpp"
		"${MPDir}/client/snd_music.hpp"
		)
	source_group("client" FILES ${MPEngineClientFiles})
	set(MPEngineFiles ${MPEngineFiles} ${MPEngineClientFiles})

	set(MPEngineRatlFiles
		"${MPDir}/Ratl/bits_vs.hpp"
		"${MPDir}/Ratl/ratl_common.hpp"
		"${MPDir}/Ratl/vector_vs.hpp"
		)
	source_group("ratl" FILES ${MPEngineRatlFiles})
	set(MPEngineFiles ${MPEngineFiles} ${MPEngineRatlFiles})

	set(MPEngineRavlFiles
		"${MPDir}/Ravl/CVec.hpp"
		)
	source_group("ravl" FILES ${MPEngineRavlFiles})
	set(MPEngineFiles ${MPEngineFiles} ${MPEngineRavlFiles})

	set(MPEngineRendererFiles
		"${MPDir}/rd-common/tr_types.hpp"
		)
	source_group("rd-common" FILES ${MPEngineRendererFiles})
	set(MPEngineFiles ${MPEngineFiles} ${MPEngineRendererFiles})

	set(MPEngineMp3Files
		"${MPDir}/mp3code/cdct.c"
		"${MPDir}/mp3code/config.h"
		"${MPDir}/mp3code/csbt.c"
		"${MPDir}/mp3code/csbtb.c"
		"${MPDir}/mp3code/csbtl3.c"
		"${MPDir}/mp3code/cup.c"
		"${MPDir}/mp3code/cupini.c"
		"${MPDir}/mp3code/cupl1.c"
		"${MPDir}/mp3code/cupl3.c"
		"${MPDir}/mp3code/cwin.c"
		"${MPDir}/mp3code/cwinb.c"
		"${MPDir}/mp3code/cwinm.c"
		"${MPDir}/mp3code/htable.h"
		"${MPDir}/mp3code/hwin.c"
		"${MPDir}/mp3code/jdw.h"
		"${MPDir}/mp3code/l3.h"
		"${MPDir}/mp3code/l3dq.c"
		"${MPDir}/mp3code/l3init.c"
		"${MPDir}/mp3code/mdct.c"
		"${MPDir}/mp3code/mhead.c"
		"${MPDir}/mp3code/mhead.h"
		"${MPDir}/mp3code/mp3struct.h"
		"${MPDir}/mp3code/msis.c"
		"${MPDir}/mp3code/port.h"
		"${MPDir}/mp3code/small_header.h"
		"${MPDir}/mp3code/tableawd.h"
		"${MPDir}/mp3code/towave.c"
		"${MPDir}/mp3code/uph.c"
		"${MPDir}/mp3code/upsf.c"
		"${MPDir}/mp3code/wavep.c"
		)
	source_group("mp3code" FILES ${MPEngineMp3Files})
	set(MPEngineFiles ${MPEngineFiles} ${MPEngineMp3Files})

	set(MPEngineSysFiles
		"${SharedDir}/sdl/sdl_window.cpp"
		"${SharedDir}/sdl/sdl_icon.hpp"
		"${SharedDir}/sdl/sdl_input.cpp"
		"${SharedDir}/sdl/sdl_sound.cpp"
		"${SharedDir}/sys/sys_loadlib.hpp"
		"${SharedDir}/sys/sys_local.hpp"
		"${SharedDir}/sys/sys_main.cpp"
		"${SharedDir}/sys/sys_event.cpp"
		"${SharedDir}/sys/sys_public.hpp"
		"${SharedDir}/sys/con_local.hpp"
		"${SharedDir}/sys/con_log.cpp"
		)

	if(WIN32)
		set(MPEngineWin32Files
			"${MPDir}/win32/icon.ico"
			"${MPDir}/win32/resource.hpp"
			"${MPDir}/win32/openjk.rc"
			"${SharedDir}/sys/win_manifest.manifest"
			)

		set(MPEngineFiles ${MPEngineFiles} ${MPEngineWin32Files})
		source_group("win32" FILES ${MPEngineWin32Files})

		set(MPEngineSysFiles
			${MPEngineSysFiles}
			"${SharedDir}/sys/sys_win32.cpp"
			"${SharedDir}/sys/con_passive.cpp"
			)
	else(WIN32)
		set(MPEngineSysFiles
			${MPEngineSysFiles}
			"${SharedDir}/sys/sys_unix.cpp"
			"${SharedDir}/sys/con_tty.cpp"
			)
	endif(WIN32)

	set(MPEngineFiles ${MPEngineFiles} ${MPEngineSysFiles})
	source_group("sys" FILES ${MPEngineSysFiles})

	if(MakeApplicationBundles)
		set(MACOSX_BUNDLE_BUNDLE_NAME ${MPEngine})
		set(MACOSX_BUNDLE_DISPLAY_NAME ${MPEngine})
		set(MACOSX_BUNDLE_ICON_FILE OpenJK.icns)
		set_source_files_properties(${MPDir}/macosx/OpenJK.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set(MPEngineFiles ${MPEngineFiles} ${MPDir}/macosx/OpenJK.icns)
		set_source_files_properties(${MPDir}/macosx/English.lproj/InfoPlist.strings PROPERTIES MACOSX_PACKAGE_LOCATION Resources/English.lproj)
		set(MPEngineFiles ${MPEngineFiles} ${MPDir}/macosx/English.lproj/InfoPlist.strings)
	endif(MakeApplicationBundles)

	if(WIN32)
		add_executable(${MPEngine} WIN32 ${MPEngineFiles})
		if(UseInternalSDL2)
			if(CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(MPEngineExtraInstallFiles
					${OpenJKLibDir}/SDL2/bin/x86/SDL2.dll
					)
			else()
				set(MPEngineExtraInstallFiles
					${OpenJKLibDir}/SDL2/bin/x64/SDL2.dll
					)
			endif()
		endif()
	else(WIN32)
		if(MakeApplicationBundles)
			add_executable(${MPEngine} MACOSX_BUNDLE ${MPEngineFiles})
			set_target_properties(${MPEngine} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${MPDir}/macosx/OpenJKInfo.plist")
		else(MakeApplicationBundles)
			add_executable(${MPEngine} ${MPEngineFiles})
		endif(MakeApplicationBundles)
	endif(WIN32)

	if(MakeApplicationBundles)
		install(CODE "
			include(BundleUtilities)
			set(BU_CHMOD_BUNDLE_ITEMS ON)
			fixup_bundle(\"${CMAKE_BINARY_DIR}/${MPEngine}.app\" \"\" \"\")
			"
			COMPONENT Runtime)
		install(TARGETS ${MPEngine}
			BUNDLE
			DESTINATION ${JKAInstallDir}
			COMPONENT ${JKAMPClientComponent})
	else(MakeApplicationBundles)
		install(TARGETS ${MPEngine}
			RUNTIME
			DESTINATION ${JKAInstallDir}
			COMPONENT ${JKAMPClientComponent})
		if(MPEngineExtraInstallFiles)
			install(FILES ${MPEngineExtraInstallFiles}
				DESTINATION ${JKAInstallDir}
				COMPONENT ${JKAMPClientComponent})
		endif()
	endif(MakeApplicationBundles)

	set_target_properties(${MPEngine} PROPERTIES COMPILE_DEFINITIONS "${MPEngineDefines}")

	# Hide symbols not explicitly marked public.
	set_property(TARGET ${MPEngine} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

	set_target_properties(${MPEngine} PROPERTIES INCLUDE_DIRECTORIES "${MPEngineIncludeDirectories}")
	set_target_properties(${MPEngine} PROPERTIES PROJECT_LABEL "MP Client")
	target_link_libraries(${MPEngine} ${MPEngineLibraries})
endif(BuildMPEngine)

#        Dedicated Server (Engine) (jampded.exe)

if(BuildMPDed)
	set(MPDedLibraries ${MPEngineAndDedLibraries})
	set(MPDedIncludeDirectories ${MPEngineAndDedIncludeDirectories})
	set(MPDedFiles ${MPEngineAndDedFiles})
	set(MPDedDefines ${MPSharedDefines} "_CONSOLE" "BUILD_DEDICATED")

	set(MPDedFiles ${MPDedFiles}
		"${MPDir}/qcommon/com_cvars.cpp")

	# Dedicated renderer is compiled with the server.
	set(MPDedicatedRendererFiles
		"${MPDir}/ghoul2/G2_gore.cpp"
		"${MPDir}/rd-common/mdx_format.hpp"
		"${MPDir}/rd-common/tr_public.hpp"
		"${MPDir}/rd-dedicated/tr_local.hpp"
		"${MPDir}/rd-dedicated/G2_API.cpp"
		"${MPDir}/rd-dedicated/G2_bolts.cpp"
		"${MPDir}/rd-dedicated/G2_bones.cpp"
		"${MPDir}/rd-dedicated/G2_misc.cpp"
		"${MPDir}/rd-dedicated/G2_surfaces.cpp"
		"${MPDir}/rd-dedicated/tr_backend.cpp"
		"${MPDir}/rd-dedicated/tr_cvars.cpp"
		"${MPDir}/rd-dedicated/tr_ghoul2.cpp"
		"${MPDir}/rd-dedicated/tr_init.cpp"
		"${MPDir}/rd-dedicated/tr_main.cpp"
		"${MPDir}/rd-dedicated/tr_mesh.cpp"
		"${MPDir}/rd-dedicated/tr_model.cpp"
		"${MPDir}/rd-dedicated/tr_shader.cpp"
		"${MPDir}/rd-dedicated/tr_skin.cpp")
	source_group("renderer" FILES ${MPDedicatedRendererFiles})
	set(MPDedFiles ${MPDedFiles} ${MPDedicatedRendererFiles})

	set(MPDedRendererComFiles
		"${MPDir}/rd-common/tr_types.hpp"
		)
	source_group("rd-common" FILES ${MPDedRendererComFiles})
	set(MPDedFiles ${MPDedFiles} ${MPDedRendererComFiles})

	set(MPDedNullFiles
		"${MPDir}/null/null_client.cpp"
		"${MPDir}/null/null_input.cpp"
		"${MPDir}/null/null_renderer.cpp"
		"${MPDir}/null/null_snddma.cpp"
		)
	source_group("null" FILES ${MPDedNullFiles})
	set(MPDedFiles ${MPDedFiles} ${MPDedNullFiles})

	if(WIN32)
		set(MPDedWin32Files
			"${MPDir}/win32/openjkded.rc"
			)
		source_group("win32" FILES ${MPDedWin32Files})
		set(MPDedFiles ${MPDedFiles} ${MPDedWin32Files})

		set(MPDedSysFiles
			"${SharedDir}/sys/sys_loadlib.hpp"
			"${SharedDir}/sys/sys_local.hpp"
			"${SharedDir}/sys/sys_main.cpp"
			"${SharedDir}/sys/sys_event.cpp"
			"${SharedDir}/sys/sys_public.hpp"
			"${SharedDir}/sys/sys_win32.cpp"
			"${SharedDir}/sys/con_local.hpp"
			"${SharedDir}/sys/con_log.cpp"
			"${SharedDir}/sys/con_win32.cpp"
			)
		set(MPDedLibraries ${MPDedLibraries} ${CMAKE_DL_LIBS})
		source_group("sys" FILES ${MPDedSysFiles})
		set(MPDedFiles ${MPDedFiles} ${MPDedSysFiles})
	else(WIN32)
		set(MPDedSysFiles
			"${SharedDir}/sys/sys_loadlib.hpp"
			"${SharedDir}/sys/sys_local.hpp"
			"${SharedDir}/sys/sys_main.cpp"
			"${SharedDir}/sys/sys_event.cpp"
			"${SharedDir}/sys/sys_public.hpp"
			"${SharedDir}/sys/sys_unix.cpp"
			"${SharedDir}/sys/con_local.hpp"
			"${SharedDir}/sys/con_log.cpp"
			"${SharedDir}/sys/con_tty.cpp"
			)
		set(MPDedLibraries ${MPDedLibraries} ${CMAKE_DL_LIBS})
		source_group("sys" FILES ${MPDedSysFiles})
		set(MPDedFiles ${MPDedFiles} ${MPDedSysFiles})
	endif(WIN32)

	add_executable(${MPDed} ${MPDedFiles})
	install(TARGETS ${MPDed}
		RUNTIME
		DESTINATION ${JKAInstallDir}
		COMPONENT ${JKAMPServerComponent})

	set_target_properties(${MPDed} PROPERTIES COMPILE_DEFINITIONS "${MPDedDefines}")

	# Hide symbols not explicitly marked public.
	set_property(TARGET ${MPDed} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

	set_target_properties(${MPDed} PROPERTIES INCLUDE_DIRECTORIES "${MPDedIncludeDirectories}")
	set_target_properties(${MPDed} PROPERTIES PROJECT_LABEL "MP Dedicated Server")
	target_link_libraries(${MPDed} ${MPDedLibraries})
endif(BuildMPDed)

	set(GameLibsBuilt)
	if(BuildMPGame)
		set(GameLibsBuilt ${GameLibsBuilt} ${MPGame})
	endif()

	if(BuildMPCGame)
		set(GameLibsBuilt ${GameLibsBuilt} ${MPCGame})
	endif()

	if(BuildMPUI)
		set(GameLibsBuilt ${GameLibsBuilt} ${MPUI})
	endif()

if(WIN32)
set(GameLibFullPaths)
	if(MSVC)
		foreach(GameLib ${GameLibsBuilt})
			set(GameLibFullPaths
				${GameLibFullPaths}
				${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${GameLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(GameLib)
	else()
		foreach(GameLib ${GameLibsBuilt})
			set(GameLibFullPaths
				${GameLibFullPaths}
				${CMAKE_BINARY_DIR}/${GameLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(GameLib)
	endif()

	include(InstallZIP)
	add_zip_command(${AssetsPk3}
		FILES ${GameLibFullPaths}
		DEPENDS "${GameLibsBuilt}")
	add_custom_target(Assets
		ALL
		DEPENDS ${AssetsPk3})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${AssetsPk3}
		DESTINATION "${JKAInstallDir}/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
endif()
