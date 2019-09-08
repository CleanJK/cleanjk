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

set(MPGameIncludeDirectories
	"${MPDir}"
	"${SharedDir}"
	"${GSLIncludeDirectory}"
	)
if(WIN32)
	set(MPGameLibraries "winmm")
endif(WIN32)
set(MPGameDefines ${MPSharedDefines} "_GAME" )
set(MPGameGameFiles
	"${MPDir}/game/ai_main.c"
	"${MPDir}/game/ai_util.c"
	"${MPDir}/game/ai_wpnav.c"
	"${MPDir}/game/bg_g2_utils.c"
	"${MPDir}/game/bg_misc.c"
	"${MPDir}/game/bg_panimate.c"
	"${MPDir}/game/bg_pmove.c"
	"${MPDir}/game/bg_saber.c"
	"${MPDir}/game/bg_saberLoad.c"
	"${MPDir}/game/bg_saga.c"
	"${MPDir}/game/bg_slidemove.c"
	"${MPDir}/game/bg_weapons.c"
	"${MPDir}/game/g_active.c"
	"${MPDir}/game/g_bot.c"
	"${MPDir}/game/g_client.c"
	"${MPDir}/game/g_cmds.c"
	"${MPDir}/game/g_combat.c"
	"${MPDir}/game/g_cvar.c"
	"${MPDir}/game/g_exphysics.c"
	"${MPDir}/game/g_ICARUScb.c"
	"${MPDir}/game/g_items.c"
	"${MPDir}/game/g_log.c"
	"${MPDir}/game/g_main.c"
	"${MPDir}/game/g_mem.c"
	"${MPDir}/game/g_misc.c"
	"${MPDir}/game/g_missile.c"
	"${MPDir}/game/g_mover.c"
	"${MPDir}/game/g_nav.c"
	"${MPDir}/game/g_navnew.c"
	"${MPDir}/game/g_object.c"
	"${MPDir}/game/g_saga.c"
	"${MPDir}/game/g_session.c"
	"${MPDir}/game/g_spawn.c"
	"${MPDir}/game/g_svcmds.c"
	"${MPDir}/game/g_target.c"
	"${MPDir}/game/g_team.c"
	"${MPDir}/game/g_timer.c"
	"${MPDir}/game/g_trigger.c"
	"${MPDir}/game/g_turret.c"
	"${MPDir}/game/g_turret_G2.c"
	"${MPDir}/game/g_utils.c"
	"${MPDir}/game/g_weapon.c"
	"${MPDir}/game/tri_coll_test.c"
	"${MPDir}/game/w_force.c"
	"${MPDir}/game/w_saber.c"
	"${MPDir}/game/ai_main.h"
	"${MPDir}/game/anims.h"
	"${MPDir}/game/b_local.h"
	"${MPDir}/game/b_public.h"
	"${MPDir}/game/bg_local.h"
	"${MPDir}/game/bg_public.h"
	"${MPDir}/game/bg_saga.h"
	"${MPDir}/game/bg_weapons.h"
	"${MPDir}/game/chars.h"
	"${MPDir}/game/g_ICARUScb.h"
	"${MPDir}/game/g_local.h"
	"${MPDir}/game/g_public.h"
	"${MPDir}/game/g_xcvar.h"
	"${MPDir}/game/inv.h"
	"${MPDir}/game/match.h"
	"${MPDir}/game/say.h"
	"${MPDir}/game/surfaceflags.h"
	"${MPDir}/game/w_saber.h"
	)
source_group("game" FILES ${MPGameGameFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameGameFiles})

set(MPGameCommonFiles
	"${MPDir}/qcommon/q_shared.c"
	"${MPDir}/qcommon/disablewarnings.h"
	"${MPDir}/qcommon/q_shared.h"
	"${MPDir}/qcommon/tags.h"

	${SharedCommonFiles}
	)
source_group("common" FILES ${MPGameCommonFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameCommonFiles})

set(MPGameCommonSafeFiles
	${SharedCommonSafeFiles}
	)
source_group("common/safe" FILES ${MPGameCommonSafeFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameCommonSafeFiles})

set(MPGameCgameFiles
	"${MPDir}/cgame/animtable.h"
	"${MPDir}/cgame/cg_public.h"
	)
source_group("cgame" FILES ${MPGameCgameFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameCgameFiles})

set(MPGameRendererFiles
	"${MPDir}/rd-common/tr_types.h"
	)
source_group("rd-common" FILES ${MPGameRendererFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameRendererFiles})

set(MPGameBotlibFiles
	"${MPDir}/botlib/be_ea.h"
	"${MPDir}/botlib/botlib.h"
	)
source_group("botlib" FILES ${MPGameBotlibFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameBotlibFiles})

set(MPGameG2Files
	"${MPDir}/ghoul2/G2.h"
	)
source_group("ghoul2" FILES ${MPGameG2Files})
set(MPGameFiles ${MPGameFiles} ${MPGameG2Files})

set(MPGameIcarusFiles
	"${MPDir}/icarus/interpreter.h"
	"${MPDir}/icarus/Q3_Interface.h"
	)
source_group("icarus" FILES ${MPGameIcarusFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameIcarusFiles})

set(MPGameUiFiles
	"${MPDir}/ui/menudef.h"
	)
source_group("ui" FILES ${MPGameUiFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameUiFiles})

add_library(${MPGame} SHARED ${MPGameFiles})

if(NOT MSVC)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${MPGame} PROPERTIES PREFIX "")
endif()

if(MakeApplicationBundles AND BuildMPEngine)
	install(TARGETS ${MPGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/${MPEngine}.app/Contents/MacOS/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	install(TARGETS ${MPGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/${MPEngine}.app/Contents/MacOS/base"
		COMPONENT ${JKAMPCoreComponent})
elseif(WIN32)
	install(TARGETS ${MPGame}
		RUNTIME
		DESTINATION "${JKAInstallDir}/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	if (WIN64)
		# Don't do this on 32-bit Windows to avoid overwriting
		# vanilla JKA's DLLs
		install(TARGETS ${MPGame}
			RUNTIME
			DESTINATION "${JKAInstallDir}/base"
			COMPONENT ${JKAMPCoreComponent})
	endif()
else()
	install(TARGETS ${MPGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/OpenJK"
		COMPONENT ${JKAMPCoreComponent})
	install(TARGETS ${MPGame}
		LIBRARY
		DESTINATION "${JKAInstallDir}/base"
		COMPONENT ${JKAMPCoreComponent})
endif()

set_target_properties(${MPGame} PROPERTIES COMPILE_DEFINITIONS "${MPGameDefines}")

# Hide symbols not explicitly marked public.
set_property(TARGET ${MPGame} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

set_target_properties(${MPGame} PROPERTIES INCLUDE_DIRECTORIES "${MPGameIncludeDirectories}")
set_target_properties(${MPGame} PROPERTIES PROJECT_LABEL "MP Game Library")
# no libraries used
if(MPGameLibraries)
	target_link_libraries(${MPGame} ${MPGameLibraries})
endif(MPGameLibraries)
