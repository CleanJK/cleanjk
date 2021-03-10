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
set(MPGameDefines ${MPSharedDefines} "BUILD_GAME" )
set(MPGameGameFiles
	"${MPDir}/game/ai_botlib.cpp"
	"${MPDir}/game/ai_chars.hpp"
	"${MPDir}/game/ai_main.cpp"
	"${MPDir}/game/ai_util.cpp"
	"${MPDir}/game/ai_wpnav.cpp"
	"${MPDir}/game/bg_g2_utils.cpp"
	"${MPDir}/game/bg_misc.cpp"
	"${MPDir}/game/bg_panimate.cpp"
	"${MPDir}/game/bg_pmove.cpp"
	"${MPDir}/game/bg_saber.cpp"
	"${MPDir}/game/bg_saberLoad.cpp"
	"${MPDir}/game/bg_saga.cpp"
	"${MPDir}/game/bg_slidemove.cpp"
	"${MPDir}/game/bg_weapons.cpp"
	"${MPDir}/game/g_active.cpp"
	"${MPDir}/game/g_bot.cpp"
	"${MPDir}/game/g_client.cpp"
	"${MPDir}/game/g_cmds.cpp"
	"${MPDir}/game/g_combat.cpp"
	"${MPDir}/game/g_cvar.cpp"
	"${MPDir}/game/g_engine.cpp"
	"${MPDir}/game/g_exphysics.cpp"
	"${MPDir}/game/g_ICARUScb.cpp"
	"${MPDir}/game/g_items.cpp"
	"${MPDir}/game/g_log.cpp"
	"${MPDir}/game/g_main.cpp"
	"${MPDir}/game/g_mem.cpp"
	"${MPDir}/game/g_misc.cpp"
	"${MPDir}/game/g_missile.cpp"
	"${MPDir}/game/g_mover.cpp"
	"${MPDir}/game/g_nav.cpp"
	"${MPDir}/game/g_navnew.cpp"
	"${MPDir}/game/g_object.cpp"
	"${MPDir}/game/g_saga.cpp"
	"${MPDir}/game/g_session.cpp"
	"${MPDir}/game/g_spawn.cpp"
	"${MPDir}/game/g_svcmds.cpp"
	"${MPDir}/game/g_target.cpp"
	"${MPDir}/game/g_team.cpp"
	"${MPDir}/game/g_timer.cpp"
	"${MPDir}/game/g_trigger.cpp"
	"${MPDir}/game/g_turret.cpp"
	"${MPDir}/game/g_turret_G2.cpp"
	"${MPDir}/game/g_utils.cpp"
	"${MPDir}/game/g_weapon.cpp"
	"${MPDir}/game/tri_coll_test.cpp"
	"${MPDir}/game/w_force.cpp"
	"${MPDir}/game/w_saber.cpp"
	"${MPDir}/game/ai_botlib.hpp"
	"${MPDir}/game/ai_main.hpp"
	"${MPDir}/game/anims.hpp"
	"${MPDir}/game/b_local.hpp"
	"${MPDir}/game/b_public.hpp"
	"${MPDir}/game/bg_local.hpp"
	"${MPDir}/game/bg_public.hpp"
	"${MPDir}/game/bg_weapons.hpp"
	"${MPDir}/game/g_ICARUScb.hpp"
	"${MPDir}/game/g_inventory.hpp"
	"${MPDir}/game/g_local.hpp"
	"${MPDir}/game/g_engine.hpp"
	"${MPDir}/game/g_xcvar.hpp"
	"${MPDir}/game/w_saber.hpp"
	)
source_group("game" FILES ${MPGameGameFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameGameFiles})

set(MPGameCommonFiles
	"${MPDir}/qcommon/q_shared.cpp"
	"${MPDir}/qcommon/disablewarnings.hpp"
	"${MPDir}/qcommon/q_shared.hpp"
	"${MPDir}/qcommon/tags.hpp"

	${SharedCommonFiles}
	)
source_group("common" FILES ${MPGameCommonFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameCommonFiles})


set(MPGameCgameFiles
	"${MPDir}/cgame/animtable.hpp"
	"${MPDir}/cgame/cg_engine.hpp"
	)
source_group("cgame" FILES ${MPGameCgameFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameCgameFiles})

set(MPGameRendererFiles
	"${MPDir}/rd-common/tr_types.hpp"
	)
source_group("rd-common" FILES ${MPGameRendererFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameRendererFiles})

set(MPGameBotlibFiles
	"${MPDir}/botlib/be_ea.hpp"
	"${MPDir}/botlib/botlib.hpp"
	)
source_group("botlib" FILES ${MPGameBotlibFiles})
set(MPGameFiles ${MPGameFiles} ${MPGameBotlibFiles})

set(MPGameG2Files
	"${MPDir}/ghoul2/G2.hpp"
	)
source_group("ghoul2" FILES ${MPGameG2Files})
set(MPGameFiles ${MPGameFiles} ${MPGameG2Files})

set(MPGameIcarusFiles
	"${MPDir}/icarus/interpreter.hpp"
	"${MPDir}/icarus/Q3_Interface.hpp"
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
