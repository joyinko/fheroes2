/***************************************************************************
 *   Free Heroes of Might and Magic II: https://github.com/ihhub/fheroes2  *
 *   Copyright (C) 2019 - 2022                                             *
 *                                                                         *
 *   Free Heroes2 Engine: http://sourceforge.net/projects/fheroes2         *
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "game_hotkeys.h"
#include "localevent.h"
#include "logging.h"
#include "screen.h"
#include "settings.h"
#include "system.h"
#include "tinyconfig.h"
#include "tools.h"

#include <array>
#include <cassert>
#include <fstream>
#include <map>
#include <set>

namespace
{
    enum class HotKeyCategory : uint8_t
    {
        DEFAULT_EVENTS,
        MAIN_GAME,
        WORLD_MAP,
        BATTLE,
        CASTLE,
        MONSTER
    };

    const char * getHotKeyCategoryName( const HotKeyCategory category )
    {
        switch ( category ) {
        case HotKeyCategory::DEFAULT_EVENTS:
            return "Default actions";
        case HotKeyCategory::MAIN_GAME:
            return "Main Menu";
        case HotKeyCategory::WORLD_MAP:
            return "World Map";
        case HotKeyCategory::BATTLE:
            return "Battle";
        case HotKeyCategory::CASTLE:
            return "Castle";
        case HotKeyCategory::MONSTER:
            return "Monster";
        default:
            // Did you add a new category? Add the logic above!
            assert( 0 );
            break;
        }

        return "";
    }

    struct HotKeyEventInfo
    {
        HotKeyEventInfo() = default;

        HotKeyEventInfo( const HotKeyCategory category_, const char * name_, const fheroes2::Key key_ )
            : category( category_ )
            , name( name_ )
            , key( key_ )
        {
            // Do nothing.
        }

        HotKeyEventInfo( const HotKeyEventInfo & ) = default;
        HotKeyEventInfo( HotKeyEventInfo && ) = default;
        ~HotKeyEventInfo() = default;

        HotKeyEventInfo & operator=( const HotKeyEventInfo & ) = default;
        HotKeyEventInfo & operator=( HotKeyEventInfo && ) = default;

        HotKeyCategory category = HotKeyCategory::DEFAULT_EVENTS;

        const char * name = "";

        fheroes2::Key key = fheroes2::Key::NONE;
    };

    std::array<HotKeyEventInfo, Game::NO_EVENT> hotKeyEventInfo;

    void initializeHotKeyEvents()
    {
        // Make sure that event name is unique!
        hotKeyEventInfo[Game::MAIN_MENU_NEW_GAME] = { HotKeyCategory::MAIN_GAME, "new game", fheroes2::Key::KEY_N };
        hotKeyEventInfo[Game::MAIN_MENU_LOAD_GAME] = { HotKeyCategory::MAIN_GAME, "load game", fheroes2::Key::KEY_L };
        hotKeyEventInfo[Game::MAIN_MENU_HIGHSCORES] = { HotKeyCategory::MAIN_GAME, "highscores", fheroes2::Key::KEY_H };
        hotKeyEventInfo[Game::MAIN_MENU_CREDITS] = { HotKeyCategory::MAIN_GAME, "credits", fheroes2::Key::KEY_C };
        hotKeyEventInfo[Game::MAIN_MENU_STANDARD] = { HotKeyCategory::MAIN_GAME, "standard game", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::MAIN_MENU_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "campaign game", fheroes2::Key::KEY_C };
        hotKeyEventInfo[Game::MAIN_MENU_MULTI] = { HotKeyCategory::MAIN_GAME, "multi-player game", fheroes2::Key::KEY_M };
        hotKeyEventInfo[Game::MAIN_MENU_SETTINGS] = { HotKeyCategory::MAIN_GAME, "settings", fheroes2::Key::KEY_T };
        hotKeyEventInfo[Game::MAIN_MENU_SELECT_MAP] = { HotKeyCategory::MAIN_GAME, "select map", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::MAIN_MENU_MAP_SIZE_SMALL] = { HotKeyCategory::MAIN_GAME, "select small map size", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::MAIN_MENU_MAP_SIZE_MEDIUM] = { HotKeyCategory::MAIN_GAME, "select medium map size", fheroes2::Key::KEY_M };
        hotKeyEventInfo[Game::MAIN_MENU_MAP_SIZE_LARGE] = { HotKeyCategory::MAIN_GAME, "select large map size", fheroes2::Key::KEY_L };
        hotKeyEventInfo[Game::MAIN_MENU_MAP_SIZE_EXTRA_LARGE] = { HotKeyCategory::MAIN_GAME, "select extra large map size", fheroes2::Key::KEY_X };
        hotKeyEventInfo[Game::MAIN_MENU_MAP_SIZE_ALL] = { HotKeyCategory::MAIN_GAME, "select all map sizes", fheroes2::Key::KEY_A };
        hotKeyEventInfo[Game::MAIN_MENU_HOTSEAT] = { HotKeyCategory::MAIN_GAME, "hotseat game", fheroes2::Key::KEY_H };
        hotKeyEventInfo[Game::MAIN_MENU_BATTLEONLY] = { HotKeyCategory::MAIN_GAME, "battle only game", fheroes2::Key::KEY_B };
        hotKeyEventInfo[Game::MAIN_MENU_NEW_CAMPAIGN_SELECTION_SUCCESSION_WARS]
            = { HotKeyCategory::MAIN_GAME, "the succession wars campaign selection", fheroes2::Key::KEY_O };
        hotKeyEventInfo[Game::MAIN_MENU_NEW_CAMPAIGN_SELECTION_PRICE_OF_LOYALTY]
            = { HotKeyCategory::MAIN_GAME, "the price of loyalty campaign selection", fheroes2::Key::KEY_E };
        hotKeyEventInfo[Game::NEW_ROLAND_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "roland campaign", fheroes2::Key::KEY_1 };
        hotKeyEventInfo[Game::NEW_ARCHIBALD_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "archibald campaign", fheroes2::Key::KEY_2 };
        hotKeyEventInfo[Game::NEW_PRICE_OF_LOYALTY_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "the price of loyalty campaign", fheroes2::Key::KEY_1 };
        hotKeyEventInfo[Game::NEW_VOYAGE_HOME_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "voyage home campaign", fheroes2::Key::KEY_2 };
        hotKeyEventInfo[Game::NEW_WIZARDS_ISLE_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "wizard's isle campaign", fheroes2::Key::KEY_3 };
        hotKeyEventInfo[Game::NEW_DESCENDANTS_CAMPAIGN] = { HotKeyCategory::MAIN_GAME, "descendants campaign", fheroes2::Key::KEY_4 };

        hotKeyEventInfo[Game::DEFAULT_READY] = { HotKeyCategory::DEFAULT_EVENTS, "default okay event", fheroes2::Key::KEY_RETURN };
        hotKeyEventInfo[Game::DEFAULT_EXIT] = { HotKeyCategory::DEFAULT_EVENTS, "default cancel event", fheroes2::Key::KEY_ESCAPE };
        hotKeyEventInfo[Game::DEFAULT_LEFT] = { HotKeyCategory::DEFAULT_EVENTS, "left selection", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::DEFAULT_RIGHT] = { HotKeyCategory::DEFAULT_EVENTS, "right selection", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::MOVE_LEFT] = { HotKeyCategory::DEFAULT_EVENTS, "move left", fheroes2::Key::KEY_LEFT };
        hotKeyEventInfo[Game::MOVE_RIGHT] = { HotKeyCategory::DEFAULT_EVENTS, "move right", fheroes2::Key::KEY_RIGHT };
        hotKeyEventInfo[Game::MOVE_TOP] = { HotKeyCategory::DEFAULT_EVENTS, "move up", fheroes2::Key::KEY_UP };
        hotKeyEventInfo[Game::MOVE_BOTTOM] = { HotKeyCategory::DEFAULT_EVENTS, "move bottom", fheroes2::Key::KEY_DOWN };
        hotKeyEventInfo[Game::MOVE_TOP_LEFT] = { HotKeyCategory::DEFAULT_EVENTS, "move top bottom", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::MOVE_TOP_RIGHT] = { HotKeyCategory::DEFAULT_EVENTS, "move top right", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::MOVE_BOTTOM_LEFT] = { HotKeyCategory::DEFAULT_EVENTS, "move bottom left", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::MOVE_BOTTOM_RIGHT] = { HotKeyCategory::DEFAULT_EVENTS, "move bottom right", fheroes2::Key::NONE };
        hotKeyEventInfo[Game::SYSTEM_FULLSCREEN] = { HotKeyCategory::DEFAULT_EVENTS, "toggle fullscreen", fheroes2::Key::KEY_F4 };

        hotKeyEventInfo[Game::BATTLE_RETREAT] = { HotKeyCategory::BATTLE, "retreat from battle", fheroes2::Key::KEY_R };
        hotKeyEventInfo[Game::BATTLE_SURRENDER] = { HotKeyCategory::BATTLE, "surrender during battle", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::BATTLE_AUTOSWITCH] = { HotKeyCategory::BATTLE, "toggle battle auto mode", fheroes2::Key::KEY_A };
        hotKeyEventInfo[Game::BATTLE_OPTIONS] = { HotKeyCategory::BATTLE, "battle options", fheroes2::Key::KEY_O };
        hotKeyEventInfo[Game::BATTLE_SKIP] = { HotKeyCategory::BATTLE, "skip turn in battle", fheroes2::Key::KEY_SPACE };
        hotKeyEventInfo[Game::BATTLE_WAIT] = { HotKeyCategory::BATTLE, "wait in battle", fheroes2::Key::KEY_W };

        hotKeyEventInfo[Game::SAVE_GAME] = { HotKeyCategory::WORLD_MAP, "save game", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::NEXT_HERO] = { HotKeyCategory::WORLD_MAP, "next hero", fheroes2::Key::KEY_H };
        hotKeyEventInfo[Game::CONTINUE_HERO_MOVEMENT] = { HotKeyCategory::WORLD_MAP, "continue hero movement", fheroes2::Key::KEY_M };
        hotKeyEventInfo[Game::CAST_SPELL] = { HotKeyCategory::WORLD_MAP, "cast spell", fheroes2::Key::KEY_C };
        hotKeyEventInfo[Game::SLEEP_HERO] = { HotKeyCategory::WORLD_MAP, "put hero to sleep", fheroes2::Key::KEY_Z };
        hotKeyEventInfo[Game::NEXT_TOWN] = { HotKeyCategory::WORLD_MAP, "next town", fheroes2::Key::KEY_T };
        hotKeyEventInfo[Game::END_TURN] = { HotKeyCategory::WORLD_MAP, "end turn", fheroes2::Key::KEY_E };
        hotKeyEventInfo[Game::FILE_OPTIONS] = { HotKeyCategory::WORLD_MAP, "file options", fheroes2::Key::KEY_F };
        hotKeyEventInfo[Game::SYSTEM_OPTIONS] = { HotKeyCategory::WORLD_MAP, "system options", fheroes2::Key::KEY_O };
        hotKeyEventInfo[Game::PUZZLE_MAP] = { HotKeyCategory::WORLD_MAP, "puzzle map", fheroes2::Key::KEY_P };
        hotKeyEventInfo[Game::SCENARIO_INFORMATION] = { HotKeyCategory::WORLD_MAP, "scenario information", fheroes2::Key::KEY_I };
        hotKeyEventInfo[Game::DIG_ARTIFACT] = { HotKeyCategory::WORLD_MAP, "dig for artifact", fheroes2::Key::KEY_D };
        hotKeyEventInfo[Game::KINGDOM_SUMMARY] = { HotKeyCategory::WORLD_MAP, "kingdom summary", fheroes2::Key::KEY_K };
        hotKeyEventInfo[Game::VIEW_WORLD] = { HotKeyCategory::WORLD_MAP, "view world", fheroes2::Key::KEY_V };
        hotKeyEventInfo[Game::DEFAULT_ACTION] = { HotKeyCategory::WORLD_MAP, "default action", fheroes2::Key::KEY_SPACE };
        hotKeyEventInfo[Game::OPEN_FOCUS] = { HotKeyCategory::WORLD_MAP, "open focus", fheroes2::Key::KEY_RETURN };
        hotKeyEventInfo[Game::CONTROL_PANEL] = { HotKeyCategory::WORLD_MAP, "control panel", fheroes2::Key::KEY_1 };
        hotKeyEventInfo[Game::SHOW_RADAR] = { HotKeyCategory::WORLD_MAP, "show radar", fheroes2::Key::KEY_2 };
        hotKeyEventInfo[Game::SHOW_BUTTONS] = { HotKeyCategory::WORLD_MAP, "show game buttons", fheroes2::Key::KEY_3 };
        hotKeyEventInfo[Game::SHOW_STATUS] = { HotKeyCategory::WORLD_MAP, "show status", fheroes2::Key::KEY_4 };
        hotKeyEventInfo[Game::SHOW_ICONS] = { HotKeyCategory::WORLD_MAP, "show icons", fheroes2::Key::KEY_5 };
        hotKeyEventInfo[Game::SCROLL_LEFT] = { HotKeyCategory::WORLD_MAP, "scroll left", fheroes2::Key::KEY_KP_4 };
        hotKeyEventInfo[Game::SCROLL_RIGHT] = { HotKeyCategory::WORLD_MAP, "scroll right", fheroes2::Key::KEY_KP_6 };
        hotKeyEventInfo[Game::SCROLL_UP] = { HotKeyCategory::WORLD_MAP, "scroll up", fheroes2::Key::KEY_KP_8 };
        hotKeyEventInfo[Game::SCROLL_DOWN] = { HotKeyCategory::WORLD_MAP, "scroll down", fheroes2::Key::KEY_KP_2 };

        hotKeyEventInfo[Game::SPLIT_STACK_BY_HALF] = { HotKeyCategory::MONSTER, "split stack by half", fheroes2::Key::KEY_SHIFT };
        hotKeyEventInfo[Game::SPLIT_STACK_BY_ONE] = { HotKeyCategory::MONSTER, "split stack by one", fheroes2::Key::KEY_CONTROL };
        hotKeyEventInfo[Game::JOIN_STACKS] = { HotKeyCategory::MONSTER, "join stacks", fheroes2::Key::KEY_ALT };
        hotKeyEventInfo[Game::UPGRADE_TROOP] = { HotKeyCategory::MONSTER, "upgrade troop", fheroes2::Key::KEY_U };
        hotKeyEventInfo[Game::DISMISS_TROOP] = { HotKeyCategory::MONSTER, "dismiss troop", fheroes2::Key::KEY_D };

        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_1] = { HotKeyCategory::CASTLE, "town dwelling level 1", fheroes2::Key::KEY_1 };
        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_2] = { HotKeyCategory::CASTLE, "town dwelling level 2", fheroes2::Key::KEY_2 };
        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_3] = { HotKeyCategory::CASTLE, "town dwelling level 3", fheroes2::Key::KEY_3 };
        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_4] = { HotKeyCategory::CASTLE, "town dwelling level 4", fheroes2::Key::KEY_4 };
        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_5] = { HotKeyCategory::CASTLE, "town dwelling level 5", fheroes2::Key::KEY_5 };
        hotKeyEventInfo[Game::TOWN_DWELLING_LEVEL_6] = { HotKeyCategory::CASTLE, "town dwelling level 6", fheroes2::Key::KEY_6 };
        hotKeyEventInfo[Game::TOWN_WELL] = { HotKeyCategory::CASTLE, "well", fheroes2::Key::KEY_W };
        hotKeyEventInfo[Game::TOWN_MAGE_GUILD] = { HotKeyCategory::CASTLE, "mage guild", fheroes2::Key::KEY_S };
        hotKeyEventInfo[Game::TOWN_MARKETPLACE] = { HotKeyCategory::CASTLE, "marketplace", fheroes2::Key::KEY_M };
        hotKeyEventInfo[Game::TOWN_THIEVES_GUILD] = { HotKeyCategory::CASTLE, "thieves guild", fheroes2::Key::KEY_T };
        hotKeyEventInfo[Game::TOWN_SHIPYARD] = { HotKeyCategory::CASTLE, "shipyard", fheroes2::Key::KEY_N };
        hotKeyEventInfo[Game::TOWN_TAVERN] = { HotKeyCategory::CASTLE, "tavern", fheroes2::Key::KEY_R };
        // It is also used to build castle in a town.
        hotKeyEventInfo[Game::TOWN_JUMP_TO_BUILD_SELECTION] = { HotKeyCategory::CASTLE, "castle construction", fheroes2::Key::KEY_B };
        hotKeyEventInfo[Game::WELL_BUY_ALL_CREATURES] = { HotKeyCategory::CASTLE, "buy all monsters in well", fheroes2::Key::KEY_M };
    }

    std::string getHotKeyFileContent()
    {
        std::ostringstream os;
        os << "# fheroes2 hotkey file (saved by version " << Settings::GetVersion() << ")" << std::endl;
        os << std::endl;

        HotKeyCategory currentCategory = hotKeyEventInfo[Game::NONE + 1].category;
        os << "# " << getHotKeyCategoryName( currentCategory ) << ':' << std::endl;

#if defined( WITH_DEBUG )
        std::set<const char *> duplicationStringVerifier;
#endif

        for ( int32_t eventId = Game::NONE + 1; eventId < Game::NO_EVENT; ++eventId ) {
            if ( currentCategory != hotKeyEventInfo[eventId].category ) {
                currentCategory = hotKeyEventInfo[eventId].category;
                os << std::endl;
                os << "# " << getHotKeyCategoryName( currentCategory ) << ':' << std::endl;
            }

            assert( strlen( hotKeyEventInfo[eventId].name ) > 0 );
#if defined( WITH_DEBUG )
            const bool isUnique = duplicationStringVerifier.emplace( hotKeyEventInfo[eventId].name ).second;
            assert( isUnique );
#endif

            os << hotKeyEventInfo[eventId].name << " = " << StringUpper( KeySymGetName( hotKeyEventInfo[eventId].key ) ) << std::endl;
        }

        return os.str();
    }
}

bool Game::HotKeyPressEvent( const HotKeyEvent eventID )
{
    const LocalEvent & le = LocalEvent::Get();
    return le.KeyPress() && le.KeyValue() == hotKeyEventInfo[eventID].key;
}

bool Game::HotKeyHoldEvent( const HotKeyEvent eventID )
{
    const LocalEvent & le = LocalEvent::Get();
    return le.KeyHold() && le.KeyValue() == hotKeyEventInfo[eventID].key;
}

std::string Game::getHotKeyNameByEventId( const HotKeyEvent eventID )
{
    return StringUpper( KeySymGetName( hotKeyEventInfo[eventID].key ) );
}

void Game::HotKeysLoad( std::string filename )
{
    initializeHotKeyEvents();

    bool isFilePresent = System::IsFile( filename );
    if ( isFilePresent ) {
        TinyConfig config( '=', '#' );
        isFilePresent = config.Load( filename );

        if ( isFilePresent ) {
            std::map<std::string, fheroes2::Key> nameToKey;
            for ( int32_t i = static_cast<int32_t>( fheroes2::Key::NONE ); i < static_cast<int32_t>( fheroes2::Key::LAST_KEY ); ++i ) {
                const fheroes2::Key key = static_cast<fheroes2::Key>( i );
                nameToKey.emplace( StringUpper( KeySymGetName( key ) ), key );
            }

            for ( int eventId = NONE + 1; eventId < NO_EVENT; ++eventId ) {
                const char * eventName = hotKeyEventInfo[eventId].name;
                std::string value = config.StrParams( eventName );
                if ( value.empty() ) {
                    continue;
                }

                value = StringUpper( value );
                auto foundKey = nameToKey.find( value );
                if ( foundKey == nameToKey.end() ) {
                    continue;
                }

                hotKeyEventInfo[eventId].key = foundKey->second;
                DEBUG_LOG( DBG_GAME, DBG_INFO, "Event '" << hotKeyEventInfo[eventId].name << "' has key '" << value << "'" )
            }
        }
    }

    if ( !isFilePresent ) {
        filename = System::ConcatePath( System::GetConfigDirectory( "fheroes2" ), "fheroes2.key" );
        std::fstream file;
        file.open( filename.data(), std::fstream::out | std::fstream::trunc );
        if ( !file )
            return;

        const std::string & data = getHotKeyFileContent();
        file.write( data.data(), data.size() );
    }
}

void Game::KeyboardGlobalFilter( int sdlKey, int mod )
{
    // system hotkeys
    if ( fheroes2::getKeyFromSDL( sdlKey ) == hotKeyEventInfo[SYSTEM_FULLSCREEN].key && !( ( mod & KMOD_ALT ) || ( mod & KMOD_CTRL ) ) ) {
        fheroes2::engine().toggleFullScreen();
        fheroes2::Display::instance().render();

        Settings & conf = Settings::Get();
        conf.setFullScreen( fheroes2::engine().isFullScreen() );
        conf.Save( Settings::configFileName );
    }
}
