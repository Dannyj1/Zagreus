//-----------------------------------------------------------------------------
// Copyright (c) 2015-2019 Shawn Chidester <zd3nik@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "UCIAdapter.h"
#include "Output.h"

namespace senjo {

//-----------------------------------------------------------------------------
namespace Token {
static const std::string DEBUG("debug");
static const std::string EXIT("exit");
static const std::string FEN("fen");
static const std::string GO("go");
static const std::string HELP("help");
static const std::string IS_READY("isready");
static const std::string MOVES("moves");
static const std::string NAME("name");
static const std::string NEW("new");
static const std::string OPTS("opts");
static const std::string PERFT("perft");
static const std::string PONDER_HIT("ponderhit");
static const std::string POSITION("position");
static const std::string PRINT("print");
static const std::string QUIT("quit");
static const std::string REGISTER("register");
static const std::string SET_OPTION("setoption");
static const std::string START_POS("startpos");
static const std::string STOP("stop");
static const std::string TEST("test");
static const std::string UCI("uci");
static const std::string UCI_NEW_GAME("ucinewgame");
static const std::string VALUE("value");
}  // namespace Token

//-----------------------------------------------------------------------------
    UCIAdapter::UCIAdapter(ChessEngine &chessEngine)
            : engine(chessEngine) {}

//-----------------------------------------------------------------------------
    UCIAdapter::~UCIAdapter() {
        if (lastCommand) {
            lastCommand->waitForFinish();
        }
    }

//-----------------------------------------------------------------------------
    bool UCIAdapter::doCommand(const std::string &line) {
        Parameters params(line);
        if (params.empty()) {
            return true; // ignore empty lines
        }

        if (engine.isDebugOn()) {
            Output() << "received command: " << line;
        }

        std::string command = params.popString();
        if (iEqual(Token::GO, command)) {
          doStopCommand();
          execute(
              std::shared_ptr<BackgroundCommand>(new GoCommandHandle(engine)),
              params);
        } else if (iEqual(Token::POSITION, command)) {
          doStopCommand();
          doPositionCommand(line, params);
        } else if (iEqual(Token::STOP, command)) {
          doStopCommand(params);
        } else if (iEqual(Token::SET_OPTION, command)) {
          doSetOptionCommand(params);
        } else if (iEqual(Token::IS_READY, command)) {
          doIsReadyCommand(params);
        } else if (iEqual(Token::UCI, command)) {
          doUCICommand(params);
        } else if (iEqual(Token::UCI_NEW_GAME, command)) {
          doStopCommand();
          doUCINewGameCommand(params);
        } else if (iEqual(Token::NEW, command)) {
          doStopCommand();
          doNewCommand(params);
        } else if (iEqual(Token::DEBUG, command)) {
          doDebugCommand(params);
        } else if (iEqual(Token::REGISTER, command)) {
          doStopCommand(params);
          execute(std::shared_ptr<BackgroundCommand>(
                      new RegisterCommandHandle(engine)),
                  params);
        } else if (iEqual(Token::PONDER_HIT, command)) {
          doPonderHitCommand(params);
        } else if (iEqual(Token::FEN, command)) {
          doFENCommand(params);
        } else if (iEqual(Token::PRINT, command)) {
          doPrintCommand(params);
        } else if (iEqual(Token::PERFT, command)) {
          doStopCommand();
          execute(std::shared_ptr<BackgroundCommand>(
                      new PerftCommandHandle(engine)),
                  params);
        } else if (iEqual(Token::OPTS, command)) {
          doOptsCommand(params);
        } else if (iEqual(Token::HELP, command)) {
          doHelpCommand(params);
        } else if (iEqual(Token::EXIT, command) ||
                   iEqual(Token::QUIT, command)) {
          if (doQuitCommand(params)) {
            return false;
          }
        } else if (isMove(command)) {
          doStopCommand();
          params.push_front(command);
          doMoveCommand(params);
        } else {
          Output() << "Unknown command: '" << command << "'";
          Output() << "Enter 'help' for a list of commands";
        }
        return true;
    }

//-----------------------------------------------------------------------------
//! \brief Output list of available commands (not a UCI command)
//-----------------------------------------------------------------------------
    void UCIAdapter::doHelpCommand(Parameters & /*params*/) {
        Output() << engine.getEngineName() << ' ' << engine.getEngineVersion()
                 << " by " << engine.getAuthorName();
        Output() << "UCI commands:";
        Output() << "  " << Token::DEBUG;
        Output() << "  " << Token::GO;
        Output() << "  " << Token::IS_READY;
        Output() << "  " << Token::POSITION;
        Output() << "  " << Token::QUIT;
        Output() << "  " << Token::SET_OPTION;
        Output() << "  " << Token::STOP;
        Output() << "  " << Token::UCI;
        Output() << "  " << Token::UCI_NEW_GAME;
        Output() << "Additional commands:";
        Output() << "  " << Token::EXIT;
        Output() << "  " << Token::FEN;
        Output() << "  " << Token::HELP;
        Output() << "  " << Token::NEW;
        Output() << "  " << Token::PERFT;
        Output() << "  " << Token::PRINT;
        Output() << "Also try '<command> help' for help on a specific command";
        Output() << "Or enter move(s) in coordinate notation, e.g. d2d4 g8f6";
    }

//-----------------------------------------------------------------------------
//! \brief Execute the given background command thread
//-----------------------------------------------------------------------------
    void UCIAdapter::execute(std::shared_ptr<BackgroundCommand> command,
                             Parameters &params) {
        if (!command) {
            return;
        }

        if (params.firstParamIs(Token::HELP)) {
          Output() << "usage: " << command->usage();
          Output() << command->description();
          return;
        }

        if (lastCommand) {
            lastCommand->stop();
            lastCommand->waitForFinish();
        }

        if (command->parseAndExecute(params)) {
            lastCommand.swap(command);
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the "fen" command (not a UCI command)
//! Print the FEN string for the current board position
//-----------------------------------------------------------------------------
    void UCIAdapter::doFENCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::FEN;
        Output() << "Output FEN string of the current position.";
        return;
      }

        if (lastCommand) {
            lastCommand->stop();
            lastCommand->waitForFinish();
        }

        if (!engine.isInitialized()) {
            engine.initialize();
        }

        Output() << engine.getFEN();
    }

//-----------------------------------------------------------------------------
//! \brief Do the "print" command (not a UCI command)
//! Output an ascii representation of the current board position
//-----------------------------------------------------------------------------
    void UCIAdapter::doPrintCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::PRINT;
        Output() << "Output text representation of the current position.";
        return;
      }

        if (!engine.isInitialized()) {
            engine.initialize();
        }

        engine.printBoard();
    }

//-----------------------------------------------------------------------------
//! \brief Do the "new" command (not a UCI command)
//! Clear search data, set position, and apply moves (if any given).
//-----------------------------------------------------------------------------
    void UCIAdapter::doNewCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::NEW << " [" << Token::START_POS << "|"
                 << Token::FEN << " <fen_string>] [" << Token::MOVES
                 << "] <movelist>";
        Output() << "Clear search data, set position, and apply <movelist>.";
        Output() << "If no position is specified " << Token::START_POS
                 << " is assumed.";
        return;
      }

        doUCINewGameCommand();

        if (params.empty() || params.popParam(Token::START_POS) ||
            params.popParam(Token::MOVES)) {
          if (!engine.setPosition(ChessEngine::STARTPOS)) {
            return;
          }
        } else {
          // consume "fen" token if present
          params.popParam(Token::FEN);
          std::string remain;
          if (!engine.setPosition(params.toString(), &remain)) {
            return;
          }
          params.parse(remain);
        }

        // consume "moves" token if present
        params.popParam(Token::MOVES);

        // apply moves (if any)
        while (!params.empty() && isMove(params.front())) {
          std::string move = params.popString();
          if (!engine.makeMove(move)) {
            Output() << "Invalid move: " << move;
            break;
          }
        }

        if (engine.isDebugOn()) {
            engine.printBoard();
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the "opts" command (not a UCI command)
//! Output current engine option values
//-----------------------------------------------------------------------------
    void UCIAdapter::doOptsCommand(Parameters & /*params*/) {
      for (const auto &opt : engine.getOptions()) {
        switch (opt.getType()) {
          case EngineOption::Checkbox:
          case EngineOption::Spin:
          case EngineOption::String:
            Output() << opt.getTypeName() << ':' << opt.getName() << ' '
                     << opt.getValue();
            break;
          case EngineOption::ComboBox: {
            const std::set<std::string> &values = opt.getComboValues();
            Output out;
            out << opt.getTypeName() << ':' << opt.getName();
            for (const auto &value : values) {
              out << ' ' << value;
            }
            break;
          }
          default:
            break;
        }
      }
    }

//-----------------------------------------------------------------------------
//! \brief Execute the given move(s) on the current position
//-----------------------------------------------------------------------------
    void UCIAdapter::doMoveCommand(Parameters &params) {
        if (!engine.isInitialized()) {
            engine.initialize();
        }

        if (lastCommand) {
            lastCommand->stop();
            lastCommand->waitForFinish();
        }

        lastPosition.clear();
        while (!params.empty()) {
          std::string move = params.popString();
          if (!isMove(move) || !engine.makeMove(move)) {
            Output() << "Invalid move: " << move;
            return;
          }

          if (engine.isDebugOn()) {
            engine.printBoard();
          }
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "quit" command
//! UCI specification:
//!   Quit the program as soon as possible.
//! \return true if quit requested, otherwise false
//-----------------------------------------------------------------------------
    bool UCIAdapter::doQuitCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::QUIT;
        Output() << "Stop engine and terminate program.";
        return false;
      }

        engine.stopSearching();

        if (lastCommand) {
            lastCommand->stop();
            lastCommand->waitForFinish();
        }

        return true;
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "debug" command
//! UCI specification:
//!   Switch the debug mode of the engine on and off.  In debug mode the engine
//!   should send additional infos to the GUI, e.g. with the "info string"
//!   command, to help debugging, e.g. the commands that the engine has
//!   received etc.  This mode should be switched off by default and this
//!   command can be sent any time, also when the engine is thinking.
//-----------------------------------------------------------------------------
    void UCIAdapter::doDebugCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::DEBUG;
        Output() << "Toggle debug mode.";
        return;
      }

        engine.setDebug(!engine.isDebugOn());
        Output() << "debug " << (engine.isDebugOn() ? "on" : "off");
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "isready" command
//! UCI specification:
//!   This is used to synchronize the engine with the GUI.  When the GUI has
//!   sent a command or multiple commands that can take some time to complete,
//!   this command can be used to wait for the engine to be ready again or
//!   to ping the engine to find out if it is still alive.  E.g. this should be
//!   sent after setting the path to the tablebases as this can take some time.
//!   This command is also required once before the engine is asked to do any
//!   search to wait for the engine to finish initializing.  This command must
//!   always be answered with "readyok" and can be sent also when the engine
//!   is calculating in which case the engine should also immediately answer
//!   with "readyok" without stopping the search.
//-----------------------------------------------------------------------------
    void UCIAdapter::doIsReadyCommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::IS_READY;
        Output() << "Output readyok when engine is ready to receive input.";
        return;
      }

        if (!engine.isInitialized()) {
            engine.initialize();
        }

        if (lastCommand) {
            // don't set stop flag
            lastCommand->waitForFinish();
        }

        Output(Output::NoPrefix) << "readyok";
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "stop" command
//! UCI specification:
//!   Stop calculating as soon as possible, don't forget the "bestmove" and
//!   possibly the "ponder" token when finishing the search.
//-----------------------------------------------------------------------------
    void UCIAdapter::doStopCommand(const Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::STOP;
        Output() << "Stop engine if it is calculating.";
        return;
      }

      engine.stopSearching();
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "uci" command
//! UCI specification:
//!   Tell engine to use the uci (universal chess interface), this will be sent
//!   once as a first command after program boot to tell the engine to switch
//!   to uci mode.  After receiving the uci command the engine must identify
//!   itself with the "id" command and send the "option" commands to tell the
//!   GUI which engine settings the engine supports if any.  After that the
//!   engine should send "uciok" to acknowledge the uci mode.  If no uciok is
//!   sent within a certain time period, the engine task will be killed by the
//!   GUI.
//-----------------------------------------------------------------------------
    void UCIAdapter::doUCICommand(Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::UCI;
        Output() << "Output engine info and options followed by uciok.";
        return;
      }

        Output(Output::NoPrefix) << "id name " << engine.getEngineName()
                                 << ' ' << engine.getEngineVersion();

        const std::string author = engine.getAuthorName();
        if (!author.empty()) {
          Output(Output::NoPrefix) << "id author " << author;
        }

        const std::string email = engine.getEmailAddress();
        if (!email.empty()) {
          Output(Output::NoPrefix) << "id email " << email;
        }

        const std::string country = engine.getCountryName();
        if (!country.empty()) {
          Output(Output::NoPrefix) << "id country " << country;
        }

        for (const auto &opt : engine.getOptions()) {
          Output out(Output::NoPrefix);
          out << "option name " << opt.getName() << " type "
              << opt.getTypeName();
          if (!opt.getDefaultValue().empty()) {
            out << " default " << opt.getDefaultValue();
          }
          if (opt.getMinValue() > INT64_MIN) {
            out << " min " << opt.getMinValue();
          }
          if (opt.getMaxValue() < INT64_MAX) {
            out << " max " << opt.getMaxValue();
          }
          for (const auto &val : opt.getComboValues()) {
            out << " var " << val;
          }
        }

        Output(Output::NoPrefix) << "uciok";

        if (engine.isCopyProtected()) {
            Output(Output::NoPrefix) << "copyprotection checking";
            if (engine.copyIsOK()) {
                Output(Output::NoPrefix) << "copyprotection ok";
            } else {
                Output(Output::NoPrefix) << "copyprotection error";
            }
        }

        if (!engine.isRegistered()) {
            Output(Output::NoPrefix) << "registration error";
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "ucinewgame" command
//! UCI specification:
//!   This is sent to the engine when the next search (started with "position"
//!   and "go") will be from a different game.  This can be a new game the
//!   engine should play or a new game it should analyse but also the next
//!   position from a testsuite with positions only.  If the GUI hasn't sent a
//!   "ucinewgame" before the first "position" command, the engine shouldn't
//!   expect any further ucinewgame commands as the GUI is probably not
//!   supporting the ucinewgame command.  So the engine should not rely on this
//!   command even though all new GUIs should support it.  As the engine's
//!   reaction to "ucinewgame" can take some time the GUI should always send
//!   "isready" after "ucinewgame" to wait for the engine to finish its
//!   operation.
//-----------------------------------------------------------------------------
    void UCIAdapter::doUCINewGameCommand(const Parameters &params) {
      if (params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::UCI_NEW_GAME;
        Output() << "Clear all search data.";
        return;
      }

      if (!engine.isInitialized()) {
        engine.initialize();
      }

      if (lastCommand) {
        lastCommand->stop();
        lastCommand->waitForFinish();
      }

      lastPosition.clear();
      engine.clearSearchData();
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "position" command
//! UCI specification:
//!   Set up the position described in fenstring on the internal board and
//!   play the moves on the internal chess board.  If the game was played from
//!   the start position the string "startpos" will be sent.
//!   Note: no "new" command is needed.  However, if this position is from a
//!   different game than the last position sent to the engine, the GUI should
//!   have sent a "ucinewgame" inbetween.
//-----------------------------------------------------------------------------
    void UCIAdapter::doPositionCommand(const std::string &fenstring,
                                       Parameters &params) {
      if (params.empty() || params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::POSITION << " {" << Token::START_POS
                 << "|" << Token::FEN << " <fen_string>} [<movelist>]";
        Output() << "Set a new position and apply <movelist> (if given).";
        return;
      }

        if (!engine.isInitialized()) {
            engine.initialize();
            lastPosition.clear();
        }

        if (lastCommand) {
            lastCommand->stop();
            lastCommand->waitForFinish();
        }

        if (!lastPosition.empty() &&
            lastPosition == fenstring.substr(0, lastPosition.size())) {
          // continue from current position
          params.parse(fenstring.substr(lastPosition.size() + 1));
        } else {
          if (params.firstParamIs(Token::START_POS)) {
            if (!engine.setPosition(ChessEngine::STARTPOS)) {
              return;
            }
          } else {
            // consume "fen" token if present
            params.popParam(Token::FEN);
            std::string remain;
            if (!engine.setPosition(params.toString(), &remain)) {
              return;
            }
            params.parse(remain);
          }
        }

        // remember this position command for next time
        lastPosition = fenstring;

        // consume "moves" token if present
        params.popParam(Token::START_POS);
        params.popParam(Token::MOVES);

        // apply moves (if any)
        while (!params.empty() && isMove(params.front())) {
          std::string move = params.popString();
          if (!engine.makeMove(move)) {
            Output() << "Invalid move: " << move;
            lastPosition.clear();
            break;
          }
        }

        if (engine.isDebugOn()) {
            engine.printBoard();
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "setoption" command
//! UCI specification:
//!   This is sent to the engine when the user wants to change the internal
//!   parameters of the engine.  For the "button" type no value is needed.
//!   One string will be sent for each parameter and this will only be sent
//!   when the engine is waiting.  The name and value of the option in <id>
//!   should not be case sensitive and can inlude spaces.  The substrings
//!   "value" and "name" should be avoided in <id> and <x> to allow unambiguous
//!   parsing, for example do not use <name> = "draw value".
//!   Here are some strings for the example below:
//!      "setoption name Nullmove value true\n"
//!      "setoption name Selectivity value 3\n"
//!      "setoption name Style value Risky\n"
//!      "setoption name Clear Hash\n"
//!      "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"
//-----------------------------------------------------------------------------
    void UCIAdapter::doSetOptionCommand(Parameters &params) {
      if (params.empty() || params.firstParamIs(Token::HELP)) {
        Output() << "usage: " << Token::SET_OPTION << ' ' << Token::NAME
                 << " <option_name> [" << Token::VALUE << " <option_value>]";
        Output() << "Set the value of the specified option name.";
        Output() << "If no value specified the option's default value is used,";
        Output() << "or the option will be triggered if it's a button option.";
        return;
      }

        std::string name;
        std::string value;

        if (!params.firstParamIs(Token::NAME)) {
          Output() << "Missing name token";
          return;
        }

        if (!params.popString(Token::NAME, name, Token::VALUE)) {
          Output() << "Missing name value";
          return;
        }

        if (params.firstParamIs(Token::VALUE)) {
          if (!params.popString(Token::VALUE, value)) {
            Output() << "Missing value";
            return;
          }
        }

        if (!params.empty()) {
          Output() << "Unexpected token: " << params.front();
          return;
        }

        if (!engine.setEngineOption(name, value)) {
            Output() << "Unknown option name '" << name
                     << "' or invalid option value '" << value << "'";
        }
    }

//-----------------------------------------------------------------------------
//! \brief Do the UCI "ponderhit" command
//! UCI specification:
//! 	The user has played the expected move. This will be sent if the engine
//!   was told to ponder on the same move the user has played. The engine
//!   should continue searching but switch from pondering to normal search.
//-----------------------------------------------------------------------------
    void UCIAdapter::doPonderHitCommand(Parameters & /*params*/) {
        engine.ponderHit();
    }

} // namespace senjo
