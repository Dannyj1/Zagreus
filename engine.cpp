#include "senjo/ChessEngine.h"
#include "senjo/Output.h"
#include "engine.h"

bool isEngineRegistered = false;

std::string Engine::getEngineName() const {
    return "Engine";
}

std::string Engine::getEngineVersion() const {
    return "0.1-SNAPSHOT";
}

std::string Engine::getAuthorName() const {
    return "Dannyj1";
}

std::string Engine::getEmailAddress() const {
    return "";
}

std::string Engine::getCountryName() const {
    return "The Netherlands";
}

std::list<senjo::EngineOption> Engine::getOptions() const {
    return {};
}

bool Engine::setEngineOption(const std::string &optionName, const std::string &optionValue) {
    return false;
}

void Engine::initialize() {
    // TODO: create new board
}

bool Engine::isInitialized() const {
    // TODO: implement
    return true;
}

bool Engine::setPosition(const std::string &fen, std::string* remain) {
    // TODO: implement
    return false;
}

bool Engine::makeMove(const std::string &move) {
    // TODO: implement
    return false;
}

std::string Engine::getFEN() const {
    // TODO: implement
    return "";
}

void Engine::printBoard() const {
    // TODO: implement
}

bool Engine::whiteToMove() const {
    // TODO: implement
    return true;
}

void Engine::clearSearchData() {
    // TODO: implement
}

void Engine::ponderHit() {
}

bool Engine::isRegistered() const {
    return isEngineRegistered;
}

void Engine::registerLater() {

}

bool Engine::doRegistration(const std::string &name, const std::string &code) {
    return false;
}

bool Engine::isCopyProtected() const {
    return false;
}

bool Engine::copyIsOK() {
    return true;
}

void Engine::setDebug(const bool flag) {

}

bool Engine::isDebugOn() const {
    return false;
}

bool Engine::isSearching() {
    // TODO: implement
    return false;
}

void Engine::stopSearching() {
    // TODO: implement
}

bool Engine::stopRequested() const {
    // TODO: implement
    return false;
}

void Engine::waitForSearchFinish() {
    // TODO: implement
}

uint64_t Engine::perft(const int depth) {
    // TODO: implement
    return 0;
}

std::string Engine::go(const senjo::GoParams &params, std::string* ponder) {
    // TODO: implement
    return "";
}

senjo::SearchStats Engine::getSearchStats() const {
    // TODO: implement
    return {};
}

void Engine::resetEngineStats() {
    // TODO: implement
}

void Engine::showEngineStats() const {
    // TODO: implement
}