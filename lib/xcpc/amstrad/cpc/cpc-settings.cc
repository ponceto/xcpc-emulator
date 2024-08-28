/*
 * cpc-settings.cc - Copyright (c) 2001-2024 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-priv.h>
#include "cpc-settings.h"

// ---------------------------------------------------------------------------
// <anonymous>::OptionEnum
// ---------------------------------------------------------------------------

namespace {

enum OptionEnum
{
    OPT_COMPANY      =  0,
    OPT_MACHINE      =  1,
    OPT_MONITOR      =  2,
    OPT_REFRESH      =  3,
    OPT_KEYBOARD     =  4,
    OPT_MEMORY       =  5,
    OPT_SYSROM       =  6,
    OPT_ROM000       =  7,
    OPT_ROM001       =  8,
    OPT_ROM002       =  9,
    OPT_ROM003       = 10,
    OPT_ROM004       = 11,
    OPT_ROM005       = 12,
    OPT_ROM006       = 13,
    OPT_ROM007       = 14,
    OPT_ROM008       = 15,
    OPT_ROM009       = 16,
    OPT_ROM010       = 17,
    OPT_ROM011       = 18,
    OPT_ROM012       = 19,
    OPT_ROM013       = 20,
    OPT_ROM014       = 21,
    OPT_ROM015       = 22,
    OPT_DRIVE0       = 23,
    OPT_DRIVE1       = 24,
    OPT_SNAPSHOT     = 25,
    OPT_SPEEDUP      = 26,
    OPT_XSHM         = 27,
    OPT_NO_XSHM      = 28,
    OPT_SCANLINES    = 29,
    OPT_NO_SCANLINES = 30,
    OPT_HELP         = 31,
    OPT_VERSION      = 32,
    OPT_QUIET        = 33,
    OPT_TRACE        = 34,
    OPT_DEBUG        = 35,
};

}

// ---------------------------------------------------------------------------
// <anonymous>::OptionType
// ---------------------------------------------------------------------------

namespace {

struct OptionType
{
    const char* name;
    const char* text;
};

}

// ---------------------------------------------------------------------------
// some useful stuff
// ---------------------------------------------------------------------------

namespace {

const std::string not_set("{not-set}");

}

// ---------------------------------------------------------------------------
// options
// ---------------------------------------------------------------------------

namespace {

const OptionType options[] = {
    { "--company={value}"    , "Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad" },
    { "--machine={value}"    , "cpc464, cpc664, cpc6128"                                       },
    { "--monitor={value}"    , "color, green, gray, ctm640, ctm644, gt64, gt65, cm14, mm12"    },
    { "--refresh={value}"    , "50Hz, 60Hz"                                                    },
    { "--keyboard={value}"   , "english, french, german, spanish, danish"                      },
    { "--memory={value}"     , "64kb, 128kb, 192kb, 256kb, 320kb, 384kb, 448kb, 512kb"         },
    { "--sysrom={filename}"  , "32Kb system rom"                                               },
    { "--rom000={filename}"  , "16Kb expansion rom #00"                                        },
    { "--rom001={filename}"  , "16Kb expansion rom #01"                                        },
    { "--rom002={filename}"  , "16Kb expansion rom #02"                                        },
    { "--rom003={filename}"  , "16Kb expansion rom #03"                                        },
    { "--rom004={filename}"  , "16Kb expansion rom #04"                                        },
    { "--rom005={filename}"  , "16Kb expansion rom #05"                                        },
    { "--rom006={filename}"  , "16Kb expansion rom #07"                                        },
    { "--rom007={filename}"  , "16Kb expansion rom #08"                                        },
    { "--rom008={filename}"  , "16Kb expansion rom #09"                                        },
    { "--rom009={filename}"  , "16Kb expansion rom #10"                                        },
    { "--rom010={filename}"  , "16Kb expansion rom #11"                                        },
    { "--rom011={filename}"  , "16Kb expansion rom #12"                                        },
    { "--rom012={filename}"  , "16Kb expansion rom #13"                                        },
    { "--rom013={filename}"  , "16Kb expansion rom #14"                                        },
    { "--rom014={filename}"  , "16Kb expansion rom #15"                                        },
    { "--rom015={filename}"  , "16Kb expansion rom #16"                                        },
    { "--drive0={filename}"  , "drive0 disk image"                                             },
    { "--drive1={filename}"  , "drive1 disk image"                                             },
    { "--snapshot={filename}", "initial snapshot"                                              },
    { "--speedup={factor}"   , "speeds up emulation by an integer factor"                      },
    { "--xshm"               , "use the XShm extension"                                        },
    { "--no-xshm"            , "don't use the XShm extension"                                  },
    { "--scanlines"          , "simulate crt scanlines"                                        },
    { "--no-scanlines"       , "don't simulate crt scanlines"                                  },
    { "--help"               , "display this help and exit"                                    },
    { "--version"            , "display the version and exit"                                  },
    { "--quiet"              , "set the loglevel to quiet mode"                                },
    { "--trace"              , "set the loglevel to trace mode"                                },
    { "--debug"              , "set the loglevel to debug mode"                                },
};

}

// ---------------------------------------------------------------------------
// cpc::Settings
// ---------------------------------------------------------------------------

namespace cpc {

Settings::Settings()
    : xcpc::Settings()
    , opt_program(not_set)
    , opt_company("default")
    , opt_machine("default")
    , opt_monitor("default")
    , opt_refresh("default")
    , opt_keyboard("default")
    , opt_memory("default")
    , opt_sysrom(not_set)
    , opt_rom000(not_set)
    , opt_rom001(not_set)
    , opt_rom002(not_set)
    , opt_rom003(not_set)
    , opt_rom004(not_set)
    , opt_rom005(not_set)
    , opt_rom006(not_set)
    , opt_rom007(not_set)
    , opt_rom008(not_set)
    , opt_rom009(not_set)
    , opt_rom010(not_set)
    , opt_rom011(not_set)
    , opt_rom012(not_set)
    , opt_rom013(not_set)
    , opt_rom014(not_set)
    , opt_rom015(not_set)
    , opt_drive0(not_set)
    , opt_drive1(not_set)
    , opt_snapshot(not_set)
    , opt_xshm(true)
    , opt_scanlines(true)
    , opt_help(false)
    , opt_version(false)
    , opt_loglevel(Utils::get_loglevel())
{
}

Settings::Settings(int& argc, char**& argv)
    : Settings()
{
    parse(argc, argv);
}

void Settings::parse(int& argc, char**& argv)
{
    auto check_option = [&](const char* expected, const char* argument) -> bool
    {
        const char* equals = ::strchr(expected, '=');
        int         result = 0;

        if(equals != nullptr) {
            result = ::strncmp(expected, argument, ((equals - expected) + 1));
        }
        else {
            result = ::strcmp(expected, argument);
        }
        return result == 0 ? true : false;
    };

    auto value_of = [&](const char* string) -> const char*
    {
        const char* equals = ::strchr(string, '=');

        if(equals != nullptr) {
            string = equals + 1;
        }
        else {
            string = not_set.c_str();
        }
        return string;
    };

    auto is_option = [&](const int index, const char* argument) -> bool
    {
        return check_option(options[index].name, argument);
    };

    auto do_loglevel = [&]() -> void
    {
        const int old_loglevel = Utils::get_loglevel();

        if(opt_loglevel != old_loglevel) {
            opt_loglevel = Utils::set_loglevel(opt_loglevel);
        }
    };

    auto do_dump_all = [&]() -> void
    {
        ::xcpc_log_debug("xcpc.settings.program   = %s", opt_program.c_str() );
        ::xcpc_log_debug("xcpc.settings.company   = %s", opt_company.c_str() );
        ::xcpc_log_debug("xcpc.settings.machine   = %s", opt_machine.c_str() );
        ::xcpc_log_debug("xcpc.settings.monitor   = %s", opt_monitor.c_str() );
        ::xcpc_log_debug("xcpc.settings.refresh   = %s", opt_refresh.c_str() );
        ::xcpc_log_debug("xcpc.settings.keyboard  = %s", opt_keyboard.c_str());
        ::xcpc_log_debug("xcpc.settings.memory    = %s", opt_memory.c_str()  );
        ::xcpc_log_debug("xcpc.settings.sysrom    = %s", opt_sysrom.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom000    = %s", opt_rom000.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom001    = %s", opt_rom001.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom002    = %s", opt_rom002.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom003    = %s", opt_rom003.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom004    = %s", opt_rom004.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom005    = %s", opt_rom005.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom006    = %s", opt_rom006.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom007    = %s", opt_rom007.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom008    = %s", opt_rom008.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom009    = %s", opt_rom009.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom010    = %s", opt_rom010.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom011    = %s", opt_rom011.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom012    = %s", opt_rom012.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom013    = %s", opt_rom013.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom014    = %s", opt_rom014.c_str()  );
        ::xcpc_log_debug("xcpc.settings.rom015    = %s", opt_rom015.c_str()  );
        ::xcpc_log_debug("xcpc.settings.drive0    = %s", opt_drive0.c_str()  );
        ::xcpc_log_debug("xcpc.settings.drive1    = %s", opt_drive1.c_str()  );
        ::xcpc_log_debug("xcpc.settings.snapshot  = %s", opt_snapshot.c_str());
        ::xcpc_log_debug("xcpc.settings.speedup   = %s", opt_speedup.c_str() );
        ::xcpc_log_debug("xcpc.settings.xshm      = %d", opt_xshm            );
        ::xcpc_log_debug("xcpc.settings.scanlines = %d", opt_scanlines       );
        ::xcpc_log_debug("xcpc.settings.help      = %d", opt_help            );
        ::xcpc_log_debug("xcpc.settings.version   = %d", opt_version         );
        ::xcpc_log_debug("xcpc.settings.loglevel  = %d", opt_loglevel        );
    };

    auto do_parse = [&]() -> void
    {
        const std::vector<char*> arguments(argv, argv + argc);

        int index = 0;
        for(auto argument : arguments) {
            argv[index] = nullptr;
            if(index == 0) {
                opt_program = argument;
                argc = 0;
                argv[argc++] = argument;
            }
            else if(is_option(OPT_COMPANY     , argument)) { opt_company   = value_of(argument);  }
            else if(is_option(OPT_MACHINE     , argument)) { opt_machine   = value_of(argument);  }
            else if(is_option(OPT_MONITOR     , argument)) { opt_monitor   = value_of(argument);  }
            else if(is_option(OPT_REFRESH     , argument)) { opt_refresh   = value_of(argument);  }
            else if(is_option(OPT_KEYBOARD    , argument)) { opt_keyboard  = value_of(argument);  }
            else if(is_option(OPT_MEMORY      , argument)) { opt_memory    = value_of(argument);  }
            else if(is_option(OPT_SYSROM      , argument)) { opt_sysrom    = value_of(argument);  }
            else if(is_option(OPT_ROM000      , argument)) { opt_rom000    = value_of(argument);  }
            else if(is_option(OPT_ROM001      , argument)) { opt_rom001    = value_of(argument);  }
            else if(is_option(OPT_ROM002      , argument)) { opt_rom002    = value_of(argument);  }
            else if(is_option(OPT_ROM003      , argument)) { opt_rom003    = value_of(argument);  }
            else if(is_option(OPT_ROM004      , argument)) { opt_rom004    = value_of(argument);  }
            else if(is_option(OPT_ROM005      , argument)) { opt_rom005    = value_of(argument);  }
            else if(is_option(OPT_ROM006      , argument)) { opt_rom006    = value_of(argument);  }
            else if(is_option(OPT_ROM007      , argument)) { opt_rom007    = value_of(argument);  }
            else if(is_option(OPT_ROM008      , argument)) { opt_rom008    = value_of(argument);  }
            else if(is_option(OPT_ROM009      , argument)) { opt_rom009    = value_of(argument);  }
            else if(is_option(OPT_ROM010      , argument)) { opt_rom010    = value_of(argument);  }
            else if(is_option(OPT_ROM011      , argument)) { opt_rom011    = value_of(argument);  }
            else if(is_option(OPT_ROM012      , argument)) { opt_rom012    = value_of(argument);  }
            else if(is_option(OPT_ROM013      , argument)) { opt_rom013    = value_of(argument);  }
            else if(is_option(OPT_ROM014      , argument)) { opt_rom014    = value_of(argument);  }
            else if(is_option(OPT_ROM015      , argument)) { opt_rom015    = value_of(argument);  }
            else if(is_option(OPT_DRIVE0      , argument)) { opt_drive0    = value_of(argument);  }
            else if(is_option(OPT_DRIVE1      , argument)) { opt_drive1    = value_of(argument);  }
            else if(is_option(OPT_SNAPSHOT    , argument)) { opt_snapshot  = value_of(argument);  }
            else if(is_option(OPT_SPEEDUP     , argument)) { opt_speedup   = value_of(argument);  }
            else if(is_option(OPT_XSHM        , argument)) { opt_xshm      = true;                }
            else if(is_option(OPT_NO_XSHM     , argument)) { opt_xshm      = false;               }
            else if(is_option(OPT_SCANLINES   , argument)) { opt_scanlines = true;                }
            else if(is_option(OPT_NO_SCANLINES, argument)) { opt_scanlines = false;               }
            else if(is_option(OPT_HELP        , argument)) { opt_help      = true;                }
            else if(is_option(OPT_VERSION     , argument)) { opt_version   = true;                }
            else if(is_option(OPT_QUIET       , argument)) { opt_loglevel  = XCPC_LOGLEVEL_QUIET; }
            else if(is_option(OPT_TRACE       , argument)) { opt_loglevel  = XCPC_LOGLEVEL_TRACE; }
            else if(is_option(OPT_DEBUG       , argument)) { opt_loglevel  = XCPC_LOGLEVEL_DEBUG; }
            else {
                argv[argc++] = argument;
            }
            ++index;
        }
        do_loglevel();
        do_dump_all();
        if(opt_help != false) {
            usage();
            return;
        }
        if(opt_version != false) {
            version();
            return;
        }
    };

    return do_parse();
}

void Settings::usage()
{
    auto basename_of = [&](const char* string) -> const char*
    {
        const char* slash = ::strrchr(string, '/');

        if(slash != nullptr) {
            string = (slash + 1);
        }
        return string;
    };

    auto print_cmd = [&](const char* string) -> void
    {
        ::xcpc_println("Usage: %s [toolkit-options] [xcpc-options] [files...]", basename_of(string));
    };

    auto print_str = [&](const char* string) -> void
    {
        ::xcpc_println("%s", string);
    };

    auto print_opt = [&](const int index) -> void
    {
        ::xcpc_println("    %-24s    %s", options[index].name, options[index].text);
    };

    print_cmd(opt_program.c_str() );
    print_str(""                  );
    print_str("Help options:"     );
    print_opt(OPT_HELP            );
    print_opt(OPT_VERSION         );
    print_str(""                  );
    print_str("Emulation options:");
    print_opt(OPT_COMPANY         );
    print_opt(OPT_MACHINE         );
    print_opt(OPT_MONITOR         );
    print_opt(OPT_REFRESH         );
    print_opt(OPT_KEYBOARD        );
    print_opt(OPT_MEMORY          );
    print_opt(OPT_SYSROM          );
    print_opt(OPT_ROM000          );
    print_opt(OPT_ROM001          );
    print_opt(OPT_ROM002          );
    print_opt(OPT_ROM003          );
    print_opt(OPT_ROM004          );
    print_opt(OPT_ROM005          );
    print_opt(OPT_ROM006          );
    print_opt(OPT_ROM007          );
    print_opt(OPT_ROM008          );
    print_opt(OPT_ROM009          );
    print_opt(OPT_ROM010          );
    print_opt(OPT_ROM011          );
    print_opt(OPT_ROM012          );
    print_opt(OPT_ROM013          );
    print_opt(OPT_ROM014          );
    print_opt(OPT_ROM015          );
    print_opt(OPT_DRIVE0          );
    print_opt(OPT_DRIVE1          );
    print_opt(OPT_SNAPSHOT        );
    print_str(""                  );
    print_str("Misc. options:"    );
    print_opt(OPT_SPEEDUP         );
    print_opt(OPT_XSHM            );
    print_opt(OPT_NO_XSHM         );
    print_opt(OPT_SCANLINES       );
    print_opt(OPT_NO_SCANLINES    );
    print_str(""                  );
    print_str("Debug options:"    );
    print_opt(OPT_QUIET           );
    print_opt(OPT_TRACE           );
    print_opt(OPT_DEBUG           );
    print_str(""                  );
}

void Settings::version()
{
    const int major = Utils::get_major_version();
    const int minor = Utils::get_minor_version();
    const int micro = Utils::get_micro_version();

    ::xcpc_println("Xcpc - Amstrad CPC emulator - v%d.%d.%d", major, minor, micro);
}

bool Settings::quit()
{
    if((opt_help != false) || (opt_version != false)) {
        return true;
    }
    return false;
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
