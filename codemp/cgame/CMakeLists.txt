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

set(MPCGameIncludeDirectories
	"${MPDir}"
	"${SharedDir}"
	"${GSLIncludeDirectory}"
	)
if(WIN32)
	set(MPCGameLibraries "odbc32" "odbccp32") # what are these even?
endif(WIN32)
set(MPCGameDefines ${MPSharedDefines} "_CGAME" )
set(MPCGameGameFiles
	"${MPDir}/game/bg_g2_utils.cpp"
	"${MPDir}/game/bg_misc.cpp"
	"${MPDir}/game/bg_panimate.cpp"
	"${MPDir}/game/bg_pmove.cpp"
	"${MPDir}/game/bg_saber.cpp"
	"${MPDir}/game/bg_saberLoad.cpp"
	"${MPDir}/game/bg_saga.cpp"
	"${MPDir}/game/bg_slidemove.cpp"
	"${MPDir}/game/bg_weapons.cpp"
	"${MPDir}/game/anims.h"
	"${MPDir}/game/bg_local.h"
	"${MPDir}/game/bg_public.h"
	"${MPDir}/game/bg_weapons.h"
	)
source_group("game" FILES ${MPCGameGameFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameGameFiles})

set(MPCGameCgameFiles
	"${MPDir}/cgame/cg_consolecmds.cpp"
	"${MPDir}/cgame/cg_cvar.cpp"
	"${MPDir}/cgame/cg_draw.cpp"
	"${MPDir}/cgame/cg_drawtools.cpp"
	"${MPDir}/cgame/cg_effects.cpp"
	"${MPDir}/cgame/cg_ents.cpp"
	"${MPDir}/cgame/cg_event.cpp"
	"${MPDir}/cgame/cg_info.cpp"
	"${MPDir}/cgame/cg_light.cpp"
	"${MPDir}/cgame/cg_localents.cpp"
	"${MPDir}/cgame/cg_main.cpp"
	"${MPDir}/cgame/cg_marks.cpp"
	"${MPDir}/cgame/cg_media.cpp"
	"${MPDir}/cgame/cg_newDraw.cpp"
	"${MPDir}/cgame/cg_players.cpp"
	"${MPDir}/cgame/cg_playerstate.cpp"
	"${MPDir}/cgame/cg_predict.cpp"
	"${MPDir}/cgame/cg_scoreboard.cpp"
	"${MPDir}/cgame/cg_servercmds.cpp"
	"${MPDir}/cgame/cg_snapshot.cpp"
	"${MPDir}/cgame/cg_spawn.cpp"
	"${MPDir}/cgame/cg_turret.cpp"
	"${MPDir}/cgame/cg_view.cpp"
	"${MPDir}/cgame/cg_weaponfx.cpp"
	"${MPDir}/cgame/cg_weaponinit.cpp"
	"${MPDir}/cgame/cg_weapons.cpp"
	"${MPDir}/cgame/animtable.h"
	"${MPDir}/cgame/cg_local.h"
	"${MPDir}/cgame/cg_media.h"
	"${MPDir}/cgame/cg_public.h"
	"${MPDir}/cgame/cg_weaponfx.h"
	"${MPDir}/cgame/cg_xcvar.h"
	)
source_group("cgame" FILES ${MPCGameCgameFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameCgameFiles})

set(MPCGameCommonFiles
	"${MPDir}/qcommon/q_shared.cpp"
	"${MPDir}/qcommon/disablewarnings.h"
	"${MPDir}/qcommon/q_shared.h"
	"${MPDir}/qcommon/q_files.h"
	"${MPDir}/qcommon/tags.h"

	${SharedCommonFiles}
	)
source_group("common" FILES ${MPCGameCommonFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameCommonFiles})

set(MPCGameCommonSafeFiles
	${SharedCommonSafeFiles}
	)
source_group("common/safe" FILES ${MPCGameCommonSafeFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameCommonSafeFiles})

set(MPCGameUiFiles
	"${MPDir}/ui/ui_fonts.cpp"
	"${MPDir}/ui/ui_shared.cpp"
	"${MPDir}/ui/menudef.h"
	"${MPDir}/ui/ui_fonts.h"
	"${MPDir}/ui/ui_shared.h"
	)
source_group("ui" FILES ${MPCGameUiFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameUiFiles})

set(MPCGameClientFiles
	"${MPDir}/client/cl_keycodes.h"
	)
source_group("client" FILES ${MPCGameClientFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameClientFiles})

set(MPCGameRendererFiles
	"${MPDir}/rd-common/tr_types.h"
	)
source_group("rd-common" FILES ${MPCGameRendererFiles})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameRendererFiles})

set(MPCGameGFiles
	"${MPDir}/ghoul2/G2.h"
	)
source_group("ghoul2" FILES ${MPCGameG2Files})
set(MPCGameFiles ${MPCGameFiles} ${MPCGameG2Files})

add_library(${MPCGame} SHARED ${MPCGameFiles})

if(NOT MSVC)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${MPCGame} PROPERTIES PREFIX "")
endif()

if(MakeApplicationBundles AND BuildMPEngine)
	install(TARGETS ${MPCGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/${MPEngine}.app/Contents/MacOS/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	install(TARGETS ${MPCGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/${MPEngine}.app/Contents/MacOS/base"
		COMPONENT ${JKAMPCoreComponent})
elseif(WIN32)
	install(TARGETS ${MPCGame}
		RUNTIME
		DESTINATION "${JKAInstallDir}/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	if (WIN64)
		# Don't do this on 32-bit Windows to avoid overwriting
		# vanilla JKA's DLLs
		install(TARGETS ${MPCGame}
			RUNTIME
			DESTINATION "${JKAInstallDir}/base"
			COMPONENT ${JKAMPCoreComponent})
	endif()
else()
	install(TARGETS ${MPCGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	install(TARGETS ${MPCGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/base"
		COMPONENT ${JKAMPCoreComponent})
endif()

set_target_properties(${MPCGame} PROPERTIES COMPILE_DEFINITIONS "${MPCGameDefines}")

# Hide symbols not explicitly marked public.
set_property(TARGET ${MPCGame} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

set_target_properties(${MPCGame} PROPERTIES INCLUDE_DIRECTORIES "${MPCGameIncludeDirectories}")
set_target_properties(${MPCGame} PROPERTIES PROJECT_LABEL "MP Client Game Library")
# no libraries used
if(MPCGameLibraries)
	target_link_libraries(${MPCGame} ${MPCGameLibraries})
endif(MPCGameLibraries)
